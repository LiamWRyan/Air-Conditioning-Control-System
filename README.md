# Air-Conditioning-Control-System (Febuary 2023)

## Objectives

### Create a low-powered embedded system for simulating an air conditioning control system.
    
This software will run on a microcontroller without an OS,
Control LEDs to meet the strict timing requirements, 
Configure UART and communicate through it,
Configure a timer,
Utilize ISRs,
and lower power applications.


## Description 
                                            
### USER (commands) --> MICROCONTROLLER  --> heating unit (red LED) OR cooling unit (green LED) OR Sleep
### MICROCONTROLLER (data) --> USER               

The red and green LEDs are simulating the heating and cooling units resepctively.

In this system, the user chooses a temperature value (Tchoosen) and sends it to the CC1350 
microcontroller. The microcontroller should then turn on the *heating unit* (red LED) or the 
cooling unit (Green led) periodically until the chosen temperature is reached. 


### A. Communication between user computer and CC1350 microcontroller through UART:
  BAUD RATE: 115200,
  8 bits per character,
  no parity bit,
  1 stop bit,

### B. Possible user commands: 

**Command:** Any value between 20 and 30 followed by two spaces.
**Operation:** Turn the system to active mode and set Tchosen to the received value. The system should then turn on periodically heating or cooling unit until Tchosen is reached.

**Command:** "FF "
**Opertaion:** Turn the system to sleep mode.

### C. System Modes

Active mode: the chosen temperature is not reached. Therefore, the microcontroller is periodically turning one the red Led (heating unit) or the green LED (cooling unit) until the chosen temperature is achieved.

Sleep mode: the microcontroller should be in sleep state with all the LEDs turned off. The microcontroller
in this mode should *ONLY* be able to receive user commands through the UART interface.

### D. System operation

Initially the system should be in sleep mode with a current temperature value (Tcurrent) equals to 0.

When a user command is received with a chosen temperature (Tchosen), the system goes to the active mode.

If Tcurrent < Tchosen, the system enters active mode and runs multiple heating periods. At the end of each period: If Tcurrent reaches Tchosen, the system goes to sleep mode.

If Tcurrent > Tchosen, the system enters active mode and runs multiple cooling periods. At the end of each period: If Tcurrent reaches Tchosen, the system goes to sleep mode.

If the system receives a user command while it is in active mode: If the received command is "FF ", the system waits until the end of heating/cooling period. Then, it goes to sleep mode. If the received command is a value between (and including) 20 and 30 followed by two spaces, the system waits until the end of the heating/cooling period, then it updates Tchosen.

After entering sleep mode, the system should maintain the current temperature value.

The system should NOT leave the sleep mode until a numerical command between (and including) 20 and 30 followed by two spaces is received from the user.

All invalid commands should be ignored by the system.

### E. Heating period: this period has a length of 1 second and consists of two parts:
In each sub-period, the microcontroller should enter the sleep state after changing the LEDs state till the end of the sub-period.

**ON sub-period (600 ms):** at the start of this sub-period, the letter 'H' should be sent to the user computer through UART interface. The red LED should be turned on until the end of this sub-period

**OFF sub-period (400ms):** At the start of this sub-period, the current temperature is updated as follows:Tcurrent = Tcurrent + R, Where R is a randomly generated value that is either 0 or 1. 

The pair (Tcurrent,Tchosen) is sent to the user through UART.
Example: if Tcurrent = 20 and Tchosen = 30, the system should send thefollowing message “(20,30)”.

All the LEDs should be turned off until the end of this sub-period.

### F. Cooling period: this period has a length of 1 second and consits of two parts:

**ON sub-period (500 milli-seconds):**
At the start of this sub-period, the letter ‘C’ should be sent to the user computer through UART interface. The green LED should be turned on until the end of this sub-period.

**OFF sub-period (500 milli-seconds):** At the start of this sub-period, the current temperature (Tcurrent) is updated as follows: Tcurrent = Tcurrent - R, Where R is a randomly generated value that is either 0 or 1.
        
The pair (Tcurrent,Tchosen) is sent to the user through UART.
Example: if Tcurrent = 20 and Tchosen = 30, the system should send the following message “(20,30)”.

All the LEDs should be turned off until the end of this sub-period.
