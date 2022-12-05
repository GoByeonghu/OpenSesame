#ifndef __SHA256_H__
#define __SHA256_H__

#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

int SHA256_Encode(char *target, char *result);

#endif