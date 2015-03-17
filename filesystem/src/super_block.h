/**
* @author Charlie Bloomfield
* March 15, 2015
*/

#include "types.h"
#define MAGIC 0xAABB0005

//hard coded for now, could be flexible based on variable block size / inode size later on
#define MAX_INODES_PER_BLOCK 13 //512 bytes per block / 37 bytes per inode = 13
#define INODE_TABLE_LENGTH 128

typedef struct {
	half_word magic;
	half_word blockSize;
	half_word numBlocks;
	half_word rootDirBlockNum;
	byte inodeTableLength;
} SuperBlock;

SuperBlock* create_super_block();

//used to write and read the super block from disk
void super_block_to_string(SuperBlock* sb, byte* buffer);
SuperBlock* super_block_from_string(byte* buffer);