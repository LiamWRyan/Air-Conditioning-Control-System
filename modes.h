#include <stdlib.h>
#include "driverlib/timer.h"
#include "systemstate.h"
#include "driverlib/gpio.h"

#include "header.h"

#ifndef MODES_H_
#define MODES_H_


void enter_active_mode(struct SystemState *ss);

void enter_sleep_mode(struct SystemState *ss);

#endif /* MODES_H_ */
