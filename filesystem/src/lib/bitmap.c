/**
* @author Charlie Bloomfield
* March 22, 2015
**/

#include "../include/bitmap.h"
#include <stdint.h>   /* for uint32_t */
#include <stdlib.h>
#include <stdio.h>

/**
*
*/
Bitmap* bitmap_init(unsigned int size){
	int numBytes = size / 8;
	Bitmap* bitmap = (Bitmap*)malloc(sizeof(bitmap));
	bitmap->bits = (byte*)malloc(numBytes);
	bitmap->size = size;

	return bitmap;
}

void set_bit(Bitmap* b, unsigned int bitNum){
	b->bits[bitNum / 8] |= 1 << (bitNum & 7);

	// printf("[set_bit] Set bit [%d] to used.\n", bitNum);
	// fflush(stdout);
}

void clear_bit(Bitmap* b, unsigned int bitNum){
	 b->bits[bitNum / 8] &= ~(1 << (bitNum & 7));
}

int get_bit(Bitmap* b, unsigned int bitNum){
	return b->bits[bitNum / 8] & (1 << (bitNum & 7)) ? 1 : 0;
}

int find_free_bit(Bitmap* b){
	int i, bitNum;
	for(i = 0; i < b->size; i++){
		if((bitNum = get_bit(b, i)) == 0){
			return bitNum;
		}
	}
}