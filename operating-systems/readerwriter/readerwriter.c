/**
* @author Charlie Bloomfield
* February 19, 2015
* Comp 310 Assignment 2
*/

#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct data{
	double min;
	double avg;
	double max;
}runtimeData;

runtimeData readerData[100];
runtimeData writerData[10];

int numLoops;
int sharedVariable = 0;
int readCount = 0;
sem_t sharedVariableMutex;
sem_t readCountMutex;

/**
* @param timeval t1
* @param timeval t2
* @return the difference, in milliseconds, between start and finish
*/
double timeDiffernceInMilliseconds(struct timeval start, struct timeval finish){
	double mStart = (start.tv_sec) * 1000 + (start.tv_usec) / 1000;
	double mFinish = (finish.tv_sec) * 1000 + (finish.tv_usec) / 1000;

	//printf("[mStart] %f. [mFinish] %f.\n", mStart, mFinish);
	return (mFinish - mStart);
}

/** 
*@param in upperBound
*@return a pseudo random number between 0 and upperBound
*/
int getRandomBoundedBy(int upperBound){
	srand(time(NULL));
	return rand() % upperBound;
}

/**
* displays the runtime data for each reader and writer thread.
*/
void displayRuntimeData(){
	int i;
	for(i = 0; i < 10; i++){
		printf("[Writer Thread (%d)] [Min] %f. [Avg] %f. [Max] %f.\n", (i + 1), writerData[i].min, writerData[i].avg, writerData[i].max);
	}

	for(i = 0; i < 100; i++){
		printf("[Reader Thread (%d)] [Min] %f. [Avg] %f. [Max] %f.\n", (i + 1), readerData[i].min, readerData[i].avg, readerData[i].max);
	}
}

/**
* prints the min, avg, and max valus from the input double array.
*/
void storeWaitingTimes(double waitTimes[], int numWaitTimes, int isReader, int threadNum){
	double min = -1, sum = 0, max = 0, avg, current;
	int i;
	for(i = 0; i < numWaitTimes; i++){
		current = waitTimes[i];
		if(min < 0)
			min = current; //first assignment
		if(current < min)
			min = current;
		if(current > max)
			max = current;
		sum += current;
	}

	double numElements = (double) numWaitTimes;
	avg = sum / numElements;

	if(isReader){
		readerData[threadNum].min = min, readerData[threadNum].avg = avg, readerData[threadNum].max = max;
	}
	else{
		writerData[threadNum].min = min, writerData[threadNum].avg = avg, writerData[threadNum].max = max;
	}
}

/**
* loops and tries to read the shared variable
*/
void* readerFunction(void* arg){
	int threadNum = *((int *) arg);
	double waitTimes[numLoops];
	struct timeval start, finish;

	int i;
	for(i = 0; i < numLoops; i++){
		if(sem_wait(&readCountMutex) == -1) //wait until read count variable is available
			printf("Error waiting on readCountMutex.\n");

		readCount ++; //increment read count variable - this thread is now reading

		gettimeofday(&start, NULL); //start timer
		if(readCount == 1){ //if this is the only thread currently accessing readCount, block sharedVariable
			if(sem_wait(&sharedVariableMutex) == -1) //increment readCount variable
				printf("Error waiting on sharedvariableMutex.\n");
		}
		gettimeofday(&finish, NULL); //stop timer
		waitTimes[i] = timeDiffernceInMilliseconds(start, finish);

		if(sem_post(&readCountMutex) == -1) //singal reader threads waiting for access to read count variable
			printf("Error signalling readCountMutex.\n");

		//DO READ. SHVINKTER!

		if(sem_wait(&readCountMutex) == -1) 
			printf("Error waiting on readCountMutex.\n"); //wait until read count variable is available

		readCount --; //decrement read count variable - this thread is no longer reading

		if(readCount == 0){ //if there are no reader threads reading the shared variable
			if(sem_post(&sharedVariableMutex) == -1) //signal threads waiting for access to the shared variable
				printf("Error signalling sharedVariableMutex.\n");
		}

		if(sem_post(&readCountMutex) == -1) //signal reader threads waiting for access to read count variable
			printf("Error signalling readCountMutex.\n");

		usleep(getRandomBoundedBy(100) * 1000);
	}

	storeWaitingTimes(waitTimes, numLoops, 1, threadNum);
	return NULL;
}

/**
* loops and tries to write to the shared variable
*/
void* writerFunction(void* arg){
	int threadNum = *((int *) arg);
	double waitTimes[numLoops];
	struct timeval start, finish;

	int i;
	for(i = 0; i < numLoops; i++){

		gettimeofday(&start, NULL); //start timer
		if(sem_wait(&sharedVariableMutex) == -1) //if can gain access to the shared variable
			printf("Error waiting on sharedVariableMutex.\n");
		gettimeofday(&finish, NULL); //stop timer
		waitTimes[i] = timeDiffernceInMilliseconds(start, finish);

		sharedVariable += 10; //increment shared variable by 10
		if(sem_post(&sharedVariableMutex) == -1) //signal threads waiting on sharedVariable access
			printf("Error signalling sharedVariableMutex.\n");

		usleep(getRandomBoundedBy(100) * 1000); //sleep for a while, BROSEPH
	}

	storeWaitingTimes(waitTimes, numLoops, 0, threadNum);
	return NULL;
}

int main(int argc, char* argv[]){
	//TODO parse integer passed from command line
	numLoops = 500;


	if(sem_init(&sharedVariableMutex, 0, 1) == -1){
		printf("Error creating sharedVariableMutex semaphore.\n");
		exit(-1);
	}

	if(sem_init(&readCountMutex, 0, 1) == -1){
		printf("Error creating readCountMutex semaphore.\n");
		exit(-1);
	}

	pthread_t writerThreads[10];
	pthread_t readerThreads[100];

	int validator, i;

	for(i = 0; i < 100; i ++){
		validator = pthread_create(&readerThreads[i], NULL, readerFunction, &i);
	}

	for(i = 0; i < 10; i ++){
		validator = pthread_create(&writerThreads[i], NULL, writerFunction, &i);
	}

	char* b;
	for(i = 0; i < 100; i++){
		pthread_join(readerThreads[i], (void**)&b);
	}

	for(i = 0; i < 10; i++){
		pthread_join(writerThreads[i], (void**)&b);
	}
	displayRuntimeData();
	printf("[Value of shared variable back in main] %d.\n", sharedVariable);
}