#ifndef TIM2_H
#define TIM2_H

#include "stm32l476xx.h"
#include "input_pa1.h"

// Sets up the TIM2 registers for Channel 2 on PA1
void timer2_init(void);
void timer2_pwm_init(void);

void timer2_start(void);
void timer2_stop(void);
uint32_t timer2_count(void);
uint32_t timer2_capture(void);
uint32_t timer2_event(void);

// Sets the PWM duty cycle for the specified timer channel
void setDuty(int channel, int duty_cycle);

// Return the difference between the current timer count
// and the provided start_time parameter.
uint32_t get_timer2_elapsed(uint32_t start_time);

// Blocking wait for a specified time in milliseconds
void waitFor(uint32_t ms);

#endif
