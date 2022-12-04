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
void RSA_generateKeys(){
    EVP_PKEY*pkey=EVP_RSA_gen(1024);
    if(pkey==NULL){
        fprintf(stderr,"error: rsa gen\n");
        ERR_print_errors_fp(stderr);
        return;
    }
    FILE*fp=fopen("PublicKey.pem","wt");
    if(fp!=NULL){
        PEM_write_PUBKEY(fp,pkey);
        fclose(fp);
    }else{
        perror("file error");
    }
    fp=fopen("PrivateKey.pem","wt");
    if(fp!=NULL){
        PEM_write_PrivateKey(fp,pkey,NULL,NULL,0,NULL,NULL);
        fclose(fp);
    }else{
        perror("file error");
    }
    EVP_PKEY_free(pkey);
}

//func: Get RSA public key
//pre: public.txt must exist.
//post: The public key is stored in the parameter
int RSA_GetPublicKey(char *read_buf){
    FILE* fp = NULL;
    fp = fopen("PublicKey.pem", "r");
    if (fp == NULL)
    {
        printf("open error1\n");
        return 0;
    }
    fread(read_buf, sizeof(char), 4096, fp);
 
    printf("[실행결과]\n%s\n", read_buf);
    fclose(fp);
    return 1;
}

//func: Set RSA public key
//pre: None
//post: The public key is stored in the parameter
int RSA_SetPublicKey(char *read_buf){
    FILE* fp = NULL;
    fp = fopen("PublicKey.pem", "w");
    if (fp == NULL)
    {
        printf("open error2\n");
        return 0;
    }
    fwrite(read_buf, sizeof(char), strlen(read_buf),fp);
    fclose(fp);
    return 1;
}

//func: RSA encryption
//pre: The target string must not be too long.
//post: src is encrypted and stored as dst with a length of length.
uchar*RSA_encrypt(uchar*src,uint len,uchar*dst,int*length){
    FILE*fp=fopen("PrivateKey.pem","r");
    if(fp==NULL){
        perror("file error");
        return NULL;
    }
    EVP_PKEY*pkey=PEM_read_PrivateKey(fp,NULL,NULL,NULL);
    fclose(fp);
    if(pkey==NULL){
        fprintf(stderr,"error: read private key\n");
        return NULL;
    }
    EVP_PKEY_CTX*ctx=EVP_PKEY_CTX_new(pkey,NULL);
    EVP_PKEY_encrypt_init(ctx);
    size_t outl;
    if(!EVP_PKEY_encrypt(ctx,dst,&outl,src,(size_t)len)){
        fprintf(stderr,"error: encrypt\n");
        EVP_PKEY_free(pkey);
        return NULL;
    }
    int len2=outl;
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    if(length!=NULL){
    	*length=len2;
    }
    return dst;
}

//func: RSA decryption
//pre: dst must be a uchar of length 4096.
//post: src is decrypted and stored as dst with a length of length.
uchar*RSA_decrypt(uchar*src,int len, uchar*dst, int* length){
    FILE*fp=fopen("PublicKey.pem","r");
    if(fp==NULL){
        perror("file error");
        return NULL;
    }
    EVP_PKEY*pkey;
    pkey=PEM_read_PUBKEY(fp,NULL,NULL,NULL);
    fclose(fp);

    if(pkey==NULL){
        fprintf(stderr,"error: read public key\n");
        return NULL;
    }
    EVP_PKEY_CTX*ctx=EVP_PKEY_CTX_new(pkey,NULL);
    EVP_PKEY_decrypt_init(ctx);
    size_t outl=2048;
    size_t inl=len;
    if(!EVP_PKEY_decrypt(ctx,dst,&outl,src,inl)){
        fprintf(stderr,"error: decrypt\n");
        free(dst);
        dst=NULL;
    }else{
	*length=outl;
    }
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    return dst;
}


//for test
//This provides an example of using the function.
//In fact, when adding this file, it is necessary to comment out or delete it.
int main(){
 
 //initialization 
 RSA_generateKeys();
 uchar *src= "hiimgbh";
 uchar*dst=(uchar*)malloc(2048);
 uint len=8;
  
 //encryption
 int length;
 RSA_encrypt(src,len,dst,&length);
 BIO_dump_fp(stdout,dst,length);
 printf("len: %d, len2: %d\n",len,length);

 // decryption
 uchar*rst=(uchar*)malloc(2048);
 int dlength; 
 RSA_decrypt(dst,length,rst,&dlength);
 BIO_dump_fp(stdout,rst,dlength);
 printf("len: %d, outl: %d\n",length,dlength);

 //Key Get&Set
 int BUF_SIZE = 4096;
 char*read_buf=(char*)malloc(BUF_SIZE);
 RSA_GetPublicKey(read_buf);
 RSA_SetPublicKey(read_buf);

 //deallocate
 free(dst);

 return 0;

}

//This is the source code for the old version of openssl (not applicable after openssl 3.0).
//Will be deleted in the future.
/*
#include "user.h"

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/rand.h>

BIO *publickey_bio = NULL;  
BIO *privatekey_bio = NULL; 
BIO *stdout_bio = NULL;   

int padding = RSA_PKCS1_PADDING;

void closefiles();

int openfiles(int isPEMFromat)
{
 stdout_bio = BIO_new(BIO_s_file());
 if(stdout_bio)
 BIO_set_fp(stdout_bio, stdout, BIO_NOCLOSE | BIO_FP_TEXT);
 
 publickey_bio = BIO_new(BIO_s_file());
 privatekey_bio = BIO_new(BIO_s_file());

 if(publickey_bio == NULL || privatekey_bio == NULL)
 {
 closefiles();
 return -1;
 }

 if(isPEMFromat)
 {
 if(BIO_write_filename(publickey_bio, "./PublicKey.pem") <= 0)
 {
 closefiles();
 return -1;
 }

 if(BIO_write_filename(privatekey_bio, "./PrivateKey.pem") <= 0)
 {
 closefiles();
 return -1;
 }
 }
 else
 {
 if(BIO_write_filename(publickey_bio, "./PublicKey.dem") <= 0)
 {
 closefiles();
 return -1;
 }

 if(BIO_write_filename(privatekey_bio, "./PrivateKey.dem") <= 0)
 {
 closefiles();
 return -1;
 }
 }

 return 0;     
}

int writefiles(RSA *rsa,int isPEMFromat)
{
 if(isPEMFromat) 
 {
 if(!PEM_write_bio_RSA_PUBKEY(stdout_bio, rsa)) 
 {
 return -1;
 }   
 
 if(!PEM_write_bio_RSA_PUBKEY(publickey_bio, rsa)) 
 {
 return -1;
 }
 
 if(!PEM_write_bio_RSAPrivateKey(stdout_bio, rsa, NULL, NULL, 0, NULL, NULL))
 {
 return -1;
 }

 if(!PEM_write_bio_RSAPrivateKey(privatekey_bio, rsa, NULL, NULL, 0, NULL, NULL))
 {
 return -1;
 }
 }
 else
 {  
 if(!i2d_RSA_PUBKEY_bio(stdout_bio, rsa)) 
 {
 return -1;
 }

 if(!i2d_RSA_PUBKEY_bio(publickey_bio, rsa))
 {
 return -1;
 }
 
 if(!i2d_RSAPrivateKey_bio(stdout_bio, rsa))
 {
 return -1;
 }

 if(!i2d_RSAPrivateKey_bio(privatekey_bio, rsa))
 {
 return -1;
 }
 }
}

void closefiles()
{
 if(publickey_bio) BIO_free_all(publickey_bio);
 if(privatekey_bio) BIO_free_all(privatekey_bio);
 if(stdout_bio) BIO_free_all(stdout_bio);
}


RSA *gen_key(int key_len)
{
 RAND_status();
 return RSA_generate_key(key_len, RSA_F4, NULL, NULL);
}

int makeRSAkey(int keyLen) {
    int i = 0;
    RSA *rsa = NULL;
    int isPEMFromat = 1;
    
    printf("===== RSA 키 생성을 시작합니다. =====\n");
    rsa = gen_key(keyLen);
    if(rsa) {
	    if(openfiles(isPEMFromat) == 0) {
		    writefiles(rsa, isPEMFromat);
		    closefiles();
	    }
	    RSA_free(rsa);
	    return 1; // success
    }
    else {
	    return 0; // failed
    
    }
}
 
RSA * createRSA(unsigned char * key,int public)
{
    RSA *rsa= NULL;
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, -1); // 읽기 전용 메모리 만들기 BIO
    if (keybio==NULL)
    {
        printf( "Failed to create key BIO");
        return 0;
    }
    
    // PEM형식인 키 파일을 읽어와서 RSA 구조체 형식으로 변환
    
    if(public) // PEM public 키로 RSA 생성
    {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
    }else // PEM private 키로 RSA 생성
    {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
    }
    
    if(rsa == NULL)
    {
        printf( "Failed to create RSA");
    }
 
    return rsa;
}

// 공개키로 암호화
int public_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted) 
{
    RSA * rsa = createRSA(key,1);
    int result = RSA_public_encrypt(data_len,data,encrypted,rsa,padding);
    return result; // RSA_public_encrypt() returns the size of the encrypted data 
}

// 복호화 암호화
int private_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted)
{
    RSA * rsa = createRSA(key,0);
    int result = RSA_private_encrypt(data_len,data,encrypted,rsa,padding);
    return result;
}
*/


