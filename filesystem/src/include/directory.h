/**
* @author Charlie Bloomfield
* March 15, 2015
*/

#include "constants.h"
#include "types.h"

//size of directory entry to disk is 22 bytes. Linked list of directory entries
typedef struct {
	byte fileName[MAX_FILE_NAME_LENGTH + MAX_FILE_EXTENSION_LENGTH];
	half_word inodeNum;
} DirectoryEntry;

typedef struct {
	DirectoryEntry* directoryEntryTable[MAX_NUM_FILES];
	int fileIDLoopCounter;
} Directory;

//directory deals with mappnig file names to inode numbers

void DIR_init();

//return -1 if the file does not exist
int DIR_get_inode_number(char* fileName);
void DIR_add_file(char* fileName, int inodeNum);
void DIR_remove_file(char* fileName);
int DIR_get_next_file_name(char* fileNameBuffer);

void DIR_to_string(byte* buffer);
void DIR_from_string(byte* buffer);