# USV_MINI - UART Communication Specification & Hardware Wiring

Tài liệu đặc tả giao thức truyền thông và sơ đồ kết nối phần cứng UART giữa **ESP32 (Web Dashboard / Gateway)** và **STM32F407 (Controller thật)** cũng như **ESP32 Giả lập (Simulator)**.

---

## 1. Sơ đồ kết nối phần cứng (Hardware Wiring)

### A. Kết nối ESP32 với STM32F407 (Mạch thật)
Cả **ESP32** và **STM32F407VET6** đều sử dụng mức logic **3.3V TTL**, do đó có thể kết nối trực tiếp các chân tín hiệu với nhau mà không cần mạch chuyển đổi mức logic (Logic Level Shifter).

> [!IMPORTANT]
> **BẮT BUỘC** phải nối chung mass (**GND**) giữa 2 board để ổn định điện thế tham chiếu.

```text
       ESP32 (Dashboard / Telemetry)              STM32F407VET6 (Core Controller)
    ┌─────────────────────────────────┐        ┌───────────────────────────────────┐
    │                                 │        │                                   │
    │  GPIO 17 (UART2 TX)  ───────────┼───────>│  PB11 (USART3 RX)                 │
    │                                 │        │                                   │
    │  GPIO 16 (UART2 RX)  <──────────┼────────┤  PB10 (USART3 TX)                 │
    │                                 │        │                                   │
    │  GND                 ───────────┼────────┤  GND (Chung Mass)                 │
    └─────────────────────────────────┘        └───────────────────────────────────┘
```

#### Bảng tra cứu chân kết nối (Pinout Table):
| ESP32 Pin | Hướng tín hiệu | STM32F407 Pin | Chức năng STM32 | Cấu hình UART |
| :--- | :---: | :--- | :--- | :--- |
| **GPIO 17** (TX2) | $\longrightarrow$ | **PB11** | USART3_RX | 115200 bps, 8N1 |
| **GPIO 16** (RX2) | $\longleftarrow$ | **PB10** | USART3_TX | 115200 bps, 8N1 |
| **GND** | $\longleftrightarrow$ | **GND** | Ground | Chung mass |

*(Lưu ý: Chân PA9/PA10 trên STM32 đã được dành riêng cho USART1 kết nối module GPS ATGM336H).*

---

### B. Kết nối khi dùng ESP32 Giả lập (Simulator - Board to Board)
Khi chưa gắn STM32 thật, có thể dùng 1 board ESP32 thứ 2 nạp code `STM32_Gialap.ino` để test toàn bộ giao diện Web Dashboard:

| ESP32 (Dashboard) | Hướng | ESP32 (Giả lập STM32) | Ghi chú |
| :--- | :---: | :--- | :--- |
| **GPIO 17** (TX2) | $\longrightarrow$ | **GPIO 16** (RX2) | Đấu chéo TX - RX |
| **GPIO 16** (RX2) | $\longleftarrow$ | **GPIO 17** (TX2) | Đấu chéo RX - TX |
| **GND** | $\longleftrightarrow$ | **GND** | Chung mass |

---

## 2. Đặc tả giao thức UART (Frame Overview)

Tất cả các gói tin đều sử dụng định dạng chuỗi ký tự ASCII, phân tách các trường bằng dấu gạch đứng **`|`** và kết thúc khung bằng ký tự xuống dòng **`\n`**.

| Hướng truyền | Loại Frame | Định dạng / Ví dụ (ASCII) | Ký tự kết thúc |
| :--- | :--- | :--- | :---: |
| **ESP32 $\rightarrow$ STM32** | **Lệnh điều khiển** | `MOTOR\|60\|-30\n` | `\n` |
| | **Dừng khẩn cấp** | `STOP\n` | `\n` |
| | **Bắt đầu tự hành**| `AUTO_START\n` | `\n` |
| | **Dừng tự hành** | `AUTO_STOP\n` | `\n` |
| | **Chuyển chế độ** | `MODE\|MANUAL\n` hoặc `MODE\|AUTO\n` | `\n` |
| | **Cài Waypoint** | `WAYPOINT\|21.028511\|105.804817\n` | `\n` |
| | **Rải thức ăn** | `FEED\|3000\n` *(thời gian ms)* | `\n` |
| **STM32 $\rightarrow$ ESP32** | **Telemetry (STATUS)** | `STATUS\|21.028511\|105.804817\|90.0\|1.20\|11.40\|2.50\|28.5\|95\|MANUAL\|1\n` | `\n` |
| | **Báo lỗi (ERROR)** | `ERROR\|INVALID_PARAMETER\n` | `\n` |

---

## 3. Chi tiết các trường dữ liệu

### A. Lệnh từ ESP32 gửi xuống STM32
- `STOP`: Dừng lập tức cả 2 động cơ (PWM = 0).
- `AUTO_START`: Chuyển sang chế độ chạy tự động theo Waypoint (yêu cầu GPS hợp lệ và đã có Waypoint).
- `AUTO_STOP`: Dừng tự hành, trả hệ thống về chế độ điều khiển thủ công (`MANUAL`).
- `MODE|<MANUAL|AUTO>`: Ép chế độ hoạt động.
- `MOTOR|<left>|<right>`: Công suất động cơ trái / phải (dải giá trị: `-100` đến `100`).
- `WAYPOINT|<lat>|<lon>`: Toạ độ điểm đích (Lat: `-90` đến `90`, Lon: `-180` đến `180`).
- `FEED|<timeMs>`: Kích hoạt cơ cấu rải thức ăn trong khoảng thời gian `timeMs` (từ `1` đến `10000` ms).

### B. Dữ liệu trạng thái từ STM32 gửi lên ESP32 (`STATUS`)
Cú pháp:
```text
STATUS|<lat>|<lon>|<heading>|<speed>|<battery>|<current>|<temp>|<feed>|<mode>|<gps>\n
```

| Trường (Field) | Kiểu dữ liệu | Đơn vị / Ý nghĩa |
| :--- | :--- | :--- |
| **lat** | float (6 số thập phân) | Vĩ độ hiện tại (Latitude) |
| **lon** | float (6 số thập phân) | Kinh độ hiện tại (Longitude) |
| **heading** | float (1 số thập phân) | Hướng la bàn từ trường (0° - 359.9°) |
| **speed** | float (2 số thập phân) | Tốc độ di chuyển ước tính (m/s) |
| **battery** | float (2 số thập phân) | Điện áp nguồn Pin (V) |
| **current** | float (2 số thập phân) | Dòng tiêu thụ tổng (A) |
| **temp** | float (1 số thập phân) | Nhiệt độ nước từ cảm biến DS18B20 (°C) |
| **feed** | int | Phần trăm thức ăn còn lại trong khoang (0 - 100%) |
| **mode** | string | Chế độ hiện tại: `MANUAL` hoặc `AUTO` |
| **gps** | int (0 hoặc 1) | Trạng thái GPS (1 = Fix hợp lệ, 0 = Mất tín hiệu) |

*Tần suất gửi khuyến nghị:* Gửi định kỳ **500ms / lần** (`STATUS_INTERVAL = 500`).

### C. Gói tin báo lỗi (`ERROR`)
Cú pháp: `ERROR|<MÃ_LỖI>\n`
- `GPS_INVALID`: Chưa bắt được tín hiệu GPS khi bấm kích hoạt tự hành.
- `INVALID_STATE`: Trạng thái không hợp lệ (ví dụ chưa cài Waypoint mà bật Auto).
- `INVALID_PARAMETER`: Tham số vượt ngưỡng cho phép (ví dụ tốc độ động cơ ngoài dải [-100, 100]).
- `BUFFER_OVERFLOW`: Bộ đệm nhận UART bị đầy (>200 bytes).
- `UNKNOWN_COMMAND`: Lệnh không xác định.

---

## 4. Hướng dẫn lập trình nhận dữ liệu trên STM32 (Thật)

Để STM32 nhận dữ liệu UART từ ESP32 mà không bị treo chương trình điều khiển:
1. **Khuyến nghị dùng DMA với sự kiện ngắt rảnh (Idle Line Interrupt):**
   ```c
   // Kích hoạt nhận dữ liệu UART3 bằng DMA với Idle detection
   HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rxDmaBuffer, RX_BUFFER_SIZE);
   ```
2. **Callback xử lý khi nhận xong 1 gói:**
   Trong hàm `HAL_UARTEx_RxEventCallback(...)`, kiểm tra ký tự cuối `\n` và phân tích lệnh tương tự bộ bóc tách chuỗi.
