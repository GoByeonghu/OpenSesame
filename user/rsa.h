#include  <stdio.h>
#include  <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/err.h>

typedef unsigned char uchar;
typedef unsigned int uint;

//func: Generate key pair(RSA public key, RSA private key) and store to file
//pre: None
//post: Create public.txt and private.txt, (both in pem format) 
void RSA_generateKeys();


//func: Get RSA public key
//pre: public.txt must exist.
//post: The public key is stored in the parameter
int RSA_GetPublicKey(char *read_buf);


//func: Set RSA public key
//pre: None
//post: The public key is stored in the parameter
int RSA_SetPublicKey(char *read_buf);


//func: RSA encryption
//pre: The target string must not be too long.
//post: src is encrypted and stored as dst with a length of length.
uchar*RSA_encrypt(uchar*src,uint len,uchar*dst,int*length);


//func: RSA decryption
//pre: dst must be a uchar of length 4096.
//post: src is decrypted and stored as dst with a length of length.
uchar*RSA_decrypt(uchar*src,int len, uchar*dst, int* length);