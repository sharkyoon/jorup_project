#include <cstdlib>
#include <iostream>
#include <ctime>
#include <sys/types.h>
#include <sys/shm.h>
#include <cstddef>
#include <cstdio>
#include <cstring>
using namespace std;
const key_t key = 7970;
#define PAGE_SIZE 4096
#define POOL_SIZE 100
#define pagepool PAGE_SIZE*POOL_SIZE
#define MALLOC_ALIGNMENT 4

void* addr;
int shmid = shmget(key, pagepool, IPC_CREAT | 0660);
void* pool = (int*)shmat(shmid, (void*) 0, 0);
int* head = (int*)pool;
bool init = false;
bool check[POOL_SIZE];

int getRandom() {
	int random = rand() % (POOL_SIZE + 1);
	while(check[random]==false){
		random = rand() % (POOL_SIZE + 1);
	}
	cout << "page index :\t" << random << endl;
	check[random] = false;
	return random;
}



void* m_Alloc(int size){
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
	char* pagehead = (char*) head + PAGE_SIZE*pageindex;
	char* mm = (char*)pagehead + *pagehead + sizeof(int);
	*pagehead = *pagehead + (int)((size)/MALLOC_ALIGNMENT+1)*MALLOC_ALIGNMENT;
	if(*((int*)pagehead) > PAGE_SIZE - MALLOC_ALIGNMENT - 1){
		check[pageindex] = false;
	}
	cout << "pagehead :\t" << *((int*)pagehead) << endl;
	cout << "-------------------------------------" << endl;
	return mm;
}
