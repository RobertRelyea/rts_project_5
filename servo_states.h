#ifndef SERVO_STATES_H
#define SERVO_STATES_H

#include "Drivers/tim2.h"
#include "Drivers/UART.h"
#include "servo_recipes.h"

// This is a good way to define the status of the display.
// This should be in a header (.h) file.
typedef enum recipe_status_t 
{
	status_running,
	status_paused,
	status_ended,
	status_command_error,
	status_nested_error 
}recipe_status;

// This is a good way to define the state of a servo motor.
// This should be in a header (.h) file.
typedef enum servo_states_t
{
	state_at_position,		// use a separate integer to record the current position (0 through 5) for each servo.
	state_unknown,
	state_moving,
	state_recipe_ended
}servo_states;

// This is a good way to define the event transitions between states.
// This also should be in a header (.h) file.
// More events are needed.
typedef enum events_t
{
	move_left,
	move_right,
	pause,
	resume,
	begin,
	recipe_ended,
	nop,
    terminate
}events;

// Struct for managing recipe information
// r_idx 				->  Index of current recipe step
// r_move				->  Number of steps required to complete current MOV
// r_wait 			->  Number of steps required to complete current WAIT
// r_loop				->  Starting step of current loop
// r_loop_iter	->  Number of iterations left in current LOOP
// r_status     ->  Current status of recipe
typedef struct recipe_t
{
	int idx;
	int move;
	int wait;
	int loop;
	int loop_iter;
	int length;
	recipe_status status;
	unsigned char *recipe;
}recipe_type;

// Struct for managining servo and recipe status
// servo_positions -> Measured positions for servo
// servo_position  -> Current position of servo
// state 					 -> Current state of servo
// recipe					 -> Recipe struct associated with servo
typedef struct servo_t
{
	int *positions;
	int position;
	servo_states state;
	recipe_type recipe;
	int channel;
}servo_type;

extern servo_type servo1;
extern servo_type servo2;

void recipeStep(void);
servo_type recipeStepHelper(servo_type servo);
void processInput(events one_event, events two_event);
servo_type processEvent(events one_event, servo_type servo);
void initServos(void);

#endif
