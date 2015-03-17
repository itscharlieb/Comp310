/**
* @author Charlie Bloomfield
* March 14, 2015
*/

//Inode size = 37 byes

#include "types.h"
#define NUM_DIRECT_POINTERS 12
#define INODE_SIZE 37
#define MAX_ALLOCATED_DATA_BLOCKS 268

typedef struct {
	mem_word size; //max file size is approx 2^19 bytes
	half_word mode, linkCount, uid, gid;
	half_word directPointers[NUM_DIRECT_POINTERS];
	half_word indirect;
} Inode;

Inode* create_inode();
void reset_inode(Inode* i);

//returns the number of data blocks allocated to Inode i
int get_allocated_data_blocks(Inode* i, int blocks[]);

//these are used to write/read Inodes to/from disk 
void inode_to_string(Inode* i, byte* buffer);
Inode* inode_from_string(byte* buffer);
