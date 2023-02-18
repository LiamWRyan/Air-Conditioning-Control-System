#include <stdlib.h>
#define BUFFER_SIZE 4

/*
 *
 *
 * 
 */
struct Buffer {
	char buffer[BUFFER_SIZE]; // buffer
	int index;                // current index
	int count;                // keeping track of total elements 
};

void null_buffer(struct Buffer *b);

int get_count(struct Buffer *b);

int is_full(struct Buffer *b);

int interpret_cmd(struct Buffer *b);

int is_valid_cmd(struct Buffer *b);

void insert(struct Buffer *b, char data);

void init_b(struct Buffer *b);
