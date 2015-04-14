/**
* @author Charlie Bloomfield
* March 31, 2015s
**/

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

extern char* my_malloc_error;

//allocates size bytes on the heap
void* my_malloc(int size);

//frees data associated
void my_free(void* pointer);

//specifies the free block list policy
void my_mall_opt(int policy);

//prints the current heap state to stdout
void my_mall_info();

//0 for false, 1 for true
void set_debug(int debugMode);

#endif
