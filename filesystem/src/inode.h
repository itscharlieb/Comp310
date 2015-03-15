/**
* @author Charlie Bloomfield
* March 14, 2015
*/

#define NUM_DIRECT_POINTERS 12

typedef struct {
	int mode, linkCount, gid, size;
	int directPointers[NUM_DIRECT_POINTERS];
	Inode* indirect;
} Inode;

Inode* create_inode();

//these are used to write/read Inodes to/from disk 
void to_string(Inode* i, char buffer[]);
Inode* from_string(char buffer[]);
