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
	int* test = (int*)dlmalloc(sizeof(int));
	*test = 31;
	//cout<<"address : " <<test<<endl;
	int* test2 = (int*)dlmalloc(sizeof(int));
	*test2 = 35;
	//cout<<"address : "<<test2<<endl;
	//cout<< *test << endl;
	int* test3 = (int*)dlmalloc(sizeof(int));
	int* test4 = (int*)dlmalloc(sizeof(int));
	int* test5 = (int*)dlmalloc(sizeof(int));
	int* test6 = (int*)dlmalloc(sizeof(int));
	dlfree(test);
	dlfree(test2);
	int* test7 = (int*)dlmalloc(sizeof(int));
	*test3 = 39;
	//cout<<"address : "<<test3<<endl;
	return 0;
}



