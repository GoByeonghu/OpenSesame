#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#include "tcp.h"
#include "./../tee/user_REE.c"
#include "./../tee/user_TEE.c"

// 파라미터 flag: 1(공개키), 2(암호문), 3(파일), filename: 보낼 파일 명
int sendToDoorlock(int flag, char *filename) {
	int sockfd, n;
	struct sockaddr_in servAddr;
	FILE *file;
	char buf[256];
	int sendFlag;
	size_t fsize, nsize = 0;
	MsgType msg;
	Encrypt enc;

	// socket을 생성한다.
	// 이때 ipv4(PF_INET), TCP(SOCK_STREAM)를 파라미터로 설정한다.
	if((sockfd=socket(PF_INET,SOCK_STREAM,0))<0){
		perror("socket");
		exit(1);
	}

	bzero(&servAddr,sizeof(servAddr));		
	servAddr.sin_family=PF_INET;			
	servAddr.sin_addr.s_addr=inet_addr(SERV_HOST_ADDR); 

	// connect로 server의 주소에 접속해서 연결한다.
	if(connect(sockfd,(struct sockaddr *)&servAddr, sizeof(servAddr))<0){
		perror("connect");
		exit(1);
	}

	// 공개키 보내는 경우
	if (flag == SEND_PUBLICKEY) {
		printf("===== 공개키 전송을 시작합니다... =====\n");

		// 플래그 1을 먼저 보냄
		sendFlag = flag;
		send(sockfd, sendFlag, 4, 0);

		// 공개키 파일 이름
		file = fopen(filename, "rb");
		fseek(file, 0, SEEK_END);
		fsize = ftell(file);
		fseek(file, 0, SEEK_SET);

		while (nsize != fsize) {
			int fpsize = fread(buf, 1, 256, file);
			nsize += fpsize;
			send(sockfd, buf, fpsize, 0);
		}		
		fclose(file);
		
		// sd에서 서버에서 온 reply를 읽는다.
		if((n=read(sockfd,&msg,sizeof(msg)))<0){
			perror("read");
			exit(1);
		}

		// 수신 성공한 경우
		if (msg.type == MSG_OK) {
			printf("도어락 고유번호: %s\n", msg.data);
			// 도어락 고유 번호 저장
			tee_store("doorlock_serialnumber", msg.data);
		}
		// 수신 실패한 경우
		else {
			printf("도어락 고유번호 수신 실패\n");
		}
		close(sockfd);
	}

	// 암호문 결과 파일 보내는 경우
	else if (flag == SEND_ENCRYPTFILE) {
		printf("===== 암호문 전송을 시작합니다... =====\n");
		// 플래그 2을 먼저 보냄
		sendFlag = flag;
		send(sockfd, sendFlag, 4, 0);
	
		// 암호문 file 전송
		file = fopen(filename, "rb");
		fseek(file, 0, SEEK_END);
		fsize = ftell(file);
		fseek(file, 0, SEEK_SET);

		while (nsize != fsize) {
			int fpsize = fread(buf, 1, 256, file);
			nsize += fpsize;
			send(sockfd, buf, fpsize, 0);
		}		
		fclose(file);

		// sd에서 서버에서 온 reply를 읽는다.
		if((n = read(sockfd, &msg, sizeof(msg)))<0){
			perror("read");
			exit(1);
		}
		
		// 전송 성공한 경우
		if (msg.type) {
			printf("암호문 전송 성공\n");
		}
		// 전송 실패한 경우
		else {
			printf("암호문 전송 실패\n");
		}
		
		close(sockfd); 

	}

	// 파일 보내는 경우
	else if (flag == SEND_FILE) {
		printf("===== 파일 전송을 시작합니다... =====\n");
		// 플래그 3을 먼저 보냄
		sendFlag = flag;
		send(sockfd, sendFlag, 4, 0);
	
		file = fopen(filename, "rb");
		fseek(file, 0, SEEK_END);
		fsize = ftell(file);
		fseek(file, 0, SEEK_SET);

		while (nsize != fsize) {
			int fpsize = fread(buf, 1, 256, file);
			nsize += fpsize;
			send(sockfd, buf, fpsize, 0);
		}		
		fclose(file);

		// sd에서 서버에서 온 reply를 읽는다.
		if((n = read(sockfd, &msg, sizeof(msg)))<0){
			perror("read");
			exit(1);
		}
		
		// 전송 성공한 경우
		if (msg.type) {
			printf("파일 전송 성공\n");
		}
		// 전송 실패한 경우
		else {
			printf("파일 전송 실패\n");
		}
		
		close(sockfd); 
	}
	
	return 0;
}

