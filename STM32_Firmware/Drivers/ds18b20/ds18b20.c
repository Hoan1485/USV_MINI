#include <DS18B20/DS18B20.h>

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

// --- Các hàm cho người dùng (API) ---

// Hàm khởi tạo cảm biến nhiệt độ
void init_ds18b20(void) {
  DWT_Delay_Init(); // Khởi tạo bộ đếm thời gian trễ micro-giây
  // Bật xung nhịp cho cổng kết nối với cảm biến
  if (DS18B20_PORT == GPIOE) {
    __HAL_RCC_GPIOE_CLK_ENABLE();
  }
}

// Hàm gửi lệnh yêu cầu cảm biến bắt đầu đo nhiệt độ
void request_temperature(void) {
  if (DS18B20_Reset()) {
    DS18B20_Write_Byte(0xCC); // Lệnh Skip ROM (Bỏ qua việc gọi địa chỉ, vì chỉ có 1 cảm biến)
    DS18B20_Write_Byte(0x44); // Lệnh Convert T (Bảo cảm biến hãy đo nhiệt độ đi)
  }
}

// Hàm đọc kết quả nhiệt độ (Sau khi đã yêu cầu đo)
float read_temperature(void) {
  uint8_t temp_l, temp_h;
  uint16_t temp;
  float temperature = -999.0f; // Trả về giá trị -999 nếu bị lỗi (đứt dây, hỏng...)

  if (DS18B20_Reset()) {
    DS18B20_Write_Byte(0xCC); // Lệnh Skip ROM
    DS18B20_Write_Byte(0xBE); // Lệnh Read Scratchpad (Đọc bộ nhớ của cảm biến)

    // Đọc 2 byte dữ liệu nhiệt độ
    temp_l = DS18B20_Read_Byte(); // Byte thấp
    temp_h = DS18B20_Read_Byte(); // Byte cao

    // Ghép 2 byte lại thành 1 số 16-bit
    temp = (temp_h << 8) | temp_l;
    
    // Chia cho 16 để ra nhiệt độ thực tế (theo datasheet của DS18B20)
    temperature = (float)temp / 16.0f;
  }
  return temperature;
}
