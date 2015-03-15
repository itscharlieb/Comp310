/**
* @author Charlie Bloomfield
* March 15, 2015
*/

typedef struct {

} FreeBlockManager;

int find_free_block(FreeBlockManager* f);
void set_used(FreeBlockManager* f, int blockNumber);

void to_string(FreeBlockManager* f);
void from_string(FreeBlockManager* f, char buffer[]);