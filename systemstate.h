/*  Program: systemstate.h
 *
 *  Purpose: Defines the SystemState strucutre and its attributes. Additionally, establishes
 *           the interface for the functions which perform actions on this strucutre found in
 *           systemstate.c.
 * 
 *  Author: Liam Ryan
 */


/*INCLUDES & DEFINES*/
#include <stdlib.h>
#define T_INITIAL 0
/*INCLUDES & DEFINES END*/

/* When this struct is initialized its state will be 0 for sleep.
 * heating will be set to 0 for not heating
 * cooling will be set to 0 for not cooling
 * t_curr and t_goal will be set to T_INITIAL (0)
 */
struct SystemState
{
    int state;      // 0 for sleep, 1 for active
    int heating;    // 1 for heating, 0 for not
    int cooling;    // 1 for cooling, 0 for not
    int t_curr;     // the current temp, initially zero
    int t_goal;     // the goal temp, initially zero
};

/* INTERFACES */
int get_goal_temp(struct SystemState *ss);

void set_goal_temp(struct SystemState *ss, int new_temp);

void update_current_temp(struct SystemState *ss, int new_temp, int increase_temp);

int get_current_temp(struct SystemState *ss);

int get_system_state(struct SystemState *ss);

void set_heating_to_off(struct SystemState *ss);

void set_heating_to_on(struct SystemState *ss);

void set_cooling_to_on(struct SystemState *ss);

void set_cooling_to_off(struct SystemState *ss);

int should_heat(struct SystemState *ss);

int should_cool(struct SystemState *ss);

void set_system_state_sleep(struct SystemState *ss);

void set_system_state_active(struct SystemState *ss);

char* get_current_and_goal_temps(struct SystemState *ss);

void init_ss(struct SystemState *ss);

