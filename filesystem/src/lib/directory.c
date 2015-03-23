/**
* @author Charlie Bloomfield
* March 21, 2015
*/

#include <string.h>

Directory* create_directory(){
	Directory* d = (Directory*)malloc(sizeof(Directory));

	return d;
}

int get_inode_number_from_directory(Directory* d, char* fileName){
	int i;
	DirectoryEntry* tmp;

	//search through the 100 files and look for the filename
	for(i = 0; i < MAX_NUM_FILES; i++){
		tmp = d->directoryEntryTable[i]
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
void cascase_directory_entries_forward(Directory* d){
	int i, numDeletedFiles;
	DirectoryEntry* tmp;

	for(i = 0; i < MAX_NUM_FILES; i++){
		if(tmp == NULL){
			numDeletedFiles ++;
		}

		//cascade
		else{
			d->directoryEntryTable[i - numDeletedFiles] = tmp;
		}
	}
}

void add_file_to_directory(Directory* d, char* fileName, int inodeNum){
	int i;
	DirectoryEntry* tmp;

	for(i = 0; i < MAX_NUM_FILES; i++){
		tmp = d->directoryEntryTable[i];

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
void remove_file_from_directory(Directory* d, char* fileName){

	int i;
	DirectoryEntry* tmp;

	for(i = 0; i < MAX_NUM_FILES; i++){
		tmp = d->directoryEntryTable[i];

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
int get_next_file_name(Directory* d, char* fileNameBuffer){
	DirectoryEntry* tmp;
	tmp = d->directoryEntryTable[fileIDLoopCounter];

	//if we've looped through the entire directory
	if(tmp == null){
		reset_file_id_loop_pointer(d);
		d->fileIDLoopCounter ++; //increment loop counter
		return 0;
	}
	else{
		strcpy(fileNameBuffer, tmp->fileName);
		d->fileIDLoopCounter = 0; //reset loop counter
		return 1;
	}
}

void directory_to_string(Directory* d, byte* buffer){

}

Directory* directory_from_string(byte* buffer){

}