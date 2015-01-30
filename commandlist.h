/*
Author: Charles Bloomfield, 260520615
For Comp 310, Assignment 1
*/

#include <stdlib.h>

//linked list of commands, used to implement history feature
typedef struct commandNode{
	char* command[40];
	int recency;
	struct commandNode* previous;
}commandNode;

commandNode* addCommandToHistory(commandNode* headCommand, char* newCommand[]);
commandNode* getCommand(commandNode* headCommand, char c);
