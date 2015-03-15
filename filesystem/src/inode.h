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

//TODO Probably do not need this file at all, it's not really providing any helpful functionality, just adding indirection

//TODO make deep malloc of inode
inode* create_inode(){
	inode* i = malloc(sizeof(inode))
}

int get_mode(inode* i){
	return i->mode;
}

int get_link_count(inode* i){
	return i->linkCount;
}

int get_gid(inode* i){
	return i->gid;
}

int get_size(inode* i){
	return i->size;
}

int get_direct(inode* i, int index){
	return i->directPointers[index];
}

int* get_directs(inode* i){
	return i->directPointers;
}

inode* get_indirect(inode* i){
	return i->indirect;
}

