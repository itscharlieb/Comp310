/**
* @author Charlie Bloomfield
* March 22, 2015
**/

#include "../include/bitmap.h"
#include <stdint.h>   /* for uint32_t */
#include <stdlib.h>

/**
*
*/
Bitmap* init_bitmap(unsigned int size){
	int numBytes = size / 8;
	Bitmap* bitmap = (Bitmap*)malloc(sizeof(bitmap));
	bitmap->bits = (byte*)malloc(numBytes);
	return bitmap;
}

void set_bit(Bitmap* b, unsigned int bitNum){
	b->bits[bitNum / 8] |= 1 << (bitNum & 7);
}

void clear_bit(Bitmap* b, unsigned int bitNum){
	 b->bits[bitNum / 8] &= ~(1 << (bitNum & 7));
}

int get_bit(Bitmap* b, unsigned int bitNum){
	return b->bits[bitNum / 8] & (1 << (bitNum & 7)) ? 1 : 0;
}