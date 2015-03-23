/**
* @author Charlie Bloomfield
* March 21, 2015
*/

#include "directory.h"
#include <string.h>

static Directory* root;

void DIR_init(){
	root = (Directory*)malloc(sizeof(Directory));
}

int DIR_get_inode_number(char* fileName){
	int i;
	DirectoryEntry* tmp;

	//search through the 100 files and look for the filename
	for(i = 0; i < MAX_NUM_FILES; i++){
		tmp = root->directoryEntryTable[i]
		if(tmp == NULL){
			return;
		}
		if(strcmp(tmp->fileName, fileName) == 0){
			return tmp->inodeNum;
		}
	}
	return -1;
}

/*
* Moves all of the directory entries to the front of the DirectoryEntryTable array
*/
void cascase_directory_entries_forward(){
	int i, numDeletedFiles;
	DirectoryEntry* tmp;

	for(i = 0; i < MAX_NUM_FILES; i++){
		if(tmp == NULL){
			numDeletedFiles ++;
		}

		//cascade
		else{
			root->directoryEntryTable[i - numDeletedFiles] = tmp;
		}
	}
}

void DIR_add_file(char* fileName, int inodeNum){
	int i;
	DirectoryEntry* tmp;

	for(i = 0; i < MAX_NUM_FILES; i++){
		tmp = root->directoryEntryTable[i];

		//if found an available entry
		if(tmp == NULL){
			tmp = (DirectoryEntry*)malloc(sizeof(DirectoryEntry));
			memcpy(fileName, tmp->fileName);
			tmp->inodeNum = inodeNum;
		}
	}

	printf("Directory already contains the maximum number of file entries.\n")
}

//frees the directory entry associated with the specified fileName from memory
void DIR_remove_file(char* fileName){

	int i;
	DirectoryEntry* tmp;

	for(i = 0; i < MAX_NUM_FILES; i++){
		tmp = root->directoryEntryTable[i];

		if(tmp != NULL){
			if(strcmp(tmp->fileName, fileName) == 0){
				free(tmp);
				break;
			}
		}
	}

	cascase_directory_entries_forward(d);
}

/*
* copy the next fileName into the parameter fileNameBuffer. If this is the final file in the directory,
* return 0 and reset the loop counter. Else, return 1.
*/
int DIR_get_next_file_name(char* fileNameBuffer){
	DirectoryEntry* tmp;
	tmp = root->directoryEntryTable[fileIDLoopCounter];

	//if we've looped through the entire directory
	if(tmp == null){
		reset_file_id_loop_pointer(d);
		root->fileIDLoopCounter ++; //increment loop counter
		return 0;
	}
	else{
		strcpy(fileNameBuffer, tmp->fileName);
		root->fileIDLoopCounter = 0; //reset loop counter
		return 1;
	}
}

void DIR_to_string(byte* buffer){

}

void DIR_from_string(byte* buffer){

}