# ESP32 Firmware Workspace

Thư mục này dành cho ESP32, làm nhiệm vụ phụ trợ giao tiếp (Communication Node):
- Đọc dữ liệu từ STM32 qua UART
- Kết nối Wi-Fi
- Gửi/Nhận dữ liệu MQTT với Dashboard

Mọi logic phức tạp sẽ do STM32 đảm nhận. ESP32 chỉ đóng vai trò như một "cái cầu nối". Mọi code xử lý chính sẽ nằm trong thư mục `src/`.
