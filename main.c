#include "stm32l476xx.h"
#include "Drivers/SysClock.h"
#include "Drivers/UART.h"
#include "Drivers/tim2.h"
#include "Drivers/tim5.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define READING_NUM (100)

char RxComByte = 0;
uint8_t inBuffer[BufferSize];
uint8_t buffer[BufferSize];
int buffer_index=0;
int in_buffer_index = 0;

// Power On Self Test
int post(void)
{
	
	// Clear any events
	if (timer5_event() & 0xF)
		timer5_capture();
	
	// Send a trigger pulse
	setDuty(2, 1);	
	while(timer2_count() > TIM2->CCR3);
	setDuty(2, 0);
	
	// Keep track of time elapsed
	uint32_t time = timer5_count();
	
	// Store times for two pulses
	uint32_t pulse_one = 0;
	uint32_t pulse_two = 0;
	
	// Run for 1000 microseconds
	while((timer5_count() - time) < 5000)
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

	
	// Wait for the falling edge of the echo signal
	if (pulse_one != 0 && pulse_two == 0)
	{	
		while((timer5_event() & 0xF) == 0);
		pulse_two = timer5_capture();
	}
	
	int measurement = pulse_two - pulse_one;
	
	return measurement;
	
}

uint32_t measure_ultrasonic(void)
{
	uint32_t count = 0;
	uint32_t prev_count = 0;

	
	// Wait for timer_event
	while((timer5_event() & 0xF) == 0)
	{}
		
	// Read current TIM2 channel 2 counter value
	prev_count = timer5_capture();
		
	// Wait for timer_event
	while((timer5_event() & 0xF) == 0)
	{}
	// Read current TIM2 channel 2 counter value
	count = timer5_capture();
		
	// Return distance in mm
	return ((count - prev_count)*10) / 58;
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

	// Run POST routine
	int post_value = 0;
	while(post_value > 1000 || post_value < 300)
	{
		sprintf((char *)buffer, "\r\nStarting POST...\r\n");
  	USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
		post_value = post();
		if (post_value > 1000 || post_value < 300)
		{
			char* instruction = (post_value > 1000) ? "far" : "close";
			sprintf((char *)buffer, "POST failed with val: %d, the object is too %s.\n\r", 
							 post_value, instruction);
			USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
			sprintf((char *)buffer, "Retry? (Y/n)\n\r");
			USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
			
			char reply = getChar(USART2);
			if(reply != 'y' && reply != 'Y')
			{
				post_value = 0;
				break;
			}
		}
	}
	
	// Exit program if post failed
	if(post_value == 0)
		return 0;
	
	//Begin Measurements
	sprintf((char *)buffer, "Beginning measurements...\r\n");
	USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
	
	
	
	setDuty(2, 1);
	while(timer2_count() > TIM2->CCR3);
	
	int reading = 0;
	uint32_t measurements[READING_NUM];
	uint32_t measurement = 0;

	// Display first Measurement
	measurement = measure_ultrasonic();
	// Check if first measurement is valid
	if(measurement < 500 && measurement > 50)
	{
		sprintf((char *)buffer, "First measurement: %d mm\r\n", measurement);
		USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
	}
	else
	{
		sprintf((char *)buffer, "First measurement: *** mm\r\n");
		USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
	}

	// Collect READING_NUM measurements
	while(reading <= READING_NUM)
	{
		
		if(USART_Received(USART2))
		{
			putLine(USART2, "Measurements interrupted\n\r");
			break;
		}
		measurement = measure_ultrasonic();

		// Check if measurement is invalid
		if (measurement < 50 || measurement > 500)
			continue;
		
		// Valid measurement, add to measurments array
		measurements[reading] = measurement;
		reading++;
	}
	
	reading = (reading > READING_NUM) ? READING_NUM : reading;
	
	// Stop trigger signal and clocks
	setDuty(2, 0);
	timer2_stop();
	timer5_stop();

	uint32_t max_measurement = 50;
	uint32_t min_measurement = 500;
	
	int i = 0;
	// Print out measurements
	for(; i < reading; ++i)
	{
		sprintf((char *)buffer, "%d,%d\r\n", i + 1, measurements[i]);
		USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
		
		if(measurements[i] > max_measurement)
			max_measurement = measurements[i];
		if(measurements[i] < min_measurement)
			min_measurement = measurements[i];
	}	
	
	sprintf((char *)buffer, "Maximum distance measured: %d\r\n", max_measurement);
	USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
	sprintf((char *)buffer, "Minimum distance measured: %d\r\n", min_measurement);
	USART_Write(USART2,(uint8_t *)buffer, strlen((char *)buffer));
}
