/*  Program: userbuffer.c
 *
 *  Purpose: Provides functionality for the Buffer strucutre defined in userbuffer.h.
 * 
 *  Author: Liam Ryan
 */


/*INCLUDES*/
#include "userbuffer.h"
/*INCLUDES END*/

/* Function: null_buffer()
 *
 * Purpose: set all values in the buffer to the null byte. Reset the index and the count.
 * 
 * Paramters:
 *  struct Buffer *: the address of a Buffer structure.
 *
 * Return: void
 */
void null_buffer(struct Buffer *b)
{
	for (int i = 0; i < BUFFER_SIZE; i++)
	{
    	b->buffer[i] = '\0';
	}

    b->count = 0;
    b->index = 0;

}

/* Function: get_count()
 *
 * Purpose: This function will return the count of the buffer, that is, the 
 *          amount of elements.
 * 
 * Paramters:
 *  struct Buffer *: the address of a Buffer structure.
 *
 * Return:
 *  int: number of elements in the buffer.
 */
int get_count(struct Buffer *b)
{
	return b->count;
}

/* Function: interpret_cmd()
 *
 * Purpose: This function will interpret the command after it has been validated. Because this function
 *          it meant to be called post validation it simply checks the first character, if it is an 'F'
 *          we know the user wants to enter sleep mode, indicated by the return of (-1). Otherwise, they 
 *          have provided a goal temperature, which will be returned instead.
 * 
 * Paramters:
 *  struct Buffer *: the address of a Buffer structure.
 *
 * Return:
 *  int: the goal temperature or sleep mode (-1).
 */
int interpret_cmd(struct Buffer *b)
{
	if (b->buffer[0] == 'F')
	{
    	return -1;
	}
	else
	{
        // grabe first and second digit
    	int first_digit_ascii  =  b->buffer[0];
    	int second_digit_ascii =  b->buffer[1];

        // convert to appropriate integer value by subtracting ascii 0 (48)
    	int first_digit_adjusted_to_int  = first_digit_ascii-48;
    	int second_digit_adjusted_to_int = second_digit_ascii-48;

    	// convert first digit to 10s
    	int temp = first_digit_adjusted_to_int*10;
    	// add second digit to 1s
    	int temperature_value = temp + second_digit_adjusted_to_int;


    	return temperature_value;
	}


}

/* Function: is_valid_cmd()
 *
 * Purpose: This function will return a boolean (1) or (0) if the provided command is valid.
 *          A valid command follows one of two particular patterns, this function compares
 *          the provided user input against those patterns. If the first character is 'F',
 *          the next character must also be an 'F' followed by two spaces. If the command 
 *          begins with a digit, the first digit must be 2 or 3. If the first digit is 2, the second 
 *          digit can be any digit between 0-9, however, if the first digit is 3, the second digit must be 0.
 *          If either of the digit conditions are met, it must then be followed by two spaces.
 * 
 *          Any data provided that does not meet these specifications is to be ignored and will result in
 *          the emptying of the buffer via null_buffer().
 * 
 * Paramters:
 *  struct Buffer *: the address of a Buffer structure.
 *
 * Return:
 *  int: if valid (1), if invalid (0).
 */
int is_valid_cmd(struct Buffer *b)
{

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

/* Function: is_full()
 *
 * Purpose: This function will determine if the buffer is full by comparing the count 
 *          (current number of elements), to the BUFFER_SIZE (4).
 * 
 * Paramters:
 *  struct Buffer *: the address of a Buffer structure.
 *
 * Return:
 *  int: If full returns (1), if empty return (0).
 */
int is_full(struct Buffer *b)
{
	return b->count == BUFFER_SIZE;
}

/* Function: insert()
 *
 * Purpose: This function will insert a char into the buffer at the current index on 
 *          the condition that the buffer is not null, and the buffer is not full. If 
 *          it can insert the data, it will increment the counter and the current index.
 * 
 * Paramters:
 *  struct Buffer *: the address of a Buffer structure.
 *
 * Return: void.
 */
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
 *  	struct Buffer *: the address of a Buffer structure.
 *
 * Purpose: This function will initialize the attributes of the Buffer structure
 *          which is used to hold user provided data via the UART interface. It will 
 *          on instantiation, null the buffer, set the index to (0), and the count to (0). 
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

