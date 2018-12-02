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
/*
int post(void)
{
	// Start timer
	timer_start();
	
	
	uint32_t time = timer_count();
	
	// Store times for two pulses
	uint32_t pulse_one = 0;
	uint32_t pulse_two = 0;
	
	// Run for 100 milliseconds
	while( (timer_count() - time) < 100000)
	{
		// Check for timer event (rising edge seen on pa1)
		if (timer_event() & 0xF)
		{
			if (pulse_one == 0)
				pulse_one = timer_capture();
			else if (pulse_two == 0)
				pulse_two = timer_capture();
		}
	}
	
	timer_stop();

	// Ensure we have seen a complete pulse (two rising edges) within 100ms
	//if (events_captured > 2)
	if ((pulse_two - pulse_one < 100000) && (pulse_two != 0))
		return 1;
	return 0;
	
}*/

int getInt(void)
{
	char rxByte;
	
	in_buffer_index = 0;
	while((rxByte != '\r') && (in_buffer_index < BufferSize - 2))
	{
		rxByte = USART_Read(USART2); //Read the input and store it into rxByte
		if((rxByte != '\r') && (rxByte >= '0') && (rxByte <= '9'))
		{
			USART_Write(USART2, (uint8_t *)&rxByte, 1);
			inBuffer[in_buffer_index] = rxByte;
			in_buffer_index++;
		}
	}
	inBuffer[in_buffer_index] = '\0';
	
	return atoi((char *)inBuffer);
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
	
	// Begin Measurements
	sprintf((char *)buffer, "\n\rBeginning measurements...\r\n");
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

