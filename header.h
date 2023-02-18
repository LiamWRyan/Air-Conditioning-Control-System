#include <stdio.h>
#include <stdlib.h> // for random function

#include "systemstate.h"
#include "userbuffer.h"

/*
#include "driverlib/uart.h"
#include "driverlib/ioc.h"
#include "driverlib/prcm.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
*/

#define BAUD_RATE 11520
//#define T_INITIAL 0
//#define BUFFER_SIZE 4
#define CLOCK 48000000

#define HEATING_SUB_ON  3000000*0.6
#define HEATING_SUB_OFF 3000000*0.4
#define COOLING_SUB_ON  3000000*0.5
#define COOLING_SUB_OFF 3000000*0.5

#define HEATING_UNIT_ID 6
#define COOLING_UNIT_ID 7 



