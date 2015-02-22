/**
@author Charlie Bloomfield
February 19, 2015
Comp 310 Assignment 2
*/

#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h> //for random number generation

int sharedVariable = 0;
int readCount = 0;
sem_t sharedVariableMutex;
sem_t readCountMutex;

/** 
*@param in upperBound
*@return a pseudo random number between 0 and upperBound
*/
int getRandomBoundedBy(int upperBound){
	srand(time(NULL));
	return rand() % upperBound;
}

/**
*@returns a thread that reads the global variable sharedVariable
*and then sleeps for a random amount of time between 0 and 100 milliseconds
*/
void* readerFunction(void* arg){
	int numLoops = *((int *) arg);

	int i;
	for(i = 0; i < numLoops; i++){
		sem_wait(&readCountMutex); //wait until read count variable is available
		readCount ++; //increment read count variable - this thread is now reading
		if(readCount == 1){ //if there is already at least one thread reading, increment readCount variable
			sem_wait(&sharedVariableMutex);
		}
		sem_post(&readCountMutex); //singal reader threads waiting for access to read count variable

		printf("%d\n", sharedVariable); //read variable

		sem_wait(&readCountMutex); //wait until read count variable is available
		readCount --; //decrement read count variable - this thread is no longer reading
		if(readCount == 0){ //if there are no reader threads reading the shared variable
			sem_post(&sharedVariableMutex); //signal a writer thread
		}
		sem_post(&readCountMutex); //signal reader threads waiting for access to read count variable
		sleep(getRandomBoundedBy(100));
	}

	return NULL;
}

/**
*@returns a thread the increments the global variable sharedVariable by 10
*and then sleeps for a random amount of time between 0 and 100 milliseconds
*/
void* writerFunction(void* arg){
	int waitTime = 0;
	int numLoops = *((int *) arg);
	int i;
	for(i = 0; i < numLoops; i++){
		if(sem_trywait(&sharedVariableMutex)){ //if can gain access to the shared variable
			sharedVariable += 10; //increment shared variable by 10
			sem_post(&sharedVariableMutex); //signal threads waiting on sharedVariable access

			//TODO store waitTime somehow
			return;
		}
		int random = getRandomBoundedBy(100);
		sleep(random); //sleep for a while, BROSEPH
		waitTime += random; //increment the amount of time that this writer thread has waited to access the shared variable
	}
}

int main(int argc, char* argv[]){
	//TODO parse integer passed from command line
	int numLoops = 1;

	sem_init(&sharedVariableMutex, 0, 1);
	sem_init(&readCountMutex, 0, 1);

	pthread_t writerThreads[10];
	pthread_t readerThreads[100];

	int validator;
	int i;
	for(i = 0; i < 10; i ++){
		validator = pthread_create(&writerThreads[i], NULL, writerFunction, &numLoops);
	}

	// for(i = 0; i < 100; i ++){
	// 	validator = pthread_create(&readerThreads[i], NULL, readerFunction, &numLoops);
	// }

	printf("%d\n", sharedVariable);
}