/**
* @author Charlie Bloomfield
* March 14, 2015
**/

#include "../include/output_file.h"
#include <stdio.h>

void init_output_file(){
	FILE* outputStream = freopen(OUTPUT_FILE, "w", stdout);
	if(outputStream == NULL){
		printf("Error setting up log file.\n");
	}

	else{
		printf("Opened log file.\n");
		fflush(outputStream);
	}
}

void reset_output_file(){
	FILE* outputStream = freopen("/dev/tty", "a", stdout);

	if(outputStream == NULL){
		printf("Error restoring output file.\n");
	}
}