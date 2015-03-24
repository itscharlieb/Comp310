binpath = bin
srcpath = lib
vendorpath = vendor
headerpath = include

$(binpath)/inode.o: $(headerpath)/inode.h $(srcpath)/inode.c
	gcc -c $(srcpath)/inode.c -o $(binpath)/inode.o

$(binpath)/file_descriptor_table.o: $(headerpath)/file_descriptor_table.h $(srcpath)/file_descriptor_table.c
	gcc -c $(srcpath)/file_descriptor_table.c -o $(binpath)/file_descriptor_table.o

$(binpath)/free_inode_map.o: $(headerpath)/free_inode_map.h $(srcpath)/free_inode_map.c
	gcc -c $(srcpath)/free_inode_map.c -o $(binpath)/free_inode_map.o

$(binpath)/free_block_map.o: $(headerpath)/free_block_map.h $(srcpath)/free_block_map.c
	gcc -c $(srcpath)/free_block_map.c -o $(binpath)/free_block_map.o

$(binpath)/types.o: $(headerpath)/types.h $(srcpath)/types.c
	gcc -c $(srcpath)/types.c -o $(binpath)/types.o
