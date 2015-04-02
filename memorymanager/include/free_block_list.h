/**
* @author Charlie Bloomfield
* March 31, 2015s
**/

#ifndef FREE_BLOCK_LIST_H
#define FREE_BLOCK_LIST_H

typedef struct{
    unsigned int headAddress;
    unsigned int size;
    FreeBlockNode* previous;
    FreeBlockNode* next;
} FreeBlockNode;

typedef struct {
    FreeBlockNode* head;
    FreeBlockNode* tail;
    unsigned int size;
} FreeBlockList;

void FBL_init(FreeBlockList* fbl);
void FBL_add_block(FreeBlockList* fbl, FreeBlockNode* fbn);
void FBL_find_block_with_size(FreeBlockList* fbl, unsigned int size);
void FBL_remove_block(FreeBlockList* fbl, FreeBlockNode* fbn);

#endif
