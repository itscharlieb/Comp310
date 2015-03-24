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

byte* write_half_word_as_bytes(byte* buffer, half_word hw);
byte* write_mem_word_as_bytes(byte* buffer, mem_word mw);