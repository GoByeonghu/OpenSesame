// user단 호출

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"

int main(){
	char *ptr;
	int shmid;

	// SHM_KEY로 shared memory를 생성하고 id를 shmid에 저장함
	if((shmid=shmget(SHM_KEY,SHM_SIZE,SHM_MODE))<0){
		perror("shmget");
		exit(1);
	}

	// shared memory를 virtual address와 mapping하여 pointer를 반환함
	if((ptr=shmat(shmid,NULL,0))==(void*)-1){
		perror("shmat");
		exit(1);
	}

	// shared memory의 4byte는 동기화를 위한 flag로 사용
	int *pInt=(int *)ptr;

	// flag가 0인 경우, shared memory에 써진 data가 없으므로 기다림(busy waiting)
	// 이때 shared memory를 만들면 0으로 초기화가 되기 때문에 처음에는 항상 기다리게 됨
	while(*pInt==0)
		;

	// data가 있는 경우 읽어서 출력하고 다시 씀
	char *pData=ptr+sizeof(int);
	printf("Received request: %s.....",pData);
	sprintf(pData,"This is a reply from %d.",getpid());
	*pInt=0; // 다시 flag 변경
	printf("Replied.\n");

	sleep(1);
	// shared memory mapping 해제
	if(shmdt(ptr)<0){
		perror("shmdt");
		exit(1);
	}

	// shared memory를 제거함
	if(shmctl(shmid,IPC_RMID,NULL)<0){
		perror("shmctl");
		exit(1);
	}

	return 0;
}

