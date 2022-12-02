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
