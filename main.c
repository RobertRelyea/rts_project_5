#include "stm32l476xx.h"
#include "Drivers/SysClock.h"
#include "Drivers/UART.h"
#include "Drivers/tim2.h"
#include "Drivers/LED.h"
#include "servo_states.h"

#include <stdlib.h>

events parseInput(char input)
{
	// Handle user input
	if (input == 'C' || input == 'c') // Continue
		return resume;
	else if (input == 'P' || input == 'p') // Pause
		return pause;
	else if (input == 'L' || input == 'l') // move_left
		return move_left;
	else if (input == 'R' || input == 'r') // move_right
		return move_right;
	else if (input == 'B' || input == 'b') // begin
		return begin;
    else if (input == 'X' || input == 'x') // Terminate command
        return terminate;
	return nop;
}

int main()
{
    // Initialize Devices
	System_Clock_Init();
	UART2_Init();
    timer2_pwm_init();
	timer2_start();
	LED_Init();
	initServos();
    
	// Place new command indicator
	newLine(USART2);
	putLine(USART2, ">");
	
	// Space to store user input chars
	uint8_t user_input;
    events user_events[2];
	uint8_t input_idx = 0;
	
    
	while(1)
	{
		// Track time required for UI, state processing, servo commands
		uint32_t start_time = timer2_count();
        
        
		// Check for user input
		if(USART_Received(USART2))
		{
			if (input_idx < 2) // Append to input command
            {
				user_input = getChar(USART2);
                user_events[input_idx] = parseInput(user_input);
                
                // Check for termination of current command
                // (And increment input_idx++)
                if (user_events[input_idx++] == terminate)
                {
                    // Reset input buffer index
                    input_idx = 0;
                    
                    // Place new command indicator
                    newLine(USART2);
                    putLine(USART2, ">");
                }
            }
			
			else // Input command fully constructed, wait for CR
			{
				// Check for carraige return
				if (getChar(USART2) == '\r')
				{
					// Determine events input by user
					processInput(user_events[0], user_events[1]);
				}
				// Reset input buffer index
				input_idx = 0;
				
				// Place new command indicator
				newLine(USART2);
				putLine(USART2, ">");
			}
		}
		
        // Run recipe state machine
		recipeStep();
		
        // Process state for LEDs
        switch (servo1.recipe.status)
		{
			case status_running:
				Green_LED_On();
                Red_LED_Off();
				break;
			case status_paused:
                Red_LED_Off();
                Green_LED_Off();
                break;
			case status_command_error:
                Red_LED_On();
                Green_LED_Off();
                break;
			case status_nested_error:
                Green_LED_On();
                Red_LED_On();
                break;
			case status_ended:
				Red_LED_Off();
                Green_LED_Off();
                break;
		}			
        
        // Determine time consumed
		uint32_t time_elapsed = get_timer2_elapsed(start_time);
		
        // Block for the duration of the 100ms loop period
		waitFor(100 - time_elapsed);
	} 
}

