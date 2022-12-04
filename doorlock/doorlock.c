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
    recvFromUser("opencommand", 1);

    // 명목상 문 열기
    openSesame();

}

// [Name]: openSesame
// [Function]: 문 개폐 제어
// [input]: nothing
// [output]: nothing
void openSesame() {
    unsigned char   *buf;
    unsigned char   *command;
    unsigned char   *hashfile;

    hashfile = tee_read("opencommand_privatekey");
    buf = tee_read("opencommand_symkey");
    command = buf;
    SHA256_Encode(buf, buf);

    if (strcmp(buf, hashfile) == 0) {
        printf("인증되었습니다.\n");
    }

    else {
        printf("인증이 실패하였습니다.\n");
        return;
    }
    
    if (strcmp(command, "open") == 0) {
        printf("===== 문이 열렸습니다. =====\n");
    }
    else if (strcmp(command, "close") == 0) {
        printf("====== 문이 닫혔습니다. =====\n");
    }
    else {
        printf("===== 유효한 명령이 아닙니다. =====\n");
    }
}
