#include "comm.h"
#include "stepper.h"
#include "usart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Biến lưu trạng thái hiện tại của tàu (điều khiển bằng tay hay tự động)
BoatMode current_mode = MODE_MANUAL; // Mặc định là điều khiển bằng tay (Manual)

// Tọa độ điểm đến (Latitude - vĩ độ, Longitude - kinh độ)
float target_waypoint_lat = 0.0f;
float target_waypoint_lon = 0.0f;
uint8_t new_waypoint_flag = 0; // Cờ báo hiệu có điểm đến mới

// Tốc độ động cơ ở chế độ Manual (1500 là đứng yên)
int manual_left_speed = 1500;
int manual_right_speed = 1500;

// Cờ yêu cầu rải mồi
uint8_t feed_request_flag = 0;

// Cấu hình bộ nhớ đệm (buffer) để nhận lệnh
#define RX_CMD_MAX_LEN 64 // Chiều dài tối đa của một câu lệnh là 64 ký tự
char rx_cmd_buffer[RX_CMD_MAX_LEN]; // Mảng lưu các chữ cái nhận được
uint8_t rx_byte;                    // Biến lưu 1 chữ cái (1 byte) vừa nhận được
int rx_index = 0;                   // Vị trí hiện tại trong mảng

// Biến lưu thời gian nhận lệnh cuối cùng (dùng cho tính năng An Toàn Failsafe)
uint32_t last_cmd_time = 0;

// Hàm khởi tạo giao tiếp
void Comm_Init(void) {
  // Kích hoạt chức năng ngắt UART2 để lắng nghe dữ liệu từ ESP32
  // Khi nào ESP32 gửi 1 chữ cái xuống, hàm ngắt (callback) sẽ tự động chạy
  extern UART_HandleTypeDef huart2;
  HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
}

// Hàm giải mã lệnh nhận được từ ESP32 (từ điện thoại)
void Comm_ParseCommand(char *cmd) {
  // --- HIỂN THỊ DỮ LIỆU TỪ ESP32 LÊN MÁY TÍNH (DEBUG) ---
  char debug_msg[100];
  int len = sprintf(debug_msg, "[ESP32 -> STM32] Nhan duoc: %s\r\n", cmd);
  HAL_UART_Transmit(&huart3, (uint8_t *)debug_msg, len, 100);
  // ------------------------------------------------------

  // Cập nhật lại thời gian vừa nhận lệnh (Reset bộ đếm Failsafe)
  last_cmd_time = HAL_GetTick();

  // Lệnh MODE|MANUAL hoặc MODE|AUTO
  if (strncmp(cmd, "MODE|", 5) == 0) {
    if (strncmp(cmd + 5, "AUTO", 4) == 0) {
      current_mode = MODE_AUTO;
    } else {
      current_mode = MODE_MANUAL;
    }
  }
  // Lệnh STOP (Dừng khẩn cấp)
  else if (strcmp(cmd, "STOP") == 0) {
    current_mode = MODE_MANUAL;
    manual_left_speed = 1500;
    manual_right_speed = 1500;
  }
  // Lệnh điều khiển động cơ: MOTOR|left|right (-100 đến 100)
  else if (strncmp(cmd, "MOTOR|", 6) == 0) {
    char *l_str = cmd + 6;
    char *r_str = strchr(l_str, '|');
    if (r_str != NULL) {
      *r_str = '\0';
      r_str++;

      // Chuyển từ % (-100 -> 100) sang chuẩn PWM (1000 -> 2000)
      int val_l = atoi(l_str);
      int val_r = atoi(r_str);
      manual_left_speed = 1500 + (val_l * 5);
      manual_right_speed = 1500 + (val_r * 5);
    }
  }
  // Lệnh cho cá ăn FEED|ms
  else if (strncmp(cmd, "FEED|", 5) == 0) {
    feed_request_flag =
        1; // Chỉ đặt cờ báo, không gọi hàm có chứa HAL_Delay ở đây
  }
  // Các lệnh Auto (Chưa kết nối UI Map, tạm để cờ kích hoạt)
  else if (strcmp(cmd, "AUTO_START") == 0) {
    current_mode = MODE_AUTO;
  } else if (strcmp(cmd, "AUTO_STOP") == 0) {
    current_mode = MODE_MANUAL;
    manual_left_speed = 1500;
    manual_right_speed = 1500;
  }
}

// Hàm kiểm tra An Toàn (Failsafe)
// Nếu quá 3 giây không nhận được lệnh nào từ ESP32, ép tàu dừng lại
void Comm_CheckFailsafe(void) {
  // Chờ 3000ms = 3 giây
  if (HAL_GetTick() - last_cmd_time > 3000) {
    current_mode = MODE_MANUAL;
    manual_left_speed = 1500; // 1500 là tốc độ dừng
    manual_right_speed = 1500;
  }
}

// Hàm gửi dữ liệu (Telemetry) từ STM32 lên ESP32
void Comm_SendTelemetry(float lat, float lon, float heading, float temp,
                        float speed) {
  char txBuffer[100]; // Mảng chứa chuỗi tin nhắn sẽ gửi đi

  // Ghép các con số thành một câu hoàn chỉnh có cấu trúc cho Web Dashboard
  int len = sprintf(txBuffer,
                    "STATUS|Kinh độ: %.6f|Vĩ độ: %.6f|Hướng: %.1f|Nhiệt độ: "
                    "%.2f|12.6|0.5|Tốc độ: %.1f|0|MANUAL|1\n",
                    lat, lon, heading, speed, temp);

  // Gửi chuỗi này qua UART2 (nối với ESP32)
  extern UART_HandleTypeDef huart2;
  HAL_UART_Transmit(&huart2, (uint8_t *)txBuffer, len, 100);
}

// Hàm ngắt (Callback) được tự động gọi mỗi khi nhận được 1 chữ cái qua UART
void Comm_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  // Nếu nhận từ cổng UART2 (Cổng nối với ESP32)
  if (huart->Instance == USART2) {
    // Nếu chữ cái nhận được là dấu "Xuống dòng" (Kết thúc 1 câu)
    if (rx_byte == '\n' || rx_byte == '\r') {
      rx_cmd_buffer[rx_index] = '\0'; // Đánh dấu kết thúc chuỗi chữ

      // Nếu chuỗi không bị rỗng
      if (rx_index > 0) {
        // Đem chuỗi vừa nhận đi dịch lệnh
        Comm_ParseCommand(rx_cmd_buffer);
        rx_index = 0; // Đặt lại vị trí về 0 để chuẩn bị nhận câu tiếp theo
      }
    } else {
      // Nếu chưa xuống dòng thì tiếp tục lưu chữ cái vào mảng
      if (rx_index < RX_CMD_MAX_LEN - 1) {
        rx_cmd_buffer[rx_index++] = (char)rx_byte;
      }
    }

    // Kích hoạt lại việc lắng nghe chữ cái tiếp theo (Rất quan trọng!)
    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
  }
}
