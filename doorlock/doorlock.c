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
    int flag = 1;
    openSesame(flag);
}

// [Name]: openSesame
// [Function]: 문 개폐 제어
// [input]: int flag (1: 열림, 0: 닫힘)
// [output]: nothing
void openSesame(int flag) {
    if (flag == 1) {
        printf("===== 문이 열렸습니다. =====");
    }
    else if (flag == 0) {
        printf("====== 문이 닫혔습니다. =====");
    }
}
