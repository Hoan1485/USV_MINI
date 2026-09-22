#include "qmc5883l.h"

#define QMC5883L_ADDR (0x0D << 1)

#define QMC_DATA_START 0x00
#define QMC_CTRL_REG1 0x09
#define QMC_CTRL_REG2 0x0A
#define QMC_SET_RESET 0x0B

extern I2C_HandleTypeDef hi2c1;

void QMC5883L_Init(void)
{
  uint8_t config = 0;
  // 1. Setting SET/RESET Period by Datasheet
  config = 0x01;
  HAL_I2C_Mem_Write(&hi2c1, QMC5883L_ADDR, QMC_SET_RESET, 1, &config, 1, 1000);
  // 2. Setting Control Register 1
  // OSR=512, RNG=8G, ODR=200Hz, MODE=Continuous -> 00011101 (0x1D)
  config = 0x1D;
  HAL_I2C_Mem_Write(&hi2c1, QMC5883L_ADDR, QMC_CTRL_REG1, 1, &config, 1, 1000);
  HAL_Delay(10);
}

void QMC5883L_ReadRaw(QMC5883L_Data_t *data)
{
  uint8_t buffer[6] = {0}; // QMC5883L has 3 axis X, Y, Z. Each bit respective two byte

  // Read 6 byte, begin from 0x00 to 0x06
  HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
      &hi2c1, QMC5883L_ADDR, QMC_DATA_START, 1, buffer, 6, 1000);

  if (status == HAL_OK)
  {
    // Lưu ý cực kỳ quan trọng: QMC5883L truyền byte LSB (thấp) trước, MSB (cao)
    // sau. Điều này hoàn toàn ngược lại so với MPU6050!
    data->MagX = (int16_t)(buffer[1] << 8 | buffer[0]);
    data->MagY = (int16_t)(buffer[3] << 8 | buffer[2]);
    data->MagZ = (int16_t)(buffer[5] << 8 | buffer[4]);

    // Chuyển đổi sang đơn vị vật lý (Gauss).
    // Với dải đo (RNG) = 8G, hệ số độ phân giải là 3000 LSB/Gauss
    data->Mx = data->MagX / 3000.0f;
    data->My = data->MagY / 3000.0f;
    data->Mz = data->MagZ / 3000.0f;
  }
  else if (status == HAL_ERROR)
  {
	uint32_t err_code = HAL_I2C_GetError(&hi2c1);
	uint8_t value_err = 0;
	if (err_code == HAL_I2C_ERROR_AF) // Lỗi sai địa chỉ hoặc hỏng dây, NACK
	{
		value_err = 1;
	}
	else if(err_code == HAL_I2C_ERROR_BERR) // Do nhiễu đường truyền hoặc thiếu điện trở kéo lên
	{
		value_err = 2;
	}
	else if (err_code == HAL_I2C_ERROR_ARLO) // Tranh chấp Bus
	{
		value_err = 3;
	}
	data->MagX = value_err;
	data->MagY = value_err;
	data->MagZ = value_err;

	float value_err_f = (float)value_err;

	data->Mx = value_err_f;
	data->My = value_err_f;
	data->Mz = value_err_f;
  HAL_I2C_DeInit(&hi2c1);
  HAL_I2C_Init(&hi2c1);
  }
}
