#include "servo_states.h"

/******        CHANGE DEMO RECIPES HERE        ******/
// Variables for conveniently changing demo recipes
unsigned char *recipe_data1 = recipe_jump_error;
unsigned char *recipe_data2 = recipe_jump;


// Declare servo structs
servo_type servo1;
servo_type servo2;
recipe_type recipe_t1;
recipe_type recipe_t2;

// Code to start the move (adjust PWM) and start the timing delay based on the
// current position.
static servo_type startMove(servo_states new_state, servo_type servo, unsigned char position )
{
	servo.state = new_state;
	servo.recipe.move = (servo.position > position) ? 
		((servo.position - position) * 2 - 1) : ((position - servo.position) * 2 - 1);
	servo.position = position;
	setDuty(servo.channel, servo.positions[position]);
	return servo;
}

int recipeLength(recipe_type recipe)
{
	int len = 0;
	while(!((recipe.recipe[len] & 0xE0) == RECIPE_END))
		len++;
	
	return len + 1;
}


void initServos()
{
	// Set servo positions
	servo1.positions = servo1_positions;
	servo2.positions = servo2_positions;
	
	// Set current servo position
	servo1.position = 0;
	servo2.position = 0;
	
	// Set current servo status
	servo1.state = state_unknown;
    servo2.state = state_unknown;
	
	// Set servo channels
	servo1.channel = 2;
	servo2.channel = 3;
	
	// Initialize servo recipes
	recipe_t1.idx = 0;
	recipe_t1.move = 0;
	recipe_t1.wait = 0;
	recipe_t1.loop = 0;
	recipe_t1.loop_iter = 0;
	recipe_t1.status = status_paused;
	recipe_t1.recipe =  recipe_data1;
	recipe_t1.length = recipeLength(recipe_t1);
	servo1.recipe = recipe_t1;
	
	recipe_t2.idx = 0;
	recipe_t2.move = 0;
	recipe_t2.wait = 0;
	recipe_t2.loop = 0;
	recipe_t2.loop_iter = 0;
	recipe_t2.status = status_paused;
	recipe_t2.recipe = recipe_data2;
	recipe_t2.length = recipeLength(recipe_t2);
	servo2.recipe = recipe_t2;
	
	// Move servos to starting position (0)
	servo1 = startMove(state_moving, servo1, 0);
	servo2 = startMove(state_moving, servo2, 0);
}


void recipeStep()
{	
	servo1 = recipeStepHelper(servo1);
	servo2 = recipeStepHelper(servo2);
}

servo_type recipeStepHelper(servo_type servo)
{
	if (servo.recipe.status != status_running)
	{
		servo.state = state_at_position;
		return servo;
	}
	
	// Grab the current recipe step
	unsigned char step = servo.recipe.recipe[servo.recipe.idx];
	
	// Extract the command
	unsigned char command = (step & 0xE0); 
	// Extract the param
	unsigned char param = (step & 0x1F);
	
	if (param > 31)
	{
		servo.recipe.status = status_command_error;
		return servo;
	}
	
	switch(command)
	{
		case MOV:
			if(param > NUM_SERVO_POS - 1)
			{
				// Out of range error
				servo.recipe.status = status_command_error;
			}
			else if (servo.state != state_moving)
			{
				// Move servo
				servo = startMove(state_moving, servo, param);
			}
			else if (servo.recipe.move > 0) // Still moving...
				servo.recipe.move--;
			else
			{
				servo.state = state_at_position;
				servo.recipe.idx++;
			}
			break;
			
		case WAIT:
			if (servo.recipe.wait > 1) // WAIT has commenced
				servo.recipe.wait--;
			else if (servo.recipe.wait == 1) // WAIT will complete
			{
				servo.recipe.wait--;
				servo.recipe.idx++;
			}
			else // New WAIT command
				if (param == 0)
					servo.recipe.idx++;
				else
					servo.recipe.wait = param;
			break;
			
		case LOOP:
			// Check for nested loop
			if (servo.recipe.loop != 0)
			{
				// Nested loop error
				servo.recipe.status = status_nested_error;
			}
			else
			{
				// Set loop flag, loop block index and loop iter count
				servo.recipe.loop = servo.recipe.idx + 1;
				servo.recipe.loop_iter = param;
			}
			servo.recipe.idx++;
			break;
		
		case END_LOOP:
			if (servo.recipe.loop_iter > 0) // 
			{
				servo.recipe.loop_iter--;
				servo.recipe.idx = servo.recipe.loop;
			}
			if (servo.recipe.loop_iter <=0) // This will catch the end
			{	
				servo.recipe.loop = 0;
				servo.recipe.idx++;
			}
			break;
			
		case SKIP:
			// Ensure we do not skip over the end of the recipe
			if(servo.recipe.idx + 2 >= servo.recipe.length)
			{
				// Command error
				servo.recipe.status = status_command_error;
			}
			else
			{
				// Skip next instruction
				servo.recipe.idx += 2;
			}
			break;
			
		case JUMP:
			// Ensure we do not jump over the end of the recipe
			if(servo.recipe.idx + param >= servo.recipe.length || param == 0)
			{
				// Command error
				servo.recipe.status = status_command_error;
			}
			else
			{
				// Condition for jump: if the servo is at position 3 or greater, 
				// jump ahead by param steps in the recipe.
				if (servo.position > 2)
					servo.recipe.idx += param;
				else
					servo.recipe.idx++;
			}
			break;
			
		
		case RECIPE_END:
			servo.state = state_recipe_ended;
			servo.recipe.status = status_ended;
			break;	
	}
	return servo;
}

void processInput(events one_event, events two_event)
{
	servo1 = processEvent(one_event, servo1);
	servo2 = processEvent(two_event, servo2);
}

servo_type processEvent(events event, servo_type servo)
{
	if (event == begin)
	{
		servo.recipe.idx = 0;
		servo.recipe.loop = 0;
		servo.recipe.loop_iter = 0;
		servo.recipe.wait = 0;
		servo.recipe.status = status_running;
	}
	
	if (event == pause && servo.recipe.status != status_command_error && 
		  servo.recipe.status != status_nested_error &&
          servo.recipe.status != status_ended)
	{
		servo.recipe.status = status_paused;
	}
	
	if (event == resume && servo.recipe.status != status_command_error && 
		  servo.recipe.status != status_nested_error &&
          servo.recipe.status != status_ended)
	{
		servo.recipe.status = status_running;
	}
	
	// User servo position commands are valid only when paused
	if (servo.recipe.status == status_paused)
	{
		switch ( servo.state )
		{
			case state_at_position:		// servo is stationary at a known position
				// Left movement requested
				if ( event == move_left && servo.position < 5 )
					servo = startMove(state_moving, servo, servo.position + 1 );

				// Right movement requested
				if (event == move_right && servo.position > 0)
					servo = startMove(state_moving, servo, servo.position - 1);
				break;
			case state_unknown :
				break;
			case state_recipe_ended :
				break;
			case state_moving:
				break;
		}
	}	
	return servo;
}
