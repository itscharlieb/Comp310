/**
* @author Charlie Bloomfield
* March 15, 2015
*/

#include "types.h"
#define MAGIC 0xAABB0005
#define BLOCK_SIZE 512
#define NUM_BLOCKS 4096
#define INODE_TABLE_LENGTH 128
#define ROOT_DIRERCTORY_INODE_NUM 0
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)

typedef struct {
	half_word magic;
	half_word blockSize;
	half_word numBlocks;
	half_word rootDirectoryBlockNum;
	byte inodeTableLength;
} SuperBlock;

SuperBlock* init_super_block();

//used to write and read the super block from disk
void super_block_to_string(SuperBlock* sb, byte* buffer);
SuperBlock* super_block_from_string(byte* buffer);