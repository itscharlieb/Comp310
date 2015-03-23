/**
* @author Charlie Bloomfield
* March 14, 2015
*/

#include "types.h"
#include "bitmap.h"

typedef struct {

} FreeInodeMap;

FreeInodeMap* create_free_inode_map(int numInodes);

int find_free_inode(FreeInodeMap* f);
void set_inode_used(FreeInodeMap* f, int inodeNum);
void free_inode(FreeInodeMap* f, int inodeNum);

void to_string(FreeInodeMap* f, byte* buffer);
FreeInodeMap* from_string(byte* buffer);