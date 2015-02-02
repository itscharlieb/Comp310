/*
Author: Charles Bloomfield, 260520615
For Comp 310, Assignment 1
*/

#include <commandlist.h>

commandList* createCommandList(){
	commandList* c = malloc(sizeof(commandList));
	c->head = -1;
	c->numCommandsAdded = 0;
	return c;
}

char firstChar(char** cpp){
	return **cpp;
}

//adds a command to the history, removing and old command if necessary
void addCommandToList(commandList* commandList, char* command[]){
	commandList->head ++;
	commandList->numCommandsAdded ++;

	//loop back
	if(commandList->head == 10){
		commandList->head = 0;
	}

	commandList->commands[commandList->head] = command;
}

//returns a reference to the first command starting with character c
char** getCommand(commandList* commandList, char c){
	int i;
	int n = commandList->numCommandsAdded;
	// printf("Request getCommand starting with %c.\n", c);
	//printList(commandList);
	int head = commandList->head;

	//do circular search if more than n commands have been added
	if(n > 10){
		for(i = head; i >= (head - 10); i--){
			if(firstChar(commandList->commands[i % 10]) == c){
				return commandList->commands[i % 10];
			}
		}
	}

	//search starting at the head back to the beginning with no circulating.
	else{
		for(i = head; i >= 0; i--){
			if(firstChar(commandList->commands[i]) == c){
				return commandList->commands[i];
			}
		}
	}

	printf("There is no command starting with character '%c' in your history.\n", c);
	return NULL;
}

//returns a reference to the most recently added command
char** getHeadCommand(commandList* commandList){
	return commandList->commands[commandList->head];
}
