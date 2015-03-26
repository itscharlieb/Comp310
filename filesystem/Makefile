binpath = bin
srcpath = src/lib
vendorpath = src/vendor
headerpath = src/include
testpath = src/test

allobjects = $(binpath)/sfs_api.o $(binpath)/fuse_wrappers.o $(binpath)/disk_emu.o $(binpath)/super_block.o $(binpath)/directory.o $(binpath)/file_descriptor_table.o $(binpath)/free_block_map.o $(binpath)/inode_cache.o $(binpath)/free_inode_map.o $(binpath)/inode.o $(binpath)/bitmap.o $(binpath)/types.o $(binpath)/util.o $(binpath)/output_file.o 
testobjects = $(binpath)/sfs_api.o $(binpath)/disk_emu.o $(binpath)/super_block.o $(binpath)/directory.o $(binpath)/file_descriptor_table.o $(binpath)/free_block_map.o $(binpath)/inode_cache.o $(binpath)/free_inode_map.o $(binpath)/inode.o $(binpath)/bitmap.o $(binpath)/types.o $(binpath)/util.o $(binpath)/output_file.o

linkflags = `pkg-config fuse --cflags --libs`


all: $(allobjects)
	gcc -o $(binpath)/sfs $(allobjects) $(linkflags) 

############################### LIB ####################################

$(binpath)/sfs_api.o: $(headerpath)/disk_emu.h $(headerpath)/sfs_api.h $(headerpath)/constants.h $(headerpath)/inode.h $(headerpath)/inode_cache.h $(headerpath)/directory.h $(headerpath)/file_descriptor_table.h $(headerpath)/free_block_map.h $(headerpath)/free_inode_map.h $(headerpath)/super_block.h $(headerpath)/util.h $(headerpath)/types.h $(srcpath)/sfs_api.c
	gcc -c $(srcpath)/sfs_api.c -o $(binpath)/sfs_api.o

$(binpath)/super_block.o: $(headerpath)/super_block.h $(srcpath)/super_block.c
	gcc -c $(srcpath)/super_block.c -o $(binpath)/super_block.o

$(binpath)/directory.o: $(headerpath)/directory.h $(headerpath)/constants.h $(srcpath)/directory.c
	gcc -c $(srcpath)/directory.c -o $(binpath)/directory.o

$(binpath)/file_descriptor_table.o: $(headerpath)/file_descriptor_table.h $(headerpath)/constants.h $(srcpath)/file_descriptor_table.c
	gcc -c $(srcpath)/file_descriptor_table.c -o $(binpath)/file_descriptor_table.o

$(binpath)/free_block_map.o: $(headerpath)/free_block_map.h  $(headerpath)/bitmap.h $(headerpath)/types.h $(srcpath)/free_block_map.c
	gcc -c $(srcpath)/free_block_map.c -o $(binpath)/free_block_map.o

$(binpath)/inode_cache.o: $(headerpath)/inode_cache.h  $(headerpath)/types.h  $(headerpath)/inode.h $(srcpath)/inode_cache.c
	gcc -c $(srcpath)/inode_cache.c -o $(binpath)/inode_cache.o

$(binpath)/free_inode_map.o: $(headerpath)/free_inode_map.h  $(headerpath)/bitmap.h $(headerpath)/constants.h $(srcpath)/free_inode_map.c
	gcc -c $(srcpath)/free_inode_map.c -o $(binpath)/free_inode_map.o

$(binpath)/inode.o: $(headerpath)/inode.h $(headerpath)/constants.h $(srcpath)/inode.c
	gcc -c $(srcpath)/inode.c -o $(binpath)/inode.o

$(binpath)/bitmap.o: $(headerpath)/bitmap.h $(srcpath)/bitmap.c
	gcc -c $(srcpath)/bitmap.c -o $(binpath)/bitmap.o

$(binpath)/types.o: $(headerpath)/types.h $(srcpath)/types.c
	gcc -c $(srcpath)/types.c -o $(binpath)/types.o

$(binpath)/util.o: $(headerpath)/util.h $(srcpath)/util.c
	gcc -c $(srcpath)/util.c -o $(binpath)/util.o

$(binpath)/output_file.o: $(headerpath)/output_file.h $(srcpath)/output_file.c
	gcc -c $(srcpath)/output_file.c -o $(binpath)/output_file.o

############################## VENDOR ##################################

$(binpath)/fuse_wrappers.o: $(headerpath)/sfs_api.h $(headerpath)/disk_emu.h $(vendorpath)/fuse_wrappers.c
	gcc -c $(vendorpath)/fuse_wrappers.c -o $(binpath)/fuse_wrappers.o -D_FILE_OFFSET_BITS=64

$(binpath)/disk_emu.o: $(headerpath)/disk_emu.h $(vendorpath)/disk_emu.c
	gcc -c $(vendorpath)/disk_emu.c -o $(binpath)/disk_emu.o

############################## TEST #####################################

bitmaptest: $(testpath)/test.c $(headerpath)/bitmap.h $(binpath)/bitmap.o
	gcc $(binpath)/bitmap.o $(testpath)/test.c -o $(binpath)/bitmaptest

test: $(testobjects)
	gcc $(testobjects) $(testpath)/sfs_test.c -o $(binpath)/test

############################## CLEAN ###################################

hugo: clean
	rm $(binpath)/sfs

clean: 
	rm $(binpath)/*.o
