// doorlock단 메인 프로세스

#include "doorlock.h"

// [Name]: main()
// [Function]: entry point
// [input]: argc(int), argv(char **)
// [output]: nothing
int main(int argc, char *argv[])
{
    /* 1. 도어락 등록 */
    // 공개키 수신 (flag 역할 없음)
    recvFromUser("PublicKey.pem", 0);

    // key 암호문 수신
    recvFromUser("encrpyted_string", 0);

    // ID 수신 (flag 역할 없음))
    recvFromUser("ID", 0);

    /* 2. 도어락 개폐 수행 */
    // 개폐 명령 암호문 수신
    recvFromUser("opencommand", 2);

    sleep(5);
    // 명목상 문 열기
    openSesame();

}

// [Name]: openSesame
// [Function]: 문 개폐 제어
// [input]: nothing
// [output]: nothing
void openSesame() {
	printf("===== door control started... =====\n");
    FILE *file;
    unsigned char   command[100000];

    memset(command, 0, 100000);
    file = fopen("opencommand", "r");
    fseek(file, 0, SEEK_SET);
    while(fgets(command, 256, file)) {}
    fclose(file);
    
    printf("인증되었습니다.\n");

    if (strcmp(command, "open") == 0) {
        printf("===== 문이 열렸습니다. =====\n");
	exit(1);
    }
    else if (strcmp(command, "close") == 0) {
        printf("====== 문이 닫혔습니다. =====\n");
	exit(1);
    }
    else {
        printf("===== 유효한 명령이 아닙니다. =====\n");
	exit(1);
    }
}
