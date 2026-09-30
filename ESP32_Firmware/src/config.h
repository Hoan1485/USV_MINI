#ifndef CONFIG_H
#define CONFIG_H

// ==========================================
// 1. CẤU HÌNH WIFI ACCESS POINT (Trạm phát WiFi)
// ==========================================
// Tên và mật khẩu WiFi do ESP32 phát ra để điện thoại kết nối vào
#define AP_SSID "USV_MINIdemo"
#define AP_PASSWORD "12345678"

// ==========================================
// 2. CẤU HÌNH UART (Giao tiếp với chip STM32)
// ==========================================
// ESP32 và STM32 sẽ "nói chuyện" với nhau qua 2 dây tín hiệu là TX (gửi) và RX
// (nhận). Ở đây dùng Hardware Serial 2 (Cổng kết nối số 2) của ESP32.
#define STM32_RX_PIN 16 // Chân RX của ESP32 (nối với TX của STM32)
#define STM32_TX_PIN 17 // Chân TX của ESP32 (nối với RX của STM32)

// Tốc độ truyền dữ liệu (Baudrate). Phải cài đặt giống hệt nhau ở cả ESP32 và
// STM32 thì 2 chip mới hiểu nhau!
#define UART_BAUDRATE 115200

#endif // CONFIG_H
