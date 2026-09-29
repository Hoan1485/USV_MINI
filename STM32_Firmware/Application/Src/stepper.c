#include "../../Application/Inc/stepper.h"

// Trình tự kích xung half-step cho động cơ 28BYJ-48
const uint8_t step_sequence[8] = {
    0b0001, // 1
    0b0011, // 3
    0b0010, // 2
    0b0110, // 6
    0b0100, // 4
    0b1100, // 12
    0b1000, // 8
    0b1001  // 9
};

// Hàm kích một bước
void step_motor(uint8_t step) {
  HAL_GPIO_WritePin(INT1_GPIO_Port, INT1_Pin,
                    (step_sequence[step] & 0x01) ? GPIO_PIN_SET
                                                    : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(INT2_GPIO_Port, INT2_Pin,
                    (step_sequence[step] & 0x02) ? GPIO_PIN_SET
                                                    : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(INT3_GPIO_Port, INT3_Pin,
                    (step_sequence[step] & 0x04) ? GPIO_PIN_SET
                                                    : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(INT4_GPIO_Port, INT4_Pin,
                    (step_sequence[step] & 0x08) ? GPIO_PIN_SET
                                                    : GPIO_PIN_RESET);
}

// --- CÁC BIẾN CHO STATE MACHINE (NON-BLOCKING) ---
static uint32_t stepper_target_steps = 0;
static uint32_t stepper_current_step = 0;
static uint8_t stepper_direction = 1;
static uint32_t last_step_time = 0;

// Hàm thiết lập mục tiêu góc xoay
void rotate_stepper(float angle, uint8_t direction) {
  // Thay vì chạy vòng lặp bắt main chờ, ta chỉ lưu lại mục tiêu số bước
  stepper_target_steps = (uint32_t)((angle / 360.0f) * 4076.0f);
  stepper_current_step = 0;
  stepper_direction = direction;
}

// Hàm thực hiện hành động rải mồi theo thời gian
void drop_bait(uint32_t duration_ms) {
  // Đặt mục tiêu số bước theo thời gian (1 bước tốn 2ms)
  stepper_target_steps = duration_ms / 2;
  stepper_current_step = 0;
  stepper_direction = 1; // Thuận
}

// Hàm này BẮT BUỘC phải được gọi liên tục bên trong vòng lặp while(1) của main.c
void stepper_update(void) {
  // Nếu vẫn chưa đi hết số bước mục tiêu
  if (stepper_current_step < stepper_target_steps) {
    // Đủ 2 mili-giây thì nhích thêm 1 bước
    if (HAL_GetTick() - last_step_time >= 2) {
      last_step_time = HAL_GetTick();
      
      if (stepper_direction) { // Quay thuận
        step_motor(stepper_current_step % 8);
      } else { // Quay ngược
        step_motor(7 - (stepper_current_step % 8));
      }
      
      stepper_current_step++;
      
      // Nếu vừa đạt tới bước cuối cùng thì tắt điện động cơ cho đỡ nóng
      if (stepper_current_step >= stepper_target_steps) {
        HAL_GPIO_WritePin(INT1_GPIO_Port, INT1_Pin | INT2_Pin | INT3_Pin | INT4_Pin,
                          GPIO_PIN_RESET);
      }
    }
  }
}
