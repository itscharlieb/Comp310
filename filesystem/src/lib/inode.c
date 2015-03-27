/**
* @author Charlie Bloomfield
* March 14, 2015
*/

#include "../include/inode.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//TODO make deep malloc of inode
Inode* init_inode(){
	Inode* inode = malloc(sizeof(Inode));
	
	return inode;
}

void clear_inode(Inode* inode){
	inode->size = 0;
	inode->mode = 0;
	inode->linkCount = 0;
	inode->uid = 0;
	inode->gid = 0;

	int i;
	for(i = 0; i < MAX_NUM_DIRECT_POINTERS; i++){
		inode->directPointers[i] = 0;
	}

	inode->indirect = 0;
}

//these are used to write/read Inodes to/from disk 
void inode_to_string(Inode* inode, byte* buffer){
	memcpy(buffer, inode, INODE_SIZE);
}

Inode* inode_from_string(byte* buffer){
	Inode* inode = (Inode*)malloc(sizeof(Inode));
	memcpy(inode, buffer, INODE_SIZE);
	return inode;
}

//stores the indirect data block numbers in the parameter dataBlocks array
void read_indirect_data_blocks(byte* indirectBlockBuffer, int* dataBlocks, int numIndirectBlocks){
	memcpy(dataBlocks, indirectBlockBuffer, numIndirectBlocks * HALF_WORD_SIZE);
}


