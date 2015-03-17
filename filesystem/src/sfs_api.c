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

#define BLOCK_SIZE 512
#define NUM_BLOCKS 4096
#define MAX_FILE_NAME_LENGTH 16
#define MAX_FILE_EXTENSION_LENGTH 3
#define MAX_OPEN_FILES 10
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE)

//constant block numbers
#define SUPER_BLOCK_BLOCK_NUM 0
#define FREE_INODE_MAP_BLOCK_NUM 4094
#define FREE_BLOCK_MAP_BLOCK_NUM 4095

const char fileName[] = "itscharlieb FS";

//keep the following globally accessible as they are used by every function the sfs provides
SuperBlock* superBlock;
FreeInodeMap* freeInodeMap;
FreeBlockMap* freeBlockMap;
InodeCache* inodeCache;
Directory* rootDirectory;
FileDescriptorTable* fileDescriptorTable;


/**********************************************************************/

void init_free_inode_map(byte buffer[]){
	//initializes the map with all inodes free
	freeInodeMap = create_free_inode_map();
	free_inode_map_to_string(freeInodeMap, buffer)
	write_blocks(FREE_INODE_MAP_BLOCK_NUM, 1, buffer);
}

void init_free_block_map(byte buffer[]){
	FreeBlockMap* freeBlockMap = create_free_block_map();

	set_block_used(freeBlockMap, SUPER_BLOCK_BLOCK_NUM);

	int i;
	// blocks 1-128 hold inodes, not to be used for data
	for(int i = 1; i < 129; i++){
		set_block_used(freeBlockMap, i);
	}

	set_block_used(freeBlockMap, FREE_BLOCK_MAP_BLOCK_NUM);
	set_block_used(freeBlockMap, FREE_INODE_MAP_BLOCK_NUM);

	free_block_map_to_string(freeBlockMap, buffer);
	write_blocks(FREE_BLOCK_MAP_BLOCK_NUM, 1, buffer);
}

int init_super_block(byte buffer[]){
	//init super block, write it to disk
	superBlock = create_super_block();
	super_block_to_string(superBlock, buffer);
	write_blocks(SUPER_BLOCK_BLOCK_NUM, 1, buffer);
}

int init_sfs(){
	//if unsuccessful load of the emulation disk
	if(init_fresh_disk(fileName, BLOCK_SIZE, NUM_DISK_BLOCKS) == -1){
		printf("Failed to initialize the file system");
		return -1;
	}

	//buffer can be reused to initialize superblock/mappings
	byte buffer[BLOCK_SIZE];

	init_super_block(buffer);
	init_free_block_map(buffer);
	init_free_inode_map(buffer);

	return 0;
}

void load_free_inode_map(byte buffer[]){
	read_blocks(FREE_INODE_MAP_BLOCK_NUM, 1, buffer);
	freeInodeMap = free_inode_map_from_string(buffer);
}

void load_free_block_map(byte buffer[]){
	read_blocks(FREE_BLOCK_MAP_BLOCK_NUM, 1, buffer);
	freeBlockMap = free_block_map_from_string(buffer);
}

void load_super_block(byte buffer[]){
	read_blocks(SUPER_BLOCK_BLOCK_NUM, 1, buffer);
	superBlock = super_block_from_string(buffer);
}

int load_sfs(){
	//if unsuccessful creation of the emulation disk
	if(init_disk(fileName, BLOCK_SIZE, NUM_DISK_BLOCKS) == -1){
		printf("Failed to initialize the file system");
		return -1;
	}

	load_super_block();
	load_free_block_map();
	load_free_inode_map();

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

	fileDescriptorTable = create_file_descriptor_table(MAX_OPEN_FILES);
	return 0;	
}

/**********************************************************************************/

//returns the address of the first byte of the specified inode relative to the head of it's block (0 - 512)
int get_inode_byte_offset(int inodeNum){
	int indexInBlock = inodeNum % INODES_PER_BLOCK;
	return indexInBlock * INODE_SIZE;
}

//returns the block number that the specified inode exists on
int get_inode_block_num(int inodeNum){
	//offset by one to account for the super block being in 
	return 1 + inodeNum % INODES_PER_BLOCK;
}

void write_inode_to_disk(int inodeNum){
	int blockNum = get_inode_block_num(inodeNum);
	
}

Inode* load_inode_from_disk(int inodeNum){
	int inodeBlockNum = get_inode_block_num(inodeNum);
	byte* buffer = (byte*)malloc(sizeof(byte) * BLOCK_SIZE);

	//read block that contains inode
	read_blocks(inodeBlockNum, 1, buffer);
	byte* inodeStartAddress = buffer + get_inode_byte_offset(inodeNum);

	Inode* loadedInode = inode_from_string(inodeStartAddress);
	put_inode(inodeCache, inodeNum, loadedInode);

	free(buffer);
	return loadedInode;
}

//loads the inode associated with inodeNum into memory and stores it in the inodeCache
void load_file(int inodeNum){
	//load inode and put it in the inode cache
	Inode* loadedInode = load_inode_from_disk(inodeNum);
	put_inode(inodeCache, inodeNum, loadedInode);
}

//returns the inode number of the newly created file. Stores the inode in the inodeCache
int create_file(char* fileName){
	int inodeNum = find_free_inode(freeInodeMap);
	set_inode_used(freeInodeMap, inodeNum);
	add_file_to_directory(rootDirectory, fileName, inodeNum);

	Inode* newInode = load_inode_from_disk(inodeNum);
	reset_inode(newInode); //make sure to reset any residual value of this inode
	put_inode(inodeCache, inodeNum, newInode);
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
	int inodeNum = get_inode_number_from_directory(rootDirectory, name);

	//if the file does not already exist
	if(inodeNum == -1){
		inodeNum = create_file(fileName);
	}

	int fileID;
	//file already exists, load it from disk
	else{
		//if file is already open, return it's current fileID
		fileID = get_file_id(fileDescriptorTable, inodeNum);
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
	fileID = put_file_descriptor(fileDescriptorTable, inodeNum);
	return fileID;
}

/***********************************************************************************/

void update_write_pointer(FileDescriptor* fd, int offset){
	fd->writePtr += offset;
}

void update_read_pointer(FileDescriptor* fd, int offset){
	fd->writePtr += offset;
}

//TODO March 16 - should the specified file be flushed? are there residual changes from previous reads/writes?
int sfs_fclose(int fileID){
	//TODO check that the file is already in memory?
	FileDescriptor* fd = remove_file_descriptor(fileDescriptorTable, fileID);
	int inodeNum = fd->inodeNum;
	remove_inode(inodeCache, inodeNum);
}

//TODO March 16 - assuming that only appending is supported
int sfs_fwrite(int fileID, const char* buf, int length){

}

int sfs_fread(int fileID, char* buf, int length){

}

int sfs_fseek(int fileID, int offset){
	FileDescriptor* fd = get_file_descriptor(fileDescriptorTable, fileID);
	update_read_pointer(fd, offset);
	update_write_pointer(fd, offset);
}
/*******************************************************************************************/
void clear_data_blocks(Inode* i){
	int dataBlocks[] = (int*) malloc(sizeof(int*) * MAX_ALLOCATED_DATA_BLOCKS);
	int numAllocatedDataBlocks = get_allocated_data_blocks(inode, dataBlocks);

	int i;
	for(i = 0; i < numAllocatedDataBlocks; i++){
		free_block(freeBlockMap, dataBlocks[i]);
	}
}

int sfs_remove(char* fileName){
	Inode* targetInode;
	int inodeNum = get_inode_number_from_directory(rootDirectory, fileName);
	remove_file_from_directory(rootDirectory, fileName);

	if(contains_inode(inodeCache, inodeNum)){
		targetInode = get_inode(inodeCache, inodeNum);
		remove_inode(inodeCache, inodeNum);
	}

	else{
		targetInode = load_inode_from_disk(inodeNum);
	}

	if(contains_file_id(fileDescriptorTable, inodeNum)){

		//TODO please fix this awful two call system
		int fileID = get_file_id(fileDescriptorTable, inodeNum);
		remove_file_descriptor(fileDescriptorTable, fileID);
	}

	clear_data_blocks(targetInode);
}

/*****************************************************************************************/

int sfs_get_next_filename(char* filename){
	int nextFileID = get_next_file_id(rootDirectory);

}

int sfs_get_file_size(const char* path){
	//TODO the form of path might be incompatible IE it might need to be parsed
	int targetInodeNum = get_inode_number_from_directory(rootDirectory, path);

	//if the file's inode already exists in the memory cache
	if(contains_inode(inodeCache, inodeNum)){
		Inode* targetInode = get_inode(inodeCache, inodeNum);
		return targetInode->size;
	}

	//otherwise, load the inode from disk and return it's size
	else{
		//TODO maybe store the inode in memory?
		Inode* targetInode = load_inode_from_disk(inodeNum);
		return targetInode->size;
	}
}
