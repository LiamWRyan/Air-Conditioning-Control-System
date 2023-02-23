#include "header.h"

struct SystemState ss; // globally defined struct, repersenting system state.
struct Buffer       b; // globally defined struct, a buffer of user input.

int global_period_tracker = 0; // used for tracking the sub periods of cooling and heating periods.


/* Function: off_sub_period()
 *
 * Parameters:
 *  int heating: (1) for heating, (0) for not heating and thus cooling.
 *
 * Purpose: This function performs the logic of the off period of both cooling and heating
 *          operations. If heating or cooling it will update the current temperature and send
 *          the user the current temperature of the system compared to the goal temperature, it will
 *          also disable the corresponding LED for heating (red) and cooling (green).
 *
 * Return: void
 */
void off_sub_period(int heating){

    if (heating) {
        update_current_temp(&ss, rand()%2, heating);
        //GPIO_clearDio(COOLING_UNIT_ID);           // in case it is on, turn it off
        GPIO_clearDio(HEATING_UNIT_ID);


    }
    else {  // assume cooling if not heating
        update_current_temp(&ss, rand()%2, heating);  // heating will be zero if this case is reached
        //GPIO_clearDio(HEATING_UNIT_ID);              // in case it is on, turn it off
        GPIO_clearDio(COOLING_UNIT_ID);
    }

    // After one of the above cases has executed, the temperature value will have
    // been updated, and we can now get the current temp and the goal temp.
    char * temperature_copmarison = get_current_and_goal_temps(&ss);

    // regardless of whether were are heating or cooling, during the off sub period we want to send to the user
    // a comparison of the temperatures (Tcurrent,Tgoal).
    for (int i = 0; i < 9; i++){
        char ch = temperature_copmarison[i];
        UARTCharPut(UART0_BASE, (uint8_t) ch);
    }

}

/* Function: on_sub_period()
 *
 * Parameters:
 *  int heating: (1) for heating, (0) for not heating and thus cooling.
 *
 * Purpose: This function represents the logic of the on period of both cooling and heating
 *          operations. If it is heating it will send an 'H' to the user and set the red LED.
 *          If it is cooling, it will send a 'C'to the user and set the green LED.
 *
 * Return: void
 */
void on_sub_period(int heating){

    if (heating){
        UARTCharPut(UART0_BASE, (uint8_t) 'H');
        GPIO_setDio(HEATING_UNIT_ID);   // red LED
        GPIO_clearDio(COOLING_UNIT_ID); // ensure green LED it is not on

        return;
    }
    else { // if not heating, assume cooling
        UARTCharPut(UART0_BASE, (uint8_t) 'C');
        GPIO_setDio(COOLING_UNIT_ID);   // green LED
        GPIO_clearDio(HEATING_UNIT_ID); // ensure red LED is not on.
    }

}


/* Function: interrupt_fn()
 *
 * Parameters: void
 *
 * Purpose: This function is the timer ISR. It will check the state of the system upon entry. If the system is in active mode.
 *          it will then check if it should be heating or cooling. It will also utilize a global tracker to determine if it
 *          should be in the one or off period. In the on period, it wil let the user know if it is heating or cooling and enable
 *          the appropriate LED. In the off period, it will update the current temp, inform the user of the temps, and check
 *          if the current temp equals the goal temp. If they are equal it will go back into sleep mode.
 *
 * Return: void
 */
void interrupt_fn(){
    TimerIntClear(GPT0_BASE, TIMER_TIMA_TIMEOUT);

    if (get_system_state(&ss) == 0) { // if the system is in sleep mode, exit.
        return;
    }

    if (should_heat(&ss) && (global_period_tracker == 0)){
        // USE UART to send data ('H') to user, and turns on the red LED
        on_sub_period(1); // one indicates heating
        global_period_tracker = 1;

        TimerLoadSet(GPT0_BASE, TIMER_A, HEATING_SUB_ON);  // 600ms

        TimerPrescaleSet(GPT0_BASE, TIMER_A, 0x000000FF);
        TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
        TimerEnable(GPT0_BASE,TIMER_A);

    }
    if (should_heat(&ss) && (global_period_tracker == 1))
    {

        off_sub_period(1); // one indicates heating
        global_period_tracker = 0;

        TimerLoadSet(GPT0_BASE, TIMER_A, HEATING_SUB_OFF); // 400ms


        TimerPrescaleSet(GPT0_BASE, TIMER_A, 0x000000FF);
        TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
        TimerEnable(GPT0_BASE,TIMER_A);

        if (get_goal_temp(&ss) == get_current_temp(&ss)){
            // will disable the timer, set the cooling and heating status to 0 (off), and
            // set the system state to sleep.
            enter_sleep_mode(&ss);
            return;
        }
    }



    if (should_cool(&ss) && (global_period_tracker == 0)){
        // USE UART to send data ('C') to user, and turns on the red LED
        on_sub_period(0); // zero indicates cooling
        global_period_tracker = 1;


        TimerLoadSet(GPT0_BASE, TIMER_A, COOLING_SUB_ON);

        TimerPrescaleSet(GPT0_BASE, TIMER_A, 0x000000FF);
        TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
        TimerEnable(GPT0_BASE,TIMER_A);

    }

    if ((should_cool(&ss) && (global_period_tracker == 1))){

        off_sub_period(0); // zero indicates cooling
        global_period_tracker = 0;

        TimerLoadSet(GPT0_BASE, TIMER_A, COOLING_SUB_OFF);


        TimerPrescaleSet(GPT0_BASE, TIMER_A, 0x000000FF);
        TimerIntEnable(GPT0_BASE,TIMER_TIMA_TIMEOUT);
        TimerEnable(GPT0_BASE,TIMER_A);

        if (get_goal_temp(&ss) == get_current_temp(&ss)){
            enter_sleep_mode(&ss);
            return;
        }

    }

    TimerIntClear(GPT0_BASE, TIMER_TIMA_TIMEOUT);

}


/* Function: UART_Interrupt_Handler()
 *
 * Parameters: void
 *
 * Purpose: This function is the ISR for UART. It receives and handles user input. It will validate
 *          the commands sent to it by the user.
 *
 * Return: void
 */
void UART_Interrupt_Handler(){

    // if the interrupt status isn't receive intereupt, return
    if(UARTIntStatus(UART0_BASE,  true) !=  UART_INT_RX) return;

    UARTIntClear(UART0_BASE,UART_INT_RX|UART_INT_TX);


    // UARTCharsAvail needs to be called before calling UARTCharGetNonBlocking()
    // UARTCharGetNonBlocking will return the char read from the specied port, or a -1 if no char is present.
    while(UARTCharsAvail(UART0_BASE)){

        int32_t ch =   UARTCharGetNonBlocking(UART0_BASE) & 0x000000FF;

        insert(&b, (char) ch);

        // the buffer contains 4 chars, we can check for a command
        if (get_count(&b) == 4){
           break;
        }

    }


    if (!is_valid_cmd(&b)) { // will return 0 for invalid cmd, and 1 for valid cmd
        // clear the buffer once we have interpreted the command
        null_buffer(&b);
        return;
    }

    int cmd = interpret_cmd(&b);
    null_buffer(&b);

    // INTERPRET THE CMD, if -1 then SLEEP COMMAND ISSUED
    if (cmd == -1) {
        // CHECK THE SYSTEM STATE, the system state will be 1 if it is active
        if (get_system_state(&ss)){

            // wait till the end of the current heating or cooling period...
            // check the status of the LEDs, when one is on, we know we aren't at the end of the heating or cooling
            // period.
            /*
            while (1){

                if (((HWREG(GPIO_BASE + GPIO_O_DOUT7_4) & GPIO_DOUT7_4_DIO6 == GPIO_DOUT7_4_DIO6) && (HWREG(GPIO_BASE + GPIO_O_DOUT7_4) & GPIO_DOUT7_4_DIO7) == GPIO_DOUT7_4_DIO7)){
                    break;
                }
            }
            */
            // system will enter sleep mode...
            enter_sleep_mode(&ss);
            return;

        }
        else {
            return; // system is already sleeping, do nothing
        }

    }
    else { // TEMPERAUTE COMMAND ISSUED

        // if the system is already active, we know its heating or cooling, we will need to wait till the end of
        // the current heating / cooling period before updating the goal temperature.
        if (get_system_state(&ss)){ // if get_system_state() returns 1, the system is currently heating or cooling.
            // wait for it to finish current heating or cooling period before setting new temp.
            /*
            while (1){

                if (((HWREG(GPIO_BASE + GPIO_O_DOUT7_4) & GPIO_DOUT7_4_DIO6 == GPIO_DOUT7_4_DIO6) && (HWREG(GPIO_BASE + GPIO_O_DOUT7_4) & GPIO_DOUT7_4_DIO7) == GPIO_DOUT7_4_DIO7)){
                    break;
                }
            }
            */

            // Update the goal temp, cmd is the return from interpert_cmd (the temp or -1 for sleep)
            set_goal_temp(&ss, cmd);

        }
        else { // the system is in sleep mode, we should set it to active mode.
            set_goal_temp(&ss, cmd);
        }

    }

    // if the current temp is equal to the goal temp, then we can enter sleep mode.
    if (get_current_temp(&ss) == get_goal_temp(&ss)) {
       enter_sleep_mode(&ss);
       return;
    }
    else if (get_current_temp(&ss) > get_goal_temp(&ss)) {
       // set system state to cooling
       set_cooling_to_on(&ss);

    }
    else { //if (get_current_temp(&ss) < get_goal_temp(&ss))
       // set system state to heating
       set_heating_to_on(&ss);

    }
    // turn on the timer and timer interrupt and set the system to active mode.
    enter_active_mode(&ss);

}


/* Function: setup_UART_Interrupt()
 *
 * Parameters: void
 *
 * Purpose: configure the UART for baud 115200 (BAUD_RATE) and 8-N-1 and UART ISR.
 *
 * Return: void
 */
void setup_UART_Interrupt()
{
    // pg. 1460
    PRCMPowerDomainOn(PRCM_DOMAIN_SERIAL);
    while(PRCMPowerDomainStatus(PRCM_DOMAIN_SERIAL) != PRCM_DOMAIN_POWER_ON);

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


/* Function: setup_GPIO_LEDs()
 *
 * Parameters: void
 *
 * Purpose: this funciton configures and enables the red and green LED.
 *
 * Return: void
 */
void setup_GPIO_LEDs()
{
    // Turn on the GPIO power domain (will likely already be on, but just in case...)
    PRCMPowerDomainOn(PRCM_DOMAIN_PERIPH);
    while(PRCMPowerDomainStatus(PRCM_DOMAIN_PERIPH) != PRCM_DOMAIN_POWER_ON);

    // Enable GPIO peripheral
    PRCMPeripheralRunEnable(PRCM_PERIPH_GPIO);
    PRCMPeripheralSleepEnable(PRCM_PERIPH_GPIO);

    // synchorinze the load settings (propigate conifigurations)
    PRCMLoadSet();
    while(! PRCMLoadGet());

    // enable output for the green LED
    GPIO_setOutputEnableDio(IOID_7, GPIO_OUTPUT_ENABLE);

    // enable output for the red LED
    GPIO_setOutputEnableDio(IOID_6, GPIO_OUTPUT_ENABLE);

}

/* Function: setup_timer_with_interrupt()
 *
 * Parameters: void
 *
 * Purpose: Configure the timer and timer ISR. Disable both on start, the system
 *          is required to start in sleep mode. This timer uses timer A in
 *          16 bit mode, with a prescalar of 255, and a division set of 16.
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

    // Since we want the system to start in sleep mode, we will disable the timer and the interrupt
    // in this initial configuration.
    TimerDisable(GPT0_BASE,TIMER_A);
    TimerIntDisable(GPT0_BASE,TIMER_A);

    // Configure the timer in one shot mode.
    // DOCS; technical refernce manual (pg1200), and TimerConfigure in API driver lib.
    TimerConfigure(GPT0_BASE, TIMER_CFG_ONE_SHOT);
    // Assign the interrupt handler
    TimerIntRegister(GPT0_BASE, TIMER_A, interrupt_fn);

    // CONFIGURE TIMER to 16 bits counting mode with prescalar 256
    HWREG(GPT0_BASE + GPT_O_CFG) = 0x4;
    // Set prescaler to 256
    TimerPrescaleSet(GPT0_BASE, TIMER_A, 0x000000FF);

}


int main()
{

    init_ss(&ss);
    init_b(&b);

    setup_GPIO_LEDs();

    setup_timer_with_interrupt();

    setup_UART_Interrupt();

    enter_sleep_mode(&ss);
    PRCMSleep();

    return 0;
}
