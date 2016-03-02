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
	int* test = (int*)dlmalloc(4000*sizeof(int));
	dlfree(test);
	int* test2 = (int*)dlmalloc(4000*sizeof(int));
	return 0;
}



