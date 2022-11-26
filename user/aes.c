/* AES 대칭키 생성*/
//#include "user.h"/

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

/// Code from OpenSSL Wiki at http://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
/// Needs libssl-dev (e.g. sudo apt-get install libssl-dev )
/// Compile with gcc [filename].c -o [outputfile] -lcrypto -ggdb

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
  printf("log(2)\n");
  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))  handleErrors();
 printf("log(3)\n");
  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))  handleErrors();
  plaintext_len = len;
printf("log(4)\n");
  /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */

  if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
  plaintext_len += len;
printf("log(5)\n");
  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}



int AES_CreateKey(){
   FILE *fp;

   /* A 256 bit key */
   /// this is still a 256-bit (32 byte) key
   //unsigned char *key = "ee12c03ceacdfb5d4c0e67c8f5ab3362";
  
   unsigned char key[KEY_LENGTH + 1];
   
   // generate 32-byte random string
   srand((unsigned int)time(NULL));
   for (int i = 0; i < KEY_LENGTH; i++) { // 10번 반복

	//key[i] = 33 +  rand()%93; // random character
	key[i] = 33 +  rand()%60; // random character
    }
    key[KEY_LENGTH]= '\0'; 
   // write it to file
   fp = fopen("SymmetricKey256.txt", "w");
   fputs(key,fp);
   fclose(fp);

   return 0;
}

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

int AES_UpdateKey(){
	return 0;
}

int AES_DelateKey(){
	return 0;
}

int AES_Encrypt(unsigned char *target, unsigned char *result){
 unsigned char key[KEY_LENGTH+1];
 AES_GetKey(key);
  /* A 256 bit IV */
  unsigned char *iv = "d36a4bf2e6dd9c68";

  /* Message to be encrypted */
  //unsigned char plaintext[sizeof(target)];
  //unsigned char plaintext[BUFSIZE];
  unsigned char plaintext[128];
  strcpy(plaintext,target);

  /* Buffer for ciphertext. Ensure the buffer is long enough for the
   * ciphertext which may be longer than the plaintext, dependant on the
   * algorithm and mode
   */
  
  //unsigned char ciphertext[sizeof(target+30)];
  //unsigned char ciphertext[BUFSIZ+EVP_MAX_BLOCK_LENGTH];
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
  printf("iv:%s\n",iv);
  //after
  EVP_cleanup();
  ERR_free_strings();
  strcpy(result,ciphertext);
  for(int i=ciphertext_len; i<128; i++){
          result[i]='\0';
  }

	return 0;
}

int AES_Decrypt(unsigned char *target, unsigned char *result){
  /* A 256 bit key */
  /// this is still a 256-bit (32 byte) key
  unsigned char key[KEY_LENGTH+1];
  AES_GetKey(key);
  /* A 256 bit IV */
  unsigned char *iv = "d36a4bf2e6dd9c68";
  
  //unsigned char ciphertext[sizeof(target)];
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
  printf("ciphertext:%ld\n",sizeof(ciphertext));
  printf("ciphertext:%s\n",ciphertext);
  printf("target:%ld\n",sizeof(target));
  printf("target:%s\n",target);
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

  printf("lenis : %d",len);
  decryptedtext_len = decrypt(ciphertext, len, key, iv, decryptedtext);
  
  /* Add a NULL terminator. We are expecting printable text */
  decryptedtext[decryptedtext_len] = '\0';
  /* Show the decrypted text */
  printf("Decrypted text is:\n");/* Clean up */
  EVP_cleanup();
  ERR_free_strings();
  printf("%s\n", decryptedtext);

	return 0;
}

//for test
int main(int arc, char *argv[])
{
  unsigned char temp_key[KEY_LENGTH +1];
  AES_CreateKey();
  printf("key:");
  AES_GetKey(temp_key);
  printf("%s", temp_key);
   
  ////////////////////////////////////////////
  /* Set up the key and vi*/

  /* A 256 bit key */
   /// this is still a 256-bit (32 byte) key
  //unsigned char *key = "ee12c03ceacdfb5d4c0e67c8f5ab3362";
  unsigned char key[KEY_LENGTH+1];
  AES_GetKey(key);
  /* A 128 bit IV */
  /// unsigned char *iv = "01234567890123456";
  /* A 256 bit IV */
  unsigned char *iv = "d36a4bf2e6dd9c68";

  /* Message to be encrypted */
  unsigned char *plaintext =
    "Go Byeonghu";
  
  unsigned char c[128];
  unsigned char d[128];
  AES_Encrypt(plaintext,c);
  printf("crt:%s\n",c);
  AES_Decrypt(c,d);
  printf("-------------------\n");
  /* Buffer for ciphertext. Ensure the buffer is long enough for the
   * ciphertext which may be longer than the plaintext, dependant on the
   * algorithm and mode
   */
  unsigned char ciphertext[128];

  /* Buffer for the decrypted text */
  unsigned char decryptedtext[128];

  int decryptedtext_len, ciphertext_len;

  /* Initialise the library */
  ERR_load_crypto_strings();
  OpenSSL_add_all_algorithms();
  //OPENSSL_config(NULL);  //it does not work and i don't know it's func

  /* Encrypt the plaintext */
  ciphertext_len = encrypt(plaintext, strlen(plaintext), key, iv, ciphertext);

  /* Do something useful with the ciphertext here */
  printf("Ciphertext is:\n");
  BIO_dump_fp(stdout, ciphertext, ciphertext_len);/////////////
  printf("2:%s\n", ciphertext);
  printf("sizeofC%ld\n",sizeof(ciphertext));
  printf("lenofC%d\n",ciphertext_len);
  /* Decrypt the ciphertext */
  decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, decryptedtext);

  /* Add a NULL terminator. We are expecting printable text */
  decryptedtext[decryptedtext_len] = '\0';

  /* Show the decrypted text */
  printf("Decrypted text is:\n");/* Clean up */
  printf("%s\n", decryptedtext);

  /* Clean up */
  EVP_cleanup();
  ERR_free_strings();
  printf("%d\n",BUFSIZ);
  printf("%d\n",EVP_MAX_BLOCK_LENGTH);
  return 0;
}


/*
#include <openssl/aes.h>
//#include <openssl/des.h>

typedef unsigned char   U8;

static const U8 cipher_key[]= {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5};

#define KEY_BIT 128
*/

/*
 * func:
 * pre:
 * post:
 */
/*
int aes_keyGenerate(){
	const unsigned char *userKey;
	const int bits;
	AES_KEY key;
	if(AES_set_encrypt_key(userKey, bits, &key)== -2){
		printf("error");
		return -1;
	}
	printf( "key:%s",userKey);
	//printf(key);
	return 1;
}

//for test
int main(){
	aes_keyGenerate();

	return 0;
}
*/

//ver.jsy
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
