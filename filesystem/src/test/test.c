#include <stdio.h>
#include <stdlib.h>

#include "../include/bitmap.h"

int main(void){
	Bitmap* b = bitmap_init(64);
	set_bit(b, 0);
	printf("Set bit 0.\n");
	printf("Bit 0 is [%d].\n", get_bit(b, 0));
	printf("Bit 1 is [%d].\n", get_bit(b, 1));

	printf("Free bit at index [%d].\n", find_free_bit(b));


}