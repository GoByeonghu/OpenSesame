// user단 호출

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"

// 암호화 (flag 0: key 암호문, flag1: 개폐명령 암호문)
int tee_encrypt(char *filename, int flag){
	char	*ptr;
	int		shmid;
	pid_t	pid;
	int		status;
	int		msg;

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

	if ((pid = fork()) < 0) {
		perror("fork error");
		exit(1);
	}
	// tee 프로세스
	else if (pid == 0){
		// 플래그도 같이 전송해서 어떤 걸 암호화 하는지
		// filename: close, flag: 1
		do_encrypt(filename, flag);
	}

	// 현재( == user)  프로세스
	else {
		waitpid(pid, &status, 0);

		// 암호화가 성공했는지 아닌지 체크
		sprintf(msg, ptr);

		// 성공했을 경우
		if (msg) {
			printf("암호화 성공\n");
		}

		// 실패했을 경우
		else {
			printf("암호화 실패\n");
		}

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

	}

	return 1;
}

