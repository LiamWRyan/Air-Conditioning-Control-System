/*  Program: userbuffer.h
 *
 *  Purpose: Defines the Buffer strucutre that will contain the user provided commands,
 *           Aditionally, establishes the interface for the functions which perform actions 
 *           on this strucutre found in userbuffer.c.
 * 
 *  Author: Liam Ryan
 */

/*INCLUDES & DEFINES*/
#include <stdlib.h>
#define BUFFER_SIZE 4
/*INCLUDES & DEFINES END*/

/* When initialized the buffer will be nulled out
 * the index will be 0
 * the count will be 0
 */
struct Buffer {
	char buffer[BUFFER_SIZE]; // buffer
	int index;                // current index
	int count;                // keeping track of total elements 
};

/* INTERFACES */
void null_buffer(struct Buffer *b);

int get_count(struct Buffer *b);

int is_full(struct Buffer *b);

int interpret_cmd(struct Buffer *b);

int is_valid_cmd(struct Buffer *b);

void insert(struct Buffer *b, char data);

void init_b(struct Buffer *b);
