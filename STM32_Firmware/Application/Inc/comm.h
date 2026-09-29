#ifndef COMM_H
#define COMM_H

#include "main.h"

typedef enum {
    MODE_MANUAL = 0,
    MODE_AUTO = 1
} BoatMode;

extern BoatMode current_mode;
extern float target_waypoint_lat;
extern float target_waypoint_lon;
extern uint8_t new_waypoint_flag;
extern int manual_left_speed;
extern int manual_right_speed;
extern uint8_t feed_request_flag;
extern uint32_t feed_duration_ms;

void Comm_Init(void);
void Comm_ParseCommand(char *cmd);
void Comm_SendTelemetry(float lat, float lon, float heading, float temp, float speed);
void Comm_CheckFailsafe(void);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);

#endif
