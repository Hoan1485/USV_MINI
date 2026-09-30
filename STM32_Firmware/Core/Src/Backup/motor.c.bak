#include "motor.h" // Gọi thư viện chứa các khai báo cho động cơ (motor)
#include "tim.h"   // Gọi thư viện điều khiển TIMER (bộ định thời, dùng để tạo xung PWM điều khiển tốc độ)

// Hàm khởi tạo 2 động cơ
// Hàm này chỉ cần gọi 1 lần lúc bật máy để "khởi động" chức năng xuất xung PWM
void motor_init(void)
{
    // Bật kênh 1 của Timer 4 (tương ứng với động cơ bên trái)
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    
    // Bật kênh 2 của Timer 4 (tương ứng với động cơ bên phải)
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
}

// Hàm cài đặt tốc độ cho 2 động cơ (trái và phải)
// Thông thường tín hiệu PWM điều khiển ESC/Servo có giá trị từ 1000 đến 2000.
// 1500 là đứng im, <1500 là lùi, >1500 là tiến.
void motor_set_speed(int left, int right)
{
    // Ghi giá trị tốc độ vào thanh ghi điều khiển của Timer 4, Kênh 1 (Động cơ trái)
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, left);
    
    // Ghi giá trị tốc độ vào thanh ghi điều khiển của Timer 4, Kênh 2 (Động cơ phải)
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, right);
}
