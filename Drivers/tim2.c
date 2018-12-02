#include "tim2.h"




void timer2_pwm_init()
{
    
    /******        GPIO SETUP        ******/
    
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	GPIOA-> MODER &= ~(0x03 << (2*1));  // Clearing out MODER 0 and 1 bit for PA1 
	GPIOA-> MODER |= 0x02 << (2*1);     // Selecting AF mode for PA1
	
	GPIOA->AFR[0] &= ~(0x0F << (4*1));  // Clearing AFR for timer 2 PA1
	GPIOA->AFR[0] |= (0x01 << (4*1));   // Setting AF1 for PA1
	
    /******        TIM2 SETUP        ******/
    
	// Enable TIM2 in the APB1 clock enable register 1
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;   
    
	// Set prescaler value for TIM2
	TIM2->PSC = 8000;
	TIM2->EGR |= TIM_EGR_UG;
    
	// Clear output enable for capture input channel 2
	TIM2->CCER &= ~(TIM_CCER_CC2E);     
	
  // Select PWM as mode 1 for PA1 CH2
	TIM2->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;   
	TIM2->CCMR1 &= ~(TIM_CCMR1_OC2M_0);
	
	// Enable the corresponding preload register by setting the 
  // OC2PE bit in the TIM2_CCMR1 register
	TIM2->CCMR1 |= TIM_CCMR1_OC2PE;  
	
    // auto-reload preload register
	TIM2->CR1 |= TIM_CR1_ARPE;       
    
	// Turn on output enable for capture input channel 2
	TIM2->CCER |= TIM_CCER_CC2E;    
	
	TIM2->CCER &= ~(TIM_CCER_CC2P);  //set to active high
	
  // Upcounting from 0 to 999 (period of 1000 * clock period)
	// 100 ms -> 10 hz
	TIM2->ARR = 999;                 
  // Set UG bit in EGR register for force update
	TIM2->EGR |= TIM_EGR_UG;         
}

void setDuty(int channel, int duty_cycle)
{ 
	switch(channel)
	{
		case 1: // Channel 1
			TIM2->CCR1 = duty_cycle;
			break;
		case 2: // Channel 2
			TIM2->CCR2 = duty_cycle;
			break;
		case 3: // Channel 3
			TIM2->CCR3 = duty_cycle;
			break;
		case 4: // Channel 4
			TIM2->CCR4 = duty_cycle;
			break;
	}		
}

void timer2_start()
{
	TIM2->CR1 |= 0x1;
}

void timer2_stop()
{
	TIM2->CR1 &= ~(0x01);
}

uint32_t timer2_count()
{
	return TIM2->CNT; // Timer 2
}

uint32_t timer2_capture()
{
	return TIM2->CCR2; // Timer 2 Channel 2
}

uint32_t timer2_event()
{
	return (TIM2->SR & 0x4);
}

uint32_t get_timer2_elapsed(uint32_t start_time)
{
	// Get current timer count
	uint32_t current_time = timer2_count();
	// Check if the timer has wrapped around
	if (current_time < start_time)
		current_time += 200;

	// Return time elapsed between start and current times.
	return current_time - start_time;
}

void wait()
{
	uint32_t start_time = timer2_count();
	while(get_timer2_elapsed(start_time) < 100){}
}

void waitFor(uint32_t ms)
{
	for(int i = 0; i < ms / 10; ++i)
		wait();
}
