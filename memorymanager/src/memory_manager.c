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

typedef unsigned char byte;

extern char* myMallocError;
int myMallocPolicy = BEST_FIT;
unsigned int numAllocatedBytes = 0;
unsigned int numFreeBytes = 0;
unsigned int maxFreeBlockSize = 0;
unsigned int programBreak;
unsigned int programEnd;
void* headFreeBlock = NULL;

/**********************************BLOCK GETTERS**********************************/
//block helper functions for readability
int block_object_size(void* block){
	return *(int*)(block + SIZE_FIELD_OFFSET);
}

int block_is_free(void* block){
	return *(int*)(block + FREE_FIELD_OFFSET) == 0 ? FALSE : TRUE;
}

void* next_block(void* block){
	int nextBlockOffset = block_object_size(block) + MY_MALLOC_TAG_SIZE;
	return *(void**)(block + nextBlockOffset);
}

void* previous_block(void* block){
	int previousBlockObjectSize = *(int*)(block + PREVIOUS_BLOCK_SIZE_FIELD_OFFSET);
	return *(void**)(block - previousBlockObjectSize - MY_MALLOC_TAG_SIZE);
}

void* next_free_block(void* block){
	return *(byte**)(block + NEXT_FREE_BLOCK_POINTER_OFFSET);
}

void* previous_free_block(void* block){
	return *(byte**)(block + PREVIOUS_FREE_BLOCK_POINTER_OFFSET);
}

/*********************************BLOCK SETTERS************************************/
void set_block_size(void* block, int size){
	*(int*)(block + SIZE_FIELD_OFFSET) = size;
	*(int*)(block + size + MY_MALLOC_TAG_SIZE + PREVIOUS_BLOCK_SIZE_FIELD_OFFSET) = size; //TODO refactor
}

//only works properly if the block size field is properly set
void set_block_free(void* block, int isFree){ 
	*(int*)(block + FREE_FIELD_OFFSET) = isFree;
	*(int*)(block + block_object_size(block) + MY_MALLOC_TAG_SIZE + PREVIOUS_BLOCK_FREE_FIELD_OFFSET) = isFree; //TODO refactor
}

void set_next_free_block(void* block, void* nextBlock){
	*(byte**)(headFreeBlock + NEXT_FREE_BLOCK_POINTER_OFFSET) = nextBlock;
}

void set_previous_free_block(void* block, void* previousBlock){
	*(byte**)(headFreeBlock + PREVIOUS_FREE_BLOCK_POINTER_OFFSET) = previousBlock;
}

/**********************************************************************************/

void* best_fit(int size){
	printf("[best_fit] Searching for a block with size <= [%d] bytes.\n", size);

	int bestFitSize = MAX_HEAP_SIZE;
	void* bestFitBlock = NULL;
	void* tmpBlock = headFreeBlock;
	while(tmpBlock != NULL){
		int blockObjectSize = block_object_size(tmpBlock);
		if(blockObjectSize >= size && blockObjectSize < bestFitSize){
			bestFitBlock = tmpBlock;
			bestFitSize = blockObjectSize;

			printf("[best_fit] Found a sufficient block at address [%d] with size [%d].\n", bestFitBlock, bestFitSize);
			fflush(stdout);
		}

		tmpBlock = next_free_block(tmpBlock);
	}
	return bestFitBlock;
}

void* first_fit(int size){
	printf("[best_fit] Searching for a block with size <= [%d] bytes.\n", size);

	void* tmpBlock = headFreeBlock;
	while(tmpBlock != NULL){
		if(block_object_size(tmpBlock) >= size){
			printf("[best_fit] Found a sufficient block at address [%d] with size [%d].\n", (int)tmpBlock, *(int*)(tmpBlock));
			return tmpBlock;
		}

		tmpBlock = next_free_block(tmpBlock);
	}
	return NULL;
}

void* allocate_new_block(int size){
	printf("[allocate_new_block] No sufficient free block found. Allocating [%d] bytes in the heap.\n", size);
	fflush(stdout);

	void* newBlock = sbrk(size + MY_MALLOC_TAG_SIZE);
	set_block_size(newBlock, size);
	set_block_free(newBlock, FALSE);

	numAllocatedBytes += size;

	return (void*)(newBlock + OBJECT_FIELD_OFFSET);
}


//allocates size bytes on the heap
void* my_malloc(int size){
	printf("[my_malloc] Mallocing a block of size [%d].\n", size);

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
	set_block_size(recyclyedFreeBlock, size);
	set_block_free(recyclyedFreeBlock, FALSE);
	return *(void**)(recyclyedFreeBlock + OBJECT_FIELD_OFFSET);
}

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
	void* tmpBlock = headFreeBlock;
	void* tmpNextBlock = next_free_block(headFreeBlock);
	while(tmpBlock != NULL && tmpNextBlock < block){
		tmpBlock = tmpNextBlock;
		tmpNextBlock = next_free_block(tmpNextBlock);
	}
	return tmpBlock;
}

//combines the given free block with it's neighbors if either are free. Returns the address of the new free block (it only changes if the previous block is free)
void* morph_freed_block_with_neighbors(void* freedBlock){
	void* morphedBlock = freedBlock;
	void* previousBlock = previous_block(freedBlock);
	void* nextBlock = next_block(freedBlock);
	if(previousBlock != NULL && block_is_free(previousBlock)){
		morphedBlock = previous_block;

		//|SIZE|TRUE|PREVIOUS|SIZE|TRUE|-|SIZE|TRUE|FREED|SIZE|TRUE|-|SIZE|T/F|NEXT|SIZE|T/F|
		set_block_size(morphedBlock, (block_object_size(previous_block) + block_object_size(freedBlock) + MY_MALLOC_TAG_SIZE));
		//|SIZE|TRUE|-------------MORPHED----------------|SIZE|TRUE|-|SIZE|T/F|NEXT|SIZE|T/F|
	}
	if(nextBlock != NULL && block_is_free(nextBlock)){
		//|SIZE|TRUE|MORPHED|TRUE|FREE|-|SIZE|TRUE|NEXT|SIZE|TRUE|
		set_block_size(morphedBlock, (block_object_size(morphedBlock) + block_object_size(nextBlock) + MY_MALLOC_TAG_SIZE));
		//|SIZE|TRUE|------------MORPHED---------------|SIZE|TRUE|
	}

	return morphedBlock;
}

//frees data associated with the freedObject at the parameter address
void my_free(void* freedObject){
	void* freedBlock = (void*)(freedObject - OBJECT_FIELD_OFFSET);
	set_block_free(freedBlock, TRUE);

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
	printf("Number of free bytes = [%d].\n", numFreeBytes);
	printf("Size of the largest free block = [%d].\n", maxFreeBlockSize);
	//printf("Current program break = [%d].\n", sbrk(0));
	printf("-------------------------------------------\n\n");
}

#endif
