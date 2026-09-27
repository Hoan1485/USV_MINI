#include "motor.h" // Goi thu vien motor
#include "tim.h"   // Thu vien dieu khien TIMER

// Hàm khởi tạo 2 động cơ
void motor_init(void)
{
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
}

// Hàm set tốc độ cho 2 động cơ trái phải
void motor_set_speed(int left, int right)
{
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, left);
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, right);
}
