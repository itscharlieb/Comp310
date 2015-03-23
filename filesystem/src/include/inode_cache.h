/**
* @author Charlie Bloomfield
* March 15, 2015
*/

#include <stdbool.h>
#include "inode.h"

typedef struct {
	//TODO
} InodeCache;

InodeCache* create_inode_cache();

void put_inode(InodeCache* ic, int inodeNum, Inode* i);
bool contains_inode(InodeCache* ic, int inodeNum);
Inode* get_inode(InodeCache* ic, int inodeNum);
Inode* remove_inode(InodeCache* ic, int inodeNum);