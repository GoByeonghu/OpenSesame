// tee단 호출 process

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include "shm.h"

void tee_store(char *, char *);

// todo: encrpyt()
char *encrpyt(int flag) {
	unsigned char	*encrpyted_string;
	unsigned char	*plaintext;

	// todo: 암호화 하기

	// key 암호화: 대칭키와 id 암호화 -> 개인키로 암호화
	if (flag == 0) {
		plaintext = tee_read("SymmetricKey256.txt");
		// AES_Encrypt("test", encrpyted_string);

		// 해쉬 함수 
		SHA256_Encode(encrpyted_string, encrpyted_string);
	}

	// 개폐 명령 암호화: 개폐 명령 + 인증서 -> 대칭키로 암호화
	else if (flag == 1) {
		
	}

	//sprintf(encrpyted_string, "암호화된 문장");

	return encrpyted_string;
}


void do_encrypt(char *filename, int flag) {
	FILE	*file;
	char	*ptr;
	char	*encrypted_string;
	int		shmid;
	int		error;  // 대칭키 에러 표시

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

	// 대칭키가 없는 경우 새로 생성하고 file로 저장하는 함수 호출
	if (!flag) {
		error = AES_CreateKey();
	}
	else {
		error = 0;
	}

	char *pData=ptr;
	// 대칭키 생성이 오류난 경우: shared memory에 0 씀
	if (error) 
		sprintf(pData, 0);

	// 대칭키 생성이 성공인 경우: shared memory에 1 씀
	else {
		sprintf(pData, 1);

		// 대칭키 생성이 성공하면 암호화 호출
		encrypted_string = encrpyt(flag);

		// encrpyt() 결과를 파일로 저장
		tee_store(filename, encrypted_string);
	}

	// shared memory mapping 해제
	if(shmdt(ptr)<0){
		perror("shmdt");
		exit(1);
	}
}

// 파일로 저장
void tee_store(char *filename, char *data) {
	FILE	*file;
	char	*pt;

	file = fopen(filename, "wt");
	fseek(file, 0, SEEK_SET);

	pt = data;
	while (pt) {
		fputc(*pt, file);
		pt++;
	}
}

// 파일 읽기
unsigned char *tee_read(char *filename) {
   FILE *file;
   unsigned char buf[100000];

   file = fopen(filename, "r");
   fseek(file, 0, SEEK_SET);
   
   while (fgets(buf, 256, file)) {}
   fclose(file);
   return buf;
}