/**
* @author Charlie Bloomfield
* March 17, 2015
*/

#include "super_block.h"

SuperBlock* create_super_block(){
}

//used to write and read the super block from disk
void super_block_to_string(SuperBlock* sb, byte* buffer){
	write_half_word_as_bytes(sb->magic, buffer);
	buffer += HALF_WORD_SIZE;
	write_half_word_as_bytes(sb->blockSize, buffer);
	buffer += HALF_WORD_SIZE;
	write_half_word_as_bytes(sb->numBlocks, buffer);
	buffer += HALF_WORD_SIZE;
	write_half_word_as_bytes(sb->rootDirectoryBlockNum, buffer);
	buffer += HALF_WORD_SIZE;
	write_mem_word_as_bytes(sb->inodeTableLength, buffer);
}

//used to 
SuperBlock* super_block_from_string(byte* buffer){
	SuperBlock* sb = malloc(sizeof(SuperBlock));
}