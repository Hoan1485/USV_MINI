/**
 ******************************************************************************
 * @file    esp32_comm.h
 * @brief   Header file for UART communication between STM32 and ESP32
 ******************************************************************************
 */

#ifndef __ESP32_COMM_H
#define __ESP32_COMM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

/* ============================================================
   USV STATE DEFINITIONS
   ============================================================ */

typedef enum {
    USV_MODE_MANUAL = 0,
    USV_MODE_AUTO   = 1
} USV_Mode_t;

typedef struct {
    /* Động cơ (-100 đến 100 %) */
    int8_t motor_left;
    int8_t motor_right;

    /* Chế độ vận hành */
    USV_Mode_t mode;
    bool auto_running;

    /* Toạ độ mục tiêu (Waypoint) */
    double target_lat;
    double target_lon;
    bool waypoint_valid;

    /* Cơ cấu rải thức ăn */
    uint32_t feed_time_ms;
    bool feed_active;
    uint32_t feed_start_tick;
    int8_t feed_percent;

    /* Dữ liệu Telemetry cảm biến gửi lên ESP32 */
    double current_lat;
    double current_lon;
    float heading;
    float speed;
    float battery_volt;
    float current_amp;
    float water_temp;
    bool gps_valid;
} USV_State_t;

extern USV_State_t g_usv_state;

/* ============================================================
   FUNCTION PROTOTYPES
   ============================================================ */

/**
 * @brief Khởi tạo giao tiếp UART với ESP32
 * @param huart Con trỏ tới UART_HandleTypeDef (thường là huart3)
 */
void ESP32Comm_Init(UART_HandleTypeDef *huart);

/**
 * @brief Callback gọi từ HAL_UART_RxCpltCallback khi có ngắt nhận byte trên USART3
 * @param huart Con trỏ tới UART_HandleTypeDef kích hoạt ngắt
 */
void ESP32Comm_RxCpltCallback(UART_HandleTypeDef *huart);

/**
 * @brief Xử lý byte nhận được trong ngắt UART Rx
 * @param c Ký tự nhận được từ USART3
 */
void ESP32Comm_RxCallback(char c);

/**
 * @brief Tác vụ kiểm tra và xử lý lệnh từ ESP32 (gọi trong while(1))
 */
void ESP32Comm_Process(void);

/**
 * @brief Định dạng và gửi gói STATUS lên ESP32 định kỳ
 */
void ESP32Comm_SendStatus(void);

/**
 * @brief Gửi thông báo lỗi lên ESP32
 * @param err_code Mã lỗi (ví dụ: "GPS_INVALID", "UNKNOWN_COMMAND", ...)
 */
void ESP32Comm_SendError(const char *err_code);

/**
 * @brief Áp dụng tốc độ động cơ xuống phần cứng (TIM4 PWM ESC)
 * @param left Tốc độ động cơ trái (-100 đến 100)
 * @param right Tốc độ động cơ phải (-100 đến 100)
 */
void Motor_ApplyPWM(int8_t left, int8_t right);

/**
 * @brief Dừng toàn bộ động cơ khẩn cấp
 */
void Motor_Stop(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP32_COMM_H */
