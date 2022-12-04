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

void handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  abort();
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
  unsigned char *iv, unsigned char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;
  int len;
  int ciphertext_len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the encryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))  handleErrors();

  /* Provide the message to be encrypted, and obtain the encrypted output.
   * EVP_EncryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) handleErrors();
  ciphertext_len = len;

  /* Finalise the encryption. Further ciphertext bytes may be written at
   * this stage.
   */
  if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
  ciphertext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
  unsigned char *iv, unsigned char *plaintext)
{
  EVP_CIPHER_CTX *ctx;
  int len;
  int plaintext_len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))  handleErrors();
  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))  handleErrors();
  plaintext_len = len;
  /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
  if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
  plaintext_len += len;
  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}

//func: Create 32bytes Symmetric Key and store to SymmetricKey256.txt
//pre: None
//post: Symmetric Key store to SymmetricKey256.txt
int AES_CreateKey(){
   FILE *fp;

   /* A 256 bit key */
   /// this is still a 256-bit (32 byte) key
   unsigned char key[KEY_LENGTH + 1];
   
   // generate 32-byte random string
   srand((unsigned int)time(NULL));
   for (int i = 0; i < KEY_LENGTH; i++) {
	key[i] = 33 +  rand()%60; // random character
    }
    key[KEY_LENGTH]= '\0'; 
   // write it to file
   fp = fopen("SymmetricKey256.txt", "w");
   fputs(key,fp);
   fclose(fp);

   return 0;
}

//func: Get 32bytes Symmetric Key from SymmetricKey256.txt
//pre: Parameter key is a 32-byte string
//post: Symmetric Key is stored in key
int AES_GetKey(unsigned char *key){
   FILE *fp;
   unsigned char buffer[KEY_LENGTH + 1];
   fp = fopen("SymmetricKey256.txt", "r");
   /* A 256 bit key */
   /// this is still a 256-bit (32 byte) key
   fgets(buffer, sizeof(buffer), fp);
   fclose(fp);
   for(int i=0; i<KEY_LENGTH; i++){
	   key[i]=buffer[i];
   }
   key[KEY_LENGTH] = '\0';
   return 0;
}

//func: Set 32bytes Symmetric Key to SymmetricKey256.txt
//pre: Parameter key is a 32-byte string
//post: Key is stored in SymmetricKey256copy.txt
int AES_SetKey(unsigned char *key){
   FILE *fp;
   unsigned char buffer[KEY_LENGTH + 1];
   for(int i=0; i<KEY_LENGTH; i++){
           buffer[i]=key[i];
   }
   buffer[KEY_LENGTH] = '\0';
   fp = fopen("SymmetricKey256.txt", "w");
   /* A 256 bit key */    
   /// this is still a 256-bit (32 byte) key
   fputs(buffer, fp);
   fclose(fp);
   return 0;
}

//func: AES Encrypt target and store to result
//pre: Target string must end with /0.
//post: The encrypted text is stored in result.
int AES_Encrypt(unsigned char *target, unsigned char *result){
 unsigned char key[KEY_LENGTH+1];
 AES_GetKey(key);
  /* A 256 bit IV */
  unsigned char *iv = "d36a4bf2e6dd9c68";

  /* Message to be encrypted */
  unsigned char plaintext[128];
  strcpy(plaintext,target);

  /* Buffer for ciphertext. Ensure the buffer is long enough for the
   * ciphertext which may be longer than the plaintext, dependant on the
   * algorithm and mode
   */
  unsigned char ciphertext[128];
  int ciphertext_len;

  /* Initialise the library */
  ERR_load_crypto_strings();
  OpenSSL_add_all_algorithms();
 /* Encrypt the plaintext */
  ciphertext_len = encrypt(plaintext, strlen(plaintext), key, iv, ciphertext);
  printf("\nCL:%d\n",ciphertext_len);
  /* Do something useful with the ciphertext here */
  printf("Ciphertext is:\n");
  BIO_dump_fp(stdout, ciphertext, ciphertext_len);
  printf("%s\n",ciphertext);
  //after
  EVP_cleanup();
  ERR_free_strings();
  //strcpy(result,ciphertext);
  for(int i=0; i<128; i++){
	  result[i]=ciphertext[i];
  }
  for(int i=ciphertext_len; i<128; i++){
          result[i]='\0';
  }

  return 0;
}

//func: AES Decrypt target and store to result
//pre: Except for the actual value of the target string, it must be '\0'
//and The size of the result buffer must be large enough.(128)
//post: plain test store to result
int AES_Decrypt(unsigned char *target, unsigned char *result){
  /* A 256 bit key */
  /// this is still a 256-bit (32 byte) key
  unsigned char key[KEY_LENGTH+1];
  AES_GetKey(key);
  /* A 256 bit IV */
  unsigned char *iv = "d36a4bf2e6dd9c68";
  unsigned char ciphertext[128]; //BUFSIZ
  //strcpy(ciphertext,target);
  for(int i =0; i<128; i++){
	  ciphertext[i] = target[i];
  }
  /* Buffer for the decrypted text */
  //unsigned char decryptedtext[sizeof(target)+30];
  unsigned char decryptedtext[128]; //BUFSIZ+EVP_MAX_BLOCK_LENGTH
  int decryptedtext_len;

  /* Initialise the library */
  ERR_load_crypto_strings();
  OpenSSL_add_all_algorithms();
  //OPENSSL_config(NULL);  //it does not work and i don't know it's func

  /* Decrypt the ciphertext */ // length-1 ?
  //decryptedtext_len = decrypt(ciphertext, sizeof(target), key, iv, decryptedtext);
  int len=0;
  bool temp[128];
  for(int i=0; i<128; i++){
	if(ciphertext[i]!='\0' &&ciphertext[i]!='\n'){
		
		printf("|%x|",ciphertext[i]);
		temp[i] = false; 
		len++;
	}
	else{
		temp[i] =true;
	}
  }
  bool flag =false;
  for(int i=128-1; i>=0; i--){
	  if(temp[i]==false){
		  flag = true;
	  }
	  if(temp[i]==true && flag ==true){
		len++;
	  }
  }

  decryptedtext_len = decrypt(ciphertext, len, key, iv, decryptedtext);
  
  /* Add a NULL terminator. We are expecting printable text */
  decryptedtext[decryptedtext_len] = '\0';
  /* Show the decrypted text */
  printf("Decrypted text is:\n");
  
  /* Clean up */
  EVP_cleanup();
  ERR_free_strings();
  printf("%s\n", decryptedtext);

	return 0;
}

//for test
//This provides an example of using the function.
//In fact, when adding this file, it is necessary to comment out or delete it.
int main(int arc, char *argv[])
{
  //Get & Set key
  unsigned char temp_key[KEY_LENGTH +1];
  AES_CreateKey();
  printf("key:");
  AES_GetKey(temp_key);
  printf("%s", temp_key);
  AES_SetKey(temp_key);
   
  //Initialization
   /* A 256 bit key */
   /// this is still a 256-bit (32 byte) key
  unsigned char key[KEY_LENGTH+1];
  AES_GetKey(key);
   //Message to be encrypted
  unsigned char *plaintext =
    "hellow hi bye slow fast A++12345678901234567890";
  
  unsigned char c[128];
  unsigned char d[128];
  
  //Encryption
  AES_Encrypt(plaintext,c);
  printf("crt:%s\n",c);
  
  //Decryption
  AES_Decrypt(c,d);
  printf("-------------------\n");
  
  // Clean up
  EVP_cleanup();
  ERR_free_strings();
  
  return 0;
}


//This is the source code for the old version of openssl (not applicable after openssl 3.0).
//Will be deleted in the future.

/*
int aes_encrypt( U8 *p_in, U8 *p_out, int size)
{
    AES_KEY aes_key;
    U8 iv_aes[AES_BLOCK_SIZE];

    bzero(iv_aes, sizeof(iv_aes));
    AES_set_encrypt_key( cipher_key, KEY_BIT, &aes_key);
    AES_cbc_encrypt( p_in, p_out, size, &aes_key , iv_aes, AES_ENCRYPT);

    return 0;
}

int aes_decrypt( U8 *p_in, U8 *p_out, int size)
{
    AES_KEY aes_key;
    U8 iv_aes[AES_BLOCK_SIZE];

    bzero(iv_aes, sizeof(iv_aes));
    AES_set_decrypt_key( cipher_key, KEY_BIT, &aes_key);
    AES_cbc_encrypt( p_in, p_out, size, &aes_key , iv_aes, AES_DECRYPT);
    return 0;
}
int main(int argc, char *args[])
{
    U8      p_sour[] = {'h', 'e', 'l', 'l', 'o'};
    U8      p_encrypt[1024];
    U8      p_decrypt[1024];
    U8      p_temp[1024];
    int     encrypt_size;

    aes_encrypt( p_sour   , p_encrypt, sizeof( p_sour));
    encrypt_size    = ( sizeof( p_sour) + AES_BLOCK_SIZE) /16 * 16;
    memcpy( p_temp, p_encrypt, encrypt_size);      // 오류 수정
    aes_decrypt( p_temp, p_decrypt, encrypt_size);   // 오류 수정

    printf( "encrypt aes: ");
    for (int ndx = 0; ndx < sizeof( p_sour); ndx++){
        printf( "%c ", p_encrypt[ndx]);
    }
    printf( "\n");

    encrypt_size    = ( sizeof( p_sour) + AES_BLOCK_SIZE) /16 * 16;
    memcpy( p_temp, p_encrypt, encrypt_size);      // 오류 수정
    aes_decrypt( p_temp, p_decrypt, encrypt_size);   // 오류 수정

    printf( "decrypt aes: ");
    for (int ndx = 0; ndx < sizeof( p_sour); ndx++){
        printf("%c", p_decrypt[ndx]);
    }
    printf( "\n");

    return 0;
}
*/
