#ifndef __DOORLOCK_TEE_H__
#define __DOORLOCK_TEE_H__

// 도어락단 TEE 프로세스
#include "shm.h"
#include "./../aes.h"
#include "./../rsa.h"
#include "./../sha256.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

//func: String to int
int my_atoi(const char* str);

// todo: 무결섬 검증, 복호화, 파일로 결과 저장 (tee_store 호출)
int decrpyt(unsigned char *buf, int flag, char *filename);

void do_decrypt(char *filename, int flag, char *decryptedfile);

// 파일로 저장
void tee_store(char *filename, unsigned char *data);

// 파일 읽기
unsigned char *tee_read(char *filename);

#endif