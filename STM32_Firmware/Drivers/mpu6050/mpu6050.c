#define MPU6050_ADDR (0x68 << 1) // Dia chi cua con cam bien
#define MPU6050_PWR 0x6B // Dia chi o nho dung de luu du lieu dau tien tra ve
#define MPU6050_DATA 0x3B

#include "mpu6050.h"
extern I2C_HandleTypeDef hi2c1;

void MPU6050_Init(void) {
  uint8_t data = 0;
  HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, MPU6050_PWR, 1, &data, 1, 1000);
  HAL_Delay(10); // Đợi cảm biến khởi động sau khi thoát chế độ sleep
}

void MPU6050_ReadRaw(MPU6050_Data_t *data) {
  uint8_t buffer[14] = {0}; // Khởi tạo mảng bằng 0 để tránh rác từ stack

  HAL_StatusTypeDef status =
      HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, MPU6050_DATA, 1, buffer, 14, 1000);

  if (status == HAL_OK) {
    data->AccX = (int16_t)(buffer[0] << 8 | buffer[1]);
    data->AccY = (int16_t)(buffer[2] << 8 | buffer[3]);
    data->AccZ = (int16_t)(buffer[4] << 8 | buffer[5]);
    // Bỏ qua buffer 6 và 7 vì đó là các bit lưu nhiệt độ
    data->GyroX = (int16_t)(buffer[8] << 8 | buffer[9]);
    data->GyroY = (int16_t)(buffer[10] << 8 | buffer[11]);
    data->GyroZ = (int16_t)(buffer[12] << 8 | buffer[13]);

    // Chuyển đổi sang đơn vị vật lý thực tế
    // Dải đo mặc định +-2g, 1g tương ứng với 16384 LSB (65536/4)
    data->Ax = data->AccX / 16384.0f;
    data->Ay = data->AccY / 16384.0f;
    data->Az = data->AccZ / 16384.0f;

    // Ở dải đo mặc định +-250 deg/s, 1 độ/s tương ứng với 131 LSB (65536/500)
    data->Gx = data->GyroX / 131.0f;
    data->Gy = data->GyroY / 131.0f;
    data->Gz = data->GyroZ / 131.0f;
  } else {
    data->AccX = 0;
    data->AccY = 0;
    data->AccZ = 0;
    data->GyroX = 0;
    data->GyroY = 0;
    data->GyroZ = 0;

    data->Ax = 0.0f;
    data->Ay = 0.0f;
    data->Az = 0.0f;
    data->Gx = 0.0f;
    data->Gy = 0.0f;
    data->Gz = 0.0f;
  }
}
