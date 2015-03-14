/**
* @author Charlie Bloomfield
* March 14, 2015
*/

#define NUM_DIRECT_POINTERS 12

typedef struct {
	int mode, linkCount, gid, size;
	int directPointers[NUM_DIRECT_POINTERS];
	inode* indirect;
} inode;

//TODO Probably do not need this file at all, it's not really providing any helpful functionality

inode* createInode(){
	inode* i = malloc(sizeof(inode))
}

int getMode(inode* i){
	return i->mode;
}

int getLinkCount(inode* i){
	return i->linkCount;
}

int getGID(inode* i){
	return i->gid;
}

int getSize(inode* i){
	return i->size;
}

int getDirect(inode* i, int index){
	return i->directPointers[index];
}

int[] getDirects(inode* i){
	return i->directPointers;
}

inode* getIndirect(inode* i){
	return i->indirect;
}

