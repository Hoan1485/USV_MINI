#ifndef DS18B20_H
#define DS18B20_H

#include "stm32f4xx_hal.h"

// Cấu hình chân kết nối DS18B20 (DQ)
#define DS18B20_PORT GPIOE
#define DS18B20_PIN  GPIO_PIN_11

void DS18B20_Init(void);
void DS18B20_Request_Temp(void);
float DS18B20_Read_Temp(void);

#endif // DS18B20_H
