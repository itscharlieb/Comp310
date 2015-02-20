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

static int sharedVariable = 0;
static int readCount = 0;
static sem_t sharedVariableMutex = 1;
static sem_t readCountMutex = 1;


int getRandomInRange(int upperBound){
	srand(time(NULL));
	return rand() % upperBound;
}

/*
*@returns a thread that reads the global variable sharedVariable
*and then sleeps for a random amount of time between 0 and 100 milliseconds
*/
static void* readerFunction(void* arg){
	sem_wait(&readCountMutex); //wait until read count variable is available
	readCount ++; //increment read count variable - this thread is now reading
	if(readCount > 0){ // if there are reader threads reading the sharedVariable

	}
	sem_post(&readCountMutex); //singal reader threads waiting for access to read count variable

	sem_wait(&readCountMutex); //wait until read count variable is available
	readCount --; //derement read count variable - this thread is no longer reading
	if(readCount == 0){ //if there are no reader threads reading the shared variable
		sem_post(&sharedVariableMutex); //signal a writer thread
	}
	sem_post(&readCountMutex); //signal reader threads waiting for access to read count variable
	sleep()
}

/**
*@returns a thread the increments the global variable sharedVariable by 10
*and then sleeps for a random amount of time between 0 and 100 milliseconds
*/
static void* writerFunction(void* arg){
	sem_wait(&sharedVariableMutex); //wait for exclusive access to the shared variable
	sharedVariable += 10; //increment shared variable by 10
	sem_post(&sharedVariableMutex); //signal threads waiting on sharedVariable access
}

int main(int argc, char** argv){
	int loopCounter;
	for(loopCounter = 0; loopCounter < 10; loopCounter ++){
		//create 10 writer threads
	}

	for(loopCounter = 0; loopCounter < 100; loopCounter ++){
		//create 100 reader threads
	}

}