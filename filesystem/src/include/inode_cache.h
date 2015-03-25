/**
* @author Charlie Bloomfield
* March 15, 2015
*/

#include "constants.h"
#include "inode.h"
#include <stdbool.h>

typedef struct {
	Inode* inodes[MAX_NUM_FILES];
} InodeCache;

void IC_init();

void IC_put(int inodeNum, Inode* i);
bool IC_contains(int inodeNum);
Inode* IC_get(int inodeNum);
void IC_remove(int inodeNum);