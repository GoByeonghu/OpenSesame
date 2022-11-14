/*==========================================================================
 * [Program Name]: tcpc.c 
 * [Description]: tcp 통신을 하는 client process
 * [Input]: nothing
 * [Output]: nothing
 * [Calls]: socket, bzero, connect, perror, exit, write, read, close
 * [특기사항]: nothing
 =========================================================================== */

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

int main(){
	int sockfd, n;
	struct sockaddr_in servAddr;
	FILE *file;
	char buf[256];
	size_t fsize, nsize = 0;
	MsgType msg;

	// socket을 생성한다.
	// 이때 ipv4(PF_INET), TCP(SOCK_STREAM)를 파라미터로 설정한다.
	if((sockfd=socket(PF_INET,SOCK_STREAM,0))<0){
		perror("socket");
		exit(1);
	}

	bzero(&servAddr,sizeof(servAddr));		// 초기화
	servAddr.sin_family=PF_INET;			// ipv4
	servAddr.sin_port=htons(SERV_TCP_PORT);		// port 번호를 network byte order로 변경
	servAddr.sin_addr.s_addr=inet_addr(SERV_HOST_ADDR); // dotted decimal을 4 byte int ip 주소로 변경

	// connect로 server의 주소에 접속해서 연결한다.
	if(connect(sockfd,(struct sockaddr *)&servAddr, sizeof(servAddr))<0){
		perror("connect");
		exit(1);
	}

	msg.type=MSG_REQUEST;
	sprintf(msg.data,"===== 공개키 전송을 시작합니다... ===== ");
	
	// 공개키 파일 이름
	file = fopen("public.pem", "rb");
	fseek(file, 0, SEEK_END);
	// calculate file size
	fsize=ftell(file);
	// move file pointer to first
	fseek(file, 0, SEEK_SET);

	while (nsize!=fsize) {
		// read from file to buf
		// 1byte * 256 count = 256byte => buf[256];
		int fpsize = fread(buf, 1, 256, file);
		nsize += fpsize;
		send(sockfd, buf, fpsize, 0);
		// write(sockfd, buf, fpsize);
	}
	
	fclose(file);
	printf("===== 공개키 전송을 시작합니다... =====\n");
	
	// sd에서 서버에서 온 reply를 읽는다.
	if((n=read(sockfd,&msg,sizeof(msg)))<0){
		perror("read");
		exit(1);
	}
	printf("Received reply: %s\n",msg.data);
	close(sockfd); // sd 닫음

	return 0;
}

