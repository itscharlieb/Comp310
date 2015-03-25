/**
* @author Charlie Bloomfield
* March 15, 2015
*/

#ifndef FILE_DESCRIPTOR_TABLE_H
#define FILE_DESCRIPTOR_TABLE_H

#include "constants.h"

//the write read pointers are not absolute addresses - they are relative to the size of the file
typedef struct {
	int readPtr;
	int writePtr; 
	int inodeNum; 
} FileDescriptor;

//will simply be an array of 100 file descriptors (there will only ever be 100 files)
typedef struct {
	FileDescriptor* fileDescriptors[MAX_NUM_FILES];
} FileDescriptorTable;

void FDT_init();

//returns -1 if the specified inodeNum does not exist in the inode table    
int FDT_get_file_id(int inodeNum);
int FDT_contains_file_id(int inodeNum);

FileDescriptor* FDT_get_file_descriptor(int fileID);
void FDT_remove_file_descriptor(int fileID);

//puts the inodeNum in the table AND returns the associated file descriptor
int FDT_put_file_descriptor(int inodeNum);

#endif