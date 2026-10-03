#include "QM5883P.h"
#include "../../Application/Inc/esp32_comm.h"

#define QMC5883P_ADDR (0x2C << 1) // HP5883L Address


#define QMC_DATA_START 0x01
#define QMC_CTRL_REG1 0x0A
#define QMC_CTRL_REG2 0x0B
#define QMC_SET_RESET 0x0C

extern I2C_HandleTypeDef hi2c1;

HAL_StatusTypeDef QMC5883P_Init(void) {
  uint8_t config = 0;
  // 1. Setting SET/RESET Period by Datasheet
  config = 0x01;
  HAL_StatusTypeDef status = HAL_I2C_Mem_Write(
      &hi2c1, QMC5883P_ADDR, QMC_SET_RESET, 1, &config, 1, 1000);
  // 2. Setting Control Register 1 (QMC5883P: OSR=4, ODR=200Hz, MODE=Continuous)

  config = 0x1F;
  HAL_I2C_Mem_Write(&hi2c1, QMC5883P_ADDR, QMC_CTRL_REG1, 1, &config, 1, 1000);
  HAL_Delay(10);
  return status;
}

void QMC5883P_ReadRaw(QMC5883P_Data_t *data) {
  uint8_t buffer[6] = {
      0}; // QMC5883P has 3 axis X, Y, Z. Each bit respective two byte

  // Read 6 byte, begin from 0x01 to 0x06
  HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
      &hi2c1, QMC5883P_ADDR, QMC_DATA_START, 1, buffer, 6, 1000);

  if (status == HAL_OK) {
    data->MagX = (int16_t)(buffer[1] << 8 | buffer[0]);
    data->MagY = (int16_t)(buffer[3] << 8 | buffer[2]);
    data->MagZ = (int16_t)(buffer[5] << 8 | buffer[4]);

    data->Mx = data->MagX / 3000.0f;
    data->My = data->MagY / 3000.0f;
    data->Mz = data->MagZ / 3000.0f;
  } else {
    uint32_t err_code = HAL_I2C_GetError(&hi2c1);

    static uint32_t last_qmc_err_tick = 0;
    if (HAL_GetTick() - last_qmc_err_tick > 1000) {
      last_qmc_err_tick = HAL_GetTick();
      if (status == HAL_TIMEOUT) {
        ESP32Comm_SendError("I2C_QMC_TIMEOUT");
      } else if (err_code == HAL_I2C_ERROR_AF) {
        ESP32Comm_SendError("I2C_QMC_NACK");
      } else if (err_code == HAL_I2C_ERROR_BERR) {
        ESP32Comm_SendError("I2C_QMC_BERR");
      } else if (err_code == HAL_I2C_ERROR_ARLO) {
        ESP32Comm_SendError("I2C_QMC_ARLO");
      } else {
        ESP32Comm_SendError("I2C_QMC_UNKNOWN");
      }
    }

    HAL_I2C_DeInit(&hi2c1);
    HAL_I2C_Init(&hi2c1);
  }
}
