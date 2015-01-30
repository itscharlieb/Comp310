/*
Author: Charles Bloomfield, 260520615
For Comp 310 Assignment 1
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
//#include <commandlist.h>
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
	char directoryBuffer[MAX_DIRECTORY_LENGTH]; //directory buffer

	//if getcwd correctly copies the working directory in the buffer
	if(getcwd(directoryBuffer, MAX_DIRECTORY_LENGTH) != NULL) {
		printf("%s\n", directoryBuffer);
	}
	//if getcwd returns NULL
	else {
		printf("Directory is too long.");
	}
}

void listBackgroundJobs(pid_t backgroundPIDs[], int *numBackgroundProcesses){
	int i, status;
	pid_t cpid, endID;

	printf("numBackgroundProcesses in jobs call = %d\n", *numBackgroundProcesses);

	if(*numBackgroundProcesses == 0){
		printf("No background processes running.\n");
	}

	else{
		printf("PIDs of jobs running in the background: \n");
		for(i = 0; i < *numBackgroundProcesses; i++){
			cpid = backgroundPIDs[i];

			printf("%d", cpid);

			endID = waitpid(cpid, &status, WNOHANG|WUNTRACED);
			printf("Return value from waitpid call: %d\n", endID);

			//waitpid failure
			if(endID == -1){
				printf("Call to waitpid failed.\n");
			}

			//process cpid is still running
			else if(endID == 0){
				printf("child process %d still running.\n", cpid);
			}

			//child process is not running
			else{
				if (WIFEXITED(status)){
					printf("Child ended normally.\n");
					--*numBackgroundProcesses;
				}
				else if (WIFSIGNALED(status))
					printf("Child ended because of an uncaught signal.\n");
				else if (WIFSTOPPED(status))
					printf("Child process has stopped.\n");
			}
		}
	}
}

void bringToForeground(int pid){
	int status;
	printf("Waiting on process %d.\n", pid);
	waitpid(pid, &status, 0);
	printf("Process %d is complete.\n", pid);
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

int main(void){
	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background; /* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE/+1]; /* command line (of 80) has max of 40 arguments */

	pid_t pid; /* pid of processes created by fork */
	int status; /* status of child process */
	//commandNode* headCommand;

	int numBackgroundProcesses = 0;
	pid_t backgroundPIDs[100];

	while (1){ /* Program terminates normally inside setup */
		background = 0;
		printf("COMMAND->\n");
		setup(inputBuffer, args, &background); /* get next command */

		if(strcmp(args[0], "cd") == 0){
			changeDirectory(args[1]);
		}

		else if(strcmp(args[0], "pwd") == 0){
			printWorkingDirectory();
		}

		else if(strcmp(args[0], "jobs") == 0){
			listBackgroundJobs(backgroundPIDs, &numBackgroundProcesses);
		}

		else if(strcmp(args[0], "fg") == 0){
			bringToForeground(*args[1]);
		}

		else if(strcmp(args[0], "r") == 0){
			//is *args[1] the proper way to reference the single character?
			//executeFromHistory(headCommand, *args[1]);
		}

		else if(strcmp(args[0], "exit") == 0){
			exit(0);
		}

		//if none of the above, create new process and use exec to run command
		else{
			pid = fork();

			//child process
			if(pid == 0){
				//if execvp returns < 0, it failed
				if(execvp(args[0], args) < 0){
		 			printf("*** FAILED TO EXECUTE COMMAND ***");
				}
			}

			//parent process
			else{
				// printf("Parent pid: %d.\n", pid);
				//if background == 1, let the child process run concurently
				if(background == 1){
					//add child pid to collection of child numBackgroundProcesses and increment numBackgroundProcesses
					backgroundPIDs[numBackgroundProcesses] = pid;
					numBackgroundProcesses++;
				}

				//otherwise, wait for the child to finish execution
				else{
					printf("waiting.... \n");
					waitpid(pid, &status, 0);
					printf("done!\n");
				}
			}
		}
	}
}
