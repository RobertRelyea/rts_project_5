// L. Kiser Feb. 8, 2017
#include "input_pa2.h"

// Turn on the peripheral clock for GPIOA
// and set PA2 to be in Alternate Function 
// mode for use with TIM5_CH3.
void init_pa2_timer( void )
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN ;		// enable clock for A group of GPIO
	GPIOA->MODER &= ~0x30; // clear out bits 0 and 1 for PA2
	// Enable alternate function mode (binary 10) for PA2
	GPIOA->MODER |= 0x20;								    
	GPIOA->AFR[0] &= ~0xF00;		// Clear PA2 field in AFRL
	GPIOA->AFR[0] |= 0x200;		// Sets AF2 as the alternate function for PA2
														  // TIM5_CH3
}
