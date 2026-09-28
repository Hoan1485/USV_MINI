#ifndef DS18B20_H
#define DS18B20_H

#include "stm32f4xx_hal.h"

// Cấu hình chân kết nối DS18B20 (DQ)
#define DS18B20_PORT GPIOE
#define DS18B20_PIN  GPIO_PIN_11

void init_ds18b20(void);
void request_temperature(void);
float read_temperature(void);

#endif // DS18B20_H
