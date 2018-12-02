#include "tim5.h"

void timer5_init()
{
	init_pa2_timer();

	// Enable TIM5 in the APB1 clock enable register 1
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN; 
	
	TIM5->PSC = 80;    // Set prescaler value for TIM5
	TIM5->EGR |= TIM_EGR_UG; 	  // Trigger an update event for timer 5
	
	TIM5->CCER &= ~(TIM_CCER_CC3E); // Disable channel 3 output
	TIM5->CCER &= ~(0xA00); // Clear CC3NP and CC3P
	TIM5->CCER |= 0xA00; // Set CC3NP and CC3P to rising edge trigger
	
	// Set up CCMRx
	TIM5->CCMR2 &= ~(0xFF);
	TIM5->CCMR2 |= 0x01; // Set CC3 channel as input, IC3 mapped on TI3
	TIM5->CCER |= TIM_CCER_CC3E; // Turn on output enable for capture input
}

void timer5_start()
{
	TIM5->CR1 |= 0x1;
}

void timer5_stop()
{
	TIM5->CR1 &= ~(0x01);
}

uint32_t timer5_count()
{
	return TIM5->CNT; // Timer 2
}

uint32_t timer5_capture()
{
	return TIM5->CCR3; // Timer 2 Channel 2
}

uint32_t timer5_event()
{
	return (TIM5->SR & 0x8);
}

uint32_t get_timer5_elapsed(uint32_t start_time)
{
	// Get current timer count
	uint32_t current_time = timer5_count();
	// Check if the timer has wrapped around
	if (current_time < start_time)
		current_time += 200;

	// Return time elapsed between start and current times.
	return current_time - start_time;
}
