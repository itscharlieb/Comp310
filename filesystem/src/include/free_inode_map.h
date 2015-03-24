/**
* @author Charlie Bloomfield
* March 14, 2015
*/

#include "types.h"
#include "bitmap.h"

typedef struct {
	Bitmap* freeInodeBitmap;
	unsigned int size;
} FreeInodeMap;

void FIM_init(int numInodes);

int FIM_find_free_inode();
void FIM_set_inode_used(int inodeNum);
void FIM_clear_inode(int inodeNum);

void FIM_to_string(byte* buffer);
void FIM_from_string(byte* buffer);