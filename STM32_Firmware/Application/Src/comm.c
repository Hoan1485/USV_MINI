#include "../../Application/Inc/comm.h"

#include "usart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../Application/Inc/stepper.h"

BoatMode current_mode = MODE_MANUAL; 


float target_waypoint_lat = 0.0f;
float target_waypoint_lon = 0.0f;
uint8_t new_waypoint_flag = 0; 


int manual_left_speed = 1500;
int manual_right_speed = 1500;

uint8_t feed_request_flag = 0;
uint32_t feed_duration_ms = 3000; 


#define RX_CMD_MAX_LEN 64 
char rx_cmd_buffer[RX_CMD_MAX_LEN]; 
uint8_t rx_byte;                    
int rx_index = 0;                   


uint32_t last_cmd_time = 0;

// Hàm khởi tạo giao tiếp
void Comm_Init(void) {
  extern UART_HandleTypeDef huart2;
  HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
}

// Hàm giải mã lệnh nhận được từ ESP32
void Comm_ParseCommand(char *cmd) {
  char debug_msg[100];
  int len = sprintf(debug_msg, "[ESP32 -> STM32] Nhan duoc: %s\r\n", cmd);
  HAL_UART_Transmit(&huart3, (uint8_t *)debug_msg, len, 100);

  last_cmd_time = HAL_GetTick();

  if (strncmp(cmd, "MODE|", 5) == 0) {
    if (strncmp(cmd + 5, "AUTO", 4) == 0) {
      current_mode = MODE_AUTO;
    } else {
      current_mode = MODE_MANUAL;
    }
  }
  else if (strcmp(cmd, "STOP") == 0) {
    current_mode = MODE_MANUAL;
    manual_left_speed = 1500;
    manual_right_speed = 1500;
  }
  else if (strncmp(cmd, "MOTOR|", 6) == 0) {
    char *l_str = cmd + 6;
    char *r_str = strchr(l_str, '|');
    if (r_str != NULL) {
      *r_str = '\0';
      r_str++;

      int val_l = atoi(l_str);
      int val_r = atoi(r_str);
      manual_left_speed = 1500 + (val_l * 5);
      manual_right_speed = 1500 + (val_r * 5);
    }
  }
  else if (strncmp(cmd, "FEED|", 5) == 0) {
    feed_duration_ms = atoi(cmd + 5);
    if (feed_duration_ms == 0) feed_duration_ms = 3000;
    feed_request_flag = 1;
  }
  else if (strncmp(cmd, "CMD,FEED", 8) == 0) {
    char *ms_str = strrchr(cmd, ',');
    if (ms_str != NULL && ms_str != cmd + 3) {
      feed_duration_ms = atoi(ms_str + 1);
    } else {
      feed_duration_ms = 3000;
    }
    if (feed_duration_ms == 0) feed_duration_ms = 3000;
    feed_request_flag = 1;
  }
  else if (strcmp(cmd, "AUTO_START") == 0) {
    current_mode = MODE_AUTO;
  } else if (strcmp(cmd, "AUTO_STOP") == 0) {
    current_mode = MODE_MANUAL;
    manual_left_speed = 1500;
    manual_right_speed = 1500;
  }
}

// Hàm kiểm tra An Toàn (Failsafe)
void Comm_CheckFailsafe(void) {
  if (HAL_GetTick() - last_cmd_time > 3000) {
    current_mode = MODE_MANUAL;
    manual_left_speed = 1500; 
    manual_right_speed = 1500;
  }
}

// Hàm gửi dữ liệu (Telemetry) từ STM32 lên ESP32
void Comm_SendTelemetry(float lat, float lon, float heading, float temp,
                        float speed) {
  char txBuffer[100]; 

  int len = sprintf(txBuffer,
                    "TEL,%.6f,%.6f,%.1f,%.1f,%.1f\n",
                    lat, lon, heading, temp, speed);

  extern UART_HandleTypeDef huart2;
  HAL_UART_Transmit(&huart2, (uint8_t *)txBuffer, len, 100);
}

// Hàm ngắt (Callback) được tự động gọi mỗi khi nhận được 1 chữ cái qua UART
void Comm_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART2) {
    if (rx_byte == '\n' || rx_byte == '\r') {
      rx_cmd_buffer[rx_index] = '\0'; 

      if (rx_index > 0) {
        Comm_ParseCommand(rx_cmd_buffer);
        rx_index = 0; 
      }
    } else {
      if (rx_index < RX_CMD_MAX_LEN - 1) {
        rx_cmd_buffer[rx_index++] = (char)rx_byte;
      }
    }

    extern UART_HandleTypeDef huart2;
    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
  }
}
