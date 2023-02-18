#include "userbuffer.h"



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
 *  	struct Buffer *: the address of a ringbuffer.
 *
 * Purpose: 
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

