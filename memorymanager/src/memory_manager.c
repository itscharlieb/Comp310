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
#define MY_MALLOC_TAG_SIZE 10 //2*4 for size fields, 2*1 for boolean field

//for used blocks
#define SIZE_FIELD_OFFSET 0
#define FREE_FIELD_OFFSET 4
#define OBJECT_FIELD_OFFSET 5

//for free blocks
#define PREVIOUS_FREE_BLOCK_POINTER_OFFSET 5
#define NEXT_FREE_BLOCK_POINTER_OFFSET 9
#define PREVIOUS_BLOCK_SIZE_FIELD_OFFSET -5
#define PREVIOUS_BLOCK_FREE_FIELD_OFFSET -1

#define TRUE 1
#define FALSE 0

#define FIRST_FIT 1
#define BEST_FIT 0

extern char* myMallocError;
int myMallocPolicy = BEST_FIT;
unsigned int programBreak;
unsigned int programEnd;
int heapIsInitialized = FALSE;
void* headFreeBlock = NULL;

unsigned int numAllocatedBytes = 0;
unsigned int numFreeBytes = 0;
unsigned int maxFreeBlockSize = 0;
unsigned int numBlocks = 0;
unsigned int numFreeBlocks = 0;

/***********************************DUMPS*****************************************/
void dump_block(void* block){
	printf("----------Dumping block-------------\n");
	printf("Address = [%d].\n", block);
	printf("Front size field = [%d].\n", *(int*)(block + SIZE_FIELD_OFFSET));
	printf("Back size field = [%d].\n", *(int*)(block + block_object_size(block) + 5));
	printf("Front free field = [%d].\n", *(int*)(block + FREE_FIELD_OFFSET));
	printf("Back free field = [%d].\n", *(int*)(block + block_object_size(block) + 9));
	printf("------------------------------------\n");
}

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

	int previousBlockObjectSize = *(int*)(block + PREVIOUS_BLOCK_SIZE_FIELD_OFFSET);
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
	*(int*)(block + size + MY_MALLOC_TAG_SIZE + PREVIOUS_BLOCK_SIZE_FIELD_OFFSET) = size; //TODO refactor
}

//only works properly if the block size field is properly set
void set_block_free(void* block, int isFree){ 
	*(int*)(block + FREE_FIELD_OFFSET) = isFree;
	*(int*)(block + block_object_size(block) + MY_MALLOC_TAG_SIZE + PREVIOUS_BLOCK_FREE_FIELD_OFFSET) = isFree; //TODO refactor
}

void set_next_free_block(void* block, void* nextBlock){
	*(void**)(headFreeBlock + NEXT_FREE_BLOCK_POINTER_OFFSET) = nextBlock;
}

void set_previous_free_block(void* block, void* previousBlock){
	*(void**)(headFreeBlock + PREVIOUS_FREE_BLOCK_POINTER_OFFSET) = previousBlock;
}

/**********************************************************************************/

void* best_fit(int size){
	int bestFitSize = MAX_HEAP_SIZE;
	void* bestFitBlock = NULL;
	void* tmpBlock = headFreeBlock;
	while(tmpBlock != NULL){
		int blockObjectSize = block_object_size(tmpBlock);
		if(blockObjectSize >= size && blockObjectSize < bestFitSize){
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

	numAllocatedBytes += size;
	numBlocks ++;

	return (void*)(newBlock + OBJECT_FIELD_OFFSET);
}


//allocates size bytes on the heap
void* my_malloc(int size){
	if(!heapIsInitialized){
		programEnd = sbrk(0);
		heapIsInitialized = TRUE;
	}
	void* recyclyedFreeBlock;
	if(myMallocPolicy == BEST_FIT){
		if((recyclyedFreeBlock = best_fit(size)) == NULL){
			return allocate_new_block(size);
		}
	}
	else{
		if((recyclyedFreeBlock = first_fit(size)) == NULL){
			return allocate_new_block(size);
		}
	}
	//reassign values of recycled block
	//TODO deal with fragmentation
	set_block_object_size(recyclyedFreeBlock, size);
	set_block_free(recyclyedFreeBlock, FALSE);
	return (void*)(recyclyedFreeBlock + OBJECT_FIELD_OFFSET);
}

/*********************************************my_free*********************************************/

//returns the address of the next free block after the paramter object
void* find_next_free_block(void* block){
	void* tmpBlock = headFreeBlock;
	while(tmpBlock != NULL && tmpBlock < block){
		tmpBlock = next_free_block(tmpBlock);
	}
	return tmpBlock;
}

//returns the address of the previous free block before the parameter object
void* find_previous_free_block(void* block){
	if(headFreeBlock == NULL){
		return NULL;
	}

	void* tmpBlock = headFreeBlock;
	void* tmpNextBlock = next_free_block(headFreeBlock);
	while(tmpBlock != NULL && tmpNextBlock < block){
		tmpBlock = tmpNextBlock;

		if(tmpNextBlock != NULL){ //
			tmpNextBlock = next_free_block(tmpNextBlock);
		}
	}

	return tmpBlock;
}

//morphs two neighboring blocks into one if both are free. Deals with updating pointers of blocks in the free block list
void* morph_neighboring_blocks(void* left, void* right){
	if(block_is_free(left) && block_is_free(right)){
		numBlocks --;
		numFreeBlocks --;

	}
}

//combines the given free block with it's neighbors if either are free. Returns the address of the new free block (it only changes if the previous block is free)
void* morph_freed_block_with_neighbors(void* freedBlock){
	void* previousBlock = previous_block(freedBlock);
	void* morphedBlock = freedBlock;

	//first, morph with previous block
	if(previousBlock != NULL){
		if(block_is_free(previousBlock)){
			//update statistics counters
			numBlocks --;
			numFreeBlocks --;

			morphedBlock = previousBlock;

			//|SIZE|TRUE|PREVIOUS|SIZE|TRUE|-|SIZE|TRUE|FREED|SIZE|TRUE|-|SIZE|T/F|NEXT|SIZE|T/F|
			int morphedBlockSize = block_object_size(previousBlock) + block_object_size(freedBlock) + MY_MALLOC_TAG_SIZE;
			printf("morphedBlockSize after morph with previous = [%d].\n", morphedBlockSize);
			set_block_object_size(morphedBlock, (morphedBlockSize - MY_MALLOC_TAG_SIZE));
			//|SIZE|TRUE|-------------MORPHED----------------|SIZE|TRUE|-|SIZE|T/F|NEXT|SIZE|T/F|
		}
	}

	printf("Dumping morphed block after morph with previous.\n");
	dump_block(morphedBlock);

	//then morph with next block, making 3 block morph if necessary
	void* nextBlock = next_block(morphedBlock);
	if(nextBlock != NULL){
		if(block_is_free(nextBlock)){
			//update statistics counters
			numBlocks --;
			numFreeBlocks --;
			//|SIZE|TRUE|MORPHED|TRUE|FREE|-|SIZE|TRUE|NEXT|SIZE|TRUE|
			int morphedBlockSize = block_object_size(morphedBlock) + block_object_size(nextBlock) + MY_MALLOC_TAG_SIZE;
			printf("morphedBlockSize after morph with next = [%d].\n", morphedBlockSize);
			set_block_object_size(morphedBlock, (morphedBlockSize - MY_MALLOC_TAG_SIZE));
			//|SIZE|TRUE|------------MORPHED---------------|SIZE|TRUE|
		}
	}

	printf("Dumping morphed block after morph with next.\n");
	dump_block(morphedBlock);

	return morphedBlock;
}

//frees data associated with the freedObject at the parameter address
void my_free(void* freedObject){
	void* freedBlock = (void*)(freedObject - OBJECT_FIELD_OFFSET);
	set_block_free(freedBlock, TRUE);
	numFreeBlocks ++; //statistics counter

	freedBlock = morph_freed_block_with_neighbors(freedBlock);

	if(headFreeBlock == NULL){
		headFreeBlock = freedBlock;
		set_next_free_block(headFreeBlock, NULL);
		set_previous_free_block(headFreeBlock, NULL);
	}

	else{
		void* previousFreeBlock = find_previous_free_block(freedBlock);
		void* nextFreeBlock = find_next_free_block(freedBlock);

		//both of the neighboring free blocks can be null
		set_previous_free_block(freedBlock, previousFreeBlock);
		set_next_free_block(freedBlock, nextFreeBlock);

		if(previousFreeBlock != NULL){
			set_next_free_block(previousFreeBlock, freedBlock);
		}
		if(nextFreeBlock != NULL){
			set_previous_free_block(nextFreeBlock, freedBlock);
		}
	}

	//update heap stats
	int freedBlockObjectSize = block_object_size(freedBlock);
	printf("Freed block object size after morphing = [%d].\n", freedBlockObjectSize);

	numAllocatedBytes -= freedBlockObjectSize;
	numFreeBytes += freedBlockObjectSize;
	if(freedBlockObjectSize > maxFreeBlockSize){
		maxFreeBlockSize = freedBlockObjectSize;
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

//prints the current heap state to stdout
void my_mall_info(){
	printf("\n-------------------------------------------\n");
	printf("[my_mall_info] Printing current heap state.\n");
	printf("Number of bytes used = [%d].\n", numAllocatedBytes);
	printf("Number of bytes free = [%d].\n", numFreeBytes);
	printf("Number of blocks = [%d].\n", numBlocks);
	printf("Number of free blocks = [%d].\n", numFreeBlocks);
	printf("Size of the largest free block = [%d].\n", maxFreeBlockSize);
	//printf("Current program break = [%d].\n", sbrk(0));
	printf("-------------------------------------------\n\n");
}

#endif
