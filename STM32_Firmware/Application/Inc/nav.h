#ifndef NAV_H
#define NAV_H

#include "main.h"

typedef struct {
    float kp;
    float ki;
    float kd;
    float prev_error;
    float integral;
} PID_Controller;

float calculate_distance(float lat1, float lon1, float lat2, float lon2);
float calculate_bearing(float lat1, float lon1, float lat2, float lon2);
float update_pid(PID_Controller *pid, float setpoint, float measured, float dt);
void auto_control_boat(float current_lat, float current_lon, float current_heading, float target_lat, float target_lon);

#endif
