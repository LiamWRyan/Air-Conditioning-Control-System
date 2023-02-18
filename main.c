#include "header.h"

struct SystemState ss; // globally defined struct, repersenting system state.
struct Buffer   	b; // globally defined struct, a buffer of user input.


void null_buffer(struct Buffer *b)
{
	for (int i = 0; i < BUFFER_SIZE; i++)
	{
    	b->buffer[i] = '\0';
	}
}


int get_count(struct Buffer *b)
{
	return b->count;
}


int interpret_cmd(struct Buffer *b)
{
	if (b->buffer[0] == 'F')
	{
    	return -1;
	}
	else
	{
    	int first_digit_ascii  =  b->buffer[0];
    	int second_digit_ascii =  b->buffer[1];


    	int first_digit_adjusted_to_int  = first_digit_ascii-48;
    	int second_digit_adjusted_to_int = second_digit_ascii-48;


    	// convert first digit to 10s
    	int temp = first_digit_adjusted_to_int*10;
    	// add second digit to 1s
    	int temperature_value = temp + second_digit_adjusted_to_int;


    	return temperature_value;
	}


}


int is_valid_cmd(struct Buffer *b)
{

	// 1 indicates a valid command

	// check for "FF  ", enter sleep mode.
	if (b->buffer[0] == 'F' && b->buffer[1] == 'F' && b->buffer[2] == ' ' && b->buffer[3] == ' ')
	{
    	return 1;
	}

	// RANGE OF ALLOWED VALUES: 20-30

	// check that the 0th element equals 2 or 3
	if (b->buffer[0]  == '2' || b->buffer[0] == '3')
	{
    	// if the 0th element was 2, the 1st element can be 0-9
    	if (b->buffer[0] == '2')
    	{
        	// check the 1st element is between 0 and 9, and the 2nd and 3rd are spaces.
        	if (b->buffer[1] <= '9' && b->buffer[1] >= '0' && b->buffer[2] == ' ' && b->buffer[3] == ' ')
        	{
            	return 1;
        	}
    	}
    	else // the 0th element was a 3, the 1st element must be a 0.
    	{
        	// check the 1st element is 0, and the 2nd and 3rd are spaces.
        	if (b->buffer[1] == '0' && b->buffer[2] == ' ' && b->buffer[3] == ' ')
        	{
            	return 1;
        	}


    	}
	}

	return 0;
}

int is_full(struct Buffer *b)
{
	return b->count == BUFFER_SIZE;
}


void insert(struct Buffer *b, char data) {


	// Check null
	if (b == NULL)
	{
    	return;
	}
	// if buffer is full, ignore input data.
	if (is_full(b))
	{
    	//printf("Error: buffer is full\n");
    	return;
	}
	b->buffer[b->index] = data;
	// if rb -> in == buffer_size, then, buffer_size%buffer_size = 0, index is reset
	// e.g., 3%10 = 3, 10%10 = 0.
	b->index = (b->index + 1) % BUFFER_SIZE;


	b->count++; //increment counter
}



/* Function: init_b()
 *
 * Parameters:
 *  	struct RingerBuffer *: the address of a ringbuffer.
 *
 * Purpose: given the address of a ring buffer struct, initialize its
 *      	attributes.
 *
 * Return: void
 */
void init_b(struct Buffer *b)
{
	b->buffer[0] = '\0';            	// null buffer
	b->buffer[1] = '\0';            	// 	^
	b->buffer[2] = '\0';            	// 	^
	b->buffer[3] = '\0';            	// 	^
	b->index  = 0;                  	// init to 0
	b->count  = 0;                  	// init to 0
}


// ==========================================================================

int get_goal_temp(struct SystemState *ss)
{
	return ss->t_goal;
}

void set_goal_temp(struct SystemState *ss, int new_temp)
{
	ss->t_goal = new_temp;
}

void set_current_temp(struct SystemState *ss, int new_temp, int increase_temp)
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


// ==========================================================================


void off_sub_period(int heating)
{

	
	if (heating)
	{
		set_current_temp(&ss, rand()%1, heating);

		GPIO_clearDio(HEATING_UNIT_ID);

	}
	else // assume cooling if not heating
	{
		set_current_temp(&ss, rand()%1, heating); // heating will be zero if this case is reached

		GPIO_clearDio(COOLING_UNIT_ID);

	}

	// after one of the above cases has executed, the temperature value will have 
	// been updated, and we can now get the current temp and the goal temp.
	char * temperature_copmarison = get_current_and_goal_temps(&ss);

	// regardless of whether heating or cooling, at the end of the off period we want to send to the user
	// a comparison of the temperatures.
	for (int i = 0; i < 0; i++)
	{
		char ch = temperature_copmarison[i];
		UARTCharPut(UART0_BASE, (uint8_t) ch);
	}

	/* IM NOT SURE IF THIS IS NEEDED HERE OR IN THE INTERRUPT_FN
	TimerLoadSet(GPT0_BASE, TIMER_A, HEATING_SUB_OFF);
	TimerIntClear(GPT0_BASE, TIMER_TIMA_TIMEOUT);
	// Enable the interrupt
	TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
	// Enable the timer
	TimerEnable(GPT0_BASE,TIMER_A);	
	*/

}

void on_sub_period(int heating)
{

	if (heating)
	{
		UARTCharPut(UART0_BASE, (uint8_t) 'H');
		GPIO_setDio(HEATING_UNIT_ID); // red LED
	}
	else // if you are not heating, you are cooling
	{
		UARTCharPut(UART0_BASE, (uint8_t) 'C');
		GPIO_setDio(COOLING_UNIT_ID); // green LED
	}

	/* IM NOT SURE IF THIS IS NEEDED HERE OR IN THE INTERRUPT_FN
	TimerLoadSet(GPT0_BASE, TIMER_A, HEATING_SUB_ON);
	TimerIntClear(GPT0_BASE, TIMER_TIMA_TIMEOUT);
	// Enable the interrupt
	TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
	// Enable the timer
	TimerEnable(GPT0_BASE,TIMER_A);	
	*/

}

/* Function: interrupt_fn()
 *
 * Parameters: void
 *
 * Purpose: 
 *
 * Return: void
 */
void interrupt_fn()
{
	TimerIntClear(GPT0_BASE, TIMER_TIMA_TIMEOUT);

	
	if (!get_system_state(&ss)) // if the system is in sleep mode, we can exit.
	{
		return;
	}

	if (should_heat)
	{
		on_sub_period(1); // one indicates heating
		TimerLoadSet(GPT0_BASE, TIMER_A, HEATING_SUB_ON);

		off_sub_period(1); // one indicates heating
		TimerLoadSet(GPT0_BASE, TIMER_A, HEATING_SUB_OFF);

		if (get_goal_temp(&ss) == get_current_temp(&ss))
		{
			
			// Temps are equal, update system state to indicate heating is not needed.
			set_heating_to_off(&ss);

			// set system state to sleep mode
			set_system_state_sleep(&ss);
			// the temperatures are equal, thus we are nethier cooling or heating.
			set_cooling_to_off(&ss);
			set_heating_to_off(&ss);

			// enter sleep mode
			

		}
	
	}
	else // assume if heating is not needed, that cooling is needed.
	{

		on_sub_period(0); // zero indicates cooling
		TimerLoadSet(GPT0_BASE, TIMER_A, COOLING_SUB_ON);

		off_sub_period(0); // zero indicates cooling
		TimerLoadSet(GPT0_BASE, TIMER_A, COOLING_SUB_OFF);

		if (get_goal_temp(&ss) == get_current_temp(&ss))
		{
			
			// Temps are equal, update system state to indicate heating is not needed.
			set_heating_to_off(&ss);

			// set system state to sleep mode
			set_system_state_sleep(&ss);
			// the temperatures are equal, thus we are nethier cooling or heating.
			set_cooling_to_off(&ss);
			set_heating_to_off(&ss);
			
			// enter sleep mode
			
		}

	}


	// set the timer again
	//TimerLoadSet(GPT0_BASE, TIMER_A, 0xE4E1C0);  

	// clear the interrupt
	TimerIntClear(GPT0_BASE, TIMER_TIMA_TIMEOUT);
	// Enable the interrupt
	TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
	// Enable the timer
	TimerEnable(GPT0_BASE,TIMER_A);

}


/* Function: UART_Interrupt_Handler()
 *
 * Parameters: void
 *
 * Purpose:
 *
 *
 * Return: void
 */
void UART_Interrupt_Handler()
{
	// if the interrupt status isn't receive intereupt, return
	if(UARTIntStatus(UART0_BASE,  true) !=	UART_INT_RX) return;


	UARTIntClear(UART0_BASE,UART_INT_RX|UART_INT_TX);


	// UARTCharsAvail needs to be called before calling UARTCharGetNonBlocking()
	// UARTCharGetNonBlocking will return the char read from the specied port, or a -1 if no char is present.
	while(UARTCharsAvail(UART0_BASE))
	{
    	int32_t ch =   UARTCharGetNonBlocking(UART0_BASE) & 0x000000FF;

    	insert(&b, (char) ch);

    	// the buffer contains 4 chars, we can check for a command
    	if (get_count(&b) == 4)
    	{
        	if (is_valid_cmd(&b)) // will return 0 for invalid cmd, and 1 for valid cmd
        	{

				int cmd = interpret_cmd(&b);
            	// INTERPRET THE CMD, if -1 then SLEEP COMMAND ISSUED
            	if (cmd == -1) // returns -1 for sleep mode, otherwise the desired positive temp.
            	{
                	// CHECK THE SYSTEM STATE, the system state will be 1 if it is active
                	if (get_system_state(&ss))
                	{
                    	
						// wait till the end of the current heating or cooling period...
						// need a way to determine when this period is over and to stop  (IMPLEMET A CHECK FOR THIS)

                    	// put system in sleep mode


                    	// update system state to sleep
						set_system_state_sleep(&ss);

                	}
                	else
                	{
                    	// system is already sleeping, do nothing
                	}


            	}
            	// TEMPERAUTE COMMAND ISSUED
            	else
            	{

					// if the system is already active, we know its heating or cooling, we will need to wait till the end of 
					// heating / cooling period before updating the goal temperature.
					if (get_system_state(&ss)) // if get_system_state() returns 1, the system is currently heating or cooling.
					{
						// wait for it to finish current heating or cooling period (IMPLEMET A CHECK FOR THIS)

						// Update the goal temp, cmd is the return from interpert_cmd (the temp or -1 for sleep)
						set_goal_temp(&ss, cmd);

					}
					else // the system is in sleep mode, we should set it to active mode.
					{
						set_system_state_active(&ss); // set the system state to active mode.
					}


					// if the current temp is equal to the goal temp, then we can enter sleep mode.
                	if (get_current_temp(&ss) == get_goal_temp(&ss))
                	{
                    	
						// Update the system state to sleep mode.
						set_system_state_sleep(&ss);

						// PUT SYSTEM IN SLEEP MODE

                    	return;
                	}
                	else if (get_current_temp(&ss) > get_goal_temp(&ss)) 
                	{
                    	// set system state to cooling
						set_cooling_to_on(&ss);

                	}
					else //if (get_current_temp(&ss) < get_goal_temp(&ss))
					{
						// set system state to heating
						set_heating_to_on(&ss);

					}
            	}
        	}
        	else
        	{
            	// flush the buffer
            	null_buffer(&b);
        	}
    	}


	}
}


/* Function: setup_timer_with_interrupt()
 *
 * Parameters: void
 *
 * Purpose: Configure the timer and enable timer interrupts. In this function we enable the timer peripheral in both run and 
 *			sleep mode, following this we divide the CPUClock input to the timer by 16 (48*10^6) / 16 = 3*10^6 = 3MHz = 
 *			3000000 = 0x00002DC6C. This function uses timer A and B which will use 32 bits. 
 *
 * Return: void
 */
void setup_timer_with_interrupt()
{
	// enable peripheral power domain
	PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
	while(PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON);


	// Power on the TIMER0 peripheral
	PRCMPeripheralRunEnable(PRCM_PERIPH_TIMER0);
	// Enable TIMER0 to continue counting while the MCU sleeps
	PRCMPeripheralSleepEnable(PRCM_PERIPH_TIMER0);
	PRCMLoadSet();
	while ( !PRCMLoadGet() );

	// This line will result that the input clock to the Timer = CPUClock / 16
	// which is 48MHz/16 = 3MHz = 3000000 = 0x00002DC6C0
	PRCMGPTimerClockDivisionSet(PRCM_CLOCK_DIV_16);
	PRCMLoadSet();
	while ( !PRCMLoadGet() );

	// Configure the timer in one shot mode.
	// DOCS; technical refernce manual (pg1200), and TimerConfigure in API driver lib.
	TimerConfigure(GPT0_BASE, TIMER_CFG_ONE_SHOT);
	// Configures the timer load value.
	// NOTE: This initally configures the timer in 32 bit mode...
	TimerLoadSet(GPT0_BASE, TIMER_A, 0);


	// Be sure the interrupt is clear to start
	TimerIntClear(GPT0_BASE,TIMER_TIMA_TIMEOUT);
	// Assign the interrupt handler
	TimerIntRegister(GPT0_BASE, TIMER_A, interrupt_fn);
	// Enable the interrupt
	TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
	// Enable the timer
	TimerEnable(GPT0_BASE,TIMER_A);

}

/* Function: setup_UART_Interrupt()
 *
 * Parameters: void
 *
 * Purpose: configure the UART for baud 115200 and 8-N-1, with an interrupt.
 *
 * Return: void
 */
void setup_UART_Interrupt()
{
	// 1. Step 1 in page 1460
	PRCMPowerDomainOn(PRCM_DOMAIN_SERIAL);
	while(PRCMPowerDomainStatus(PRCM_DOMAIN_SERIAL) != PRCM_DOMAIN_POWER_ON);
	//
	PRCMPeripheralRunEnable(PRCM_PERIPH_UART0);
	PRCMPeripheralSleepEnable(PRCM_PERIPH_UART0);
	PRCMLoadSet();
	while ( !PRCMLoadGet() );

	// Enable UART Pins
	IOCPinTypeUart(UART0_BASE , IOID_2, IOID_3, IOID_19, IOID_18);

	// Disable UART
	UARTDisable(UART0_BASE);

	//UART Configuration
	UARTConfigSetExpClk(UART0_BASE,CLOCK,BAUD_RATE, UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE);
	//
	UARTHwFlowControlDisable(UART0_BASE);

	// Set FIFO Thresholds
	UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8); // might need to edit

	// UART interrupt handler assignment
	UARTIntRegister(UART0_BASE, UART_Interrupt_Handler);

	// Enable Interrupts
	UARTIntEnable(UART0_BASE, UART_INT_RX);

	//Last step
	UARTEnable(UART0_BASE);

}

void setup_GPIO_LEDs()
{
	// Turn on the GPIO power domain (will likely already be on, but just in case...)
	PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
	while(PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON);

	// Enable GPIO peripheral
	PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);
	PRCMPeripheralSleepEnable(PRCM_PERIPH_GPIO);

	// enable output for the green LED
	GPIO_setOutputEnableDio(IOID_7, GPIO_OUTPUT_ENABLE);
	// enable output for the red LED
	GPIO_setOutputEnableDio(IOID_6, GPIO_OUTPUT_ENABLE);

	// synchorinze the load settings (propigate conifigurations)
	PRCMLoadSet();
	while(! PRCMLoadGet());

}

int main()
{


	init_ss(&ss);
	init_b(&b);

	setup_UART_Interrupt();
	setup_timer_with_interrupt();
	setup_GPIO_LEDs();

	// sleep mode
	PRCMSleep();

	return 0;
}

















