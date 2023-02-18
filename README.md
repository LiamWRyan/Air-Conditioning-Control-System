# Air-Conditioning-Control-System

## Objectives

### Create a low-powered embedded system for simulating an air conditioning control system.
    
    This software will run on a microcontroller without an OS,
    Control LEDs to meet the strict timing requirements, 
    Configure UART and communicate through it,
    Configure a timer,
    Utilize ISRs,
    and lower power applications.


## Description 
                                            
### USER (commands) --> MICROCONTROLLER  --> heating unit (red light) OR cooling unit (green light)
###                 <-- (data)

The red and green LEDs are simulating the heating and cooling units resepctively.

In this system, the user chooses a temperature value (Tchoosen) amd semds ot tp the CC1350 
microcontroller. The microcontroller should then turn on the heating unit (Red led) or the 
cooling unit (Green led) *periodically* untill the chosen temperature is reached. Based on the 
below specifications:

A. communication between user computer and CC1350 microcontroller through UART (8-N-1):
  BAUD RATE: 115200,
  8 bits per character,
  no parity bit,
  1 stop bit,

B. possible user commands; the user can choose a temperature between (and including) 20 and 30. 
   The user can also choose anytime to turn the system to sleep mode. The user command consists of two characters followed by two spaces that indicate the desired operation.

   Command: any value between 20 and 30 followed by two spaces
   Operation:
   Turn the system to active mode and set Tchosen to the received value. The system should then turn on periodically heating or cooling unit until Tchosen is reached.

   Command: "FF "
   Opertaion: Turn the system to sleep mode.

C. System Modes

Active mode: the chosen temperature is not reached. Therefore, the microcontroller is periodically turning one the red Led (heating unit) or the green LED (cooling unit) until the chosen temperature is achieved.

Sleep mode: the microcontroller should be in sleep state with all the LEDs turned off. The microcontroller
in this mode should *ONLY* be able to receive user commands through the UART interface.

D. System operation

Initially the system should be in sleep mode with a current temperature value (Tcurrent) equals to 0.

When a user command is received with a chosen temperature (Tchosen), the system goes to the active mode.

If Tcurrent < Tchosen, the system enters active mode and runs multiple heating periods. At the end of each period:
    If Tcurrent reaches Tchosen, the system goes to sleep mode.

If Tcurrent > Tchosen, the system enters active mode and runs multiple cooling periods. At the end of each period:
    If Tcurrent reaches Tchosen, the system goes to sleep mode.

If the system receives a user command while it is in active mode:
    If the received command is "FF ", the system waits until the end of heating/cooling period. Then, it goes to sleep mode.

    If the received command is a value between (and including) 20 and 30followed by two spaces, the system waits until the end of theheating/cooling period, then it updates Tchosen.

After entering sleep mode, the system should maintain the current temperature value.

The system should NOT leave the sleep mode until a numerical commandbetween (and including) 20 and 30 followed by two spaces is received from the user.

All invalid commands should be ignored by the system.

E. Heating period: this period has a length of 1 second and consists of two parts:
    In each sub-period, the microcontroller should enter the sleep state after changing the LEDs state till the end of the sub-period.

    ON sub-period (600 ms):
        at the start of this sub-period, the letter 'H' should be sent to the user computer through UART interface.

        The red LED should be turned on until the end of this sub-period

    OFF sub-period (400ms):
        At the start of this sub-period, the current temperature is updated as follows:
                Tcurrent = Tcurrent + R
        Where R is a randomly generated value that is either 0 or 1. (stdlib.h has a random function)

        The pair (Tcurrent,Tchosen) is sent to the user through UART.
        Example: if Tcurrent = 20 and Tchosen = 30, the system should send thefollowing message “(20,30)”

        All the LEDs should be turned off until the end of this sub-period

F. Cooling period: this period has a length of 1 second and consits of two pars:
    ON sub-period (500 milli-seconds):
        At the start of this sub-period, the letter ‘C’ should be sent to the usercomputer through UART interface.

        The green led should be turned on until the end of this sub-period.

    OFF sub-period (500 milli-seconds):

        At the start of this sub-period, the current temperature (Tcurrent) is updated as follows:

        Tcurrent = Tcurrent - R

        Where R is a randomly generated value that is either 0 or 1.
        
        The pair (Tcurrent,Tchosen) is sent to the user through UART.
        Example: if Tcurrent = 20 and Tchosen = 30, the system should send thefollowing message “(20,30)”

        All the LEDs should be turned off until the end of this sub-period