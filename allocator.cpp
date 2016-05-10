#include <cstdlib>
#include <iostream>
#include <ctime>
#include <sys/types.h>
#include <sys/shm.h>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <queue>
#include "allocator.h"
using namespace std;
const key_t key = 7970;
#define PAGE_SIZE 4096
#define POOL_SIZE 100
#define pagepool PAGE_SIZE*POOL_SIZE
#define MALLOC_ALIGNMENT 4

void* addr;
int shmid = shmget(key, pagepool, IPC_CREAT | 0660);
void* pool = (int*) shmat(shmid, (void*) 0, 0);
int* head = (int*) pool;
bool init = false;
bool check[POOL_SIZE];
queue<MemInfo> meminfo;
int occupied = 0;

int getRandom() {
	int random = rand() % (POOL_SIZE + 1);
	while (check[random] == false) {
		random = rand() % (POOL_SIZE + 1);
	}
	cout << "page index :\t" << random << endl;
	check[random] = false;
	return random;
}

void* m_Alloc(int size) {
	if (pool == (void *) -1) {
		perror("SHMAT FAILED : ");
		exit(0);
	}
	if (!init) {
		init = true;
		for (int i = 0; i < POOL_SIZE; i++) {
			check[i] = true;
		}
		srand((unsigned int) time(NULL));
	}

	int pageindex = getRandom();
	char* pagehead = (char*) head + PAGE_SIZE * pageindex;
	char* check_blank = pagehead + sizeof(int);
	char* mm;
	if (*((int*)check_blank) > size) {
		mm = (char*) check_blank + sizeof(int);
	} else {
		mm = (char*) pagehead + *pagehead + 2 * sizeof(int) + sizeof(int*);
		//sizeof(int) 하나는 원래 있던것.
		//sizeof(int) 하나는 중간 빈 부분의 size를 저장하는 곳
		//sizeof(int*)은 빈 부분의 주소를 저장하는 곳
		*pagehead = *pagehead
				+ (int) ((size) / MALLOC_ALIGNMENT + 1) * MALLOC_ALIGNMENT;
		if (*((int*) pagehead) > PAGE_SIZE - MALLOC_ALIGNMENT - 1) {
			check[pageindex] = false;
		}
	}
	cout << "pagehead :\t" << *((int*) pagehead) << endl;
	cout << "-------------------------------------" << endl;
	if (occupied > POOL_SIZE / 2) {
		occupied--;
		MemInfo checkmeminfo = meminfo.front();
		meminfo.pop();
		char* blank_info = (char*) head + PAGE_SIZE * checkmeminfo.pagenum
				+ sizeof(int);
		char* blank_addr = blank_info + sizeof(int);
		if (*((int*) blank_info) < checkmeminfo.size) {
			*((int*) blank_info) = checkmeminfo.size;
			blank_addr = checkmeminfo.address;
		}
		check[checkmeminfo.pagenum] = true;
	}

	MemInfo tempmeminfo(pageindex, size, mm);
	meminfo.push(tempmeminfo);
	occupied++;
	return mm;
}
