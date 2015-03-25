/**
* @author Charlie Bloomfield
* March 15, 2015
*/

#include "../include/inode_cache.h"
#include <stdbool.h>
#include <stdlib.h>

static InodeCache* ic;

/*
Cache is implemented as an array of 100 entries, with the inode number
as the slot where it's respective inode will be located if it is in memory.
Later on, a more sophisticated cache should be implemented
*/

void IC_init(){
	ic = (InodeCache*)malloc(sizeof(InodeCache));
}

void IC_put(int inodeNum, Inode* i){
	*(ic->inodes + inodeNum) = i;
}

bool IC_contains(int inodeNum){
	return *(ic->inodes + inodeNum) != NULL;
}

Inode* IC_get(int inodeNum){
	return *(ic->inodes + inodeNum);
}

void IC_remove(int inodeNum){
	Inode* i = *(ic->inodes + inodeNum);
	*(ic->inodes + inodeNum) = NULL;
	free(i);
}