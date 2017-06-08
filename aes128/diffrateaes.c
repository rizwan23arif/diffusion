#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include<string.h>
#include <dirent.h>
#include <time.h>
#include <math.h>



// Enable both ECB and CBC mode. Note this can be done before including aes.h or at compile-time.
// E.g. with GCC by using the -D flag: gcc -c aes.c -DCBC=0 -DECB=1
#define CBC 1
#define ECB 1

#include "aes.h"

/*function to generate a random string*/
void gen_random(char *s, const int len) {

	 static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

	 //printf("\n char by char: \n");
    for (int i = 0; i < len; i++) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
		  //printf("%c",s[i]);
    }

    s[len] = '\0';
	 //printf("\n random string inside gen function = %s", s);
}

/*A function which converts a decimal number to its binary representation.
The binary representation is stored in the array binary[], with the lowest order bit at binary[0].*/
int Decimal2Binary(uint8_t dec, bool binary[])
{
			int i=0;
			uint8_t quotient = dec;
			uint8_t remainder;
			
			//printf("\n dec = %u", dec);
			
			while(quotient!=0)
			{
					remainder = quotient % 2;
					quotient = quotient/2;
					binary[i] = remainder;
					//printf("\n binary[%d] = %d", i, binary[i]); fflush(stdout);
			  i++;
			}
			
			fflush(stdout);
			return 0;
}

int InitializeBinaryArray(bool binary[], int length)
{
	for (int i=0; i<length; i++)
	{binary[i] = 0;}
	return 0;
}

int diffrateAES(int l)
{
			int length=l; 												//Length is given in terms of number of characters.The number of bits is length*8.
			int no_samples = 10000;
			int bit_length = l*8;
			int max = 2^8; 											//Its actually 2^8-1 but when working with % we have to add 1
			float flip_total = 0;
			time_t t;
			srand((unsigned) time(&t));
	
			for(int p=0; p<no_samples; p++)
			{
						uint8_t key[16];
						uint8_t input[16];
						
						for(int i=0;i<16;i++)
						{
								key[i] = rand() % max;
								input[i] = rand() % max;
						}
						
						uint8_t output[16]; 			 	//For output
						bool bin_output[16][8];
						
						AES128_ECB_encrypt(input, key, output);
						
						for(int i=0;i<16;i++)
						{
								InitializeBinaryArray(bin_output[i],8);
								Decimal2Binary(output[i], bin_output[i]);
						}
						
						
						for(int i=0;i<16;i++)
						{
								for(int j=0;j<8;j++)
								{
										uint8_t in_temp[16];
										uint8_t out_temp[16];
										
										for(int k=0;k<16;k++)
										{in_temp[k] = input[k];}
										
										uint8_t r = 1<<j;
										
										in_temp[i] = input[i] ^ r;
										
										AES128_ECB_encrypt(in_temp, key, out_temp);
										
										for(int k=0;k<16;k++)
										{
											 uint8_t c = out_temp[k];
												bool bin_c[8];
												InitializeBinaryArray(bin_c,8);
												Decimal2Binary(c, bin_c);
												for(int h=0;h<8;h++)
												{
														if(bin_c[h] != bin_output[k][h])
														{flip_total++;}
												}
										}
										
								}
						}
						
			}

			float average = (flip_total/(128*bit_length*no_samples));
			
			printf("\n Average diffusion for 128 bit inputs is %0.2f", average);
			
			return 0;
}

int main(void)
{
    int length = 16; 				//working with 128 bit inputs, i.e., 16 characters. 
				diffrateAES(length);
    return 0;
}