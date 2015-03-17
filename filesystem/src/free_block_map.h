/**
* @author Charlie Bloomfield
* March 15, 2015
*/

#include "bitmap.h"
#include "types.h"

typedef struct {

} FreeBlockMap;

FreeBlockMap* create_free_block_map(int numBlocks);

int find_free_block(FreeBlockMap* f);
void set_block_used(FreeBlockMap* f, int blockNum);
void free_block(FreeBlockMap* f, int blockNum);

void to_string(FreeBlockMap* f, byte buffer[]);
FreeBlockMap* from_string(byte buffer[]);