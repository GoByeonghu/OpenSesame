#define SERC_TCP_PORT	7070
#define SERV_HOST_ADDR	"127.0.0.1"

#define MSG_OK		1
#define MSG_FAIL	0

// 플래그 1: 공개키
// 플래그 2: do_encrpyt() 결과 파일
// 플래그 3: 파일 전송

#define SEND_PUBLICKEY		1
#define SEND_ENCRYPTFILE	2
#define SEND_FILE			3

typedef struct {
	int		type;		// 수신 상태 메세지 (1 ok, 0 fail)
	char	data[128];	// 도어락 고유 번호
}	MsgType;

typedef struct {
	char symkey[1024];
	char encrypt[1024];
} Encrypt;
