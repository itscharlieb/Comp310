/**
* @author Charlie Bloomfield
* March 14, 2015
**/

#include "sfs_api.h"
#include "disk_emu.h"
#include "bitmap.h"
#include "directory.h"

#define BLOCK_SIZE 512
#define NUM_BLOCKS 512
#define MAX_FILE_NAME_LENGTH 16
#define MAX_FILE_EXTENSION_LENGTH 3

const char fileName[] = 'itscharlieb FS';
directory* rootDirectory; 


/**********************************************************************/

int init_sfs(){
	//if unsuccessful load of the emulation disk
	if(init_disk(fileName, BLOCK_SIZE, NUM_DISK_BLOCKS) == -1){
		printf("Failed to initialize the file system");
		return -1;
	}

	char superBlockBuffer[BLOCK_SIZE];

	write_blocks(0, 1, superBlockBuffer);
	return 0;
}

int load_sfs(){
	//if unsuccessful creation of the emulation disk
	if(init_fresh_disk(fileName, BLOCK_SIZE, NUM_DISK_BLOCKS) == -1){
		printf("Failed to initialize the file system");
		return -1;
	}

	return 0;
}

/**
* makes an instance of the SFS either by loading one from disk or by creating a new one
*/
int mksfs(int fresh){
	//if fresh, then initialize a new disk 
	if(fresh){
		init_sfs();
	}

	//otherwise, load disk
	else{
		load_sfs();
	}

	return 0;	
}

/**********************************************************************************/

void open_existing_file(char* fileName, int inodeNumber){
	
}

void create_file(char* fileName){
	//creates and properly initializes a new inode
	inode* i = create_inode();

	add_file()
}

/**
* Opens a file with the parameter name. If the file with the specified name does not already exist,
* it is created. 
* @param name of the file to oper
*/
int sfs_fopen(char* name){
	int inodeNumber = get_inode_number(rootDirectory, name);

	//if the file does not already exists
	if(inodeNumber == -1){
		create_file(name);
	}

	//file already exists in the directory, open it
	else{
		open_existing_file(name, inodeNumber);
	}
}

/***********************************************************************************/

int sfs_fclose(int fileID){

}

int sfs_fwrite(int fileID, const char *buf, int length){

}

int sfs_fread(int fileID, char *buf, int length){

}

int sfs_fseek(int fileID, int offset){

}

int sfs_remove(char *file){

}

/**************************************************************/
int fileNamePointer = 0;

int sfs_get_next_filename(char* filename){

}

int sfs_get_file_size(const char* path){

}
