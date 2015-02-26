/*
Author: Charles Bloomfield, 260520615
For Comp 310 Assignment 1
*/

/**
* setup() reads in the next command line, separating it into distinct tokens
* using whitespace as delimiters. setup() sets the args parameter as a
* null-terminated string.
*/
int setup(char inputBuffer[], char *args[], int *background);

void changeDirectory(char* directory);

void printWorkingDirectory();

void listBackgroundJobs(pid_t backgroundPIDs[], int *numBackgroundProcesses);

void bringToForeground(int pid);

void executeFromHistory(commandList* commandHistory, char c, int backgroundPIDs[], int* numBackgroundProcesses);

int distributeCommand(char* args[], commandList* commandHistory, int backgroundPIDs[], int* numBackgroundProcesses, int background);
