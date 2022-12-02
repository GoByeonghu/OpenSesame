// 도어락단 TEE 프로세스
#include "shm.h"

// todo: 무결섬 검증, 복호화, 파일로 결과 저장 (tee_store 호출)
int decrpyt(unsigned char *buf, int flag, char *filename) {
	int				status;
	unsigned char	decrpyted[1000000];

	status = 1;
	// 공개키로 복호화
	if (flag == 0) {
		unsigned char *key;
		key = tee_read("PublicKey.pem");

		// 공개키로 복호화
		status = public_decrypt(buf, sizeof(buf), key, decrpyted);

		// 저장
		tee_store(filename, decrpyted);
	}

	// 대칭키로 복호화
	else if (flag == 1) {
		status = AES_Decrypt(buf, decrpyted);
		tee_store(filename, decrpyted);
	}

    return status; // 성공 시 1, 실패 시 0
}

void do_decrypt(char *filename, int flag, char *decryptedfile) {
	char			*ptr;
	int 			shmid;
	int 			error;  // 복호화 에러 표시
	unsigned char	*buf;

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

	buf = tee_read(filename);

	// 복호화 진행
	error = decrpyt(buf, flag, decryptedfile);

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

// 파일로 저장
void tee_store(char *filename, unsigned char *data) {
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
   FILE				*file;
   unsigned char	buf[100000];

   file = fopen(filename, "r");
   fseek(file, 0, SEEK_SET);
   
   while (fgets(buf, 256, file)) {}
   fclose(file);
   return buf;
}