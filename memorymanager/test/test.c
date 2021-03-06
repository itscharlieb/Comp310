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

typedef struct {
	int anInt;
	int anotherInt;
	int hugo;
} HugoStruct;

void print_test_struct(TestStruct* ts){
	printf("\n------------[print_test_struct]------------\n");
	printf("anInt = [%d].\n", ts->anInt);
	printf("charArray = [%s].\n", ts->charArray);
	printf("-------------------------------------------\n");
	fflush(stdout);
}

void test_general_functionality(){
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
		printf("[main] ts1 has address [%p] and ts2 has address [%p].\n", ts1, firstMallocAddress);
	}
	fflush(stdout);

	my_mall_info();
}

void test_block_merging(){
	printf("[test_block_merging] Creating seven test structs.\n");
	TestStruct* ts1 = (TestStruct*)my_malloc(sizeof(TestStruct));
	TestStruct* ts2 = (TestStruct*)my_malloc(sizeof(TestStruct));
	TestStruct* ts3 = (TestStruct*)my_malloc(sizeof(TestStruct));
	TestStruct* ts4 = (TestStruct*)my_malloc(sizeof(TestStruct));
	TestStruct* ts5 = (TestStruct*)my_malloc(sizeof(TestStruct));
	TestStruct* ts6 = (TestStruct*)my_malloc(sizeof(TestStruct));
	TestStruct* ts7 = (TestStruct*)my_malloc(sizeof(TestStruct));

	my_mall_info();

	printf("[test_block_merging] Freeing ts1.\n");
	fflush(stdout);
	my_free(ts1);
	my_mall_info();

	printf("[test_block_merging] Freeing ts5.\n");
	my_free(ts5);
	my_mall_info();

	printf("[test_block_merging] Freeing ts7.\n");
	my_free(ts7);
	my_mall_info();

	printf("[test_block_merging] Freeing ts3.\n");
	fflush(stdout);
	my_free(ts3);
	my_mall_info();

	printf("[test_block_merging] Freeing ts2, ts6, ts4 respectively.\n");
	my_free(ts2);
	my_free(ts6);
	my_free(ts4);
	my_mall_info();

	printf("[test_block_merging] Freeing ts2. All 3 blocks should merge into one.\n");
	my_free(ts2);
	my_mall_info();
}

void test_free_block_recycling(){
	TestStruct* ts1 = (TestStruct*)my_malloc(sizeof(TestStruct));
	TestStruct* ts2 = (TestStruct*)my_malloc(sizeof(TestStruct));

	my_free(ts1);
	my_free(ts2);

	printf("Creating two small structs.\n");
	fflush(stdout);
	HugoStruct* hs1 = (HugoStruct*)my_malloc(sizeof(HugoStruct));//should cause fragmentations
	HugoStruct* hs2 = (HugoStruct*)my_malloc(sizeof(HugoStruct));
}

void test_my_malloc_error(){
	void* v1 = my_malloc(128001);
	printf("%s\n", my_malloc_error);

	void* v2 = my_malloc(-1);
	printf("%s\n", my_malloc_error);
}

int main(void){
	printf("\n[main] Size of TestStruct = [%d].\n", (int)sizeof(TestStruct));
	printf("[main] Size of HugoStruct = [%d].\n", (int)sizeof(HugoStruct));
	my_mall_info();//initial state should be empty

	//test_general_functionality();
	//test_block_merging();
	//test_free_block_recycling();
	//test_my_malloc_error();
}