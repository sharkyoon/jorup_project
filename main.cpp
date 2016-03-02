/*
 * main.cpp
 *
 *  Created on: 2016. 2. 24.
 *      Author: sharkyoon & sugeun
 */

#include <iostream>
#include <cstdlib>
#include "allocator.h"
using namespace std;
int main(){
 	cout<<"\n\nprocess2"<<endl;
	cout<<"\n\nTEST_SMALL_CHUNK"<<endl;
	int* test = (int*)dlmalloc(sizeof(int));
	*test = 1;
	cout<<*test<<endl;

	int* test1 = (int*)dlmalloc(4090);
	*test1 = 4090;
	cout<<*test1<<endl;

	int* test2 = (int*)dlmalloc(2050);
	*test2 = 2050;
	cout<<*test2<<endl;

	int* test3 = (int*)dlmalloc(2050);
	*test = 2050;
	cout<<*test3<<endl;


	cout<<"\n\nTEST_BIG_CHUNK"<<endl;
	int* test4 = (int*)dlmalloc(4097);
	*test4 = 2;
	cout<<*test4<<endl;

	int* test5 = (int*)dlmalloc(4097);
	*test5 = 3;
	cout<<*test5<<endl;
	return 0;
}



