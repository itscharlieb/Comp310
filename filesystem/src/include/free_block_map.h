/**
* @author Charlie Bloomfield
* March 15, 2015
*/

#include "bitmap.h"
#include "types.h"

typedef struct {
	Bitmap* freeBlockBitmap;
} FreeBlockMap;

void FBM_init(int numBlocks);

int FBM_find_free_block();
void FBM_set_block_used(int blockNum);
void FBM_clear_block(int blockNum);

void FBM_to_string(byte* buffer);
void FBM_from_string(byte* buffer);