/**
* @author Charlie Bloomfield
* March 25, 2015
*/

#include "../include/free_inode_map.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static FreeInodeMap* fim;

void FIM_init(int numInodes){
	fim = (FreeInodeMap*)malloc(sizeof(FreeInodeMap));
	fim->freeInodeBitmap = bitmap_init(numInodes);
	fim->size = numInodes;
}

int FIM_find_free_inode(){
	int inodeNum = find_free_bit(fim->freeInodeBitmap);

	printf("[FIM_find_free_inode] inodeNum [%d] is free.\n", inodeNum);
	fflush(stdout);
}

void FIM_set_inode_used(int inodeNum){
	printf("[FIM_set_inode_used] Setting inodeNum [%d] used.\n", inodeNum);
	fflush(stdout);

	set_bit(fim->freeInodeBitmap, inodeNum);
}

void FIM_clear_inode(int inodeNum){
	clear_bit(fim->freeInodeBitmap, inodeNum);
}

void FIM_to_string(byte* buffer){
	memcpy(buffer, fim->freeInodeBitmap->bits, fim->size);
}

void FIM_from_string(byte* buffer, int size){
	fim = (FreeInodeMap*)malloc(sizeof(FreeInodeMap));
	fim->size = size;
	memcpy(fim->freeInodeBitmap->bits, buffer, (size / 8));
}