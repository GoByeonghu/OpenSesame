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