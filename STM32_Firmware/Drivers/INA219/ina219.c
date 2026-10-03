#include "ina219.h"
#include "../../Application/Inc/esp32_comm.h"

// INA219 Calibration for 32V, 2A Range
// VBUS_MAX = 32V
// VSHUNT_MAX = 0.32V
// RSHUNT = 0.1 ohms
// Max expected current = 2.0A
// Current LSB = 2A / 32768 = 61.035uA (We choose 100uA for nice numbers)
// Cal = trunc(0.04096 / (Current_LSB * RSHUNT)) = trunc(0.04096 / (0.0001 *
// 0.1)) = 4096 (0x1000)

static void INA219_WriteRegister(I2C_HandleTypeDef *hi2c, uint8_t reg,
                                 uint16_t value) {
  uint8_t data[2];
  data[0] = (value >> 8) & 0xFF; // MSB
  data[1] = value & 0xFF;        // LSB
  HAL_I2C_Mem_Write(hi2c, INA219_ADDRESS, reg, 1, data, 2, 100);
}

static HAL_StatusTypeDef INA219_ReadRegister(I2C_HandleTypeDef *hi2c,
                                             uint8_t reg, uint16_t *value) {
  uint8_t data[2];
  HAL_StatusTypeDef status =
      HAL_I2C_Mem_Read(hi2c, INA219_ADDRESS, reg, 1, data, 2, 100);
  if (status == HAL_OK) {
    *value = ((uint16_t)data[0] << 8) | data[1];
  }
  return status;
}

HAL_StatusTypeDef INA219_Init(I2C_HandleTypeDef *hi2c) {
  // Config Register:
  // Bit 15: Reset = 0
  // Bit 13: Bus Voltage Range = 1 (32V FSR)
  // Bit 11-12: PGA (Shunt Voltage Only) = 11 (+/- 320mV)
  // Bit 7-10: BADC = 0011 (12-bit, 1 sample)
  // Bit 3-6: SADC = 0011 (12-bit, 1 sample)
  // Bit 0-2: Mode = 111 (Shunt and Bus, Continuous)
  // Value = 0x399F
  uint16_t config = 0x399F;

  // Calibrate for 32V, 2A
  uint16_t cal = 4096;

  INA219_WriteRegister(hi2c, INA219_REG_CALIBRATION, cal);
  INA219_WriteRegister(hi2c, INA219_REG_CONFIG, config);

  // Just to check if it's responding
  uint16_t read_conf = 0;
  return INA219_ReadRegister(hi2c, INA219_REG_CONFIG, &read_conf);
}

HAL_StatusTypeDef INA219_ReadBusVoltage_V(I2C_HandleTypeDef *hi2c,
                                          float *voltage) {
  uint16_t value = 0;
  HAL_StatusTypeDef status =
      INA219_ReadRegister(hi2c, INA219_REG_BUSVOLTAGE, &value);
  if (status == HAL_OK) {
    // Shift right 3 bits, multiply by 4mV (0.004)
    *voltage = (float)((value >> 3) * 4) / 1000.0f;
  } else {
    static uint32_t last_err = 0;
    if (HAL_GetTick() - last_err > 1000) {
      last_err = HAL_GetTick();
      ESP32Comm_SendError("I2C_INA219_ERR");
    }
  }
  return status;
}

HAL_StatusTypeDef INA219_ReadCurrent_A(I2C_HandleTypeDef *hi2c,
                                       float *current) {
  uint16_t value = 0;
  // Make sure calibration register is written before reading current!
  INA219_WriteRegister(hi2c, INA219_REG_CALIBRATION, 4096);

  HAL_StatusTypeDef status =
      INA219_ReadRegister(hi2c, INA219_REG_CURRENT, &value);
  if (status == HAL_OK) {
    int16_t signed_val = (int16_t)value;
    // Current LSB = 100uA = 0.1mA = 0.0001A
    *current = (float)signed_val * 0.0001f;
  }
  return status;
}
