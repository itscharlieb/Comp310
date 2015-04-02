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

#define MY_MALLOC_OFFSET 5 //4 for a size field, 1 for a boolean free field
#define TRUE 1
#define FALSE 0

typedef unsigned char byte;

extern char* myMallocError;
int myMallocPolicy = 0;
unsigned int numAllocatedBytes = 0;
unsigned int numFreeBytes = 0;
unsigned int largestFreeBlockSize = 0;
unsigned int programBreak;
unsigned int programEnd;
void* headFreeBlock = NULL;

/**********************************************************************************/

void* best_fit(int size){
	if(headFreeBlock == NULL){
		return NULL;
	}

	int bestFitSize = 128000; //max heap size
	void* bestFitBlock = NULL;
	void* tmpBlock = headFreeBlock;
	while(tmpBlock != NULL){
		int blockSize = *(int*)(tmpBlock);
		if(blockSize > size && blockSize < bestFitSize){
			bestFitBlock = tmpBlock;
			bestFitSize = blockSize;
		}

		tmpBlock = *(byte**)(tmpBlock + MY_MALLOC_OFFSET + 4);
	}
	
	return tmpBlock;
}

void* first_fit(int size){
	if(headFreeBlock == NULL){
		return NULL;
	}
	
	void* tmpBlock = headFreeBlock;
	while(tmpBlock != NULL){
		if(*(int*)(tmpBlock) >= size){
			return tmpBlock;
		}

		tmpBlock = *(byte**)(tmpBlock + MY_MALLOC_OFFSET + 4);
	}
	return NULL;
}

void* allocate_new_block(int size){
	printf("[my_malloc] Allocating [%d] bytes in the heap.\n", size);
	fflush(stdout);

	void* newBlock = sbrk(size + MY_MALLOC_OFFSET);

	//assign block fields
	int* sizeField = (int*)newBlock;
	*sizeField = size;
	int* freeField = (int*)(newBlock + 4);
	*freeField = FALSE;

	numAllocatedBytes += size;

	void* object = (void*)(newBlock + MY_MALLOC_OFFSET);
	return object;
}


//allocates size bytes on the heap
void* my_malloc(int size){
	void* recyclyedFreeBlock;
	if(myMallocPolicy){
		if((recyclyedFreeBlock = best_fit(size)) == NULL){
			return allocate_new_block(size);
		}
	}
	else{
		if((recyclyedFreeBlock = first_fit(size)) == NULL){
			return allocate_new_block(size);
		}
	}

	*(int*)recyclyedFreeBlock = size;
	*(byte*)(recyclyedFreeBlock + 4) = FALSE;
}

//returns the address of the next free block after the paramter object
void* find_next_free_block(void* block){
	return NULL;
}

//returns the address of the previous free block before the parameter object
void* find_previous_free_block(void* block){
	return NULL;
}

//frees data associated with the freedObject at the parameter address
void my_free(void* freedObject){
	byte* freedBlock = (byte*)(freedObject - MY_MALLOC_OFFSET);

	*(freedBlock + 4) = TRUE; //assign freedBlock free field to true

	//cast these next two pointers to byte pointers so we can do arithmetic on them
	byte* previousFreeBlock = (byte*)find_previous_free_block(freedBlock);
	byte* nextFreeBlock = (byte*)find_next_free_block(freedBlock);

	//deal with previous block
	if(previousFreeBlock != NULL){
		*(byte**)(previousFreeBlock + MY_MALLOC_OFFSET + 4) = freedBlock;
		*(byte**)(freedBlock + MY_MALLOC_OFFSET) = previousFreeBlock;
	}
	else{
		*(byte**)(freedBlock + MY_MALLOC_OFFSET) = NULL;
	}

	//deal with next block
	if(nextFreeBlock != NULL){
		*(byte**)(nextFreeBlock + MY_MALLOC_OFFSET) = freedBlock;
		*(byte**)(freedBlock + MY_MALLOC_OFFSET + 4) = nextFreeBlock;
	}
	else{
		*(byte**)(freedBlock + MY_MALLOC_OFFSET) = NULL;
	}

	numAllocatedBytes -= *(int*)freedBlock;
}

//specifies the free block list policy
void my_mall_opt(int policy){
	myMallocPolicy = policy;
}

//prints the current heap state to stdout
void my_mall_info(){
	printf("\n-------------------------------------------\n");
	printf("[my_mall_info] Printing current heap state.\n");
	printf("Number of bytes allocated = [%d].\n", numAllocatedBytes);
	printf("Number of free bytes = [%d].\n", numFreeBytes);
	printf("Size of the largest free block = [%d].\n", largestFreeBlockSize);
	//printf("Current program break = [%d].\n", sbrk(0));
	printf("-------------------------------------------\n\n");
}

#endif
