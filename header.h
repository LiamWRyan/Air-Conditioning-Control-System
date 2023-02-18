#include <stdio.h>
#include <stdlib.h> // for random function

/*
#include "driverlib/uart.h"
#include "driverlib/ioc.h"
#include "driverlib/prcm.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
*/

#define BAUD_RATE 11520
#define T_INITIAL 0
#define BUFFER_SIZE 4
#define CLOCK 48000000

#define HEATING_SUB_ON  3000000*0.6
#define HEATING_SUB_OFF 3000000*0.4
#define COOLING_SUB_ON  3000000*0.5
#define COOLING_SUB_OFF 3000000*0.5

#define HEATING_UNIT_ID 6
#define COOLING_UNIT_ID 7 




/* When this struct is initialized its state will be 0 for sleep.
 *
 *
 * 
 */
struct SystemState
{
    int state;      // 0 for sleep, 1 for active
    int heating;    // 1 for heating, 0 for not
    int cooling;    // 1 for cooling, 0 for not
    int t_curr;     // the current temp, initially zero
    int t_goal;     // the goal temp, initially zero
};


/*
 *
 *
 * 
 */
struct Buffer {
	char buffer[BUFFER_SIZE]; // buffer
	int index;                // current index
	int count;                // keeping track of total elements 
};
