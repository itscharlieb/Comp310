/**
* @author Charlie Bloomfield
* March 25, 2015
*/

#include "../include/free_block_map.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//TODO size is a CONSTANT

static FreeBlockMap* fbm;

void FBM_init(int numBlocks){
	fbm = (FreeBlockMap*)malloc(sizeof(FreeBlockMap));
	fbm->freeBlockBitmap = bitmap_init(numBlocks);
	fbm->size = numBlocks;
}

//returns -1 if there are no free blocks
int FBM_find_free_block(){
	return find_free_bit(fbm->freeBlockBitmap);
}

void FBM_set_block_used(int blockNum){
	set_bit(fbm->freeBlockBitmap, blockNum);
}

void FBM_clear_block(int blockNum){
	clear_bit(fbm->freeBlockBitmap, blockNum);
}

void FBM_to_string(byte* buffer){
	memcpy(buffer, fbm->freeBlockBitmap->bits, (fbm->size) / 8);
}

void FBM_from_string(byte* buffer, int size){
	fbm = (FreeBlockMap*)malloc(sizeof(FreeBlockMap));
	fbm->size = size;
	memcpy(fbm->freeBlockBitmap->bits, buffer, (size / 8));
}