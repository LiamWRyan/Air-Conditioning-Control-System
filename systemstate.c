#include "systemstate.h"


int get_goal_temp(struct SystemState *ss)
{
	return ss->t_goal;
}

void set_goal_temp(struct SystemState *ss, int new_temp)
{
	ss->t_goal = new_temp;
}

void update_current_temp(struct SystemState *ss, int new_temp, int increase_temp)
{
	if (increase_temp) // will = 1 when heating is 1
	{
		ss->t_curr = ss->t_curr + new_temp;
	}
	else // assume if we arent increasing the temp, we are decreasing it.
	{
		ss->t_curr = ss->t_curr - new_temp;
	}
}

int get_current_temp(struct SystemState *ss)
{
	return ss->t_curr;
}

int get_system_state(struct SystemState *ss)
{
	return ss->state;
}

void set_heating_to_off(struct SystemState *ss)
{
	ss->heating = 0;
}

void set_heating_to_on(struct SystemState *ss)
{
	ss->heating = 1;
}

void set_cooling_to_on(struct SystemState *ss)
{
	ss->cooling = 1;
}

void set_cooling_to_off(struct SystemState *ss)
{
	ss->cooling = 0;
}

int should_heat(struct SystemState *ss)
{
	return ss->heating;
}

int should_cool(struct SystemState *ss)
{
	return ss->heating;
}

void set_system_state_sleep(struct SystemState *ss)
{
	ss->state = 0; // set state to 0 for sleep mode.
}

void set_system_state_active(struct SystemState *ss)
{
	ss->state = 1; // set the state to 1 for active mode.
}

char* get_current_and_goal_temps(struct SystemState *ss)
{

	static char charArray[9];

	// get the first and second digit of the current temperature
    int first_digit_current_temp  = ss->t_curr / 10;
    int second_digit_current_temp = ss->t_curr - (first_digit_current_temp*10);

	// by adding 48 (ascii 0) to the digits and casting to a char,
    // we will effectively convert to the character representation of the digit
    char char_one_ct =  (char)(first_digit_current_temp + 48);
    char char_two_ct =  (char)(first_digit_current_temp + 48);

	// get the first and second digit of the goal temp temperature
    int first_digit_goal_temp  = ss->t_goal / 10;
    int second_digit_goal_temp = ss->t_goal - (first_digit_goal_temp*10);

	char char_one_gt =  (char)(first_digit_goal_temp + 48);
    char char_two_gt =  (char)(second_digit_goal_temp + 48);

	charArray[0] = '(';
	charArray[1] = char_one_ct;
	charArray[2] = char_two_ct;
	charArray[3] = ',';
	charArray[4] = char_one_gt;
	charArray[5] = char_two_gt;
	charArray[6] = ')';
	charArray[7] = '\r';
	charArray[8] = '\n';

	return charArray;

}

void init_ss(struct SystemState *ss)
{
	ss->state   	= 0;        	// Indicates sleep mode
	ss->heating 	= 0;        	// off
	ss->cooling 	= 0;        	// off
	ss->t_curr  	= T_INITIAL;	// initial temp
	ss->t_goal  	= T_INITIAL;	// goal temp
}