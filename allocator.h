/*
 * allocator.h
 *
 *  Created on: 2016. 2. 24.
 *      Author: sharkyoon & sugeun
 */

void* m_Alloc(int size);
void* m_Free(int size);

class MemInfo{
public:
	int pagenum;
	int size;
	char* address;
	MemInfo(int _pagenum, int _size, char* _address){
		pagenum = _pagenum;
		size = _size;
		address = _address;
	}
};
