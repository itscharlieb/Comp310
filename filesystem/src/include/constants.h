/**
* @author Charlie Bloomfield
* March 23, 2015
**/

#ifndef CONSTANTS_H
#define CONSTANTS_H

//type constants
#define BYTE_SIZE 1
#define HALF_WORD_SIZE 2
#define MEM_WORD_SIZE 4

//inode constants
#define MAX_NUM_DIRECT_POINTERS 12
#define INODE_SIZE 37
#define MAX_ALLOCATED_DATA_BLOCKS 268

//data block constants
#define SUPER_BLOCK_BLOCK_NUM 0
#define FREE_INODE_MAP_BLOCK_NUM 4094
#define FREE_BLOCK_MAP_BLOCK_NUM 4095

//directory constants
#define MAX_NUM_FILES 100
#define MAX_FILE_NAME_LENGTH 20
//#define MAX_FILE_EXTENSION_LENGTH 4

//super block constants
#define MAGIC 0xAABB0005
#define BLOCK_SIZE 512
#define NUM_BLOCKS 4096
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)
#define INODE_TABLE_LENGTH (MAX_NUM_FILES / INODES_PER_BLOCK) + 1 //TODO should only be plus 1 if MNF % IPB == 0
#define ROOT_DIRECTORY_INODE_NUM 0

#endif