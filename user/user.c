/* 도어락 단 */

#include "user.h"
#define MAX_IDPW_LENGTH 100

static void getUsrInfo(char *, char *);                

// [Name]: main()
// [Function]: entry point
// [input]: argc(int), argv(char **)
// [output]: nothing
int main(int argc, char *argv[])
{
    // todo: 임시 데이터 저장 용도 배열
    char ID[100];
    char PW[100];

    // user info 입력 받음
    getUsrInfo(ID, PW);
    // 입력 test
    //printf("%s, %s\n", ID, PW);

    int keylen = 4096;
    if (makeRSAkey(keylen) == 0) {
	    printf("키 생성을 성공하였습니다.\n");
    }
    else {
	    printf("키 생성을 실패하였습니다. 프로그램을 종료합니다.\n");
	    exit(1);
    }
    return 0;
}


// [Name]: getUsrInfo()
// [Function]: 유저 정보를 입력 받음
// [input]: ID(char *), PW(char *)
// [output]: nothing
static void getUsrInfo(char *ID, char *PW)
{
    // todo: if 도어락 등록 모드인 경우
    printf("===== 도어락 등록을 위한 사용자 ID와 PW를 입력해주세요 =====\n");
    printf("ID: ");
    if (!fgets(ID, MAX_IDPW_LENGTH, stdin))
    {
        perror("\n아이디 입력이 잘못되었습니다.\n");
        exit(0);
    }
    printf("PW: ");
    if (!fgets(PW, MAX_IDPW_LENGTH, stdin))
    {
        perror("\n패스워드 입력이 잘못되었습니다.\n");
        exit(0);
    }

    printf("\n===== 입력에 성공하였습니다. =====\n");

    // 입력에 성공했다면 개행문자 제거
    ID[strlen(ID) - 1] = '\0';
    PW[strlen(PW) - 1] = '\0';
}
