// doorlock header file

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "./socket/tcp.h"
#include "sha256.h"
#include "./tee/doorlock_TEE.h"

// [Name]: openSesame
// [Function]: 문 개폐 제어
// [input]: nothing
// [output]: nothing
void openSesame();