/**
* @author Charlie Bloomfield
* March 15, 2015
*/

//directory deals with mappnig file names to inode numbers

Directory* create_directory();

//return -1 if the file does not exist
int get_inode_number(Directory* d, char* fileName);

void add_file(Directory* d, char* fileName, unsigned int inodeNumber);

//
void to_string(Directory* d);
void from_string(Directory* d, char buffer[]);