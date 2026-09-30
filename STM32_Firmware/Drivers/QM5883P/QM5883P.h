#ifndef QMC5883P_H
#define QMC5883P_H

#include "i2c.h"
#include <stdint.h>

// Struct chứa dữ liệu từ trường (Từ kế)
typedef struct {
  int16_t MagX, MagY, MagZ;  // Dữ liệu thô (raw)
  float Mx, My, Mz;          // Từ trường (đơn vị: Gauss)
} QMC5883P_Data_t;

void QMC5883P_Init(void);
void QMC5883P_ReadRaw(QMC5883P_Data_t *data);

#endif
