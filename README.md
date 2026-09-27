# USV Mini (Unmanned Surface Vehicle)

Dự án **USV Mini** là một hệ thống tàu không người lái mini được thiết kế để thu thập dữ liệu thủy văn (nhiệt độ nước, tọa độ GPS) và thực hiện các nhiệm vụ tự động hóa như di chuyển theo quỹ đạo định trước hoặc rải thức ăn. 

Hệ thống được chia làm 2 khối xử lý chính:
1. **STM32 (Core Controller)**: Chịu trách nhiệm đọc cảm biến (GPS, La bàn, IMU, Nhiệt độ), tính toán thuật toán dẫn đường tự động (PID Navigation) và điều khiển động cơ.
2. **ESP32 (Communication & Dashboard)**: Hoạt động như một trạm phát WiFi (Access Point), cung cấp giao diện Web trực quan (Dashboard) cho phép người dùng giám sát từ xa và điều khiển tàu bằng điện thoại/máy tính mà không cần Internet.

---

## 🌟 Chức năng chính

- **Chế độ điều khiển bằng tay (Manual)**: Điều khiển hai động cơ trái/phải thông qua giao diện Web.
- **Chế độ tự hành (Auto Navigation)**: Tự động di chuyển bám theo góc hướng (Heading) sử dụng bộ điều khiển PID kết hợp giữa GPS và La bàn điện tử.
- **Giám sát dữ liệu theo thời gian thực**: Xem trực tiếp các thông số tọa độ (Lat/Lon), vận tốc, góc hướng, nhiệt độ nước, điện áp pin và dòng điện tiêu thụ.
- **Cơ cấu nhả mồi/thức ăn**: Điều khiển động cơ bước (Stepper) để rải thức ăn trong một khoảng thời gian nhất định (`FEED`).
- **Giao diện Web nhúng**: Không cần cài App, chỉ cần kết nối WiFi của ESP32 và mở trình duyệt web.

---

## 🔌 Sơ đồ đấu nối chân (Pinout & Wiring)

### 1. Giao tiếp giữa STM32 và ESP32 (UART)
Sử dụng cổng USART3 trên STM32 và Hardware Serial 2 trên ESP32. Tốc độ Baudrate: **115200**.
| STM32 (USART3) | ESP32 (Serial 2) | Chức năng |
| :--- | :--- | :--- |
| TX | RX (Pin 16) | Truyền dữ liệu trạng thái từ STM32 lên ESP32 |
| RX | TX (Pin 17) | Nhận lệnh điều khiển từ ESP32 xuống STM32 |
| GND | GND | Nối chung mass (Bắt buộc) |

### 2. Các cảm biến trên mạch STM32
| Modun / Cảm biến | Chuẩn Giao Tiếp | Chân kết nối tham khảo trên STM32 | Chức năng |
| :--- | :--- | :--- | :--- |
| **GPS (ATGM336H)** | UART (USART1) | TX1 / RX1 | Lấy tọa độ kinh độ, vĩ độ và vận tốc |
| **IMU (MPU6050)** | I2C (I2C1) | SCL1 / SDA1 | Đo gia tốc và độ nghiêng của tàu |
| **Compass (QMC5883L)** | I2C (I2C1) | SCL1 / SDA1 | Đo từ trường, xác định góc hướng (Heading) |
| **DS18B20** | 1-Wire (GPIO) | 1 Chân GPIO bất kỳ | Đo nhiệt độ nước (chuẩn chống nước) |

### 3. Động cơ và Cơ cấu chấp hành (STM32)
| Cơ cấu | Cổng điều khiển | Chức năng |
| :--- | :--- | :--- |
| **Động cơ Trái (Left Motor)** | PWM (TIM) | Điều khiển chân vịt trái |
| **Động cơ Phải (Right Motor)** | PWM (TIM) | Điều khiển chân vịt phải |
| **Cơ cấu thả thức ăn** | GPIO (Stepper) | Điều khiển động cơ bước nhả mồi |

*(Lưu ý: Bạn có thể thay đổi chi tiết cấu hình chân PWM/GPIO cụ thể trong file `.ioc` của CubeMX tùy theo cách cắm thực tế của phần cứng).*

---

## 🚀 Hướng dẫn sử dụng

### Bước 1: Khởi động hệ thống
- Cấp nguồn cho hệ thống. Đợi khoảng vài giây để STM32 và ESP32 khởi động và cấu hình các cảm biến.
- Đảm bảo GPS đã bắt được vệ tinh (đèn nhấp nháy hoặc biến `gps` báo VALID).

### Bước 2: Kết nối với Web Dashboard
1. Dùng điện thoại/máy tính dò tìm mạng WiFi do tàu phát ra.
   - **Tên WiFi (SSID)**: `USV_MINI`
   - **Mật khẩu**: `12345678`
2. Mở trình duyệt Web (Chrome, Safari...) và truy cập vào địa chỉ IP mặc định của ESP32: 
   - 👉 **http://192.168.4.1**
3. Màn hình Dashboard sẽ hiện lên hiển thị toàn bộ thông số của tàu.

### Bước 3: Điều khiển tàu
- **Mode MANUAL**: Bấm các phím `+` / `-` ở mục Motor Control để tăng giảm tốc độ chân vịt, hoặc bấm `STOP` để dừng khẩn cấp.
- **Mode AUTO**: Bấm nút `AUTO`, sau đó nhấn `AUTO START`. Tàu sẽ tự động di chuyển theo thuật toán trong file `nav.c`.
- **Feed**: Bấm `FEED 500 ms` để kích hoạt motor xả thức ăn.

---

## 🛠 Cấu trúc mã nguồn

- `STM32_Firmware/`: Chứa toàn bộ source code của bộ điều khiển trung tâm (C/C++ viết bằng STM32CubeIDE).
  - `Core/Src/main.c`: Vòng lặp chính, đọc cảm biến và gửi/nhận chuỗi UART.
  - `Core/Src/nav.c`: Thuật toán bám quỹ đạo (Navigation).
  - `Core/Src/motor.c`, `stepper.c`: Xử lý tín hiệu động cơ.
- `ESP32_Firmware/`: Code của trạm phát WiFi và Web Dashboard (viết bằng PlatformIO / Arduino framework).
  - `src/main.cpp`: Chứa code tạo Access Point, xử lý giao thức HTTP, định nghĩa giao diện HTML/CSS, và giao tiếp UART.
  - `src/config.h`: Cấu hình tên WiFi, mật khẩu và chân UART.
