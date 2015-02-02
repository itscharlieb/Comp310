/*
Author: Charles Bloomfield, 260520615
For Comp 310 Assignment 1
*/
#include <commandlist.h>
#include <shell.h>
#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

/**
* setup() reads in the next command line, separating it into distinct tokens
* using whitespace as delimiters. setup() sets the args parameter as a
* null-terminated string.
*/
int setup(char inputBuffer[], char *args[], int *background){
	int length, /* # of characters in the command line */
	i, /* loop index for accessing inputBuffer array */
	start, /* index where beginning of next command parameter is */
	ct; /* index of where to place the next parameter into args[] */

	ct = 0;

 	/* read what the user enters on the command line */
 	length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
 	start = -1;

 	if (length == 0){
 		exit(0); /* ^d was entered, end of user command stream */
 	}

 	if (length < 0){
		return -1;
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
	return 0;
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

	if(*numBackgroundProcesses == 0){
		printf("No background processes running.\n");
		return;
	}

	printf("State of current child processes: \n");
	for(i = 0; i < *numBackgroundProcesses; i++){
		cpid = backgroundPIDs[i];
		endID = waitpid(cpid, &status, WNOHANG|WUNTRACED);

		//waitpid failure
		if(endID == -1){
			printf("Unknown state of process %d. Call to waitpid failed.\n", cpid);
		}

		//process cpid is still running
		else if(endID == 0){
			printf("Child process %d still running.\n", cpid);
		}

		//child process is not running
		else{
			if (WIFEXITED(status)){
				printf("Child process %d ended normally.\n", cpid);
				--*numBackgroundProcesses;
			}
			else if (WIFSIGNALED(status)){
				printf("Child process %d ended because of an uncaught signal.\n", cpid);
				--*numBackgroundProcesses;
			}
				
			else if (WIFSTOPPED(status)){
				printf("Child process %d has stopped.\n", cpid);
				--*numBackgroundProcesses;
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
* executes the nth most recent command stored in history.
*/
void executeFromHistory(commandList* commandHistory, char c, int backgroundPIDs[], int* numBackgroundProcesses){
	char** newCommand;
	if(c != '\0'){
		newCommand = getCommand(commandHistory, c);
	}
	else{
		newCommand = getHeadCommand(commandHistory);
	}

	if(newCommand != NULL){
		distributeCommand(newCommand, commandHistory, backgroundPIDs, numBackgroundProcesses, 0);
		addCommandToList(commandHistory, newCommand);
	}
}

/*
*@returns 1 if the command is requesting from history, 0 otherwise
*/
int distributeCommand(char* args[], commandList* commandHistory, int backgroundPIDs[], int* numBackgroundProcesses, int background){
	int isHistory = 0;
	if(strcmp(args[0], "cd") == 0){
		changeDirectory(args[1]);
	}
	else if(strcmp(args[0], "pwd") == 0){
		printWorkingDirectory();
	}
	else if(strcmp(args[0], "jobs") == 0){
		listBackgroundJobs(backgroundPIDs, numBackgroundProcesses);
	}
	else if(strcmp(args[0], "fg") == 0){
		int i = atoi(args[1]);
		bringToForeground(i);
	}
	else if(strcmp(args[0], "r") == 0){
		isHistory = 1;
		//if user entered a command in the form - r 'x'
		if(args[1] != NULL){
			executeFromHistory(commandHistory, *args[1], backgroundPIDs, numBackgroundProcesses);
		}
		else{
			executeFromHistory(commandHistory, '\0', backgroundPIDs, numBackgroundProcesses);
		}
	}
	else if(strcmp(args[0], "exit") == 0){
		exit(0);
	}
	//fork a child process
	else{
		int pid = fork();

		//child process
		if(pid == 0){
			//if execvp returns < 0, it failed
			if(execvp(args[0], args) < 0){
	 			printf("*** FAILED TO EXECUTE COMMAND ***\n");
	 			exit(0);
			}
		}
		//parent process, pid is that of the child process
		else{
			// printf("Parent pid: %d.\n", pid);
			//if background == 1, let the child process run concurently
			if(background == 1){
				//add child pid to collection of child numBackgroundProcesses and increment numBackgroundProcesses
				backgroundPIDs[*numBackgroundProcesses] = pid;
				*numBackgroundProcesses = *numBackgroundProcesses + 1;
			}

			//otherwise, wait for the child to finish execution
			else{
				int status;
				waitpid(pid, &status, 0);
			}
		}
	}
	return isHistory;
}

int main(void){
	int background, status; /* equals 1 if a command is followed by '&' */
	pid_t pid; /* pid of processes created by fork */

	int numBackgroundProcesses = 0;
	pid_t backgroundPIDs[100];
	commandList* commandHistory = createCommandList();

	while (1){ /* Program terminates normally inside setup */
		//realocate these buffers for each command to make implementing history easier
		char* inputBuffer = (char*)malloc(sizeof(int) * MAX_LINE); /* buffer to hold the command entered */
		char** args = (char**)malloc(sizeof(char*) * 40); /* command line (of 80) has max of 40 arguments */

		background = 0;
		printf("Command->\n");
		//if setup returns a 0, it successfully created a command
		if(setup(inputBuffer, args, &background) == 0){
			//If the command is not a history command, add it directly to the history.
			//Otherwise, it's taken care of in executeHistory
			if(distributeCommand(args, commandHistory, backgroundPIDs, &numBackgroundProcesses, background) == 0){
				addCommandToList(commandHistory, args);
			}
		}
		else{
			printf("Could not parse input command.\n");
		}
	}
}
