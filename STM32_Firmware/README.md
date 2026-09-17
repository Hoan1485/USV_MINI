# STM32 Firmware Workspace

Thư mục này dành cho toàn bộ mã nguồn điều khiển phần cứng chính (động cơ, cảm biến, thuật toán PID).

Vì bạn sử dụng **STM32CubeIDE / STM32CubeMX**, khi bạn tạo project bằng phần mềm này, nó sẽ tự động sinh ra các thư mục như `Core/`, `Drivers/STM32...`. 

Để code của chúng ta không bị lộn xộn với code tự sinh của CubeMX, tôi đã chuẩn bị sẵn hai thư mục con:
- `App/`: Nơi chứa logic chính của tàu (ví dụ: `navigation.c`, `pid.c`).
- `Drivers/Custom/`: Nơi chứa thư viện do bạn tự viết hoặc tải về cho cảm biến bên ngoài (GPS, IMU).

Bạn sẽ chỉ code tập trung vào 2 thư mục con này và gọi chúng trong `while(1)` của file `main.c` (do CubeMX sinh ra).
