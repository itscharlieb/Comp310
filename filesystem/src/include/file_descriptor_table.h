/**
* @author Charlie Bloomfield
* March 15, 2015
*/

#include "types.h"

typedef struct {
	int readPtr;
	int writePtr;
	int inodeNum; 
} FileDescriptor;

typedef struct {
	int size;
	FileDescriptor* fileDescriptors;
} FileDescriptorTable;

FileDescriptorTable* create_file_descriptor_table(FileDescriptorTable fdt, int size);

//returns -1 if the specified inodeNum does not exist in the inode table    
int get_file_id(FileDescriptorTable* fdt, int inodeNum);
bool contains_file_id(FileDescriptorTable* fdt, int inodeNum);

FileDescriptor* get_file_descriptor(FileDescriptorTable* fdt, int fileID);
FileDescriptor* remove_file_descriptor(FileDescriptorTable* fdt, int fileID);

//puts the inodeNum in the table AND returns the associated file descriptor
int put_file_descriptor(FileDescriptorTable* fdt, int inodeNum);