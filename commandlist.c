/*
Author: Charles Bloomfield, 260520615
For Comp 310, Assignment 1
*/

#include <commandlist.h>
#include <stdlib.h>

/*
creates a new command, adds it to the head of the command list, and returns a reference
to the head of the command list
*/
commandNode* addCommandToHistory(commandNode* headCommand, char* newCommand[]){
	commandNode* newNode = malloc(sizeOf(commandNode));
	headCommand->command = newCommand;
	newNode->recency = headCommand->recency + 1;
	newNode->previous = headCommand;
	return newNode;
}

/*
finds a command in the history starting with letter
@param char c if the command exists
*/
commandNode* getCommand(commandNode* headCommand, char c){
	int n = 10;
	commandNode* tmp = headCommand;
	while(n > 0){
		if(tmp->command == NULL)
			return NULL;

		//if the first character of the command equals char c
		if(*(tmp->command[0])) == c){
			commandNode* newNode = malloc(sizeOf(commandNode));
			newNode->command = tmp->command;
			newNode->recency = headNode->recency + 1;
			newNode->previous = headNode;
			return newNode;]
		}
		tmp = tmp->previous;
	}

	return NULL; //return null if not in the most recent 10 commands
}

