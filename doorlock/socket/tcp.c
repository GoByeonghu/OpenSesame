/*==========================================================================
 * [Program Name]: tcps.c
 * [Description]: tcp 소켓을 생성하고 소켓 주소를 bind하여 server process 생성
 * [Input]: nothing
 * [Output]: nothing
 * [Calls]: bzero, socket, bind, perror, exit, listen, read, write, sprintf
 * [특기사항]: nothing
 =========================================================================== */
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
void CloseServer(int signo)
{
	close(Sockfd); // socket 닫음
	printf("\nTCP Server exit......\n");

	exit(0);
}

void recvFromUser(char *filename, int fileFlag)
{
	int newSockfd, cliAddrLen, n;
	struct sockaddr_in cliAddr, servAddr;
	FILE *file = NULL;
	size_t filesize = 0, bufsize = 256;
	char buf[256];
	MsgType msg;
	int		option;

	signal(SIGINT, CloseServer);

	// socket을 생성한다.
	// 이때 ipv4(PF_INET), TCP(SOCK_STREAM)를 파라미터로 설정한다.
	if ((Sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket");
		exit(1);
	}

	option = 1;
	setsockopt(Sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

	bzero((char *)&servAddr, sizeof(servAddr));	  // 초기화
	servAddr.sin_family = PF_INET;				  // ipv4
	servAddr.sin_port = htons(SERV_TCP_PORT);	  // port 번호를 network byte order로 변경
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // network byte order로 변경한다. 이때 ip address가 여러개인 경우 어떤 ip address로 와도 처리한다.

	// 설정한 주소를 bind 시킨다.
	if (bind(Sockfd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
	{
		perror("bind");
		exit(1);
	}

	listen(Sockfd, 1); //한번에 하나만 기다리도록 설정
	cliAddrLen = sizeof(cliAddr);
	// accept로 client의 connection을 기다린다. 이때 cliAddr에서는 client의 주소 정보가 저장됨
	if ((newSockfd = accept(Sockfd, (struct sockaddr *)&cliAddr, &cliAddrLen)) < 0)
	{
		perror("accept");
		exit(1);
	}
	
	char flag[10];
	read(newSockfd, flag, 4);

	// 공개키 수신하는 경우
	if (flag[0] == '1')
	{
		printf("===== 공개키 수신 =====\n");
		file = fopen(filename, "wb");
		int nbyte = 256;
		while (nbyte != 0)
		{
			nbyte = recv(newSockfd, buf, bufsize, 0);
			int status = fwrite(buf, sizeof(char), nbyte, file);
			if (status == 16) {
				break;
			}
		}
		fclose(file);
		printf("===== key received =====\n");

		msg.type = MSG_OK;
		// 도어락 시리얼 번호 전송
		sprintf(msg.data, DOORLOCK);

		printf("===== send serial number =====\n");
		// 연결된 client socket descriptor로 reply를 보냄
		if (write(newSockfd, &msg, sizeof(msg)) < 0)
		{
			perror("write");
			exit(1);
		}
		printf("응답 완료.\n");

		usleep(10000);
		close(newSockfd);
	}

	// 암호문 파일 수신하는 경우
	else if (flag[0] == '2')
	{
		printf("===== 암호문 수신 =====\n");

		file = fopen(filename, "wb");
		int nbyte = 256;
		while (nbyte != 0)
		{
			nbyte = recv(newSockfd, buf, bufsize, 0);
			fwrite(buf, sizeof(char), nbyte, file);
			if (nbyte < bufsize) {
				break;
			}
		}
		fclose(file);

		int status;
		status = tee_decrypt(filename, fileFlag);

		// tee_decrypt 호출 결과 반환
		if (status)
			msg.type = MSG_OK;
		else
			msg.type = MSG_FAIL;

		// 연결된 client socket descriptor로 reply를 보냄
		if (write(newSockfd, &msg, sizeof(msg)) < 0)
		{
			perror("write");
			exit(1);
		}
		printf("응답 완료.\n");

		usleep(10000);
		close(newSockfd);
	}

	// 파일 수신하는 경우 (flag == 3)
	else {
		printf("===== 파일 수신 =====\n");

		file = fopen(filename, "wb");
		int nbyte = 256;
		while (nbyte != 0)
		{
			printf("nbyte: %d\n", nbyte);
			nbyte = recv(newSockfd, buf, bufsize, 0);
			fwrite(buf, sizeof(char), nbyte, file);
			if (nbyte < bufsize)
				break;
		}
		fclose(file);

		msg.type = MSG_OK;

		// 연결된 client socket descriptor로 reply를 보냄
		if (write(newSockfd, &msg, sizeof(msg)) < 0)
		{
			perror("write");
			exit(1);
		}
		printf("응답 완료.\n");

		usleep(10000);
		close(newSockfd);
	}

	close(Sockfd);
	return;
}
