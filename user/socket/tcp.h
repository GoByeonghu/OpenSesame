#define SERC_TCP_PORT	7070
#define SERV_HOST_ADDR	"127.0.0.1"

#define MSG_REQUEST	1
#define MSG_REPLY	2

typedef struct {
	int		type;		// 수신 상태 메세지 (1 ok, 0 fail)
	char	data[128];	// 도어락 고유 번호
}	MsgType;

typedef struct {
	char symkey[1024];
	char encrypt[1024];
} Encrypt;
