/*
Author: Charles Bloomfield, 260520615
For Comp 310 Assignment 1
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <commandlist.h>
#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

/**
* setup() reads in the next command line, separating it into distinct tokens
* using whitespace as delimiters. setup() sets the args parameter as a
* null-terminated string.
*/
void setup(char inputBuffer[], char *args[], int *background){
	int length, /* # of characters in the command line */
	i, /* loop index for accessing inputBuffer array */
	start, /* index where beginning of next command parameter is */
	ct; /* index of where to place the next parameter into args[] */

	ct = 0;

 	/* read what the user enters on the command line */
 	length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
 	start = -1;

 	if (length == 0)
 		exit(0); /* ^d was entered, end of user command stream */

 	if (length < 0){
		perror("error reading the command");
		exit(-1); /* terminate with error code of -1 */
	}

 	/* examine every character in the inputBuffer */
 	for (i = 0; i < length; i++){
	 	switch (inputBuffer[i]){
		 	case ' ':
		 	case '\t': /* argument separators */
				if(start != -1){
					args[ct] = &inputBuffer[start]; /* set up pointer */
					ct++;
				}
				inputBuffer[i] = '\0'; /* add a null char; make a C string */
				start = -1;
				break;

			case '\n': /* should be the final char examined */
				if (start != -1){
					args[ct] = &inputBuffer[start];
					ct++;
				}
				inputBuffer[i] = '\0';
				args[ct] = NULL; /* no more arguments to this command */
				break;

			default: /* some other character */
				if (start == -1)
					start = i;
				if (inputBuffer[i] == '&'){
					*background = 1;
		 			inputBuffer[i] = '\0';
				}
		}
 	}
	args[ct] = NULL; /* just in case the input line was > 80 */
}

void changeDirectory(char* directory){
	chdir(directory);
}

void printWorkingDirectory() {
	int MAX_DIRECTORY_LENGTH = 128;

	//directory buffer
	char directoryBuffer[MAX_DIRECTORY_LENGTH];

	//if getcwd correctly copies the working directory in the buffer
	if(getcwd(directoryBuffer, MAX_DIRECTORY_LENGTH) != NULL) {
		printf("%s", directoryBuffer);
	}

	//if getcwd returns NULL
	else {
		printf("Directory is too long.");
	}
}

void listBackgroundJobs(int backgroundPIDs[], int numBackgroundProcesses){
	int i;
	printf("PIDs of jobs running in the background: ");
	for(i = 0; i < numBackgroundProcesses; i++){
		printf("%d, ", backgroundPIDs[i]);
	}
}

void bringToForeground(int pid){

}

/*
* exectuces the nth most recent command stored in history.
* @pre n <= 10
*/
void executeFromHistory(commandNode* headCommand, char c){
	commandNode* newCommand;
	if(newCommand = (getCommand(headCommand, c)) != NULL){

	}
	else{
		printf("A command starting with %c isn't in your history.\n", c);
	}
}

void executeKernelCommand(){

}

void executeBuiltInCommand(){

}

int main(void){
	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background; /* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE/+1]; /* command line (of 80) has max of 40 arguments */

	int pid; /* pid of processes created by fork */
	int status; /* status of child process */
	commandNode* headCommand;

	int numBackgroundProcesses = 0;
	int backgroudPIDs[100];

	while (1){ /* Program terminates normally inside setup */
		background = 0;
		printf("COMMAND->\n");
		setup(inputBuffer, args, &background); /* get next command */

		pid = fork();

		//child process
		if(pid == 0){
			// printf("Child pid: %d.\n", pid);
			if(strcmp(args[0], "cd") == 0){
				//directory is the second argument ???
				changeDirectory(args[1]);
			}

			else if(strcmp(args[0], "pwd") == 0){
				printWorkingDirectory();
			}

			else if(strcmp(args[0], "exit") == 0){
				exit(0);
			}

			else if(strcmp(args[0], "r") == 0){
				//is *args[1] the proper way to reference the single character?
				executeFromHistory(headCommand, *args[1]);
			}

			else if(strcmp(args[0], "jobs") == 0){
				listBackgroundJobs(backgroudPIDs, numBackgroundProcesses);
			}

			else if(strcmp(args[0], "fg") == 0){
				bringToForeground(args[1]);
			}

			//if none of the above, send to the kernel
			else{
				if(execvp(args[0], args) < 0){
					printf("*** FAILED TO EXECUTE COMMAND ***");
				}
			}
		}

		//parent process
		else{
			// printf("Parent pid: %d.\n", pid);
			//if backgroup == 1, the parent waits for the child process to finish execution
			if(background == 1){
				//wait on child process with id pid
				printf("waiting.... \n");
				waitpid(pid, &status, 0);
				printf("done!\n");
			}

			//otherwise, let child run concurrently
			else{
				//add child pid to collection of child numBackgroundProcesses
				backgroudPIDs[numBackgroundProcesses] = pid;
				numBackgroundProcesses++;
			}
		}
	}
}

