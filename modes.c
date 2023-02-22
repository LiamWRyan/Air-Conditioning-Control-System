#include "modes.h"


void enter_active_mode(struct SystemState *ss)
{
    // set the system state to sleep
    set_system_state_active(ss);


    // Be sure the interrupt is clear to start
    TimerIntClear(GPT0_BASE,TIMER_TIMA_TIMEOUT);
    // set the timer to start after some delay
    TimerLoadSet(GPT0_BASE, TIMER_A, START_DELAY);
    // Enable the interrupt
    TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
    // Enable the timer
    TimerEnable(GPT0_BASE,TIMER_A);

}

void enter_sleep_mode(struct SystemState *ss)
{

    // set the system state to sleep
    set_system_state_sleep(ss);

    // set the heating and cooling states to off
    set_cooling_to_off(ss);
    set_heating_to_off(ss);

    
    TimerDisable(GPT0_BASE, TIMER_A);   // disable the timer
    TimerIntDisable(GPT0_BASE,TIMER_A); // disable the interrupt

    GPIO_clearDio(COOLING_UNIT_ID); // ensure green LED it is not on
    GPIO_clearDio(HEATING_UNIT_ID); // ensure red LED is not on.

}
