/*  Program: systemstate.c
 *
 *  Purpose: Provides functionality for the SystemState strucutre defined in systemstate.h.
 *
 *  Author: Liam Ryan
 */

#include "systemstate.h"

/* Function: get_goal_temp()
 *
 * Purpose: This function returns the current goal temperature of the system. That is, the temperature the
 *          the system is attempting to reach through heating or cooling.
 *
 * Parameters:
 *  struct SystemState *: The address of a SystemState strucutre.
 *
 * Returns:
 *  int: the goal temperature value as an interger.
 */
int get_goal_temp(struct SystemState *ss)
{
    return ss->t_goal;
}

/* Function: set_goal_temp()
 *
 * Purpose: This function sets the current goal temperature of the system. That is, the temperature
 *          the system will be attempting to reach through heating or cooling post set.
 *
 * Parameters:
 *  struct SystemState *: The address of a SystemState strucutre.
 *  int new_temp:         The integer representation of the new temperature.
 *
 * Returns: void
 */
void set_goal_temp(struct SystemState *ss, int new_temp)
{
    ss->t_goal = new_temp;
}

/* Function: update_current_temp()
 *
 * Purpose: This function will update the current temperature in the second sub period
 *          of a cooling or heating phase.
 *
 * Parameters:
 *  struct SystemState *: The address of a SystemState strucutre.
 *  int new_temp:         a value to increase or decrease by, it will be 0 or 1.
 *  int increase_temp:    if 1 we will increase the temp by new_temp, else, we will decrese the temp by new_temp.
 *
 * Returns: void
 */
void update_current_temp(struct SystemState *ss, int new_temp, int increase_temp){
    // if this value is 0, we can skip updating and simply return, since adding or subtracting zero will do
    // nothing.
    if (new_temp == 0){
        return;
    }

    if (increase_temp) {  // will = 1 when heating is 1
        ss->t_curr = ss->t_curr + new_temp;
    }
    else { // assume if we arent increasing the temp, we are decreasing it.
        ss->t_curr = ss->t_curr - new_temp;
    }
}

/* Function: get_current_temp()
 *
 * Purpose: This function will return the current temperature of the system.
 *
 * Parameters:
 *  struct SystemState *: The address of a SystemState strucutre.
 *
 * Returns:
 *  int: The integer representation of the current system temperature.
 */
int get_current_temp(struct SystemState *ss){
    return ss->t_curr;
}

/* Function: get_system_state()
 *
 * Purpose: This function will return the state of the system, the system can be in one of two modes (states)
 *          sleep mode (0), and active mode (1). When the system is in sleep mode, the timer is disabled,
 *          and it is waiting for a valid command. When the system is in active mode, the system is
 *          periodically entering heating or cooling phases until it is told to sleep, or the current
 *          tempterature reaches the goal temperature.
 *
 *
 * Parameters:
 *  struct SystemState *: The address of a SystemState strucutre.
 *
 * Returns:
 *  int: The state of the system sleep (0), or active (1)
 */
int get_system_state(struct SystemState *ss){
    return ss->state;
}

/* Function: set_heating_to_off()
 *
 * Purpose: Will set the heating attribute of the SystemState to 0. After which, any references to the
 *          SystemState will now indicate that it is no longer perfomring a heating phase.
 *
 * Parameters:
 *  struct SystemState *: The address of a SystemState strucutre.
 *
 * Returns: void
 */
void set_heating_to_off(struct SystemState *ss){
    ss->heating = 0;
}

/* Function: set_heating_to_on
 *
 * Purpose: Will set the heating attribute of the SystemState to 1. After which, any references to the
 *          SystemState will now indicate that it is perfomring a heating phase.
 *
 * Parameters:
 *  struct SystemState *: The address of a SystemState strucutre.
 *
 * Returns: void
 */
void set_heating_to_on(struct SystemState *ss){
    ss->heating = 1;
}

/* Function: set_cooling_to_on()
 *
 * Purpose: Will set the cooling attribute of the SystemState to 1. After which, any references to the
 *          SystemState will now indicate that it is perfomring a cooling phase.
 *
 * Parameters:
 *  struct SystemState *: The address of a SystemState strucutre.
 *
 * Returns: void
 */
void set_cooling_to_on(struct SystemState *ss){
    ss->cooling = 1;
}

/* Function: set_cooling_to_off()
 *
 * Purpose: Will set the cooling attribute of the SystemState to 0. After which, any references to the
 *          SystemState will now indicate that it is not perfomring a cooling phase.
 *
 * Parameters:
 *  struct SystemState *: The address of a SystemState strucutre.
 *
 * Returns: void
 */
void set_cooling_to_off(struct SystemState *ss){
    ss->cooling = 0;
}

/* Function: should_heat()
 *
 * Purpose: This function will return the value of the heating attribute, if it is (1), this indicates that
 *          the system should be heating. If the attribute is (0), this indicates that the system should not
 *          be heating.
 *
 * Parameters:
 *  struct SystemState *: The address of a SystemState strucutre.
 *
 * Returns:
 *  int: Whether the system attribute heating is (1); should be heating or, (0); shouldn't be heating.
 */
int should_heat(struct SystemState *ss){
    return ss->heating;
}

/* Function: should_cool()
 *
 * Purpose: This function will return the value of the cooling attribute, if it is (1), this indicates that
 *          the system should be cooling. If the attribute is (0), this indicates that the system should not
 *          be cooling.
 *
 * Parameters:
 *  struct SystemState *: The address of a SystemState strucutre.
 *
 * Returns: Whether the system attribute cooling is (1); should be cooling or, (0); shouldn't be cooling.
 */
int should_cool(struct SystemState *ss){
    return ss->cooling;
}

/* Function: set_system_state_sleep()
 *
 * Purpose: This function sets the SystemState attribute "state" to 0 for sleep.
 *
 * Parameters:
 *  struct SystemState *: The address of a SystemState strucutre.
 *
 * Returns: void
 */
void set_system_state_sleep(struct SystemState *ss){
    ss->state = 0; // set state to 0 for sleep mode.
}

/* Function: set_system_state_active
 *
 * Purpose: This function sets the SystemState attribute "state" to 1 for sleep.
 *
 * Parameters:
 *  struct SystemState *: The address of a SystemState strucutre.
 *
 * Returns: void
 */
void set_system_state_active(struct SystemState *ss){
    ss->state = 1; // set the state to 1 for active mode.
}

/* Function: get_current_and_goal_temps()
 *
 * Purpose: In the second sub-period of a cooling or heating period, this function is utilized in the
 *          provisioning of data regarding the state of the system to the user through the UART interface.
 *          It will take the current temperature, and the goal temperature as integers and convert them to
 *          characters, digit by digit. It will then pack them into an array of characters in the format of
 *          of (Tcurrent,Tgoal). i.e., "(25,30)"
 *
 * Parameters:
 *  struct SystemState *: The address of a SystemState strucutre.
 *
 * Returns:
 *  char *: An array of characters to be sent to the user that contains the current and goal temeratures.
 */
char* get_current_and_goal_temps(struct SystemState *ss){

    static char charArray[9];

    // get the first and second digit of the current temperature
    int first_digit_current_temp  = ss->t_curr / 10;
    int second_digit_current_temp = ss->t_curr - (first_digit_current_temp*10);

    // by adding 48 (ascii 0) to the digits and casting to a char,
    // we will effectively convert to the character representation of the digit
    char char_one_ct =  (char)(first_digit_current_temp + 48);
    char char_two_ct =  (char)(second_digit_current_temp + 48);

    // get the first and second digit of the goal temp temperature
    int first_digit_goal_temp  = ss->t_goal / 10;
    int second_digit_goal_temp = ss->t_goal - (first_digit_goal_temp*10);

    // by adding 48 (ascii 0) to the digits and casting to a char,
    // we will effectively convert to the character representation of the digit
    char char_one_gt =  (char)(first_digit_goal_temp + 48);
    char char_two_gt =  (char)(second_digit_goal_temp + 48);

    // add elements to the array.
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

/* Function: init_ss()
 *
 * Purpose: This function will initialize the attributes of the system state structure, which specifies various
 *          aspects about the current state of the system. Namely, it initilaizes the state to (0) for sleep, heating
 *          and cooling to (0), indiciating it is not doing either, and t_curr and t_goal to T_INITIAL (0).
 *
 * Parameters:
 *  struct SystemState *: The address of a SystemState strucutre.
 *
 * Returns: void
 */
void init_ss(struct SystemState *ss){
    ss->state       = 0;            // Indicates sleep mode
    ss->heating     = 0;            // off
    ss->cooling     = 0;            // off
    ss->t_curr      = T_INITIAL;    // initial temp
    ss->t_goal      = T_INITIAL;    // goal temp
}
