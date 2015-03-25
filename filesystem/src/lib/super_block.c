/**
* @author Charlie Bloomfield
* March 17, 2015
*/

#include "../include/super_block.h"


//used to write and read the super block from disk
void super_block_to_string(SuperBlock* sb, byte* buffer){
	write_half_word_as_bytes(MAGIC, buffer);
	buffer += HALF_WORD_SIZE;
	write_half_word_as_bytes(BLOCK_SIZE, buffer);
	buffer += HALF_WORD_SIZE;
	write_half_word_as_bytes(NUM_BLOCKS, buffer);
	buffer += HALF_WORD_SIZE;
	write_half_word_as_bytes(ROOT_DIRECTORY_INODE_NUMBER, buffer);
	buffer += HALF_WORD_SIZE;
	write_mem_word_as_bytes(INODE_TABLE_LENGTH, buffer);
}