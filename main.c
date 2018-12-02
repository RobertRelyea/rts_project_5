#include "stm32l476xx.h"
#include "Drivers/SysClock.h"
#include "Drivers/UART.h"
#include "Drivers/tim2.h"
#include "Drivers/tim5.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

char RxComByte = 0;
uint8_t inBuffer[BufferSize];
uint8_t buffer[BufferSize];
int buffer_index=0;
int in_buffer_index = 0;

// Power On Self Test
int post(void)
{
	// Start echo timer
	timer5_start();
	
	// Clear any events
	if (timer5_event() & 0xF)
		timer5_capture();
	
	timer2_start();

	uint32_t time = timer5_count();
	
	// Store times for two pulses
	uint32_t pulse_one = 0;
	uint32_t pulse_two = 0;
	
	// Run for 1000 microseconds
	while( (timer5_count() - time) < 1000)
	{
		// Check for timer event (rising edge seen on pa1)
		if (timer5_event() & 0xF)
		{
			if (pulse_one == 0)
			{
				pulse_one = timer5_capture();
			}
			else if (pulse_two == 0)
				pulse_two = timer5_capture();
		}
	}
	
	timer2_stop();
	timer5_stop();
	

	
	// Ensure we have seen a complete pulse (two rising edges) at least 300us
	if ((pulse_two - pulse_one > 300) && (pulse_two != 0))
		return (pulse_two - pulse_one);
	return 0;
	
}


int main(void)
{
  // Initialize Devices
	System_Clock_Init();
	UART2_Init();	
  timer2_pwm_init();
	timer5_init();
	

	

	
	timer2_start();
	timer5_start();

	setDuty(2, 1);	
			// Begin Measurements (For some reason, this causes the measurements to get way off)
	//sprintf((char *)buffer, "\n\rBeginning measurements...\r\n");
	//USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));

	int readings = 0;
	uint32_t measurement = 0;
	uint32_t count = 0;
	uint32_t prev_count = 0;
	while(1)
	{
		// First Measurement
		// Wait for timer_event
		while((timer5_event() & 0xF) == 0)
		{}
//		sprintf((char *)buffer, "Event\r\n");
//		USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
			
		// Read current TIM2 channel 2 counter value
		prev_count = timer5_capture();
			
		// Wait for timer_event
		while((timer5_event() & 0xF) == 0)
		{}
		// Read current TIM2 channel 2 counter value
		count = timer5_capture();
			
		measurement = count - prev_count;
		readings++;
		sprintf((char *)buffer, "measurement: %dmm\r\n", (measurement*10) / 58);
		USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
	}
	sprintf((char *)buffer, "count: %d\r\n", count);
	USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
	
	sprintf((char *)buffer, "prev_count: %d\r\n", prev_count);
	USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
	
	

}

