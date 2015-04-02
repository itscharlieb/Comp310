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
	printf("\n--------------[print_test_struct]-------------\n");
	printf("anInt = [%d].\n", ts->anInt);
	printf("charArray = [%s].\n", ts->charArray);
	printf("-----------------------------------------------\n");
	fflush(stdout);
}

int main(void){
	printf("\n[main] Size of TestStruct = [%d].\n", (int)sizeof(TestStruct));
	my_mall_info();

	TestStruct* ts = (TestStruct*)my_malloc(sizeof(TestStruct));
	
	printf("[main] Assigning int field in the test struct.\n");
	fflush(stdout);
	ts->anInt = 4;

	printf("[main] Copying characters into the test struct.\n");
	fflush(stdout);
	strncpy(ts->charArray, testString, strlen(testString));

	print_test_struct(ts);

	my_mall_info();

	printf("[main] Freeing test struct memory from heap.\n");
	fflush(stdout);
	my_free(ts);

	my_mall_info();
}