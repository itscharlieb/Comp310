/**
* @author Charlie Bloomfield
* March 22, 2015
**/

#include <limit.h>    /* for CHAR_BIT */
#include <stdint.h>   /* for uint32_t */
#include "bitmap.h"


/**
*
*/
Bitmap* init_bitmap(unsigned int size){
	int numBytes = size / 8;
	Bitmap* bitmap = (Bitmap*)malloc(numBytes);
	bitmap->size = size;
	return bitmap;
}

void set_bit(Bitmap b, unsigned int bitNumber){

}

void clear_bit(Bitmap b, unsigned int bitNumber){

}

int get_bit(Bitmap b, unsigned int bitNumber){

}

typedef uint32_t word_t;
enum { BITS_PER_WORD = sizeof(word_t) * CHAR_BIT };
#define WORD_OFFSET(b) ((b) / BITS_PER_WORD)
#define BIT_OFFSET(b)  ((b) % BITS_PER_WORD)

void set_bit(word_t *words, int n) { 
    words[WORD_OFFSET(n)] |= (1 << BIT_OFFSET(n));
}

void clear_bit(word_t *words, int n) {
    words[WORD_OFFSET(n)] &= ~(1 << BIT_OFFSET(n)); 
}

int get_bit(word_t *words, int n) {
    word_t bit = words[WORD_OFFSET(n)] & (1 << BIT_OFFSET(n));
    return bit != 0; 
}