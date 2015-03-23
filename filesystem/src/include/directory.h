/**
* @author Charlie Bloomfield
* March 15, 2015
*/

#define MAX_FILE_NAME_LENGTH 16
#define MAX_FILE_EXTENSION_LENGTH 4
#define MAX_NUM_FILES 100

//size of directory entry to disk is 22 bytes. Linked list of directory entries
typedef struct {
	byte fileName[MAX_FILE_EXTENSION_LENGTH + MAX_FILE_EXTENSION_LENGTH];
	half_word inodeNum;
} DirectoryEntry;

typedef struct {
	DirectoryEntry directoryEntryTable[MAX_NUM_FILES];
	int fileIDLoopCounter;
} Directory;

//directory deals with mappnig file names to inode numbers

Directory* create_directory();

//return -1 if the file does not exist
int get_inode_number_from_directory(Directory* d, char* fileName);
void add_file_to_directory(Directory* d, char* fileName, int inodeNum);
void remove_file_from_directory(Directory* d, char* fileName);

//for looping through a directory
void reset_file_id_loop_pointer();
int get_next_file_id();

void directory_to_string(Directory* d, byte* buffer);
Directory* directory_from_string(byte* buffer);