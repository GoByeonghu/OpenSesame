#include "sha256.h"

int SHA256_Encode(char *target, char *result) {
    unsigned char digest[SHA256_DIGEST_LENGTH];
    char string[sizeof(target)];
    strcpy(string, target);

    SHA256((unsigned char*)&string, strlen(string), (unsigned char*)&digest);   

    char mdString[SHA256_DIGEST_LENGTH*2+1];

    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
         sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
   
    strcpy(result,mdString);
    printf("SHA256 digest: %s\n", mdString);


    return 0;
}

/*
//for test
int main(){
    char *plaintext = "hello world";
    char result[SHA256_DIGEST_LENGTH*2+1];
    SHA256_Encode(plaintext, result);
    printf("SHA256 digest: %s\n", result);
    printf("\n\n\n");
    
	return 0;
}
*/

/*
int main()
{
    unsigned char digest[SHA256_DIGEST_LENGTH];
    char string[] = "hello world";

    SHA256((unsigned char*)&string, strlen(string), (unsigned char*)&digest);

    char mdString[SHA256_DIGEST_LENGTH*2+1];

    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
         sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

    printf("SHA256 digest: %s\n", mdString);

    return 0;
}
*/