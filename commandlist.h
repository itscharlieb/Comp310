/*
Author: Charles Bloomfield, 260520615
For Comp 310, Assignment 1
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//circlur array implementation of a list, without logic needed for remove operation
typedef struct commandList{
	char** commands[10]; //and array of commands, which themselves are 2d char arrays
	int head; //stores the indice of the most recently added command
	int numCommandsAdded;
}commandList;

//allocates space for a command list and inizializes it values properly
commandList* createCommandList();

//adds a command to the history, removing and old command if necessary
void addCommandToList(commandList* commandList, char* command[]);

//returns a reference to the 
char** getCommand(commandList* commandList, char c);

//prints the current state of commandList
void printList(commandList* commandList);
