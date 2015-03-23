/**
* @author Charlie Bloomfield
* March 14, 2015
**/

#include "sfs_api.h"
#include "disk_emu.h"
#include "bitmap.h"
#include "inode.h"
#include "directory.h"
#include "types.h"
#include "super_block.h"
#include "util.h"

#define MAX_OPEN_FILES 100

//constant block numbers
#define SUPER_BLOCK_BLOCK_NUM 0
#define FREE_INODE_MAP_BLOCK_NUM 4094
#define FREE_BLOCK_MAP_BLOCK_NUM 4095

const char fileName[] = "itscharlieb FS";


/***********************************INIT**************************************/

void init_free_inode_map(byte* buffer){
	FIM_init();
	FIM_set_inode_used(ROOT_DIRECTORY_INODE_NUMBER);
	FIM_to_string(buffer)
	write_blocks(FREE_INODE_MAP_BLOCK_NUM, 1, buffer);
}

void init_free_block_map(byte* buffer){
	FBM_init();
	FBM_set_block_used(SUPER_BLOCK_BLOCK_NUM);

	int i;
	// blocks 1-128 hold inodes, not to be used for data
	for(int i = 1; i < 129; i++){
		FBM_set_block_used(i);
	}

	FBM_set_block_used(FREE_BLOCK_MAP_BLOCK_NUM);
	FBM_set_block_used(FREE_INODE_MAP_BLOCK_NUM);

	FBM_to_string(buffer);
	write_blocks(FREE_BLOCK_MAP_BLOCK_NUM, 1, buffer);
}

int init_super_block(byte* buffer){
	//init super block, write it to disk
	superBlock = create_super_block();
	super_block_to_string(superBlock, buffer);
	write_blocks(SUPER_BLOCK_BLOCK_NUM, 1, buffer);
}

int init_directory(byte* buffer){
	//TODO
}

int init_sfs(){
	//if unsuccessful load of the emulation disk
	if(init_fresh_disk(fileName, BLOCK_SIZE, NUM_DISK_BLOCKS) == -1){
		printf("Failed to initialize the file system");
		return -1;
	}

	//buffer can be reused to initialize superblock/mappings
	byte* buffer = (byte*) malloc(sizeof(byte) * BLOCK_SIZE);

	init_super_block(buffer);
	init_free_block_map(buffer);
	init_free_inode_map(buffer);
	init_directory(buffer);

	free(buffer);

	return 0;
}

void load_free_inode_map(byte* buffer){
	read_blocks(FREE_INODE_MAP_BLOCK_NUM, 1, buffer);
	FIM_from_string(buffer);
}

void load_free_block_map(byte* buffer){
	read_blocks(FREE_BLOCK_MAP_BLOCK_NUM, 1, buffer);
	FBM_from_string(buffer);
}

void load_super_block(byte* buffer){
	read_blocks(SUPER_BLOCK_BLOCK_NUM, 1, buffer);
	superBlock = super_block_from_string(buffer);
}

void load_directory(byte* buffer){
	//TODO
}

int load_sfs(){
	//if unsuccessful creation of the emulation disk
	if(init_disk(fileName, BLOCK_SIZE, NUM_DISK_BLOCKS) == -1){
		printf("Failed to initialize the file system");
		return -1;
	}

	byte* buffer = (byte*) malloc(sizeof(byte) * BLOCK_SIZE);

	load_super_block(buffer);
	load_free_block_map(buffer);
	load_free_inode_map(buffer);
	load_directory(buffer);

	free(buffer);

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

	FDT_init(MAX_OPEN_FILES);
	IC_init();
	return 0;	
}

/***************************************OPEN***************************************/

//returns the address of the first byte of the specified inode relative to the head of it's block (0 - 511)
int get_inode_byte_offset(int inodeNum){
	/**
	* Inode 15:
	* 15 % 13 -> Inode 15 has index 2 in it's block
	* 2 * 37 -> Inode 15 starts at byte 74 in it's block
	*/

	int indexInBlock = inodeNum % INODES_PER_BLOCK;
	return indexInBlock * INODE_SIZE;
}

//returns the block number that the specified inode exists on
int get_inode_block_number(int inodeNum){
	//offset by one to account for the super block being in 
	return 1 + inodeNum % INODES_PER_BLOCK;
}

/*
*@pre there is an inode with the specified inodeNum in memory
*/
void write_inode_to_disk(int inodeNum){
	byte* buffer = (byte*)malloc(sizeof(byte) * BLOCK_SIZE);
	int blockNum = get_inode_block_number(inodeNum);
	read_blocks(blockNum, 1, buffer);

	//assign inodeByteAddress to point to the correct place on the data block (pointer arithmetic)
	byte* inodeByteAddress = buffer + get_inode_byte_offset(inodeNum);
	Inode* inodeToWrite = IC_get(inodeNum);
	inode_to_string(inodeToWrite, inodeByteAddress);

	write_blocks(blockNum, 1, buffer);
	free(buffer);
}

/**
*
*/
Inode* load_inode_from_disk(int inodeNum){
	int inodeBlockNum = get_inode_block_number(inodeNum);
	byte* buffer = (byte*)malloc(sizeof(byte) * BLOCK_SIZE);

	//read block that contains inode
	read_blocks(inodeBlockNum, 1, buffer);
	byte* inodeStartAddress = buffer + get_inode_byte_offset(inodeNum);

	Inode* loadedInode = inode_from_string(inodeStartAddress);
	IC_put(inodeNum, loadedInode);

	free(buffer);
	return loadedInode;
}

//loads the inode associated with inodeNum into memory and stores it in the inodeCache
void load_file(int inodeNum){
	//load inode and put it in the inode cache
	Inode* loadedInode = load_inode_from_disk(inodeNum);
	IC_put(inodeNum, loadedInode);
}

/*
* Returns the inode number of the newly created file. Stores the inode in the inodeCache
* Marks the a
*/
int create_file(char* fileName){
	int inodeNum = FIM_find_free_inode();
	FIM_set_inode_used(inodeNum);
	DIR_add_file(fileName, inodeNum);

	Inode* newInode = load_inode_from_disk(inodeNum);
	clear_inode(newInode); //make sure to reset any residual values of this inode
	IC_put(inodeNum, newInode);
	//TODO write new directory data to disk

	return inodeNum;
}

/**
* Opens a file with the parameter name. If the file with the specified name does not already exist,
* it is created. 
* @param name of the file to open
* @return fileDescriptor associated with the opened file
*/
int sfs_fopen(char* fileName){
	int inodeNum = DIR_get_inode_number(name);

	//if the file does not already exist
	if(inodeNum == -1){
		inodeNum = create_file(fileName);
	}

	int fileID;
	//file already exists, load it from disk
	else{

		fileID = FDT_get_file_id(inodeNum);
		//if file is already open, return it's current fileID
		if(fileID != -1){
			return fileID;
		} 

		//
		else{
			load_file(inodeNum);
		}
	}

	//put the inode in the file descriptor table
	//TODO update the file write/read pointers
	fileID = FDT_put_file_descriptor(inodeNum);
	return fileID;
}

/***************************************CLOSE**********************************************/

//TODO March 16 - should the specified file be flushed? are there residual changes from previous reads/writes?
int sfs_fclose(int fileID){
	//TODO check that the file is already in memory?
	FileDescriptor* fd = FDT_get_file_descriptor(fileID);
	int inodeNum = fd->inodeNum;
	FDT_remove_file_descriptor(fileID);
	IC_remove(inodeNum);
}

/****************************************WRITE**********************************************/

//returns the number of indirect_pointer pointers allocated to the parameter inode
int get_number_indirect_pointers(Inode* i){
	int directOverflowSize = MAX_NUM_DIRECT_POINTERS * BLOCK_SIZE;
	if(i->size < directOverflowSize){
		return 0;
	}

	/*
	* TODO please explain this later
	*/
	return 1 + ((i->size - directOverflowSize) / BLOCK_SIZE);
}

//loads the indirect_pointer data block allocated to the parameter inode
void load_indirect_data_block(Inode* i, byte* buffer){
	read_blocks(i->indirect_pointer, 1, buffer);
}

//returns the number of data blocks allocated to Inode i, stores the inode numbers in the paramter dataBlocks array - maintains ordering (for fread)
//TODO refactor - no functions over 10 lines
int get_allocated_data_block_numbers(Inode* i, int* dataBlocks){
	byte* buffer;
	int k;

	//the parameter inode has both direct and indirect blocks
	if(i->size > BLOCK_SIZE * MAX_NUM_DIRECT_POINTERS){
		//copy direct pointers into dataBlocks array
		for(k = 0; i < MAX_NUM_DIRECT_POINTERS; k++){
			*(dataBlocks + k) = i->directPointers[k];
		}

		buffer = (byte*)malloc(sizeof(BLOCK_SIZE));
		load_indirect_data_block(i, buffer);
		int totalNumPointers = MAX_NUM_DIRECT_POINTERS + get_number_indirect_pointers(i);

		while(k < totalNumPointers{
			//TODO read data blocks from buffer
		}

		free(buffer);
		return totalNumPointers;
	}

	//there are only direct pointers allocated to the parameter inode
	else{
		int numDirectPointers = i->size % BLOCK_SIZE;
		for(k = 0; k < numDirectPointers; k++){
			*(dataBlocks + k) = i->directPointers[k];
		}
		return numDirectPointers;
	}
}

//just for readability - should be used anytime an entire 512 byte block is to be written to
void write_entire_block(int blockNum, byte* buffer){
	write_blocks(blockNum, 1, buffer);
}

//Reads the parameter block into memory, copies the parameter buffer into the proper place on the parameter block, 
//and writes the modified block back to disk
void write_partial_block(int blockNum, const char* buffer, int byteAddressOffset, int length){
	byte* blockBuffer = (byte*)malloc(sizeof(byte) * BLOCK_SIZE);
	read_blocks(blockNum, 1, blockbBuffer);

	byte* headByteAddress = blockBuffer + byteAddressOffset;
	memcpy(headByteAddress, buffer, length);

	write_blocks(blockNum, 1, blockBuffer);
	free(buffer);
}

//returns the number of new data blocks necesarry to perform a given write (assumed that this is only used if new blocks are needed)
int get_number_new_blocks_to_write(FileDescriptor* fd, int length, int numAllocatedDataBlocks){
	return 1 + ((fd->writePtr + length) - (numAllocatedDataBlocks * BLOCK_SIZE)) / BLOCK_SIZE;
}

//executes a file write
int execute_write(const char* buffer, int length, int* dataBlocks, int initialByteOffset){
	char* tmpBufferPointer = buffer;
	int bytesWritten = 0, bytesToBeWritten, i = 0;

	//write initial partial block
	bytesToBeWritten = min(BLOCK_SIZE - initialByteOffset, length);
	write_partial_block(*(dataBlocks + i), tmpBufferPointer, initialByteOffset, bytesToBeWritten);
	bytesWritten += bytesToBeWritten;
	tmpBufferPointer += bytesToBeWritten;
	i++;

	//write series of entire blocks
	while(length - bytesWritten > BLOCK_SIZE){
		write_entire_block(*(dataBlocks + i), tmpBufferPointer);
		bytesWritten += BLOCK_SIZE;
		tmpBufferPointer += BLOCK_SIZE;
		i++;
	}

	//write final partial block if necessary
	int numBytesRemaining = length - bytesWritten;
	if(numBytesRemaining > 0){
		write_partial_block(*(dataBlocks + i), tmpBufferPointer, 0, numBytesRemaining);
	}
}

int sfs_fwrite(int fileID, const char* buffer, int length){
	FileDescriptor* fd = FDT_get_file_descriptor(fileID);
	Inode* inode = IC_get(fd->inodeNum);

	int dataBlocks[] = (int*)malloc(sizeof(int) * MAX_ALLOCATED_DATA_BLOCKS); //stores the blocks that are allocated to the parameter file
	int numAllocatedDataBlocks = get_allocated_data_block_numbers(inode, dataBlocks);

	//if the write will overflow onto a new block,, allocate new blocks
	if(fd->writePtr + length > numAllocatedDataBlocks * BLOCK_SIZE){
		int numNewBlocksToWrite = get_number_new_blocks_to_write(fd, length, numAllocatedDataBlocks);
		int i;
		for(i = 0; i < numNewBlocksToWrite){
			//TODO is the file system full???
			int newBlockNum = FBM_find_free_block();
			FBM_set_block_used(newBlockNum);
			dataBlocks[numAllocatedDataBlocks + i] = newBlockNum;
		}

		//TODO update Inode state to reflect the new blocks
	}

	int errVal = execute_write(...)

	//TODO update file size

	free(dataBlocks);

	return errVal;
}

/***************************************READ************************************************/

void read_entire_block(int blockNum, char* buffer){
	read_blocks(blockNum, 1, buffer);
}

void read_partial_block(int blockNum, char* buffer, int byteAddressOffset, int length){
	byte* readBuffer = (byte*)malloc(sizeof(byte) * BLOCK_SIZE);
	read_blocks(blockNum, 1, readBuffer);

	int headByteAddress = readBuffer + byteAddressOffset;
	memcpy(buffer, headByteAddress, length);

	free(buffer);
}

int execute_read(char* buffer, int length, int* dataBlocks, int initialByteOffset){
	char* tmpBufferPointer = buffer;
	int bytesRead = 0, bytesToBeRead, i = 0;

	//read initial partial block
	bytesToBeRead = min(BLOCK_SIZE - initialByteOffset, length);
	read_partial_block(*(dataBlocks + i), tmpBufferPointer, initialByteOffset, bytesToBeRead);
	bytesRead += bytesToBeRead;
	tmpBufferPointer += bytesToBeRead;
	i++;

	//write series of entire blocks
	while(length - bytesRead > BLOCK_SIZE){
		read_entire_block(*(dataBlocks + i), tmpBufferPointer);
		bytesRead += BLOCK_SIZE;
		tmpBufferPointer += BLOCK_SIZE;
		i++;
	}

	//write final partial block if necessary
	int numBytesRemaining = length - bytesRead;
	if(numBytesRemaining > 0){
		read_partial_block(*(dataBlocks + i), tmpBufferPointer, 0, numBytesRemaining);
	}
}

/*
* Returns the number of elements successfully read which is less the parameter length 
* iff a read error or end-of-file is encountered.
*/
int sfs_fread(int fileID, char* buf, int length){
	FileDescriptor* fd = FDT_get_file_descriptor(fileID);
	Inode* inode = IC_get(fd->inodeNum);

	//if an EOF read is requested
	if(fd->readPtr + length > inode->size) {
		//decrease the length to be read to the difference between the inode size and the current position of the read pointer
		length = inode->size - fd->readPtr;
	}

	//get the data blocks associated with 
	int dataBlocks[] = (int*) malloc(sizeof(int*) * MAX_ALLOCATED_DATA_BLOCKS);
	int numAllocatedDataBlocks = get_allocated_data_block_numbers(inode, dataBlocks);

	return length;
}

/****************************************SEEK***********************************************/

void update_write_pointer(FileDescriptor* fd, int offset){
	fd->writePtr += offset;
}

void update_read_pointer(FileDescriptor* fd, int offset){
	fd->writePtr += offset;
}

int sfs_fseek(int fileID, int offset){
	FileDescriptor* fd = FDT_get_file_descriptor(fileID);
	update_read_pointer(fd, offset);
	update_write_pointer(fd, offset);
}

/****************************************REMOVE*********************************************/

//clears all of the data blocks allocated to the parameter inode from the free data block map
void clear_data_blocks(Inode* i){
	int dataBlocks[] = (int*) malloc(sizeof(int*) * MAX_ALLOCATED_DATA_BLOCKS);
	int numAllocatedDataBlocks = get_allocated_data_block_numbers(inode, dataBlocks);

	int i;
	for(i = 0; i < numAllocatedDataBlocks; i++){
		FBM_clear_block(dataBlocks[i]);
	}

	free(dataBlocks);
}

/*
*
*/
int sfs_remove(char* fileName){
	//Remove file entry from directory
	int inodeNum = DIR_get_inode_number(fileName);
	DIR_remove_file(fileName);

	//remove inode from cache
	Inode* targetInode;
	if(IC_contains(inodeNum)){
		targetInode = IC_get(inodeNum);
		IC_remove(inodeNum);
	}
	else{
		targetInode = load_inode_from_disk(inodeNum);
	}

	//if the file is currently open, remove it from the fileDescriptorTable
	if(FDT_contains_file_id(inodeNum)){

		//TODO please fix this awful two call system
		int fileID = FDT_get_file_id(inodeNum);
		FDT_remove_file_descriptor(fileID);
	}

	//clear blocks allocated to the removed file's inode
	clear_data_blocks(targetInode);
}

/*************************************GET_NEXT********************************************/

int sfs_get_next_filename(char* filename){
	int nextFileInodeNum = DIR_get_next_inode_number();
}

int sfs_get_file_size(const char* path){
	//TODO the form of path might be incompatible IE it might need to be parsed
	int targetInodeNum = DIR_get_inode_number(path);

	//if the file's inode already exists in the memory cache
	if(IC_contains(inodeNum)){
		Inode* targetInode = IC_get(inodeNum);
		return targetInode->size;
	}

	//otherwise, load the inode from disk and return it's size
	else{
		//TODO maybe store the inode in memory?
		Inode* targetInode = load_inode_from_disk(inodeNum);
		return targetInode->size;
	}
}

/****************************************EOF**********************************************/
/*****************************************************************************************/