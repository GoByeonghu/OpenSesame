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
    
    /* PEM형식인 키 파일을 읽어와서 RSA 구조체 형식으로 변환 */
    
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

//generate key pair
void generateKeys(){
    EVP_PKEY*pkey=EVP_RSA_gen(1024);
    if(pkey==NULL){
        fprintf(stderr,"error: rsa gen\n");
        ERR_print_errors_fp(stderr);
        return;
    }
    FILE*fp=fopen("public.txt","wt");
    if(fp!=NULL){
        PEM_write_PUBKEY(fp,pkey);
        fclose(fp);
    }else{
        perror("file error");
    }
    fp=fopen("private.txt","wt");
    if(fp!=NULL){
        PEM_write_PrivateKey(fp,pkey,NULL,NULL,0,NULL,NULL);
        fclose(fp);
    }else{
        perror("file error");
    }
    EVP_PKEY_free(pkey);
}

uchar*encrypt(uchar*src,uint len,uchar*dst,int*length){
    FILE*fp=fopen("public.txt","r");
    if(fp==NULL){
        perror("file error");
        return NULL;
    }
    EVP_PKEY*pkey;
    pkey=PEM_read_PUBKEY(fp,NULL,NULL,NULL);
    fclose(fp);
    if(pkey==NULL){
        fprintf(stderr,"error: read publics key\n");
        return NULL;
    }
    EVP_PKEY_CTX*ctx=EVP_PKEY_CTX_new(pkey,NULL);
    EVP_PKEY_encrypt_init(ctx);
    //uchar*dst=(uchar*)malloc(2048);
    size_t outl;
    if(!EVP_PKEY_encrypt(ctx,dst,&outl,src,(size_t)len)){
        fprintf(stderr,"error: encrypt\n");
        EVP_PKEY_free(pkey);
        //free(dst);
        return NULL;
    }
    int len2=outl;
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    //BIO_dump_fp(stdout,dst,len2);
    //printf("len: %d, len2: %d\n",len,len2);
    if(length!=NULL){
    	*length=len2;
    }
    return dst;
}


uchar*decrypt(uchar*src,int len, uchar*dst, int* length){
    FILE*fp=fopen("private.txt","r");
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
    EVP_PKEY_decrypt_init(ctx);
    //uchar*dst=(uchar*)malloc(2048);
    size_t outl=2048;
    size_t inl=len;
    if(!EVP_PKEY_decrypt(ctx,dst,&outl,src,inl)){
        fprintf(stderr,"error: decrypt\n");
        free(dst);
        dst=NULL;
    }else{
	*length=outl;
	/*
        BIO_dump_fp(stdout,dst,(int)outl);
        //printf("len: %d, outl: %lld\n",len,outl);
	printf("len: %d, outl: %ld\n",len,outl);
        */
    }
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    return dst;
}


//for test
int main(){
 
 generateKeys();
 uchar *src= "hiimgbh";
 uchar*dst=(uchar*)malloc(2048);
 uint len=8;
 
 /*
 int *length=(int*)malloc(sizeof(int));
 encrypt(src,len,dst,length);
 BIO_dump_fp(stdout,dst,*length);
 printf("len: %d, len2: %d\n",len,*length);
  */
 
 //encryption
 int length;
 encrypt(src,len,dst,&length);
 BIO_dump_fp(stdout,dst,length);
 printf("len: %d, len2: %d\n",len,length);

// decryption
uchar*rst=(uchar*)malloc(2048);
int dlength; 
decrypt(dst,length,rst,&dlength);
 BIO_dump_fp(stdout,rst,dlength);
 //printf("len: %d, outl: %lld\n",len,outl);
 printf("len: %d, outl: %d\n",length,dlength);

 return 0;

}
>>>>>>> encryption
