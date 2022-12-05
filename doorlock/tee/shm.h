#ifndef __SHM_H__
#define __SHM_H__

// shared memory header file

#define SHM_KEY 0x9000
#define SHM_SIZE 1024
#define SHM_MODE (SHM_R | SHM_W | IPC_CREAT)

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#endif