/*
Author: Charles Bloomfield, 260520615
For Comp 310, Assignment 1
*/

#include <commandlist.h>

/*
creates a new command, adds it to the head of the command list, and returns a reference
to the head of the command list
*/
commandNode* addCommandToHistory(struct commandNode* headCommand, char* newCommand[]){
	struct commandNode* newNode = malloc(sizeOf(commandNode));
	strncpy(newNode->command, newCommand, 40);
	newNode->recency = headCommand->recency + 1;
	newNode->previous = headCommand;
	return newNode;
}

/*
finds a command in the history starting with letter
@param c if the command exists
*/
commandNode* getCommand(commandNode* headCommand, char c){
	int n = 10;
	commandNode* tmp = headCommand;
	while(n > 0){
		if(tmp->command == NULL){
			return NULL;
		}

		//if the first character of the command equals char c
		if(*(tmp->command[0])) == c){
			commandNode* newNode = malloc(sizeOf(commandNode));
			newNode->command = tmp->command;
			newNode->recency = headNode->recency + 1;
			newNode->previous = headNode;
			return newNode;
			break;
		}
		tmp = tmp->previous;
	}
}

