// tee단 호출 process

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include "shm.h"

// 동기화 처리 필요
void genSymKey(){
	char *ptr;
	int shmid;
	int error;  // 대칭키 에러 표시

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

	// todo: 대칭키를 생성하고 file로 저장하는 함수 호출


	char *pData=ptr;

	error = 1;
	// 대칭키 생성이 오류난 경우: shared memory에 0 씀
	if (error) 
		sprintf(pData, 0);
	// 대칭키 생성이 성공인 경우: shared memory에 1 씀
	else 
		sprintf(pData, 1);

	// shared memory mapping 해제
	if(shmdt(ptr)<0){
		perror("shmdt");
		exit(1);
	}
}

// todo: encrpyt()
char *encrpyt() {
	char *encrpyted_string;

	sprintf(encrpyted_string, "암호화된 문장");

	return encypted_string;
}


// todo: do_encrpyt()
void do_encrypt() {
	char	*encrypted_string;
	FILE	*file;
	
	encrypted_string = encrpyt()

	// encrpyt() 결과를 파일로 저장
	tee_store("encrpyted_string", encrpyted_string);	
}



// 파일로 저장
void tee_store(char *filename, char *data) {
	FILE	*file;

	file = fopen(filename, "wt");
	fseek(file, 0, SEEK_SET);

	char	*pt = data;
	while(pt) {
		fputc(*pt, file);
		pt++;
	}
}

// todo: 파일 읽기
void tee_open(char *filename) {

}
