/**
* @author Charlie Bloomfield
* March 14, 2015
**/

/************ VENDOR *************/
#include "../include/disk_emu.h"

/************* LIB ***************/
#include "../include/sfs_api.h"

#include "../include/constants.h"
#include "../include/inode.h"
#include "../include/inode_cache.h"
#include "../include/directory.h"
#include "../include/file_descriptor_table.h"
#include "../include/free_block_map.h"
#include "../include/free_inode_map.h"
#include "../include/super_block.h"
#include "../include/util.h"
#include "../include/types.h"
#include "../include/output_file.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char diskName[] = "itscharlieb FS";

/**********************************DUMPS**************************************/

void dump_inode(Inode* inode){
	printf("\n------------------[dump_inode]--------------------\n");
	printf("size = [%d].\ndirect block numbers = [", inode->size);
	int i;
	for(i = 0; i < 12; i++){
		printf("%d, ", inode->directPointers[i]);
	}
	printf("].\n");
}

// void dump_file_descriptor(FileDescriptor* fd){
// 	printf("[FILE_DESCRIPTOR]\n-[Write Pointer]-[%d]\n[Read Pointer]-")
// }

/***********************************INIT**************************************/

void init_free_block_map(byte* buffer){
	printf("[init_free_block_map] Initializing free block map.\n");
	fflush(stdout);

	FBM_init(NUM_BLOCKS);

	printf("[init_free_block_map] Free block map initialization successful.\n\n");
	fflush(stdout);

	FBM_set_block_used(SUPER_BLOCK_BLOCK_NUM);

	int i;
	// blocks 1-13 hold inodes, not to be used for data
	for(i = 1; i <= INODE_TABLE_LENGTH; i++){
		FBM_set_block_used(i);
	}

	FBM_set_block_used(FREE_BLOCK_MAP_BLOCK_NUM);//4094
	FBM_set_block_used(FREE_INODE_MAP_BLOCK_NUM);//4095

	FBM_to_string(buffer);
	write_blocks(FREE_BLOCK_MAP_BLOCK_NUM, 1, buffer);
}

void init_free_inode_map(byte* buffer){
	printf("[init_free_inode_map] Initializing free inode map.\n");
	fflush(stdout);

	int numInodes = MAX_NUM_FILES;
	FIM_init(numInodes);
	FIM_set_inode_used(ROOT_DIRECTORY_INODE_NUM);
	FIM_to_string(buffer);
	write_blocks(FREE_INODE_MAP_BLOCK_NUM, 1, buffer);

	printf("[init_free_inode_map] Free inode map initialization successful.\n\n");
	fflush(stdout);
}


int init_super_block(byte* buffer){
	printf("[init_super_block] Writing super block to disk.\n");
	fflush(stdout);

	//init super block, write it to disk
	//superBlock = create_super_block();
	super_block_to_string(buffer);
	write_blocks(SUPER_BLOCK_BLOCK_NUM, 1, buffer);

	printf("[init_super_block] Super block written.\n\n");
	fflush(stdout);

}

int init_directory(byte* buffer){
	printf("[init_directory] Initializing directory.\n");
	fflush(stdout);

	DIR_init();
	//TODO write dir to disk

	printf("[init_directory] Directory initialization successful.\n\n");
	fflush(stdout);

}

int init_sfs(){

	//if unsuccessful load of the emulation disk
	if(init_fresh_disk(diskName, BLOCK_SIZE, NUM_BLOCKS) == -1){
		printf("Failed to initialize the file system.\n");
		return -1;
	}

	//buffer can be reused to initialize superblock/mappings
	byte* buffer = (byte*) malloc(sizeof(byte) * BLOCK_SIZE);

	//init_super_block(buffer); just constants

	init_super_block(buffer);
	init_free_block_map(buffer);
	init_free_inode_map(buffer);
	init_directory(buffer);

	free(buffer);

	return 0;
}

void load_free_inode_map(byte* buffer){
	read_blocks(FREE_INODE_MAP_BLOCK_NUM, 1, buffer);
	int numInodes = MAX_NUM_FILES;
	FIM_from_string(buffer, numInodes);
}

void load_free_block_map(byte* buffer){
	read_blocks(FREE_BLOCK_MAP_BLOCK_NUM, 1, buffer);
	FBM_from_string(buffer, NUM_BLOCKS);
}

// void load_super_block(byte* buffer){
// 	read_blocks(SUPER_BLOCK_BLOCK_NUM, 1, buffer);
// 	superBlock = super_block_from_string(buffer);
// }

void load_directory(byte* buffer){
	//TODO
}

int load_sfs(){
	//if unsuccessful creation of the emulation disk
	if(init_disk(diskName, BLOCK_SIZE, NUM_BLOCKS) == -1){
		printf("Failed to initialize the file system");
		return -1;
	}

	byte* buffer = (byte*) malloc(sizeof(byte) * BLOCK_SIZE);

	//load_super_block(buffer);
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
	init_output_file(); //redirects all stdout prints to a debug file
	init_sfs();
	//if fresh, then initialize a new disk 
	// if(fresh){
	// 	printf("[init_sfs] initializing.\n");
	// 	fflush(stdout);
	// 	init_sfs();
	// }

	// //otherwise, load disk
	// else{
	// 	printf("[init_sfs] loading.\n");
	// 	fflush(stdout);
	// 	load_sfs();
	// }
	printf("[mksfs] Initializing file descriptor table.\n");
	fflush(stdout);

	FDT_init(MAX_NUM_FILES);

	printf("[mksfs] File descriptor table initialization successful.\n\n");
	fflush(stdout);

	printf("[mksfs] Initializing inode cache.\n");
	fflush(stdout);

	//IC_init();

	printf("[mksfs] Inode cache initialization successful.\n\n");
	fflush(stdout);
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

//does what is says
void write_inode_indirect_block_to_disk(Inode* inode, half_word* indirectDataBlocks, int numIndirectDataBlocks){
	byte* buffer = (byte*)malloc(sizeof(byte) * BLOCK_SIZE);
	int indirectBlockNum = inode->indirect;
	memcpy(buffer, indirectDataBlocks, (numIndirectDataBlocks * HALF_WORD_SIZE));
	write_blocks(indirectBlockNum, 1, buffer);
	free(buffer);
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
* Loads the inode from disk and returns it.
*/
Inode* load_inode_from_disk(int inodeNum){
	printf("[load_inode_from_disk] Loading inode [%d].\n", inodeNum);
	fflush(stdout);

	int inodeBlockNum = get_inode_block_number(inodeNum);
	byte* buffer = (byte*)malloc(sizeof(byte) * BLOCK_SIZE);

	//read block that contains inode
	read_blocks(inodeBlockNum, 1, buffer);
	byte* inodeStartAddress = buffer + get_inode_byte_offset(inodeNum);

	Inode* loadedInode = inode_from_string(inodeStartAddress);
	free(buffer);

	printf("[load_inode_from_disk] Loaded inode [%d].\n", inodeNum);
	return loadedInode;
}

//loads the inode associated with inodeNum into memory and stores it in the inodeCache
void load_file(int inodeNum){
	//load inode and put it in the inode cache
	Inode* loadedInode = load_inode_from_disk(inodeNum);
	//IC_put(inodeNum, loadedInode);
}

/*
* Returns the inode number of the newly created file. Stores the inode in the inodeCache
* Marks the a
*/
int create_file(char* fileName){
	printf("[create_file] Creating new file [%s].\n", fileName);
	fflush(stdout);
	int inodeNum;

	if((inodeNum = FIM_find_free_inode()) == -1){
		printf("[create_file] There are no remaining inodes. File system is full.\n");
		fflush(stdout);
		return -1;
	}
	FIM_set_inode_used(inodeNum);
	DIR_add_file(fileName, inodeNum);

	//Inode* newInode = load_inode_from_disk(inodeNum);
	//clear_inode(newInode); //make sure to reset any residual values of this inode
	//IC_put(inodeNum, newInode);

	//TODO write new directory data to disk

	printf("[create_file] Successfully created [%s] allocated to inodeNum [%d].\n", fileName, inodeNum);
	fflush(stdout);
	return inodeNum;
}

/**
* Opens a file with the parameter name. If the file with the specified name does not already exist,
* it is created. 
* @param name of the file to open
* @return fileDescriptor associated with the opened file
*/
int sfs_fopen(char* fileName){
	printf("\n[sfs_open] Opening file [%s].\n", fileName);
	fflush(stdout);

	int fileID;
	int inodeNum = DIR_get_inode_number(fileName);

	//if the file does not already exist on disk
	if(inodeNum == -1){
		if((inodeNum = create_file(fileName)) == -1){
			printf("[sfs_fopen] Inode table is full - cannot create another file.\n");
			fflush(stdout);
			return -1;
		}

		fileID = FDT_put_file_descriptor(inodeNum);

		printf("[sfs_open] Opened [%s] with inodeNum [%d] and file descriptor [%d].\n", fileName, inodeNum, fileID);
		fflush(stdout);
	}

	//file already exists on disk
	else{
		//if file is not already open, put it in the FDT
		if(!FDT_contains_file_id(inodeNum)){
			fileID = FDT_put_file_descriptor(inodeNum);
			printf("[sfs_open] File [%s] exists in the directory with inodeNum [%d] and fileID [%d].\n", fileName, inodeNum, fileID);
			fflush(stdout);

			load_file(inodeNum);
			fileID = FDT_put_file_descriptor(inodeNum);
		}
		else{
			fileID = FDT_get_file_id(inodeNum);
			printf("[sfs_open] File [%s] is already open and has ID [%d].\n", fileName, fileID);
			fflush(stdout);
		}
	}
	return fileID;
}

/***************************************CLOSE**********************************************/

//TODO March 16 - should the specified file be flushed? are there residual changes from previous reads/writes?
int sfs_fclose(int fileID){
	printf("\n[sfs_fclose] Closing file with ID [%d].\n", fileID);
	fflush(stdout);

	if(!FDT_contains_file_id(fileID)){
		printf("[sfs_close] File with ID [%d] is not open!\n", fileID);
		fflush(stdout);
		return -1; //failure value
	}

	else{
		// //TODO check that the file is already in memory?
		// FileDescriptor* fd = FDT_get_file_descriptor(fileID);
		// int inodeNum = fd->inodeNum;

		// printf("[sfs_fclose] File has inodeNum [%d].\n", inodeNum);
		// fflush(stdout);

		FDT_remove_file_descriptor(fileID);

		printf("[sfs_fclose] Removed from FDT.\n");
		fflush(stdout);

		//IC_remove(inodeNum);
		return 0;//success value
	}
}

/****************************************WRITE**********************************************/

//returns the number of indirect_pointer pointers allocated to the parameter inode
int get_number_indirect_pointers(Inode* inode){
	int directOverflowSize = MAX_NUM_DIRECT_POINTERS * BLOCK_SIZE;
	if(inode->size < directOverflowSize){
		return 0;
	}

	/*
	* TODO please explain this later
	*/
	return 1 + ((inode->size - directOverflowSize) / BLOCK_SIZE);
}

//loads the indirect_pointer data block allocated to the parameter inode
void load_indirect_data_block(Inode* inode, byte* buffer){
	read_blocks(inode->indirect, 1, buffer);
}

//returns the number of data blocks allocated to Inode i, stores the inode numbers in the paramter dataBlocks array - maintains ordering (for fread)
int get_allocated_data_block_numbers(Inode* inode, half_word* dataBlocks){
	byte* buffer;
	int k;

	//the parameter inode has both direct and indirect blocks
	if(inode->size > BLOCK_SIZE * MAX_NUM_DIRECT_POINTERS){
		int numDirectPointers = MAX_NUM_DIRECT_POINTERS;
		int numIndirectPointers = get_number_indirect_pointers(inode);

		//copy direct pointers into dataBlocks array
		for(k = 0; k < numDirectPointers; k++){
			*(dataBlocks + k) = inode->directPointers[k];
		}

		buffer = (byte*)malloc(sizeof(byte) * BLOCK_SIZE);
		load_indirect_data_block(inode, buffer);

		//copy the pointers from the indirect block into the tail of the dataBlock array
		memcpy((dataBlocks + MAX_NUM_DIRECT_POINTERS), buffer, (numIndirectPointers * HALF_WORD_SIZE));
		free(buffer);
		return numDirectPointers + numIndirectPointers;
	}

	//there are only direct pointers allocated to the parameter inode
	else{
		int numDirectPointers = inode->size % BLOCK_SIZE;
		for(k = 0; k < numDirectPointers; k++){
			*(dataBlocks + k) = inode->directPointers[k];
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
	read_blocks(blockNum, 1, blockBuffer);

	printf("[write_partial_block] Value in buffer [%.*s].\n", length, buffer);

	byte* headByteAddress = blockBuffer + byteAddressOffset;
	memcpy(headByteAddress, buffer, length);

	write_blocks(blockNum, 1, blockBuffer);
	free(blockBuffer);
}

//returns the number of new data blocks necesarry to perform a given write (assumed that this is only used if new blocks are needed)
int get_number_new_blocks_to_write(FileDescriptor* fd, int length, int numAllocatedDataBlocks){
	return 1 + ((fd->writePtr + length) - (numAllocatedDataBlocks * BLOCK_SIZE)) / BLOCK_SIZE;
}

//executes a file write
int execute_write(const char* buffer, int length, half_word* dataBlocks, int initialByteOffset){

	char* copiedBuffer = (byte*)malloc(sizeof(byte) * length);
	memcpy(copiedBuffer, buffer, length);

	char* tmpBufferPointer = copiedBuffer;
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
		bytesWritten += numBytesRemaining;
	}

	return bytesWritten;
}

int sfs_fwrite(int fileID, const char* buffer, int length){
	FileDescriptor* fd = FDT_get_file_descriptor(fileID);
	Inode* inode = load_inode_from_disk(fd->inodeNum);

	half_word* dataBlocks = (half_word*)malloc(sizeof(half_word) * MAX_ALLOCATED_DATA_BLOCKS); //stores the blocks that are allocated to the parameter file
	int numAllocatedDataBlocks = get_allocated_data_block_numbers(inode, dataBlocks);

	printf("[sfs_fwrite] [%d] data blocks allocated to inode [%d].\n", numAllocatedDataBlocks, fd->inodeNum);

	//if the write will overflow onto a new block,, allocate new blocks
	if(fd->writePtr + length > numAllocatedDataBlocks * BLOCK_SIZE){
		int numNewBlocksToWrite = get_number_new_blocks_to_write(fd, length, numAllocatedDataBlocks);
		printf("[sfs_fwrite] [%d] new blocks needed to write [%d] bytes to file [%d].\n", numNewBlocksToWrite, length, fileID);
		int i;

		printf("[sfs_fwrite] Will use data blocks [");
		for(i = 0; i < numNewBlocksToWrite; i++){
			//TODO is the file system full???
			int newBlockNum = FBM_find_free_block();
			FBM_set_block_used(newBlockNum);
			*(dataBlocks + numAllocatedDataBlocks + i) = (half_word)newBlockNum;

			printf("%d, ", newBlockNum);
		}
		printf("] to execute write.\n\n");
	}

	int bytesWritten = execute_write(buffer, length, dataBlocks, fd->writePtr % BLOCK_SIZE);
	free(dataBlocks);

	printf("[sfs_fwrite] Current writePrt [%d]. New writePtr [%d].\n", fd->writePtr, fd->writePtr + bytesWritten);
	fflush(stdout);

	fd->writePtr += bytesWritten;
	fd->readPtr += bytesWritten;

	return bytesWritten;
}
/****************************************READ***********************************************/

void read_entire_block(int blockNum, char* buffer){
	read_blocks(blockNum, 1, buffer);
}

void read_partial_block(int blockNum, char* buffer, int byteAddressOffset, int length){
	byte* readBuffer = (byte*)malloc(sizeof(byte) * BLOCK_SIZE);
	read_blocks(blockNum, 1, readBuffer);

	byte* headByteAddress = readBuffer + byteAddressOffset;
	memcpy(buffer, headByteAddress, length);

	free(buffer);
}

int execute_read(char* buffer, int length, half_word* dataBlocks, int initialByteOffset){
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
	if(!FDT_contains_file_id(fileID)){
		printf("[sfs_fread] File ID [%d] is not open!. Cannot read an unopened file.\n", fileID);
		fflush(stdout);
		return -1;
	}


	FileDescriptor* fd = FDT_get_file_descriptor(fileID);
	// Inode* inode = IC_get(fd->inodeNum);
	Inode* inode = load_inode_from_disk(fd->inodeNum);

	printf("[sfs_read] Dumping inode values.\n");
	dump_inode(inode);
	fflush(stdout);

	//if an EOF read is requested
	if(fd->readPtr + length > inode->size) {
		//decrease the length to be read to the difference between the inode size and the current position of the read pointer
		length = inode->size - fd->readPtr;
	}

	//get the data blocks associated with 
	half_word* dataBlocks = (half_word*)malloc(sizeof(half_word) * MAX_ALLOCATED_DATA_BLOCKS);
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
	printf("[sfs_fseek] Seeking fileID [%d] with offset [%d].\n", fileID, offset);
	fflush(stdout);

	if(!FDT_contains_file_id(fileID)){
		printf("[sfs_fseek] FileID [%d] is not open - cannot seek a closed file.\n", fileID);
		fflush(stdout);
		return -1;
	}

	FileDescriptor* fd = FDT_get_file_descriptor(fileID);
	update_read_pointer(fd, offset);
	update_write_pointer(fd, offset);
}

/****************************************REMOVE*********************************************/

//clears all of the data blocks allocated to the parameter inode from the free data block map
void clear_data_blocks(Inode* inode){
	half_word* dataBlocks = (half_word*) malloc(sizeof(half_word) * MAX_ALLOCATED_DATA_BLOCKS);
	int numAllocatedDataBlocks = get_allocated_data_block_numbers(inode, dataBlocks);

	int i;
	for(i = 0; i < numAllocatedDataBlocks; i++){
		FBM_clear_block(*(dataBlocks + i));
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

	//if the file is currently open, remove it from the fileDescriptorTable
	if(FDT_contains_file_id(inodeNum)){

		//TODO please fix this awful two call system
		int fileID = FDT_get_file_id(inodeNum);
		FDT_remove_file_descriptor(fileID);
	}

	//remove inode from cache
	Inode* targetInode = load_inode_from_disk(inodeNum);
	// if(IC_contains(inodeNum)){
	// 	targetInode = IC_get(inodeNum);
	// 	IC_remove(inodeNum);
	// }
	// else{
	// 	targetInode = load_inode_from_disk(inodeNum);
	// }


	//clear blocks allocated to the removed file's inode
	clear_data_blocks(targetInode);
}

/*************************************GET_NEXT********************************************/

int sfs_get_next_file_name(char* fileNameBuffer){
	return DIR_get_next_file_name(fileNameBuffer);
}

int sfs_get_file_size(const char* path){
	//TODO the form of path might be incompatible IE it might need to be parsed
	int inodeNum = DIR_get_inode_number(path);

	// if the file's inode already exists in the memory cache
	// if(IC_contains(inodeNum)){
	// 	Inode* targetInode = IC_get(inodeNum);
	// 	return targetInode->size;
	// }

	// //otherwise, load the inode from disk and return it's size
	// else{
	// 	//TODO maybe store the inode in memory?
	// 	Inode* targetInode = load_inode_from_disk(inodeNum);
	// 	IC_put(inodeNum, targetInode);
	// 	return targetInode->size;
	// }

	Inode* targetInode = load_inode_from_disk(inodeNum);
	//IC_put(inodeNum, targetInode);
	return targetInode->size;
}

/******************************************EOF********************************************/
/*****************************************************************************************/