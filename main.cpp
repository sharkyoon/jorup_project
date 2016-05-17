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
int allocated_size[100];
int main(){
	for(int i = 0; i < 1300 ; i++){
		void* test1 = m_Alloc(sizeof(int));
	}
	int sum = 0;
	for(int i = 0; i < 100 ; i++){
		sum += allocated_size[i];
	}
	cout << "합계  = " << sum << endl;
	return 0;
}



