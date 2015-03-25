/**
* @author Charlie Bloomfield
* March 16, 2015
*/

#include "../include/constants.h"
#include "../include/types.h"
#include <string.h>

void write_half_word_as_bytes(byte* buffer, half_word hw){
	memcpy(buffer, &hw, HALF_WORD_SIZE);
}

void write_mem_word_as_bytes(byte* buffer, mem_word mw){
	memcpy(buffer, &mw, MEM_WORD_SIZE);
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