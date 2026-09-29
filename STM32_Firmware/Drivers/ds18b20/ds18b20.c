#include "ds18b20.h"

// --- Hàm trễ microsecond bằng DWT ---
static void DWT_Delay_Init(void) {
  // Cho phép đếm chu kỳ (TRCENA)
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  // Reset bộ đếm
  DWT->CYCCNT = 0;
  // Kích hoạt bộ đếm
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

static void delay_us(uint32_t us) {
  uint32_t startTick = DWT->CYCCNT;
  // SystemCoreClock lưu tần số của chip (168MHz cho STM32F407)
  uint32_t delayTicks = us * (SystemCoreClock / 1000000);
  while ((DWT->CYCCNT - startTick) < delayTicks)
    ;
}

// --- Các hàm cấp thấp 1-Wire ---
static void DS18B20_Set_Pin_Output(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = DS18B20_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
}

static void DS18B20_Set_Pin_Input(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = DS18B20_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DS18B20_PORT, &GPIO_InitStruct);
}

static uint8_t DS18B20_Reset(void) {
  uint8_t response = 0;
  DS18B20_Set_Pin_Output();
  HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
  delay_us(480);

  DS18B20_Set_Pin_Input();
  delay_us(80);

  if (!(HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN))) {
    response = 1; // Có cảm biến phản hồi
  } else {
    response = 0; // Không tìm thấy cảm biến
  }
  delay_us(400); // Đợi kết thúc chu kỳ reset
  return response;
}

static void DS18B20_Write_Bit(uint8_t bit) {
  DS18B20_Set_Pin_Output();
  if (bit) {
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
    delay_us(2);
    DS18B20_Set_Pin_Input(); // Nhả bit lên cao
    delay_us(60);
  } else {
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
    delay_us(60);
    DS18B20_Set_Pin_Input();
    delay_us(2);
  }
}

static uint8_t DS18B20_Read_Bit(void) {
  uint8_t bit = 0;
  DS18B20_Set_Pin_Output();
  HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
  delay_us(2);

  DS18B20_Set_Pin_Input();
  delay_us(10);

  if (HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN)) {
    bit = 1;
  }
  delay_us(50);
  return bit;
}

static void DS18B20_Write_Byte(uint8_t data) {
  for (int i = 0; i < 8; i++) {
    DS18B20_Write_Bit(data & 0x01);
    data >>= 1;
  }
}

static uint8_t DS18B20_Read_Byte(void) {
  uint8_t data = 0;
  for (int i = 0; i < 8; i++) {
    data >>= 1;
    if (DS18B20_Read_Bit()) {
      data |= 0x80;
    }
  }
  return data;
}

// --- API ---

void DS18B20_Init(void) {
  DWT_Delay_Init();
  // Bật xung nhịp cho port
  if (DS18B20_PORT == GPIOE) {
    __HAL_RCC_GPIOE_CLK_ENABLE();
  }
}

void DS18B20_Request_Temp(void) {
  if (DS18B20_Reset()) {
    DS18B20_Write_Byte(0xCC); // Skip ROM
    DS18B20_Write_Byte(0x44); // Convert T
  }
}

float DS18B20_Read_Temp(void) {
  uint8_t temp_l, temp_h;
  uint16_t temp;
  float temperature = -999.0f; // Trả về giá trị này nếu lỗi

  if (DS18B20_Reset()) {
    DS18B20_Write_Byte(0xCC); // Skip ROM
    DS18B20_Write_Byte(0xBE); // Read Scratchpad

    temp_l = DS18B20_Read_Byte();
    temp_h = DS18B20_Read_Byte();

    temp = (temp_h << 8) | temp_l;
    temperature = (float)temp / 16.0f;
  }
  return temperature;
}
