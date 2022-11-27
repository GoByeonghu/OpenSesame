// 도어락단 TEE 프로세스
#include "shm.h"

// todo: 무결섬 검증, 복호화, 파일로 결과 저장 (tee_store 호출)
int decrpyt() {

    return 1; // 성공 시 1, 실패 시 0
}

//
void do_decrypt(char *filename) {
	char *ptr;
	int shmid;
	int error;  // 복호화 에러 표시

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

	// 대칭키를 생성하고 file로 저장하는 함수 호출
	error = decrpyt();

	char *pData=ptr;
	// 복호화가 오류난 경우: shared memory에 0 씀
	if (!error) 
		sprintf(pData, 0);
	// 복호화가 성공인 경우: shared memory에 1 씀
	else 
		sprintf(pData, 1);

	// shared memory mapping 해제
	if(shmdt(ptr)<0){
		perror("shmdt");
		exit(1);
	}
}

// todo: decrypt 결과 저장
void tee_store() {

}