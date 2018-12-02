#ifndef TIM5_H
#define TIM5_H

#include "stm32l476xx.h"
#include "input_pa2.h"

// Sets up the TIM2 registers for Channel 2 on PA1
void timer5_init(void);

void timer5_start(void);
void timer5_stop(void);
uint32_t timer5_count(void);
uint32_t timer5_capture(void);
uint32_t timer5_event(void);

// Sets the PWM duty cycle for the specified timer channel
void setDuty(int channel, int duty_cycle);

// Return the difference between the current timer count
// and the provided start_time parameter.
uint32_t get_timer5_elapsed(uint32_t start_time);

#endif
