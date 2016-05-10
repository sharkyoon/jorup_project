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
	for(int i = 0; i < 200; i++){
		int* test0 = (int*)m_Alloc(sizeof(int));
	}
	return 0;
}



