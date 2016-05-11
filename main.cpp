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
	/*
	for(int i = 0; i < 200; i++){
		int* test0 = (int*)m_Alloc(sizeof(int));
		long* test1 = (long*)m_Alloc(sizeof(long));
	}
	*/
	//int* test0 = (int*)m_Alloc(sizeof(int));
	for(int i = 0; i < 14 ; i++){
		void* test1 = m_Alloc(6);
		void* test2 = m_Alloc(10);
		void* test3 = m_Alloc(14);
		void* test4 = m_Alloc(10);
	}

	//int* test2 = (int*)m_Alloc(sizeof(int));
	return 0;
}



