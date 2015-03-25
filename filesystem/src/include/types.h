/**
* @author Charlie Bloomfield
* March 16, 2015
*/

/* This file defines data types useful data transfer to and from disk */

//1 byte
typedef unsigned char byte;

//2 bytes
typedef short int half_word;

//4 bytes
typedef unsigned int mem_word;

void write_half_word_as_bytes(byte* buffer, half_word hw);
void write_mem_word_as_bytes(byte* buffer, mem_word mw);
half_word read_bytes_as_half_word(byte* buffer);
mem_word read_bytes_as_mem_word(byte* buffer);