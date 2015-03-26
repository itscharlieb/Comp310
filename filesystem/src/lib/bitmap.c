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
	if(bitNum > b->size){
		printf("[set_bit] [%d] is an invalid bitNum, this bitmap has size [%d].\n", bitNum, b->size);
		fflush(stdout);
		return;
	}

	int byteNum = bitNum / 8;
	int bitPosition = bitNum % 8;

	byte mask = 0x01 << bitPosition;
	b->bits[byteNum] |= mask;

	// printf("[set_bit] Set bit [%d] to used.\n", bitNum);
	// printf("[set_bit] Updated byte state is [0x%x].\n", b->bits[byteNum]);
	// fflush(stdout);
}

void clear_bit(Bitmap* b, unsigned int bitNum){
	int byteNum = bitNum / 8;
	int bitPosition = bitNum % 8;

	byte mask = ~(0x01 << bitPosition);
	b->bits[byteNum] &= mask;
}

int get_bit(Bitmap* b, unsigned int bitNum){
	int byteNum = bitNum / 8;
	int bitPosition = bitNum % 8;
	byte mask = (0x01 << bitPosition);

	return b->bits[byteNum] & mask ? 1 : 0;
}

int find_free_bit(Bitmap* b){
	int i, bitNum;
	for(i = 0; i < b->size; i++){
		if(get_bit(b, i) == 0){
			return i;
		}
	}
}