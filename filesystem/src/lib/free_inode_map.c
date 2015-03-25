/**
* @author Charlie Bloomfield
* March 25, 2015
*/

#include "../include/free_inode_map.h"
#include <string.h>
#include <stdlib.h>

static FreeInodeMap* fim;

void FIM_init(int numInodes){
	fim->freeInodeBitmap = init_bitmap(numInodes);
	fim->size = numInodes;
}

int FIM_find_free_inode(){
	return find_free_bit(fim->freeInodeBitmap);
}

void FIM_set_inode_used(int inodeNum){
	set_bit(fim->freeInodeBitmap, inodeNum);
}

void FIM_clear_inode(int inodeNum){
	clear_bit(fim->freeInodeBitmap, inodeNum);
}

void FIM_to_string(byte* buffer){
	memcpy(buffer, fim->freeInodeBitmap->bits, fim->size);
}

void FIM_from_string(byte* buffer, int size){
	fim->size = size;
	memcpy(fim->freeInodeBitmap->bits, buffer, (size / 8));
}