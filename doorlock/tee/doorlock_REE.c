// 도어락단 REE 프로세스
#include "shm.h"

int tee_decrypt(char *filename, int flag) {
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
        // 복호화 및 무결성 검증 함수 호출
		do_decrypt(filename, flag);
        return 1;
	}

	// 현재( == user)  프로세스
	else {
		waitpid(pid, &status, 0);

		// 복호화가 성공했는지 아닌지 체크
		sprintf(msg, ptr);

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

		// 성공했을 경우
		if (msg) {
			printf("무결성 검증 및 복호화 성공\n");
            return 1;
		}

		// 실패했을 경우
		else {
			printf("무결성 검증 및 복호화 실패\n");
            return 0;
        }

	}
    
}