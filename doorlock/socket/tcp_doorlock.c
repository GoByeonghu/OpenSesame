/*==========================================================================
 * [Program Name]: tcps.c 
 * [Description]: tcp 소켓을 생성하고 소켓 주소를 bind하여 server process 생성
 * [Input]: nothing
 * [Output]: nothing
 * [Calls]: bzero, socket, bind, perror, exit, listen, read, write, sprintf
 * [특기사항]: nothing
 =========================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include "tcp.h"

int Sockfd;

/* ==============================================================================
 * [Function Name]: CloseServer
 * [Description]: SIGINT signal이 온 경우 소켓을 닫고 server process 종료
 * [Input]: nothing
 * [Output]: nothing
 * [Call By]: main()
 * [Calls]: close, remove, perror, printf, exit
 * [Given]: int signo // signal number
 * [Returns]: nothing
 ================================================================================*/
void CloseServer(int signo){
	close(Sockfd); // socket 닫음
	printf("\nTCP Server exit......\n");

	exit(0);
}

int main(){
	int newSockfd, cliAddrLen,n;
	struct sockaddr_in cliAddr, servAddr;
	FILE *file = NULL;
	size_t filesize = 0, bufsize = 0;
	MsgType msg;

	signal(SIGINT,CloseServer);

	// socket을 생성한다.
	// 이때 ipv4(PF_INET), TCP(SOCK_STREAM)를 파라미터로 설정한다.
	if((Sockfd=socket(PF_INET,SOCK_STREAM,0))<0){
		perror("socket");
		exit(1);
	}

	bzero((char *)&servAddr,sizeof(servAddr));	// 초기화
	servAddr.sin_family=PF_INET;			// ipv4
	servAddr.sin_port=htons(SERV_TCP_PORT);		// port 번호를 network byte order로 변경
	servAddr.sin_addr.s_addr=htonl(INADDR_ANY);// network byte order로 변경한다. 이때 ip address가 여러개인 경우 어떤 ip address로 와도 처리한다.

	// 설정한 주소를 bind 시킨다.
	if(bind(Sockfd,(struct sockaddr *)&servAddr,sizeof(servAddr))<0){
		perror("bind");
		exit(1);
	}

	listen(Sockfd,5); //한번에 5개를 기다리도록 설정

	printf("TCP served started.....\n");

	cliAddrLen=sizeof(cliAddr);
	while(1){
		// accept로 client의 connection을 기다린다. 이때 cliAddr에서는 client의 주소 정보가 저장됨
		if((newSockfd=accept(Sockfd,(struct sockaddr *)&cliAddr,&cliAddrLen))<0){
			perror("accept");
			exit(1);
		}
		/* socket descriptor에서 정보를 읽음
		if((n=read(newSockfd,&msg,sizeof(msg)))<0){
			perror("read");
			exit(1);
		}
		*/

		/* 공개키를 저장할 파일 이름*/
		file = fopen("public.pem", "wb");
		
		int nbyte = 256;
		while(nbyte != 0) {
			nbyte = read(newSockfd, buf, bufsize, 0);
			fwrite(buf, sizeof(char), nbyte, file);
		}

		fclose(file);

		//printf("Received request: %s......",msg.data);
		msg.type=MSG_REPLY;
		sprintf(msg.data,"This is a reply from %d.",getpid());

		// 연결된 client socket descriptor로 reply를 보냄
		if(write(newSockfd,&msg,sizeof(msg))<0){
			perror("write");
			exit(1);
		}
		printf("Replied.\n");

		usleep(10000);
		// sd를 닫음
		close(newSockfd);
	}

	return 0;
}

