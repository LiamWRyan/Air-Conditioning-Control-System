

#ifndef HEADER_H_
#define HEADER_H_

#include <stdio.h>
#include <stdlib.h> // for random function

#include "systemstate.h"
#include "userbuffer.h"
#include "modes.h"


#include "inc/hw_memmap.h"
#include "inc/hw_wdt.h"
#include "inc/hw_gpio.h"
#include "driverlib/uart.h"
#include "driverlib/ioc.h"
#include "driverlib/prcm.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/sys_ctrl.h"
#include "driverlib/osc.h"


#define BAUD_RATE 115200
//#define T_INITIAL 0
//#define BUFFER_SIZE 4
#define CLOCK 48000000

#define START_DELAY     ((3000000/256)*0.1)
#define HEATING_SUB_ON  ((3000000/256)*0.6)
#define HEATING_SUB_OFF ((3000000/256)*0.4)
#define COOLING_SUB_ON  ((3000000/256)*0.5)
#define COOLING_SUB_OFF ((3000000/256)*0.5)

#define HEATING_UNIT_ID 6
#define COOLING_UNIT_ID 7

void setup_timer_with_interrupt();

void set_system_state_active(struct SystemState *ss);

// set the system state to sleep
void set_system_state_sleep(struct SystemState *ss);

void interrupt_fn();

    // set the heating and cooling states to off
void set_cooling_to_off(struct SystemState *ss);
void set_heating_to_off(struct SystemState *ss);




#endif /* HEADER_H_ */
