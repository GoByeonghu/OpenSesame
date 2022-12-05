// 도어락단 REE 프로세스
#ifndef __DOORLOCK_REE_H__
#define __DOORLOCK_REE_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include "doorlock_TEE.h"
#include "shm.h"

int tee_decrypt(char *filename, int flag);

#endif