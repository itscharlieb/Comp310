/**
* @author Charlie Bloomfield
* March 14, 2015
**/

#include "sfs_api.h"
#include "disk_emu.h"

#define BLOCK_SIZE 512
#define NUM_BLOCKS 512
#define MAX_FILE_NAME_LENGTH 16
#define MAX_FILE_EXTENSION_LENGTH 3

const char fileName[] = 'The itscharlieb File System';

/**********************************************************************/

int init_sfs(){
	//if unsuccessful load of the emulation disk
	if(init_disk(fileName, BLOCK_SIZE, NUM_DISK_BLOCKS) == -1){
		printf("Failed to initialize the file system");
		return -1;
	}

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

int sfs_fopen(char *name){

}

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
