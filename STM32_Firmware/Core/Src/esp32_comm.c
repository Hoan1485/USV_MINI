/**
 ******************************************************************************
 * @file    esp32_comm.c
 * @brief   Implementation of UART communication with ESP32
 ******************************************************************************
 */

#include "esp32_comm.h"
#include "tim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RX_BUFFER_SIZE 256

/* ============================================================
   PRIVATE VARIABLES
   ============================================================ */

static UART_HandleTypeDef *s_esp32_huart = NULL;

static uint8_t s_rx_byte = 0;
static char s_rx_buffer[RX_BUFFER_SIZE];
static uint16_t s_rx_index = 0;

static char s_pending_cmd[RX_BUFFER_SIZE];
static volatile bool s_cmd_ready = false;

/* Biến toàn cục trạng thái USV */
USV_State_t g_usv_state;

/* ============================================================
   INITIALIZATION
   ============================================================ */

void ESP32Comm_Init(UART_HandleTypeDef *huart)
{
    s_esp32_huart = huart;

    /* Khởi tạo dữ liệu mặc định */
    memset(&g_usv_state, 0, sizeof(USV_State_t));
    g_usv_state.mode = USV_MODE_MANUAL;
    g_usv_state.auto_running = false;
    g_usv_state.battery_volt = 12.0f;
    g_usv_state.feed_percent = 100;
    g_usv_state.gps_valid = false;
    g_usv_state.waypoint_valid = false;

    /* Khởi động PWM cho 2 kênh ESC trên TIM4 */
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1); // Trái (PB6)
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2); // Phải (PB7)
    Motor_Stop();

    /* Bắt đầu nhận ký tự đầu tiên từ USART3 qua ngắt */
    if (s_esp32_huart != NULL)
    {
        HAL_UART_Receive_IT(s_esp32_huart, &s_rx_byte, 1);
    }
}

/* ============================================================
   UART RX INTERRUPT HANDLER
   ============================================================ */

void ESP32Comm_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart != NULL && huart == s_esp32_huart)
    {
        ESP32Comm_RxCallback((char)s_rx_byte);
    }
}

void ESP32Comm_RxCallback(char c)
{
    if (c == '\n')
    {
        if (s_rx_index > 0)
        {
            s_rx_buffer[s_rx_index] = '\0';
            
            /* Chép sang pending buffer nếu chưa bị chiếm */
            if (!s_cmd_ready)
            {
                strncpy(s_pending_cmd, s_rx_buffer, RX_BUFFER_SIZE);
                s_pending_cmd[RX_BUFFER_SIZE - 1] = '\0';
                s_cmd_ready = true;
            }
            s_rx_index = 0;
        }
    }
    else if (c != '\r')
    {
        if (s_rx_index < RX_BUFFER_SIZE - 1)
        {
            s_rx_buffer[s_rx_index++] = c;
        }
        else
        {
            /* Tràn bộ đệm */
            s_rx_index = 0;
            ESP32Comm_SendError("BUFFER_OVERFLOW");
        }
    }

    /* Kích hoạt lại ngắt nhận byte tiếp theo */
    if (s_esp32_huart != NULL)
    {
        HAL_UART_Receive_IT(s_esp32_huart, &s_rx_byte, 1);
    }
}

/* ============================================================
   MOTOR CONTROL (PWM RC ESC 50Hz)
   ============================================================ */

void Motor_ApplyPWM(int8_t left, int8_t right)
{
    /* Giới hạn dải -100 đến 100 */
    if (left < -100) left = -100;
    if (left > 100)  left = 100;
    if (right < -100) right = -100;
    if (right > 100)  right = 100;

    g_usv_state.motor_left = left;
    g_usv_state.motor_right = right;

    /*
     * TIM4 Prescaler 83 (Clock = 1MHz -> 1 tick = 1us), Period = 19999 (20ms = 50Hz)
     * RC ESC chuẩn:
     *   1000 us: Lùi tối đa (hoặc 0 ga với ESC 1 chiều)
     *   1500 us: Trung điểm dừng (Neutral)
     *   2000 us: Tiến tối đa
     */
    uint32_t pulse_left = (uint32_t)(1500 + ((int32_t)left * 5));
    uint32_t pulse_right = (uint32_t)(1500 + ((int32_t)right * 5));

    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pulse_left);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, pulse_right);
}

void Motor_Stop(void)
{
    Motor_ApplyPWM(0, 0);
}

/* ============================================================
   COMMAND PARSER
   ============================================================ */

static void parse_mode(char *frame)
{
    char *sep = strchr(frame, '|');
    if (!sep)
    {
        ESP32Comm_SendError("INVALID_PARAMETER");
        return;
    }

    char *mode_str = sep + 1;
    if (strcmp(mode_str, "MANUAL") == 0)
    {
        g_usv_state.mode = USV_MODE_MANUAL;
        g_usv_state.auto_running = false;
        Motor_Stop();
    }
    else if (strcmp(mode_str, "AUTO") == 0)
    {
        g_usv_state.mode = USV_MODE_AUTO;
    }
    else
    {
        ESP32Comm_SendError("INVALID_PARAMETER");
    }
}

static void parse_motor(char *frame)
{
    if (g_usv_state.mode != USV_MODE_MANUAL)
    {
        ESP32Comm_SendError("INVALID_STATE");
        return;
    }

    char *p1 = strchr(frame, '|');
    if (!p1)
    {
        ESP32Comm_SendError("INVALID_PARAMETER");
        return;
    }

    char *p2 = strchr(p1 + 1, '|');
    if (!p2)
    {
        ESP32Comm_SendError("INVALID_PARAMETER");
        return;
    }

    *p2 = '\0';
    int left = atoi(p1 + 1);
    int right = atoi(p2 + 1);

    if (left < -100 || left > 100 || right < -100 || right > 100)
    {
        ESP32Comm_SendError("INVALID_PARAMETER");
        return;
    }

    Motor_ApplyPWM((int8_t)left, (int8_t)right);
}

static void parse_waypoint(char *frame)
{
    char *p1 = strchr(frame, '|');
    if (!p1)
    {
        ESP32Comm_SendError("INVALID_PARAMETER");
        return;
    }

    char *p2 = strchr(p1 + 1, '|');
    if (!p2)
    {
        ESP32Comm_SendError("INVALID_PARAMETER");
        return;
    }

    *p2 = '\0';
    double lat = atof(p1 + 1);
    double lon = atof(p2 + 1);

    if (lat < -90.0 || lat > 90.0 || lon < -180.0 || lon > 180.0)
    {
        ESP32Comm_SendError("INVALID_PARAMETER");
        return;
    }

    g_usv_state.target_lat = lat;
    g_usv_state.target_lon = lon;
    g_usv_state.waypoint_valid = true;
}

static void parse_feed(char *frame)
{
    char *sep = strchr(frame, '|');
    if (!sep)
    {
        ESP32Comm_SendError("INVALID_PARAMETER");
        return;
    }

    int time_ms = atoi(sep + 1);
    if (time_ms <= 0 || time_ms > 10000)
    {
        ESP32Comm_SendError("INVALID_PARAMETER");
        return;
    }

    g_usv_state.feed_time_ms = (uint32_t)time_ms;
    g_usv_state.feed_active = true;
    g_usv_state.feed_start_tick = HAL_GetTick();

    if (g_usv_state.feed_percent >= 5)
    {
        g_usv_state.feed_percent -= 5;
    }
    else
    {
        g_usv_state.feed_percent = 0;
    }
}

/* ============================================================
   PERIODIC PROCESS (Call in main while(1))
   ============================================================ */

void ESP32Comm_Process(void)
{
    /* Kiểm tra và xử lý lệnh mới từ ESP32 */
    if (s_cmd_ready)
    {
        char cmd[RX_BUFFER_SIZE];
        strncpy(cmd, s_pending_cmd, RX_BUFFER_SIZE);
        cmd[RX_BUFFER_SIZE - 1] = '\0';
        s_cmd_ready = false;

        if (strcmp(cmd, "STOP") == 0)
        {
            Motor_Stop();
        }
        else if (strcmp(cmd, "AUTO_START") == 0)
        {
            if (!g_usv_state.gps_valid)
            {
                ESP32Comm_SendError("GPS_INVALID");
            }
            else if (!g_usv_state.waypoint_valid)
            {
                ESP32Comm_SendError("INVALID_STATE");
            }
            else
            {
                g_usv_state.mode = USV_MODE_AUTO;
                g_usv_state.auto_running = true;
            }
        }
        else if (strcmp(cmd, "AUTO_STOP") == 0)
        {
            g_usv_state.auto_running = false;
            g_usv_state.mode = USV_MODE_MANUAL;
            Motor_Stop();
        }
        else if (strncmp(cmd, "MODE|", 5) == 0)
        {
            parse_mode(cmd);
        }
        else if (strncmp(cmd, "MOTOR|", 6) == 0)
        {
            parse_motor(cmd);
        }
        else if (strncmp(cmd, "WAYPOINT|", 9) == 0)
        {
            parse_waypoint(cmd);
        }
        else if (strncmp(cmd, "FEED|", 5) == 0)
        {
            parse_feed(cmd);
        }
        else
        {
            ESP32Comm_SendError("UNKNOWN_COMMAND");
        }
    }

    /* Kiểm tra hết thời gian rải thức ăn */
    if (g_usv_state.feed_active)
    {
        if (HAL_GetTick() - g_usv_state.feed_start_tick >= g_usv_state.feed_time_ms)
        {
            g_usv_state.feed_active = false;
        }
    }
}

/* ============================================================
   TRANSMIT STATUS & ERROR TO ESP32
   ============================================================ */

void ESP32Comm_SendStatus(void)
{
    if (s_esp32_huart == NULL) return;

    char tx_buf[180];
    int len = snprintf(tx_buf, sizeof(tx_buf),
        "STATUS|%.6f|%.6f|%.1f|%.2f|%.2f|%.2f|%.1f|%d|%s|%d\n",
        g_usv_state.current_lat,
        g_usv_state.current_lon,
        g_usv_state.heading,
        g_usv_state.speed,
        g_usv_state.battery_volt,
        g_usv_state.current_amp,
        g_usv_state.water_temp,
        g_usv_state.feed_percent,
        (g_usv_state.mode == USV_MODE_MANUAL) ? "MANUAL" : "AUTO",
        g_usv_state.gps_valid ? 1 : 0
    );

    if (len > 0)
    {
        HAL_UART_Transmit(s_esp32_huart, (uint8_t *)tx_buf, (uint16_t)len, 100);
    }
}

void ESP32Comm_SendError(const char *err_code)
{
    if (s_esp32_huart == NULL || err_code == NULL) return;

    char tx_buf[64];
    int len = snprintf(tx_buf, sizeof(tx_buf), "ERROR|%s\n", err_code);
    if (len > 0)
    {
        HAL_UART_Transmit(s_esp32_huart, (uint8_t *)tx_buf, (uint16_t)len, 50);
    }
}
