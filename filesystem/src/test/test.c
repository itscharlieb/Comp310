#include <stdio.h>
#include <stdlib.h>

typedef struct {
	char* bits;
} Bitmap;

/**
*
*/
Bitmap* init_bitmap(unsigned int size){
	int numBytes = size / 8;
	Bitmap* bitmap = (Bitmap*)malloc(sizeof(bitmap));
	bitmap->bits = (char*)malloc(numBytes);
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

int main(void){
	Bitmap* b = init_bitmap(8);
	set_bit(b, 13);
	printf("%d\n", get_bit(b, 13));
	printf("%d\n", get_bit(b, 12));
}