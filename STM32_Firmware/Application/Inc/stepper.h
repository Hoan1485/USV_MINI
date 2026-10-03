#ifndef STEPPER_H
#define STEPPER_H

#include "main.h"

#define STEPPER_IN1_Pin 2
#define STEPPER_IN2_Pin 3
#define STEPPER_IN3_Pin 4
#define STEPPER_IN4_Pin 5

void step_motor(uint8_t step);
void rotate_stepper(float angle, uint8_t direction);
void drop_bait(uint32_t duration_ms);
void stepper_update(void);
void stepper_stop(void);

#endif
