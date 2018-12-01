/*   
 *    Contains recipe instantiations for demos.
 */

#include "servo_recipes.h"

int servo1_positions[] = {6, 9, 11, 14, 17, 21};
int servo2_positions[] = {6, 9, 11, 14, 17, 21};


/******        Simple Recipes        ******/
unsigned char recipe_verify_moves[] = {
	MOV+0,
	MOV+1,
	MOV+2,
	MOV+3,
	MOV+4,
	MOV+5,
	RECIPE_END
	};
unsigned char recipe_verify_moves_reverse[] = {
	MOV+0,
	MOV+5,
	MOV+1,
	MOV+4,
	MOV+2,
	MOV+3,
	RECIPE_END
	};

unsigned char recipe1[] = {  
	LOOP+5,
	MOV+3, 
	MOV+5, 
	WAIT+10,
	END_LOOP,
	MOV+4,
	WAIT+3,
	MOV+1, 
	RECIPE_END };

unsigned char recipe2[] = { 
	LOOP+31,
	MOV+5, 
	MOV+2, 
	END_LOOP,
	RECIPE_END };

/******        RECIPE_END test        ******/
unsigned char recipe_continue[] = {
	MOV+5,
	WAIT+2,
	MOV+2,
	MOV+0,
	RECIPE_END,
	MOV+5,
	RECIPE_END };

/******        Demo recipe        ******/
unsigned char recipe_demo[] = {
	MOV+0,
	MOV+5,
	MOV+0,
	MOV+3,
	LOOP+0,
	MOV+1,
	MOV+4,
	END_LOOP,
	MOV+0,
	MOV+2,
	WAIT+0,
	MOV+3,
	WAIT+0,
	MOV+2,
	MOV+3,
	WAIT+31,
	WAIT+31,
	WAIT+31,
	MOV+4,
	RECIPE_END
};

/******        Nested loop error recipe        ******/
unsigned char recipe_error_loop[] = {
	MOV+5,
	WAIT+2,
	MOV+2,
	MOV+0,
	MOV+5,
	LOOP+3,
	LOOP+2,
	MOV+5,
	RECIPE_END };

/******        Command error recipe        ******/ 
unsigned char recipe_error_mov[] = {
	MOV+5,
	WAIT+2,
	MOV+2,
	MOV+0,
	MOV+9,
	MOV+5,
	RECIPE_END };
    
/******        Skip command recipe        ******/ 
unsigned char recipe_skip[] = {
	MOV+5,
	SKIP,
	MOV+0,
	MOV+3,
	RECIPE_END};
unsigned char recipe_skip_error[] = {
	MOV+2,
	MOV+3,
	SKIP,
	RECIPE_END};
    
/******        Jump command recipe        ******/ 
unsigned char recipe_jump[] = {
	MOV+3,
	WAIT+2,
	JUMP+2,
	MOV+0,
	MOV+5, // Should end up here
	WAIT+20,
	MOV+2,
	JUMP+2,
	MOV+5, // Should continue here
	MOV+0,
	RECIPE_END };
unsigned char recipe_jump_error[] = {
	MOV+3,
	WAIT+2,
	JUMP+2,
	MOV+0,
	MOV+5, // Should end up here
	WAIT+20,
	MOV+2,
	JUMP+4,// Should ERROR here
	MOV+5, 
	MOV+0,
	RECIPE_END };
