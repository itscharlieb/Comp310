/**
* @author Charlie Bloomfield
* April 2, 2015s
**/

#include "memory_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* testString = "The quick brown fox jumps of the lazy brown dog";

typedef struct {
	int anInt;
	char charArray[100];
} TestStruct;

void print_test_struct(TestStruct* ts){
	printf("\n------------[print_test_struct]------------\n");
	printf("anInt = [%d].\n", ts->anInt);
	printf("charArray = [%s].\n", ts->charArray);
	printf("-------------------------------------------\n");
	fflush(stdout);
}

int main(void){
	printf("\n[main] Size of TestStruct = [%d].\n", (int)sizeof(TestStruct));
	my_mall_info();

	TestStruct* ts1 = (TestStruct*)my_malloc(sizeof(TestStruct));
	void* firstMallocAddress = ts1;

	my_mall_info();
	
	printf("[main] Assigning int field in the test struct.\n");
	fflush(stdout);
	ts1->anInt = 4;

	printf("[main] Copying characters into the test struct.\n");
	fflush(stdout);
	strncpy(ts1->charArray, testString, strlen(testString));

	print_test_struct(ts1);

	my_mall_info();

	printf("[main] Freeing test struct memory from heap.\n");
	fflush(stdout);
	my_free(ts1);

	my_mall_info();

	printf("[main] Reallocating memory for a test struct. Both structs should have the same address in the heap.\n");
	fflush(stdout);

	TestStruct* ts2 = (TestStruct*)my_malloc(sizeof(TestStruct));

	printf("[main] The two test structs have the same heap adderss? ");
	printf((ts1 == firstMallocAddress) ? "[TRUE]\n" : "[FALSE]\n");
	if(ts1 != firstMallocAddress){
		printf("[main] ts1 has address [%d] and ts2 has address [%d].\n", ts1, firstMallocAddress);
	}
	fflush(stdout);

	my_mall_info();
}