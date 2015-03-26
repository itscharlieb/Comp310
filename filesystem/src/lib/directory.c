/**
* @author Charlie Bloomfield
* March 21, 2015
*/

#include "../include/directory.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static Directory* root;

void DIR_init(){
	root = (Directory*)malloc(sizeof(Directory));
	//root->directoryEntryTable = {NULL};
	root->fileIDLoopCounter = 0;
}

//0 if false, non zero otherwise
int directory_entry_is_empty(DirectoryEntry* dirEntry){
	if(dirEntry->inodeNum == 0){
		return 1;
	}
	return 0;
}

int DIR_get_inode_number(const char* fileName){
	int i;
	DirectoryEntry* tmp;

	//search through the 100 files and look for the filename
	for(i = 0; i < MAX_NUM_FILES; i++){
		// printf("[DIR_get_inode_number] Assessing DIR entry [%d].\n", i);
		// fflush(stdout);

		tmp = root->directoryEntryTable[i];

		if(directory_entry_is_empty(tmp)){
			printf("[DIR_get_inode_number] Dir entry [%d] is not empty.\n", i);
			fflush(stdout);

			if(strcmp(tmp->fileName, fileName) == 0){
				return tmp->inodeNum;
			}
		}
	}

	printf("[DIR_get_inode_number] [%s] does not yet exist in the root directory.\n\n", fileName);
	fflush(stdout);
	return -1;
}

/*
* Moves all of the directory entries to the front of the DirectoryEntryTable array
*/
void cascase_directory_entries_forward(){
	int i, numDeletedFiles;
	DirectoryEntry* tmp;

	for(i = 0; i < MAX_NUM_FILES; i++){
		tmp = root->directoryEntryTable[i];
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
	printf("[DIR_add_file] Trying to add [%s] with inodeNum [%d] to directory.\n", fileName, inodeNum);
	fflush(stdout);

	int i;
	DirectoryEntry* tmp;

	for(i = 0; i < MAX_NUM_FILES; i++){
		tmp = root->directoryEntryTable[i];

		//if found an available entry
		if(!(directory_entry_is_empty(tmp))){
			tmp = (DirectoryEntry*)malloc(sizeof(DirectoryEntry));
			memcpy(fileName, tmp->fileName, (MAX_FILE_NAME_LENGTH + MAX_FILE_EXTENSION_LENGTH));
			tmp->inodeNum = inodeNum;

			printf("[DIR_add_file] Successfully assigned [%s] to DirectoryEntry [%d].\n\n", fileName, inodeNum);
			fflush(stdout);
			return;
		}
	}

	printf("[DIR_add_file] Directory already contains the maximum number of file entries.\n\n");
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

	cascase_directory_entries_forward();
}

/*
* copy the next fileName into the parameter fileNameBuffer. If this is the final file in the directory,
* return 0 and reset the loop counter. Else, return 1.
*/
int DIR_get_next_file_name(char* fileNameBuffer){
	DirectoryEntry* tmp;
	tmp = root->directoryEntryTable[root->fileIDLoopCounter];

	//if we've looped through the entire directory
	if(tmp == NULL){
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