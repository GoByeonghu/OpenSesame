// user단 호출
#ifndef __USER_REE_H__
#define __USER_REE_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "shm.h"
#include "user_TEE.h"

// 암호화 (flag 0: key 암호문, flag1: 개폐명령 암호문)
int tee_encrypt(char *filename, int flag);

#endif
