#define MAXFILENAME 13

#ifndef SFS_API_H
#define SFS_API_H

int mksfs(int fresh);
int sfs_fopen(char *name);
int sfs_fclose(int fileID);
int sfs_fwrite(int fileID, const char *buf, int length);
int sfs_fread(int fileID, char *buf, int length);
int sfs_fseek(int fileID, int offset);
int sfs_remove(char *file);
int sfs_get_next_file_name(char* filename);
int sfs_get_file_size(const char* path);

#endif