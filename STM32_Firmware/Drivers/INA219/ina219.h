#ifndef __INA219_H
#define __INA219_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define INA219_ADDRESS (0x40 << 1) // Default I2C Address (A0=GND, A1=GND)

// Registers
#define INA219_REG_CONFIG 0x00
#define INA219_REG_SHUNTVOLTAGE 0x01
#define INA219_REG_BUSVOLTAGE 0x02
#define INA219_REG_POWER 0x03
#define INA219_REG_CURRENT 0x04
#define INA219_REG_CALIBRATION 0x05

/**
 * @brief Initialize INA219 with a standard 32V, 2A range
 */
HAL_StatusTypeDef INA219_Init(I2C_HandleTypeDef *hi2c);

/**
 * @brief Read Bus Voltage in Volts
 * @param voltage Pointer to store the result
 */
HAL_StatusTypeDef INA219_ReadBusVoltage_V(I2C_HandleTypeDef *hi2c, float *voltage);

/**
 * @brief Read Current in Amperes
 * @param current Pointer to store the result
 */
HAL_StatusTypeDef INA219_ReadCurrent_A(I2C_HandleTypeDef *hi2c, float *current);

#ifdef __cplusplus
}
#endif

#endif // __INA219_H
