
#define MPU6050_ADDR (0x68 << 1) // Địa chỉ của con cảm biến (dịch trái 1 bit theo chuẩn I2C)
#define MPU6050_PWR 0x6B         // Địa chỉ thanh ghi quản lý nguồn
#define MPU6050_DATA 0x3B        // Địa chỉ thanh ghi chứa dữ liệu đo đầu tiên

#include <MPU6050/MPU6050.h>
extern I2C_HandleTypeDef hi2c1;

// Hàm khởi tạo cảm biến góc nghiêng (MPU6050)
void init_mpu6050(void) {
  uint8_t data = 0;
  // Gửi số 0 vào thanh ghi nguồn để đánh thức cảm biến (vì mặc định nó đang ngủ)
  HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, MPU6050_PWR, 1, &data, 1, 1000);
  HAL_Delay(10); // Đợi cảm biến khởi động sau khi thoát chế độ sleep (ngủ)
}

// Hàm đọc dữ liệu từ cảm biến
void read_mpu6050_data(MPU6050_Data_t *data) {
  uint8_t buffer[14] = {0}; // Khởi tạo mảng bằng 0 để tránh rác từ bộ nhớ

  // Đọc liên tiếp 14 ô nhớ từ cảm biến
  HAL_StatusTypeDef status =
      HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, MPU6050_DATA, 1, buffer, 14, 1000);

  if (status == HAL_OK) {
    // Ghép 2 byte thành 1 con số có dấu (16-bit) cho gia tốc
    data->AccX = (int16_t)(buffer[0] << 8 | buffer[1]);
    data->AccY = (int16_t)(buffer[2] << 8 | buffer[3]);
    data->AccZ = (int16_t)(buffer[4] << 8 | buffer[5]);
    
    // Bỏ qua buffer 6 và 7 vì đó là các bit lưu nhiệt độ của mạch
    
    // Ghép 2 byte thành 1 con số có dấu cho vận tốc góc (Gyro)
    data->GyroX = (int16_t)(buffer[8] << 8 | buffer[9]);
    data->GyroY = (int16_t)(buffer[10] << 8 | buffer[11]);
    data->GyroZ = (int16_t)(buffer[12] << 8 | buffer[13]);

    // Chuyển đổi sang đơn vị vật lý thực tế
    // Dải đo mặc định +-2g, 1g tương ứng với 16384 LSB (65536/4)
    data->Ax = data->AccX / 16384.0f;
    data->Ay = data->AccY / 16384.0f;
    data->Az = data->AccZ / 16384.0f;

    // Ở dải đo mặc định +-250 độ/s, 1 độ/s tương ứng với 131 LSB (65536/500)
    data->Gx = data->GyroX / 131.0f;
    data->Gy = data->GyroY / 131.0f;
    data->Gz = data->GyroZ / 131.0f;
  } else if (status == HAL_ERROR) {
    // Nếu đọc bị lỗi
    uint32_t err_code = HAL_I2C_GetError(&hi2c1);
    uint8_t value_err = 0;
    if (err_code == HAL_I2C_ERROR_AF) { // Lỗi sai địa chỉ hoặc hỏng dây, NACK
      value_err = 1;
    } else if (err_code == HAL_I2C_ERROR_BERR) { // Do nhiễu đường truyền hoặc thiếu điện trở kéo lên
      value_err = 2;
    } else if (err_code == HAL_I2C_ERROR_ARLO) { // Tranh chấp Bus
      value_err = 3;
    }

    // Đặt kết quả trả về là mã lỗi để biết bị hỏng ở đâu
    data->AccX = value_err;
    data->AccY = value_err;
    data->AccZ = value_err;
    data->GyroX = value_err;
    data->GyroY = value_err;
    data->GyroZ = value_err;

    float value_err_f = (float)value_err;

    data->Ax = value_err_f;
    data->Ay = value_err_f;
    data->Az = value_err_f;
    data->Gx = value_err_f;
    data->Gy = value_err_f;
    data->Gz = value_err_f;

    // Khởi động lại chân giao tiếp I2C để thử lại lần sau
    HAL_I2C_DeInit(&hi2c1);
    HAL_I2C_Init(&hi2c1);
  }
}
