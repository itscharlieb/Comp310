/**
* @author Charlie Bloomfield
* March 25, 2015
*/

#include "free_inode_map.h"

static FreeInodeMap* fim;

void FIM_init(int numInodes){
	fim->freeInodeBitmap = init_bitmap(numInodes);
	fim->size = numInodes;
}

int FIM_find_free_inode(){
	return find_free_bit(fim->freeInodeBitmap)
}

void FIM_set_inode_used(int inodeNum){
	set_bit(fim->freeInodeBitmap, inodeNum);
}

void FIM_clear_inode(int inodeNum){
	clear_bit(fim->freeInodeBitmap, inodeNum);
}

void FIM_to_string(byte* buffer){
	memcpy(buffer, fmb->freeInodeBitmap->bits, fbm->size);
}

void FIM_from_string(byte* buffer, int numInodes){
	fbm->size = numBlocks;
	memcpy(fbm->freeInodeBitmap->bits, buffer, numInodes / 8)
}