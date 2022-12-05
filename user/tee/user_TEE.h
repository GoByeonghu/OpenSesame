// tee단 호출 process
#ifndef __USER_TEE_H__
#define __USER_TEE_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include "shm.h"

#include "./../aes.h"
#include "./../rsa.h"
#include "./../sha256.h"

// todo: encrpyt()
char *encrpyt(int flag, char *filename);

void do_encrypt(char *filename, int flag);

// 파일로 저장
void tee_store(char *filename, char *data);

// 파일 읽기
unsigned char *tee_read(char *filename);

#endif
