//The aim of this program is to 
#include <sodium.h>
#include <iostream>



#define MESSAGE ((const unsigned char *) "test")
#define MESSAGE_LEN 256
#define CIPHERTEXT_LEN (crypto_secretbox_MACBYTES + MESSAGE_LEN)

using namespace std;

/*void encryption(unsigned char& key, unsigned char& nonce, unsigned char& ciphertext)
{
	
}*/

int main()
{
	unsigned char key[crypto_secretbox_KEYBYTES];
	unsigned char nonce[crypto_secretbox_NONCEBYTES];	//nonce is the key used to encrypt and verify the messages
	unsigned char ciphertext[CIPHERTEXT_LEN];
	if (sodium_init() < 0) {
		std::cout<<"Library couldn't be initialized"<<std::endl;
    }
    crypto_secretbox_keygen(key);				//generate a random key
	randombytes_buf(nonce, sizeof nonce);			//random number generated
	crypto_secretbox_easy(ciphertext, MESSAGE, MESSAGE_LEN, nonce, key);

	unsigned char decrypted[MESSAGE_LEN];
	if (crypto_secretbox_open_easy(decrypted, ciphertext, CIPHERTEXT_LEN, nonce, key) != 0) {
    	std::cout<<"Message has been forged"<<std::endl;
	}
	//sodium.encryption(key, nonce,  ciphertext);//perform authenticated encryption
	std::cout<<"plaintext: test "<<std::endl;
	std::cout<<"key: "<<key<<std::endl;
	std::cout<<"ciphertext: "<<ciphertext<<std::endl;
	
	return 0;
}
