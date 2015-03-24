/**
* @author Charlie Bloomfield
* March 16, 2015
*/

#include "types.h"

void write_half_word_as_bytes(byte* buffer, half_word hw){
	byte b = (hw >> 8);
	*buffer = b;
	buffer ++;
}

void write_mem_word_as_bytes(byte* buffer, mem_word mw){

}

half_word read_bytes_as_half_word(byte* buffer){
	
}

mem_word read_bytes_as_mem_word(byte* buffer){
	
}