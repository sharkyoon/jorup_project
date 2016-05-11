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
	int location;
	MemInfo(int _pagenum, int _size, int _location){
		pagenum = _pagenum;
		size = _size;
		location = _location;
	}
};
