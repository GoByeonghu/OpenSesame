// user단 메인 프로세스

#include "user.h"
#include ".\socket\tcp.h"
#include ".\tee\user_REE.c"
#include ".\tee\user_TEE.c"

#define MAX_IDPW_LENGTH 100

static void getUsrInfo(char *, char *);                

// [Name]: main()
// [Function]: entry point
// [input]: argc(int), argv(char **)
// [output]: nothing
int main(int argc, char *argv[])
{
	int	status;

	/* 1. 회원 가입 */
	status = user_register();
	if (!status) {
		printf("프로그램을 종료합니다.");
		exit(0);
	}
	
	/* 2. 도어락 등록 */
	// 공개키 전송(flag: 1, filename)
	sendToDoorlock(1, "PublicKey.pem");

	// 암호문 생성
	tee_encrypt("encrpyted_string", 0);

	// 암호문 전송
	sleep(3000);
	sendToDoorlock(2, "encrpyted_string");
	sleep(3000);
	sendToDoorlock(3, "ID");

	/* 3. 개폐 명령 */
	// 사용자 로그인하고 개폐 명령 입력한 뒤 암호화 진행까지
	if (!tee_control()) {
		printf("프로그램을 종료합니다.");
		exit(0);
	}
	sleep(3000);

	// 개폐명령 암호화 파일 전송
	sendToDoorlock(2, "opencommand");
	sleep(3000);

	return 0;
}

// [Name]: user_register()
// [Function]: 사용자 회원가입 및 RSA 비대칭 키 생성
// [input]: nothing
// [output]: 상태 값(int)
int user_register()
{
    char ID[100];
    char PW[100];

    // 도어락 등록 모드인 경우
    printf("===== 도어락 등록을 위한 사용자 ID와 PW를 입력해주세요 =====\n");

    // user info 입력 받음
    getUsrInfo(ID, PW);
    // 입력 test
    //printf("%s, %s\n", ID, PW);

	// 사용자 정보 저장
	tee_store("ID", ID);
	tee_store("PW", PW);

	// RSA 비대칭키 생성 및 저장
    int keylen = 4096;
    if (RSA_generateKeys() == 0) {
	    printf("키 생성을 성공하였습니다.\n");
    }
    else {
	    printf("키 생성을 실패하였습니다. 프로그램을 종료합니다.\n");
	    return 0;
    }
    return 1;
}


// [Name]: getUsrInfo()
// [Function]: 유저 정보를 입력 받음
// [input]: ID(char *), PW(char *)
// [output]: nothing
static void getUsrInfo(char *ID, char *PW)
{
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

// [Name]: tee_control
// [Function]: 도어락 열지 말지 사용자 명령어를 받음
// [input]: nothing
// [output]: 상태 값(int)
int tee_control() {
	char choice[2];
	char ID[MAX_IDPW_LENGTH];
	char PW[MAX_IDPW_LENGTH];
	
	while(1)
	{
    	printf("\n===== 도어락을 여시겠습니까? [Y/N] ===== ");
		if (!fgets(choice, 1, stdin))
		{
			perror("명령어 입력이 잘못되었습니다.\n");
			return 0; // fail
		}

		getUsrInfo(ID, PW);

		// todo: 유저 인증
		// 그냥 ok라고 하기

		// 개폐 명령 암호화(flag: 1)
		if (choice[0] == 'Y' || choice[0] == 'y') {
			tee_encrypt("open", 1);
		}
		else if (choice[0] == 'n' || choice[0] == 'N') {
			tee_encrypt("close", 1);
		}
		else {
			printf("잘못된 명령입니다.\n");
		}
	}

	// success
    return 1;	
}
