#define SERC_TCP_PORT	7070
#define SERV_HOST_ADDR	"127.0.0.1"

#define MSG_REQUEST	1
#define MSG_REPLY	2

typedef struct {
	int		type;
	char	data[128];
}	MSGTYPE;
