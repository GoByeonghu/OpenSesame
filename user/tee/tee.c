// tee ipc

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"

// 동기화 처리 필요
int tee(){
	char *ptr;
	int shmid;

	// SHM_KEY로 생성된 shared memor의 id를 shmid에 저장함
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
	char *pData=ptr+sizeof(int);

	// shared memory에 data를 쓰고 flag를 1로 변경함
	sprintf(pData,"This is a request from %d",getpid());
	*pInt=1;
	printf("Send a request.....");

	// flag가 1인 경우, shared memory에 써진 data가 없으므로 기다림(busy waiting)
	while(*pInt==1)
		;
	printf("Received reply: %s\n",pData);


	// shared memory mapping 해제
	if(shmdt(ptr)<0){
		perror("shmdt");
		exit(1);
	}

	return 0;
}



