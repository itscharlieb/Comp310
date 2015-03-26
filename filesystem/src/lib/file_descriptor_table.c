/**
* @author Charlie Bloomfield
* March 15, 2015
*/

#include "../include/file_descriptor_table.h"
#include <stdlib.h>

static FileDescriptorTable* fdt;

/*
For simplicity, he FDT is currently an array of 100 FileDescriptors. 
Each index in the array holds the inode with the same number (there are only 100 inodes in this file system).
If a file associated with inode 42 is open, the index 42 in the fdt will hold it's FileDescriptor.
*/

void FDT_init(){
	fdt = (FileDescriptorTable*)malloc(sizeof(FileDescriptorTable));

	int i;
	for(i = 0; i < MAX_NUM_FILES; i++){
		fdt->fileDescriptors[i] = NULL;
	}
}

//returns -1 if the specified inodeNum does not exist in the inode table    
int FDT_get_file_id(int inodeNum){
	return inodeNum;
}

int FDT_contains_file_id(int inodeNum){
	return (fdt->fileDescriptors + inodeNum) != NULL ? 1 : 0;
}

//puts the inodeNum in the table AND returns the associated file descriptor
int FDT_put_file_descriptor(int inodeNum){
	FileDescriptor* fd = fdt->fileDescriptors[inodeNum];
	fd = (FileDescriptor*)malloc(sizeof(FileDescriptor));
	fd->readPtr = 0;
	fd->writePtr = 0;
	fd->inodeNum = inodeNum;
	return inodeNum; 
}

FileDescriptor* FDT_get_file_descriptor(int fileID){
	return *(fdt->fileDescriptors + fileID);
}

void FDT_remove_file_descriptor(int fileID){
	free(fdt->fileDescriptors + fileID);
}