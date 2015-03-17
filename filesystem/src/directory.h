/**
* @author Charlie Bloomfield
* March 15, 2015
*/

typedef struct {
	//TODO
} Directory;

//directory deals with mappnig file names to inode numbers

Directory* create_directory();

//return -1 if the file does not exist
int get_inode_number_from_directory(Directory* d, char* fileName);
void add_file_to_directory(Directory* d, char* fileName, int inodeNum);
void remove_file_from_directory(Directory* d, char* fileName);
void directory_to_string(Directory* d, byte* buffer);
Directory* directory_from_string(byte* buffer);

void reset_file_id_loop_pointer();
int get_next_file_id();