#include "header.h"

struct SystemState ss; // globally defined struct, repersenting system state.
struct Buffer   	b; // globally defined struct, a buffer of user input.


// ==========================================================================


void off_sub_period(int heating)
{

	if (heating)
	{
		// might complain about implicit dec
		update_current_temp(&ss, rand()%RAND_MAX, heating);
		//GPIO_clearDio(COOLING_UNIT_ID); 			// in case it is on, turn it off
		GPIO_clearDio(HEATING_UNIT_ID);

	}
	else // assume cooling if not heating
	{
		update_current_temp(&ss, rand()%1, heating);  // heating will be zero if this case is reached
		//GPIO_clearDio(HEATING_UNIT_ID);			   // in case it is on, turn it off
		GPIO_clearDio(COOLING_UNIT_ID);
	}

	// After one of the above cases has executed, the temperature value will have 
	// been updated, and we can now get the current temp and the goal temp.
	char * temperature_copmarison = get_current_and_goal_temps(&ss);

	// regardless of whether were are heating or cooling, during the off sub period we want to send to the user
	// a comparison of the temperatures (Tcurrent,Tgoal).
	for (int i = 0; i < 0; i++)
	{
		char ch = temperature_copmarison[i];
		UARTCharPut(UART0_BASE, (uint8_t) ch);
	}

}

void on_sub_period(int heating)
{

	if (heating)
	{
		UARTCharPut(UART0_BASE, (uint8_t) 'H');
		GPIO_setDio(HEATING_UNIT_ID); 	// red LED
		GPIO_clearDio(COOLING_UNIT_ID); // ensure green LED it is not on
	}
	else // if not heating, assume cooling
	{
		UARTCharPut(UART0_BASE, (uint8_t) 'C');
		GPIO_setDio(COOLING_UNIT_ID); 	// green LED
		GPIO_clearDio(HEATING_UNIT_ID); // ensure red LED is not on.
	}

}

void enter_active_mode()
{
	// set the system state to sleep
	set_system_state_active(&ss);

	setup_timer_with_interrupt();
	
}

void enter_sleep_mode()
{

	// set the system state to sleep
	set_system_state_sleep(&ss);

	// set the heating and cooling states to off
	set_cooling_to_off(&ss);
	set_heating_to_off(&ss);

	// disable the timer
	TimerDisable(GPT0_BASE, TIMER_A);
	GPIO_clearDio(COOLING_UNIT_ID); // ensure green LED it is not on
	GPIO_clearDio(HEATING_UNIT_ID); // ensure red LED is not on.

}

/* Function: interrupt_fn()
 *
 * Parameters: void
 *
 * Purpose: This function is utilized by the timer interrupt handler
 *
 * Return: void
 */
void interrupt_fn()
{
	TimerIntClear(GPT0_BASE, TIMER_TIMA_TIMEOUT);

	
	if (!get_system_state(&ss)) // if the system is in sleep mode, exit.
	{
		return;
	}

	if (should_heat)
	{
		// USE UART to send data ('H') to user, and turns on the red LED
		on_sub_period(1); // one indicates heating
		TimerLoadSet(GPT0_BASE, TIMER_A, HEATING_SUB_ON);  // 600ms
		
		// enable the timer and interrupt
		TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT); 
		TimerEnable(GPT0_BASE,TIMER_A);

		off_sub_period(1); // one indicates heating
		TimerLoadSet(GPT0_BASE, TIMER_A, HEATING_SUB_OFF); // 400ms

		if (get_goal_temp(&ss) == get_current_temp(&ss))
		{
			// will disable the timer, set the cooling and heating status to 0 (off), and
			// set the system state to sleep.
			enter_sleep_mode();
			return;
		}

		// enable the timer and interrupt
		TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT); 
		TimerEnable(GPT0_BASE,TIMER_A);
	
	}
	else // assume if heating is not needed, that cooling is needed.
	{
		// USE UART to send data ('C') to user, and turns on the red LED
		on_sub_period(0); // zero indicates cooling
		TimerLoadSet(GPT0_BASE, TIMER_A, COOLING_SUB_ON);

		// enable the timer and interrupt
		TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT); 
		TimerEnable(GPT0_BASE,TIMER_A);

		off_sub_period(0); // zero indicates cooling
		TimerLoadSet(GPT0_BASE, TIMER_A, COOLING_SUB_OFF);


		if (get_goal_temp(&ss) == get_current_temp(&ss))
		{	
			enter_sleep_mode();	
			return;
		}

		// enable the timer and interrupt
		TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT); 
		TimerEnable(GPT0_BASE,TIMER_A);

	}


}


/* Function: UART_Interrupt_Handler()
 *
 * Parameters: void
 *
 * Purpose: This function configures UART and the UART interrupt for 8-N-1, and a buad of 115200
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
        	break;
    	}

	}

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
				// check the status of the LEDs, when one is on, we know we aren't at the end of the heating or cooling
				// period.
				while ((HWREG(GPIO_BASE + GPIO_O_DOUT7_4) & GPIO_DOUT7_4_DIO6)) == GPIO_DOUT7_4_DIO6 || (HWREG(GPIO_BASE + GPIO_O_DOUT7_4) & GPIO_DOUT7_4_DIO7) == GPIO_DOUT7_4_DIO7);
				

				// system will enter sleep mode...
				enter_sleep_mode();

			}
			else
			{
				return; // system is already sleeping, do nothing
			}


		}
		// TEMPERAUTE COMMAND ISSUED
		else
		{

			// if the system is already active, we know its heating or cooling, we will need to wait till the end of 
			// the current heating / cooling period before updating the goal temperature.
			if (get_system_state(&ss)) // if get_system_state() returns 1, the system is currently heating or cooling.
			{
				// wait for it to finish current heating or cooling period before setting new temp.
				while ((HWREG(GPIO_BASE + GPIO_O_DOUT7_4) & GPIO_DOUT7_4_DIO6)) == GPIO_DOUT7_4_DIO6 || (HWREG(GPIO_BASE + GPIO_O_DOUT7_4) & GPIO_DOUT7_4_DIO7) == GPIO_DOUT7_4_DIO7);

				// Update the goal temp, cmd is the return from interpert_cmd (the temp or -1 for sleep)
				set_goal_temp(&ss, cmd);

			}
			else // the system is in sleep mode, we should set it to active mode.
			{
				// will set the system state to 1 for active mode, and enable the timer interrupt
				enter_active_mode();
				set_goal_temp(&ss, cmd); 
			}


			// if the current temp is equal to the goal temp, then we can enter sleep mode.
			if (get_current_temp(&ss) == get_goal_temp(&ss))
			{
				enter_sleep_mode();
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


/* Function: setup_timer_with_interrupt()
 *
 * Parameters: void
 *
 * Purpose: Configure the timer and enable timer interrupts. In this function we enable the timer peripheral in both run and 
 *			sleep mode, following this we divide the CPU Clock input to the timer by 16 (48*10^6) / 16 = 3*10^6 = 3MHz = 
 *			3000000 = 0x00002DC6C. This function uses timer A and B which will use 32 bits. The timer is setup for oneshot mode.
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
 * Purpose: configure the UART for baud 115200 (BAUD_RATE) and 8-N-1, with an interrupt.
 *
 * Return: void
 */
void setup_UART_Interrupt()
{
	// pg. 1460
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

	// UART Configuration
	UARTConfigSetExpClk(UART0_BASE,CLOCK,BAUD_RATE, UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|UART_CONFIG_PAR_NONE);

	// Disable flow control
	UARTHwFlowControlDisable(UART0_BASE);

	// Set FIFO Thresholds
	UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8); 

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
	PRCMSleep();

	enter_sleep_mode();

	return 0;
}

















