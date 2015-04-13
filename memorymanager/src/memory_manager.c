/**
* @author Charlie Bloomfield
* March 31, 2015s
**/

/*
Useful information

Blocks have the following structure if they are not free:
-4 bytes for size field
-1 byte for free field
-n bytes for user data

If they are free, blocks have the following structure: (which breaks if the user asks for space on the heap less than 8 bytes)
-4 bytes for size field
-1 byte for free field
-4 bytes for pointer to previous free block
-4 bytes for pointer to next free block
*/

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include "memory_manager.h"

#include <unistd.h>
#include <stdio.h>

#define MAX_HEAP_SIZE 128000
#define MY_MALLOC_TAG_SIZE 2 * 8 //2*4 for size fields, 2*1 for boolean field

//for used blocks
#define SIZE_FIELD_OFFSET 0
#define FREE_FIELD_OFFSET 4
#define OBJECT_FIELD_OFFSET 8

//for free blocks
#define PREVIOUS_FREE_BLOCK_POINTER_OFFSET 8
#define NEXT_FREE_BLOCK_POINTER_OFFSET 12
#define PREVIOUS_BLOCK_SIZE_FIELD_OFFSET 8
#define PREVIOUS_BLOCK_FREE_FIELD_OFFSET 4

#define MIN_OBJECT_SIZE 8
#define MIN_FRAGMENTATION_OBJECT_SIZE_DIFFERENCE (MY_MALLOC_TAG_SIZE + MIN_OBJECT_SIZE)

#define TRUE 1
#define FALSE 0

#define FIRST_FIT 1
#define BEST_FIT 0

int myMallocPolicy = BEST_FIT;
unsigned int programBreak;
void* programEnd;
int heapIsInitialized = FALSE;
void* headFreeBlock = NULL;

unsigned int numUsedBytes = 0;
unsigned int numFreeBytes = 0;
unsigned int maxFreeBlockSize = 0;
unsigned int numBlocks = 0;
unsigned int numFreeBlocks = 0;

/*********************************EXTRA DEFINITIONS*******************************/

void dump_heap();
void dump_block(void* block);
void rebuild_free_list(void* freedBlock);

/**********************************BLOCK GETTERS**********************************/

//block helper functions for readability
int block_object_size(void* block){
	return *(int*)(block + SIZE_FIELD_OFFSET);
}

int block_is_free(void* block){
	return *(int*)(block + FREE_FIELD_OFFSET) == 0 ? FALSE : TRUE;
}

void* next_block(void* block){
	//if this block is the last block on the heap
	if(block + block_object_size(block) + MY_MALLOC_TAG_SIZE == sbrk(0)){	 
		return NULL;
	}

	return (void*)(block + block_object_size(block) + MY_MALLOC_TAG_SIZE);
}

void* previous_block(void* block){
	//if this block is the first block on the heap
	if(block == programEnd){
		return NULL;
	}

	int previousBlockObjectSize = *(int*)(block - PREVIOUS_BLOCK_SIZE_FIELD_OFFSET);
	return (void*)(block - previousBlockObjectSize - MY_MALLOC_TAG_SIZE);
}

void* next_free_block(void* block){
	return *(void**)(block + NEXT_FREE_BLOCK_POINTER_OFFSET);
}

void* previous_free_block(void* block){
	return *(void**)(block + PREVIOUS_FREE_BLOCK_POINTER_OFFSET);
}

/*********************************BLOCK SETTERS************************************/

void set_block_object_size(void* block, int size){
	*(int*)(block + SIZE_FIELD_OFFSET) = size;
	*(int*)(block + size + MY_MALLOC_TAG_SIZE - PREVIOUS_BLOCK_SIZE_FIELD_OFFSET) = size; //TODO refactor
}

//only works properly if the block size field is properly set
void set_block_free(void* block, int isFree){ 
	*(int*)(block + FREE_FIELD_OFFSET) = isFree;
	*(int*)(block + block_object_size(block) + MY_MALLOC_TAG_SIZE - PREVIOUS_BLOCK_FREE_FIELD_OFFSET) = isFree; //TODO refactor
}

void set_next_free_block(void* block, void* nextBlock){
	*(void**)(headFreeBlock + NEXT_FREE_BLOCK_POINTER_OFFSET) = nextBlock;
}

void set_previous_free_block(void* block, void* previousBlock){
	*(void**)(headFreeBlock + PREVIOUS_FREE_BLOCK_POINTER_OFFSET) = previousBlock;
}

/**********************************************************************************/

void* best_fit(int requestedSize){
	int bestFitSize = MAX_HEAP_SIZE;
	void* bestFitBlock = NULL;
	void* tmpBlock = headFreeBlock;
	while(tmpBlock != NULL){
		int blockObjectSize = block_object_size(tmpBlock);
		if(blockObjectSize >= requestedSize && blockObjectSize < bestFitSize){
			bestFitBlock = tmpBlock;
			bestFitSize = blockObjectSize;
		}

		tmpBlock = next_free_block(tmpBlock);
	}
	return bestFitBlock;
}

void* first_fit(int size){
	void* tmpBlock = headFreeBlock;
	while(tmpBlock != NULL){
		if(block_object_size(tmpBlock) >= size){
			return tmpBlock;
		}

		tmpBlock = next_free_block(tmpBlock);
	}
	return NULL;
}

void* allocate_new_block(int size){
	void* newBlock = sbrk(size + MY_MALLOC_TAG_SIZE);
	set_block_object_size(newBlock, size);
	set_block_free(newBlock, FALSE);

	numUsedBytes += size;
	numBlocks ++;

	dump_block(newBlock);

	return (void*)(newBlock + OBJECT_FIELD_OFFSET);
}

void fragment_recycled_block(void* recyclyedBlock, int requestedSize){
	printf("Fragmenting recycled block. New block size will be [%d].\n", requestedSize);
	dump_block(recyclyedBlock);

	int recycledBlockObjectSize = block_object_size(recyclyedBlock);
	if(recycledBlockObjectSize - requestedSize >= MIN_FRAGMENTATION_OBJECT_SIZE_DIFFERENCE){
		set_block_object_size(recyclyedBlock, requestedSize);
		set_block_free(recyclyedBlock, FALSE);

		void* fragmentedBlock = (void*)(recyclyedBlock + requestedSize + MY_MALLOC_TAG_SIZE);
		set_block_object_size(fragmentedBlock, (recycledBlockObjectSize - requestedSize - MY_MALLOC_TAG_SIZE));
		set_block_free(fragmentedBlock, TRUE);

		//check if the fragmentation changes the head free block
		if(headFreeBlock == recyclyedBlock){
			headFreeBlock = fragmentedBlock;
		}
		rebuild_free_list(fragmentedBlock);
	}
	else{
		//DO NOT CHANGE THE BLOCK SIZE IF NOT FRAGMENTING TO MAINTING LIST INTEGRITY
		set_block_free(recyclyedBlock, FALSE);
	}

	dump_heap();
}


//allocates size bytes on the heap
void* my_malloc(int requestedSize){
	if(!heapIsInitialized){
		programEnd = sbrk(0);
		heapIsInitialized = TRUE;
	}

	if(requestedSize < MIN_OBJECT_SIZE){
		requestedSize = MIN_OBJECT_SIZE;
	}

	void* recyclyedFreeBlock;
	if(myMallocPolicy == BEST_FIT){
		if((recyclyedFreeBlock = best_fit(requestedSize)) == NULL){
			return allocate_new_block(requestedSize);
		}
	}
	else{
		if((recyclyedFreeBlock = first_fit(requestedSize)) == NULL){
			return allocate_new_block(requestedSize);
		}
	}

	fragment_recycled_block(recyclyedFreeBlock, requestedSize);
	return (void*)(recyclyedFreeBlock + OBJECT_FIELD_OFFSET);
}

/*********************************************my_free*********************************************/

//returns the address of the next free block after the paramter object
void* find_next_free_block(void* block){
	if(headFreeBlock == NULL){
		return NULL;
	}

	void* tmpBlock = next_block(block);
	while(tmpBlock != NULL && !(block_is_free(tmpBlock))){
		tmpBlock = next_block(tmpBlock);
	}

	return tmpBlock;
}

//returns the address of the previous free block before the parameter object
void* find_previous_free_block(void* block){
	if(headFreeBlock == NULL){
		return NULL;
	}

	void* tmpBlock = previous_block(block);
	while(tmpBlock != NULL && !(block_is_free(tmpBlock))){
		tmpBlock = previous_block(tmpBlock);
	}

	return tmpBlock;
}

//repairs the fere list around a newly freed block
void rebuild_free_list(void* freedBlock){
	if(freedBlock < headFreeBlock){
		headFreeBlock = freedBlock;
	}

	void* previousFreeBlock = find_previous_free_block(freedBlock);
	void* nextFreeBlock = find_next_free_block(freedBlock);

	set_previous_free_block(freedBlock, previousFreeBlock);
	set_next_free_block(freedBlock, nextFreeBlock);

	if(previousFreeBlock != NULL){
		set_next_free_block(previousFreeBlock, freedBlock);
	}
	if(nextFreeBlock != NULL){
		set_previous_free_block(nextFreeBlock, freedBlock);
	}
}

//merges two neighboring free blocks by merging the right block into the left block
void merge_neighboring_free_blocks(void* left, void* right){
	void* mergedBlock = left;
	int mergedBlockObjectSize = block_object_size(left) + block_object_size(right) + MY_MALLOC_TAG_SIZE;
	set_block_object_size(mergedBlock, mergedBlockObjectSize);

	numBlocks --;
	numFreeBlocks --;
	numFreeBytes += MY_MALLOC_TAG_SIZE; //because mergeing adds one tag size to the block object size
}

//Combines the given free block with it's neighbors if either are free. 
//Returns the address of the new free block (it only changes if the previous block is free)
void* merge_freed_block_with_neighbors(void* freedBlock){
	void* previousBlock = previous_block(freedBlock);
	void* nextBlock = next_block(freedBlock);
	void* mergedBlock = freedBlock;

	if(previousBlock != NULL){
		//printf("Previous is not null.\n");
		if(block_is_free(previousBlock)){
			merge_neighboring_free_blocks(previousBlock, freedBlock);
			mergedBlock = previousBlock;
		}
	}

	if(nextBlock != NULL){
		//printf("Next is not null.\n");
		if(block_is_free(nextBlock)){
			merge_neighboring_free_blocks(mergedBlock, nextBlock);
		}
	}

	return mergedBlock;
}

//frees data associated with the freedObject at the parameter address
void my_free(void* freedObject){
	dump_heap();

	void* freedBlock = (void*)(freedObject - OBJECT_FIELD_OFFSET);
	if(block_is_free(freedBlock)){
		//my_malloc_error = "No freeing of already freed blocks in my program!";
		return;
	}

	numUsedBytes -= block_object_size(freedBlock);
	numFreeBytes += block_object_size(freedBlock);

	set_block_free(freedBlock, TRUE);
	numFreeBlocks ++; //statistics counter

	if(headFreeBlock == NULL){
		headFreeBlock = freedBlock;
		set_next_free_block(headFreeBlock, NULL);
		set_previous_free_block(headFreeBlock, NULL);
		dump_block(freedBlock);
	}

	else{
		freedBlock = merge_freed_block_with_neighbors(freedBlock);
		rebuild_free_list(freedBlock);
	}

	int mergedBlockObjectSize = block_object_size(freedBlock);
	if(mergedBlockObjectSize > maxFreeBlockSize){
		maxFreeBlockSize = mergedBlockObjectSize;
	}
}

//specifies the free block list policy
void my_mall_opt(int policy){
	if(policy == 0){
		myMallocPolicy = BEST_FIT;
	}
	else{
		myMallocPolicy = FIRST_FIT;
	}
}

/*******************************MY_MALL_INFO**********************************/

//prints the current heap state to stdout
void my_mall_info(){
	printf("\n-------------------------------------------\n");
	printf("[my_mall_info] Printing current heap state.\n");
	printf("Number of bytes used = [%d].\n", numUsedBytes);
	printf("Number of bytes free = [%d].\n", numFreeBytes);
	printf("Number of blocks = [%d].\n", numBlocks);
	printf("Number of free blocks = [%d].\n", numFreeBlocks);
	printf("Size of the largest free block = [%d].\n", maxFreeBlockSize);
	//printf("Current program break = [%d].\n", sbrk(0));
	printf("-------------------------------------------\n\n");
}

/***********************************DUMPS*****************************************/
void dump_block(void* block){
	printf("----------Dumping block-------------\n");
	printf("Address = [%p].\n", block);
	printf("Front size field = [%d].\n", *(int*)(block + SIZE_FIELD_OFFSET));
	printf("Back size field = [%d].\n", *(int*)(block + block_object_size(block) + MY_MALLOC_TAG_SIZE - PREVIOUS_BLOCK_SIZE_FIELD_OFFSET));
	printf("Front free field = [%d].\n", *(int*)(block + FREE_FIELD_OFFSET));
	printf("Back free field = [%d].\n", *(int*)(block + block_object_size(block) + MY_MALLOC_TAG_SIZE - PREVIOUS_BLOCK_FREE_FIELD_OFFSET));
	if(block_is_free(block)){
		printf("Previous free block is at address [%p].\n", previous_free_block(block));
		printf("Next free block is at address [%p].\n", next_free_block(block));
	}
	printf("------------------------------------\n");
}

void dump_heap(){
	printf("------------Dumping Heap--------------\n");
	void* tmpBlock = programEnd;
	while(tmpBlock != NULL){
		int isFree = block_is_free(tmpBlock);
		int size = block_object_size(tmpBlock);
		if(isFree){
			printf("Address [%p] - |%d|%d|%p|%p|%d|%d|\n", tmpBlock, size, isFree, previous_free_block(tmpBlock), next_free_block(tmpBlock), size, isFree);
		}
		else{
			printf("Address [%p] - |%d|%d|%d bytes|%d|%d|\n", tmpBlock, size, isFree, size, size, isFree);
		}
		//printf("Next block address = [%d].\n", next_block(tmpBlock));
		tmpBlock = next_block(tmpBlock);
	}
	printf("-----------------------------------\n");
}

#endif
