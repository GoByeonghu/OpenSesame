// 도어락단 TEE 프로세스
#include "doorlock_TEE.h"

//func: String to int
int my_atoi(const char* str) {
	int ret = 0;
	int i;
	for (i = 0; str[i] != '\0'; i++)	//NULL문자만나면 for문 종료
		ret = ret * 10 + (str[i] - '0');	//숫자 문자에 '0'을 빼면 레알 숫자가 구해짐
	return ret;
}


// todo: 무결섬 검증, 복호화, 파일로 결과 저장 (tee_store 호출)
int decrpyt(unsigned char *buf, int flag, char *filename) {
	printf("decrypt()\n");
	int				status;
	unsigned char	decrpyted[1000000];

	status = 1;
	// 공개키로 복호화
	if (flag == 0) {
		printf("decrypt flag 0-1\n");
		int len;
		int decrypted_length;
		unsigned char *len_string=(unsigned char*)malloc(2048);
		unsigned char *target_string=(unsigned char*)malloc(2048);
		memset(len_string, 0, 4);
		memset(target_string, 0, 2048);
		memset(decrpyted, 0, 1000000);

		printf("buf: %s\n\n", buf);
		int k =0;
		for(int i=0; i<2046; i++){
			len_string[i]=buf[i];
			if(buf[i]=='L'){
				if(buf[i+1]='E'){
				len_string[i]='\0';
				k=0;
				}
			}
			target_string[k]=buf[i+2];
			k++;
		}
		len = my_atoi(len_string);
		printf("target: %s\n", target_string);

		// 공개키로 복호화
		//status = public_decrypt(buf, sizeof(buf), key, decrpyted);
		RSA_decrypt(target_string, len, decrpyted, &decrypted_length);
		decrpyted[decrypted_length]='\0';

		printf("decrypt flag 0-2\n");
		// 저장
		//tee_store(filename, decrpyted);
		AES_SetKey(decrpyted);///////////////////////////////////
		status=1;

		printf("decrypt flag 0-3\n");
		free(len_string);
		free(target_string);
	}

	// 대칭키로 복호화
	else if (flag == 1) {
		printf("decrypt flag 1\n");
		//get certification
		int cer_len;
                int cer_decrypted_length;
                unsigned char *len_string=(unsigned char*)malloc(4);
                unsigned char *cer_string=(unsigned char*)malloc(1024);
		unsigned char *cer_target_string=(unsigned char*)malloc(1024);
                int k =0;
		int observer_start=0;
		int observer_end=0;
                for(int i=0; i<2048; i++){
                        len_string[i]=buf[i];
                        if(buf[i]=='L'){
                                if(buf[i+1]='E'){
                                len_string[i]='\0';
                                k=0;
                                }
                        }
			if(buf[i]=='M'){
                                if(buf[i+1]='D'){
                                observer_start=i+2;
                                }
                        }
                        cer_target_string[k]=buf[i+2];
                        k++;
                }
		
		//remove crypto
		for(int i=observer_start-2; i<sizeof(cer_target_string); i++){
			cer_target_string[i] = '\0';
		}

                cer_len = my_atoi(len_string);

                // 공개키로 복호화
		unsigned char *certification=(uchar*)malloc(2048);
                RSA_decrypt(cer_target_string,cer_len, certification, &cer_len);
		
		
		//to do: check certification

		AES_Decrypt(buf+observer_start, decrpyted);
		tee_store(filename, decrpyted);
		status =1;

		free(certification);
		free(len_string);
		free(cer_string);
		free(cer_target_string);
	}

	printf("return decrypt\n");
    return status; // 성공 시 1, 실패 시 0
}

void do_decrypt(char *filename, int flag, char *decryptedfile) {
	printf("do_decrypt()\n");
	char			*ptr;
	int 			shmid;
	int 			error;  // 복호화 에러 표시
	unsigned char	*buf=(uchar*)malloc(2048);

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

	memset(buf, 0, 2048);
   	FILE *file = fopen(filename, "r");
   	fseek(file, 0, SEEK_SET);
   	
   	while (fgets(buf, 256, file)) {}
   	fclose(file);

	// 복호화 진행
	error = decrpyt(buf, flag, decryptedfile);

	printf("decrypt status\n");
	char *pData=ptr;
	// 복호화가 오류난 경우: shared memory에 0 씀
	if (!error){
	       printf("failed\n");	
		sprintf(pData, "0");
	}
	// 복호화가 성공인 경우: shared memory에 1 씀
	else {
	       printf("success\n");	
		sprintf(pData, "1");
	}

	// shared memory mapping 해제
	if(shmdt(ptr)<0){
		perror("shmdt");
		exit(1);
	}
	free(buf);
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
