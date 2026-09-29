#ifndef MPU6050_H
#define MPU6050_H

#include "i2c.h"
#include <stdint.h>

typedef struct {
  int16_t AccX, AccY, AccZ;    // Dữ liệu gia tốc thô (raw)
  int16_t GyroX, GyroY, GyroZ; // Dữ liệu tốc độ thô (raw)

  float Ax, Ay, Az; // Gia tốc tuyến tính (đơn vị: g)
  float Gx, Gy, Gz; // Tốc độ góc (đơn vị: deg/s)
} MPU6050_Data_t;

void MPU6050_Init(void);
void MPU6050_ReadRaw(MPU6050_Data_t *data);

#endif
