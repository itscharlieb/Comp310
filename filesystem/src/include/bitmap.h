/**
* @author Charlie Bloomfield
*/

#include "types.h"

typedef struct {
	byte* bits;
} Bitmap;

//@pre size must be a multiple of 8
Bitmap* init_bitmap(unsigned int size);

void set_bit(Bitmap* b, unsigned int bitNumber);
void clear_bit(Bitmap* b, unsigned int bitNumber);
int get_bit(Bitmap* b, unsigned int bitNumber);
int find_free_bit(Bitmap* b);