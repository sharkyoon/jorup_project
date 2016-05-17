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
const key_t flushkey = 1080;
#define PAGE_SIZE 4096
#define POOL_SIZE 100
#define pagepool PAGE_SIZE*POOL_SIZE
#define MALLOC_ALIGNMENT 4

//*
int shmid = shmget(key, pagepool, IPC_CREAT | 0660);
void* pool = (int*) shmat(shmid, (void*) 0, 0);
int* head = (int*) pool;
bool init = false;
bool check[POOL_SIZE];
extern int allocated_size[POOL_SIZE]; // 통계를 위한 variable
queue<MemInfo> meminfo;
int occupied = 0;

int flushid = shmget(flushkey, sizeof(int), IPC_CREAT | 0660);
void* flush0 = (int*) shmat(flushid, (void*) 0, 0);
int* flushpage = (int*) flush0;
int check_flush = 0;
/*
 *	getRandom() function returns random page index that is not used now.
 */
int getRandom() {
	int random = rand() % (POOL_SIZE);	//* get random integer in 0 ~ 100 
	while (check[random] == false) {
		random = rand() % (POOL_SIZE);
	}
	//cout << "page index :\t" << random << endl;
	check[random] = false;
	return random;
}

void* m_Alloc(int size) {
	if (check_flush != 0 && check_flush != *flushpage) {
		check_flush = *flushpage;
		check[check_flush] = true;
	}
	char* mm;
	if (size <= PAGE_SIZE - 3 * sizeof(int)) {
		cout << "====================================================" << endl;
		if (!init) {
			init = true;
			for (int i = 0; i < POOL_SIZE; i++) {
				check[i] = true;
			}
			srand((unsigned int) time(NULL));
		}

		if (pool == (void *) -1) {
			perror("SHMAT FAILED :\t");
			exit(0);
		}

		int pageindex = getRandom();
		//int pageindex = 21;
		char* pagehead = (char*) head + PAGE_SIZE * pageindex;
		char* check_blank = pagehead + sizeof(int);
		char* loca_blank = pagehead + 2 * sizeof(int);
		cout << pageindex << "번째 page에 할당되었습니다." << endl;
		cout << "할당할 데이터의 크기 : " << size << endl;
		int pagelocation;
		cout << "1. 페이지의 메모리 할당 정보" << endl;
		cout << "(1) page에 할당된 data의 끝 :\t" << *(int*) pagehead << endl;
		cout << "(2) page에 blank가 ";
		if (*(int*) check_blank == 0) {
			cout << "없음" << endl;
		} else {
			cout << "있음" << endl;
			cout << "blank size :\t" << *((int*) check_blank) << endl;
			cout << "blank location :\t" << *((int*) loca_blank) << endl;
		}
		//이와 같은 flush 방식은 만일 A 프로세스에서 flush를 하고, B 프로세스에서 다시 flush를 한 뒤
		// 다시 A 프로세스에서 flush를 할 경우 예외가 발생한다. 하지만 이는 확률이 매우 낮다.
		if (*(int*) pagehead > PAGE_SIZE * 0.8
				|| *(int*) pagehead + size > PAGE_SIZE * 0.8) {
			cout << "(3) page가 flush되었습니다." << endl;
			//cin.get();
			*flushpage = pageindex;
			check_flush = pageindex;
			*(int*) pagehead = 0;
			*(int*) check_blank = 0;
			*(int*) loca_blank = 0;
		} else {
			cout << "(3) page가 flush되지 않습니다." << endl;
		}

		cout << "2. 할당 방법" << endl;
		if (*((int*) check_blank) >= size) {
			cout << "(1) blank에 할당되었습니다." << endl;
			mm = (char*) pagehead + 3 * sizeof(int) + *((int*) loca_blank);
			cout << "할당 주소 :\t" << (int*) mm << endl;
			cout << "상대적 위치 : \t" << pageindex << "번째 페이지의 "
					<< *(int*) loca_blank << endl;
			pagelocation = *(int*) loca_blank;
			*(int*) check_blank = 0;	// why 0? -> 초기화해줘야지. 그 블랭크는 쓴 거니까
										// 더 정교하게 할거면 여기 안 쓴 부분을 새로운 블랭크로 할 수는 있겠지
			*(int*) loca_blank = 0;
		} else {
			cout << "(1) 맨 끝에 할당되었습니다." << endl;
			mm = (char*) pagehead + *pagehead + 3 * sizeof(int);
			//sizeof(int) 하나는 원래 있던것.
			//sizeof(int) 하나는 중간 빈 부분의 size를 저장하는 곳
			//sizeof(int) 하나는 빈 부분의 주소(페이지 내부에서의 위치)를 저장하는 곳

			cout << "할당 주소 :\t" << (int*) mm << endl;
			cout << "상대적 위치 : \t" << pageindex << "번째 페이지의 " << *(int*) pagehead
					<< endl;
			pagelocation = *(int*) pagehead;
			if (size % MALLOC_ALIGNMENT == 0) {
				*(int*) pagehead = *(int*) pagehead + size;
			} else {
				*(int*) pagehead = *(int*) pagehead
						+ ((size) / MALLOC_ALIGNMENT + 1) * MALLOC_ALIGNMENT;

			}
			cout << "바뀐 pagehead :\t" << *(int*) pagehead << endl;
		}
		cout << "3. QUEUE IN/OUT" << endl;
		cout << "(1) 사용된 페이지의 수가 " << occupied << "개이므로 할당 해제가 ";

		if (occupied > POOL_SIZE / 2) {
			cout << "일어났습니다." << endl;
			occupied--;
			MemInfo checkmeminfo = meminfo.front();
			meminfo.pop();
			cout << "할당이 해제되는 페이지 번호 :\t" << checkmeminfo.pagenum << endl;
			cout << "할당이 해제되어 생긴 빈 칸의 크기 :\t" << checkmeminfo.size << endl;
			cout << "할당이 해제되어 생긴 빈 칸의 상대적 위치 :\t" << checkmeminfo.location
					<< endl;
			char* blank_info = (char*) head + PAGE_SIZE * checkmeminfo.pagenum
					+ sizeof(int);
			char* blank_location = blank_info + sizeof(int);
			if (*(int*) blank_info < checkmeminfo.size) {
				cout << "할당 해제된 빈 칸이 원래의 빈 칸보다 커서 빈 칸을 대체했습니다." << endl;
				*(int*) blank_info = checkmeminfo.size;
				*(int*) blank_location = checkmeminfo.location;
			} else {
				cout << "할당 해제된 빈 칸이 원래의 빈 칸보다 작아서 빈 칸을 대체하지 못했습니다." << endl;
			}
			check[checkmeminfo.pagenum] = true;
			allocated_size[checkmeminfo.pagenum] = 0;
			cout << "페이지 상단에 저장된 빈 칸의 값 :\t" << *(int*) blank_info << endl;
			cout << "페이지 상단에 저장된 빈 칸의 위치 :\t" << *(int*) blank_location << endl;
			// 일단 예외처리는 생략.
			/*
			 if (*(blank_info - sizeof(int)) == checkmeminfo.size) { // 이 페이지에 유일하게 할당한 프로세스일 경우 blank 생성하지 않음
			 *(int*) blank_info = 0;
			 *(int*) blank_addr = 0;
			 *(blank_info - sizeof(int)) = 0; // *(pagehead) of this page is zero
			 }

			 else if (*(int*) (blank_info - sizeof(int))
			 == checkmeminfo.location) {	// page의 맨 끝에 이 process의 object가 존재할 경우 blank 생성하지 않음
			 *(int*) blank_info = 0;
			 *(int*) blank_addr = 0;
			 *(blank_info - sizeof(int)) -= checkmeminfo.size;
			 }
			 //casting 맞게 했는지 헷갈리는데 이거 번갈아가면서 할당해봐야 blank생기면서 잘 돌아가는지 확인 가능함.
			 else if (*(int*) blank_addr + *(int*) blank_info
			 == checkmeminfo.location - checkmeminfo.size) {
			 // page의 중간에 이 process의 object가 존재하되, 기존의 blank의 뒤에 바로 연결될 경우 blank를 합침
			 *(int*) blank_info = *(int*) blank_info + checkmeminfo.size;
			 } else if (*(int*) blank_addr == checkmeminfo.location) {
			 // page의 중간에 이 process의 object가 존재하되, 기존의 blank의 앞에 바로 연결될 경우 blank를 합침
			 *((int*) blank_info) = *(int*) blank_info + checkmeminfo.size;
			 *((int*) blank_addr) = checkmeminfo.location
			 - checkmeminfo.size;
			 } else if (*(int*) blank_info < checkmeminfo.size) {// page의 중간에 이 process의 object가 존재하되, 양쪽으로 다른 process의 object로 둘러싸여 있으며, 기존의 blank의 크기보다 클 때 blank를 변경
			 cout << "case4" << endl;
			 *((int*) blank_info) = checkmeminfo.size;
			 *((int*) blank_addr) = checkmeminfo.location
			 - checkmeminfo.size;
			 } else {
			 }
			 */
		} else {
			cout << "일어나지 않았습니다." << endl;
		}
		cout << "(2) 새롭게 할당된 데이터가 QUEUE에 들어갑니다. " << endl;
		/*
		 * Updating MemInfo
		 */
		//MemInfo(int _pagenum, int _size, int _location)
		/*
		 if(pagelocation <0){
		 cin.get();
		 }
		 */
		int tsize = (size / MALLOC_ALIGNMENT + 1) * MALLOC_ALIGNMENT;
		if (size % MALLOC_ALIGNMENT == 0) {
			tsize = size;
		}
		MemInfo tempmeminfo(pageindex, tsize, pagelocation);
		cout << "QUEUE에 저장되는 페이지 번호 :\t" << tempmeminfo.pagenum << endl;
		cout << "QUEUE에 저장되는 할당 크기 :\t" << tempmeminfo.size << endl;
		cout << "QUEUE에 저장되는 상대적 위치 :\t" << tempmeminfo.location << endl;

		meminfo.push(tempmeminfo);
		occupied++;
		cout << "4. 기타" << endl;
		cout << "사용되고 있는 페이지의 수 :\t" << occupied << endl;
		cout << "할당되는 메모리 주소 :\t" << (int*) mm << endl;
		allocated_size[pageindex] = size;
		return mm;
	} else {
		mm = (char*) malloc(size);
		cout << "big-object is allocated by general malloc" << endl;
		return mm;
	}
}
