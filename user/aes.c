/* AES 대칭키 생성*/

#include "user.h"
#include <openssl/aes.h>
#include <openssl/des.h>

typedef unsigned char   U8;

static const U8 cipher_key[]= {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5};

#define KEY_BIT 128

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
