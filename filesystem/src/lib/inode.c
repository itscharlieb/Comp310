/**
* @author Charlie Bloomfield
* March 14, 2015
*/

#include "inode.h"

//TODO make deep malloc of inode
inode* create_inode(){
	inode* i = malloc(sizeof(inode))
	
	return i;
}

//returns the number of data blocks allocated to Inode i
int get_allocated_data_blocks(Inode* i, int blocks[]){

}

void clear_inode(Inode* i){

}

//these are used to write/read Inodes to/from disk 
void inode_to_string(Inode* i, byte* buffer);
Inode* inode_from_string(byte* buffer);

