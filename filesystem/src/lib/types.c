/**
* @author Charlie Bloomfield
* March 16, 2015
*/

#include "../include/constants.h"
#include "../include/types.h"
#include <string.h>

void write_half_word_as_bytes(byte* buffer, half_word hw){
	half_word bitMask = 0x00FF;
	byte bytes[2];

	bytes[1] = hw >> bitMask;
	bytes[0] = hw >> 8 & bitMask;

	memcpy(buffer, &hw, HALF_WORD_SIZE);
}

void write_mem_word_as_bytes(byte* buffer, mem_word mw){
	mem_word bitMask = 0x000000FF;
	byte bytes[2];

	bytes[3] = mw & bitMask;
	bytes[2] = mw >> 8 & bitMask;
	bytes[1] = mw >> 16 & bitMask;
	bytes[0] = mw >> 24 & bitMask;

	memcpy(buffer, bytes, MEM_WORD_SIZE);
}

half_word read_bytes_as_half_word(byte* buffer){
	half_word target;
	memcpy(&target, buffer, HALF_WORD_SIZE);
	return target;
}

mem_word read_bytes_as_mem_word(byte* buffer){
	mem_word target;
	memcpy(&target, buffer, MEM_WORD_SIZE);
	return target;
}