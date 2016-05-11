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
	cout << "====================================================" << endl;
	if (pool == (void *) -1) {
		perror("SHMAT FAILED :\t");
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
	// for test
	// int pageindex = 3;
	char* pagehead = (char*) head + PAGE_SIZE * pageindex;
	char* check_blank = pagehead + sizeof(int);
	char* mm;
	int pagelocation;
	cout << "blank size :\t" << *((int*) check_blank) << endl;
	cout << "blank location :\t" << *(int*) (check_blank + sizeof(int)) << endl;
	if (*((int*) check_blank) >= size) {
		cout << "blank에 할당" << endl;
		mm = (char*) pagehead + 3 * sizeof(int)
				+ *(int*) (check_blank + sizeof(int));
		cout << (int*) mm << endl;
		*((int*) check_blank) = 0;
		pagelocation = *(int*) (check_blank + sizeof(int));
	} else {
		mm = (char*) pagehead + *pagehead + 3 * sizeof(int);
		//sizeof(int) 하나는 원래 있던것.
		//sizeof(int) 하나는 중간 빈 부분의 size를 저장하는 곳
		//sizeof(int) 하나는 빈 부분의 주소(페이지 내부에서의 위치)를 저장하는 곳
		pagelocation = *pagehead;
		*pagehead = *pagehead
				+ (int) ((size) / MALLOC_ALIGNMENT + 1) * MALLOC_ALIGNMENT;
		if (*((int*) pagehead) > PAGE_SIZE - MALLOC_ALIGNMENT - 1) {
			check[pageindex] = false;
		}
	}
	cout << "pagehead :\t" << *((int*) pagehead) << endl;
	if (occupied > POOL_SIZE / 2) {
		occupied--;
		MemInfo checkmeminfo = meminfo.front();
		meminfo.pop();
		cout << "------------------------------" << endl;
		cout << "info coming out of queue :\t" << checkmeminfo.pagenum << ", "
				<< checkmeminfo.size << ", " << checkmeminfo.location << endl;
		cout << "------------------------------" << endl;
		char* blank_info = (char*) head + PAGE_SIZE * checkmeminfo.pagenum
				+ sizeof(int);
		char* blank_addr = blank_info + sizeof(int);
		if (*((int*) blank_info) < checkmeminfo.size) {
			*((int*) blank_info) = (checkmeminfo.size / MALLOC_ALIGNMENT + 1) * MALLOC_ALIGNMENT;
			*((int*) blank_addr) = checkmeminfo.location;
		}
		check[checkmeminfo.pagenum] = true;
	}

	MemInfo tempmeminfo(pageindex, ((size) / MALLOC_ALIGNMENT + 1) * MALLOC_ALIGNMENT, *pagehead);
	cout << "info going in queue :\t" << pageindex << ", " << size << ", "
			<< *(int*)pagehead << endl;
	meminfo.push(tempmeminfo);
	occupied++;
	cout << "occupied page # :\t" << occupied << endl;
	cout << "returned memory address :\t" << (int*) mm << endl;
	return mm;
}
