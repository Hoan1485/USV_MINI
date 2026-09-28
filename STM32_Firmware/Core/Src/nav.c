#include "nav.h"
#include <math.h>
#include "motor.h"

#define PI 3.14159265358979323846
#define EARTH_RADIUS 6371000.0 // Tính bằng mét

// Đổi từ độ sang radian
float deg_to_rad(float do_goc) {
    return do_goc * (PI / 180.0);
}

// Đổi từ radian sang độ
float rad_to_deg(float radian) {
    return radian * (180.0 / PI);
}

// Hàm tính khoảng cách giữa 2 tọa độ GPS (Công thức Haversine)
float calculate_distance(float lat1, float lon1, float lat2, float lon2) {
    float dLat = deg_to_rad(lat2 - lat1);
    float dLon = deg_to_rad(lon2 - lon1);
    float a = sin(dLat / 2) * sin(dLat / 2) +
              cos(deg_to_rad(lat1)) * cos(deg_to_rad(lat2)) *
              sin(dLon / 2) * sin(dLon / 2);
    float c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS * c;
}

// Hàm tính góc hướng đi từ điểm A đến điểm B (Bearing)
float calculate_bearing(float lat1, float lon1, float lat2, float lon2) {
    float dLon = deg_to_rad(lon2 - lon1);
    lat1 = deg_to_rad(lat1);
    lat2 = deg_to_rad(lat2);

    float y = sin(dLon) * cos(lat2);
    float x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);
    float angle = atan2(y, x);
    
    angle = rad_to_deg(angle);
    if (angle < 0) {
        angle += 360.0; // Đảm bảo góc luôn dương
    }
    return angle;
}

// Hàm tính toán độ bẻ lái để giữ hướng bằng thuật toán PID
float update_pid(PID_Controller *pid, float target, float current, float dt) {
    // Tính sai số giữa mục tiêu và hiện tại
    float error = target - current;
    
    // Xử lý góc bị lật (-180 đến 180) để tàu chọn hướng quay ngắn nhất
    if (error > 180.0f) error -= 360.0f;
    if (error < -180.0f) error += 360.0f;

    pid->integral += error * dt; // Tích phân
    float derivative = (error - pid->prev_error) / dt; // Đạo hàm
    
    // Công thức PID kinh điển
    float result = (pid->kp * error) + (pid->ki * pid->integral) + (pid->kd * derivative);
    pid->prev_error = error; // Lưu lại sai số cho lần sau
    
    return result;
}

extern PID_Controller heading_pid;

// Hàm chính điều khiển tàu tự động đi tới mục tiêu
void auto_control_boat(float current_lat, float current_lon, float current_heading, float target_lat, float target_lon) {
    // 1. Tính xem còn cách đích bao xa
    float distance = calculate_distance(current_lat, current_lon, target_lat, target_lon);
    
    if (distance < 2.0) {
        // Đã đến nơi (Sai số dưới 2 mét)
        motor_set_speed(1500, 1500); // Dừng lại
        return;
    }
    
    // 2. Tính xem cần quay mặt về hướng nào
    float target_heading = calculate_bearing(current_lat, current_lon, target_lat, target_lon);
    
    // 3. Tính toán lực bẻ lái cần thiết để quay mũi tàu đúng hướng
    float steering_force = update_pid(&heading_pid, target_heading, current_heading, 0.1); // Thời gian 1 chu kỳ là 0.1 giây (10Hz)
    
    // 4. Pha trộn lực bẻ lái vào tốc độ tiến tới
    int base_speed = 1650; // Tốc độ chạy thẳng (1500 là dừng, >1500 là tới)
    
    int left_speed = base_speed + (int)steering_force;
    int right_speed = base_speed - (int)steering_force;
    
    // 5. Giới hạn tốc độ trong mức an toàn của tín hiệu PWM (1100 đến 1900)
    if (left_speed > 1900) left_speed = 1900;
    if (left_speed < 1100) left_speed = 1100;
    if (right_speed > 1900) right_speed = 1900;
    if (right_speed < 1100) right_speed = 1100;
    
    // 6. Gửi tín hiệu xuống động cơ
    motor_set_speed(left_speed, right_speed);
}
