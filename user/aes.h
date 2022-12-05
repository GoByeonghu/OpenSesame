//#include "user.h"

//in user.h
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <stdbool.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#define KEY_LENGTH 32

void handleErrors(void);

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
  unsigned char *iv, unsigned char *ciphertext);


int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
  unsigned char *iv, unsigned char *plaintext);

//func: Create 32bytes Symmetric Key and store to SymmetricKey256.txt
//pre: None
//post: Symmetric Key store to SymmetricKey256.txt
int AES_CreateKey();


//func: Get 32bytes Symmetric Key from SymmetricKey256.txt
//pre: Parameter key is a 32-byte string
//post: Symmetric Key is stored in key
int AES_GetKey(unsigned char *key);


//func: Set 32bytes Symmetric Key to SymmetricKey256.txt
//pre: Parameter key is a 32-byte string
//post: Key is stored in SymmetricKey256copy.txt
int AES_SetKey(unsigned char *key);


//func: AES Encrypt target and store to result
//pre: Target string must end with /0.
//post: The encrypted text is stored in result.
int AES_Encrypt(unsigned char *target, unsigned char *result);


//func: AES Decrypt target and store to result
//pre: Except for the actual value of the target string, it must be '\0'
//and The size of the result buffer must be large enough.(128)
//post: plain test store to result
int AES_Decrypt(unsigned char *target, unsigned char *result);
