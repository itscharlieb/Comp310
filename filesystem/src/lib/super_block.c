/**
* @author Charlie Bloomfield
* March 17, 2015
*/

#include "../include/constants.h"
#include "../include/super_block.h"

//used to write and read the super block from disk
void super_block_to_string(byte* buffer){
	write_mem_word_as_bytes(buffer, MAGIC);
	buffer += MEM_WORD_SIZE;
	write_half_word_as_bytes(buffer, BLOCK_SIZE);
	buffer += HALF_WORD_SIZE;
	write_mem_word_as_bytes(buffer, NUM_BLOCKS);
	buffer += HALF_WORD_SIZE;
	write_half_word_as_bytes(buffer, ROOT_DIRECTORY_INODE_NUM);
	buffer += HALF_WORD_SIZE;
	write_mem_word_as_bytes(buffer, INODE_TABLE_LENGTH);
}