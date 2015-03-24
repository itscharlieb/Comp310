/**
* @author Charlie Bloomfield
* March 14, 2015
*/

//Inode size = 37 byes

#include "types.h"
#include "constants.h"

typedef struct {
	mem_word size; //max file size is approx 2^19 bytes
	half_word mode, linkCount, uid, gid;
	half_word directPointers[NUM_DIRECT_POINTERS];
	half_word indirect;
} Inode;

Inode* init_inode();

//returns the number of data blocks allocated to Inode i
int get_allocated_data_blocks(Inode* i, int blocks[]);
void clear_inode(Inode* i);

//these are used to write/read Inodes to/from disk 
void inode_to_string(Inode* i, byte* buffer);
Inode* inode_from_string(byte* buffer);


//stores the indirect data block numbers in the parameter dataBlocks array
void read_indirect_data_blocks(byte* indirectBlockBuffer, int* dataBlocks, int numIndirectBlocks);

