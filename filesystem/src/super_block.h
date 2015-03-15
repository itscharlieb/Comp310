/**
* @author Charlie Bloomfield
* March 15, 2015
*/

typedef struct {
	unsigned int magic;
	unsigned int blockSize;
	unsigned int numBlocks;
	unsigned int inodeTableLength;
	unsigned int rootDirerctory;
} superBlock;

superBlock* create_super_block();

//used to write and read the super block from disk
void to_string(superBlock* sb);
void from_string(superBlock* sb, char buffer[])