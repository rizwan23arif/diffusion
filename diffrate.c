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
#include"sha256.h"
#include "sha1.h"
#include "md5.h"

/*Program to measure diffusion rates of various operations*/

/*Function to initialize binary representation arrays*/
int InitializeBinaryArray(bool binary[], int length)
{
	for (int i=0; i<length; i++)
	{binary[i] = 0;}
	return 0;
}

/*A function which converts a decimal number to its binary representation.
The binary representation is stored in the array binary[], with the lowest order bit at binary[0].*/
int Decimal2Binary(unsigned int dec, bool binary[])
{
			int i=0;
			unsigned int quotient = dec;
			unsigned int remainder;
			
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

/*void gen_random_string(){
	
	unsigned char ascii[256];
	
	for(int i=0; i<256; i++)
	{
		ascii[i] = i;
		printf("\n %d-%u",i,ascii[i]);
	}

}*/

void printSHA256Hash(unsigned char* result)
{
	for(int i=0;i<32;i++)
	{
		printf ("%02x", result[i]);
	}
}



int diffRateSHA256(int l)
{
	
	unsigned char sha256_result[32];
	unsigned char sha256_temp_result[32];
	SHA256_CTX sha256;
	SHA256_CTX sha256_temp;
	
   int i;
	int length=l; 			//Length is given in terms of number of characters.The number of bits in the input is length*8.
	int bit_length = l*8;
	float cum_avg = 0;
	
	time_t t;
	srand((unsigned) time(&t));
	
	for(int p=0; p<1000; p++)
	{
		char s[length+1];
		gen_random(s,length);
		//printf("\n random string = %s", s);
		
		sha256_init(&sha256);
		sha256_update(&sha256, (const unsigned char*)s, strlen(s));
		sha256_final(&sha256, sha256_result);
		
		//printf("\n hash of random string:");
		//printSHA256Hash(sha256_result);
		
		float flip = 0;
		for(int j=0; j<length; j++)
		{
			for(i=0; i<8;i++)
			{
				char s_temp[length+1];
				for(int p=0; p<length+1; p++)
				{s_temp[p] = s[p];}
				unsigned char k = 1<<i;
				s_temp[j] = s[j] ^ k;
				//printf("\n new string is = %s", s_temp);
		
				sha256_init(&sha256_temp);
				sha256_update(&sha256_temp, (const unsigned char*)s_temp, strlen(s_temp));
				sha256_final(&sha256_temp, sha256_temp_result);
				
				//printf("\n hash of new string:");	
				//printSHA256Hash(sha256_temp_result);
				
				//count the number of flips
				for(int r=0;r<32;r++)
				{
					k = sha256_result[r] ^ sha256_temp_result[r];
					
					//print the binary of md5 char and md5_temp char
					
					//printf("\n printing md5 char %02x (hex) in binary:", md5_result[r]);
					
					/*int mask = 0x80; 
					while (mask>0) {
					printf("%d", (md5_result[r] & mask) > 0 );
					mask >>= 1; 
					}

					printf("\n printinf md5_temp char %02x (hex) in binary:", md5_temp_result[r]);
					
					mask = 0x80; 
					while (mask>0) {
					printf("%d", (md5_temp_result[r] & mask) > 0 );
					mask >>= 1; 
					}*/
					
					bool bin_t[8];
					InitializeBinaryArray(bin_t,8);
					for(int h=0;h<8;h++)
					{
						bin_t[h] = !!((k >> h) & 0x01);
						if (bin_t[h]==1)
						{flip++;}
						//printf("\n flip = %0.3f", flip);
					}
					//printf("\n no of flips = %0.3f",flip);
				}
				
			}
			//printf("\n");
		}
		
		float single_avg = flip/(256*(float)bit_length);
		cum_avg = cum_avg + single_avg;
	}
	
	//note that the md5 output has 128 bits divided into 32 hex values each of 4 bits 
	cum_avg = cum_avg/1000;
	printf("\n Length %d - %0.5f", length, cum_avg);  
	return 0;
}

void printMD5Hash(unsigned char* result)
{
	for(int i=0;i<16;i++)
	{
		printf ("%02x", result[i]);
	}
}

//MD5 has four rounds!
int diffRateMD5(int l)
{
	MD5_CTX md5_temp;
	MD5_CTX md5;
	unsigned char md5_result[16];
	unsigned char md5_temp_result[16];
	
   int i;
	int length=l; 			//Length is given in terms of number of characters.The number of bits in the input is length*8.
	int bit_length = l*8;
	float cum_avg = 0;
	
	time_t t;
	srand((unsigned) time(&t));
	
	for(int p=0; p<1000; p++)
	{
		char s[length+1];
		gen_random(s,length);
		//printf("\n random string = %s", s);
		
		MD5_Init (&md5);
		MD5_Update (&md5, s, strlen(s));
		MD5_Final (md5_result,&md5); //The digest is now in md5_result which is an array of 16 characters (16*8=128)
		//printf("\n hash of random string:");
		//printMD5Hash(md5_result);
		
		float flip = 0;
		for(int j=0; j<length; j++)
		{
			for(i=0; i<8;i++)
			{
				char s_temp[length+1];
				for(int p=0; p<length+1; p++)
				{s_temp[p] = s[p];}
				unsigned char k = 1<<i;
				s_temp[j] = s[j] ^ k;
				//printf("\n new string is = %s", s_temp);
				
				MD5_Init (&md5_temp);
				MD5_Update (&md5_temp, s_temp, strlen(s_temp));
				MD5_Final (md5_temp_result,&md5_temp); //the temp digest is now in md5_temp_result
				//printf("\n hash of new string:");	
				//printMD5Hash(md5_temp_result);
				
				//count the number of flips
				for(int r=0;r<16;r++)
				{
					k = md5_result[r] ^ md5_temp_result[r];
					
					//print the binary of md5 char and md5_temp char
					
					//printf("\n printing md5 char %02x (hex) in binary:", md5_result[r]);
					
					/*int mask = 0x80; 
					while (mask>0) {
					printf("%d", (md5_result[r] & mask) > 0 );
					mask >>= 1; 
					}

					printf("\n printinf md5_temp char %02x (hex) in binary:", md5_temp_result[r]);
					
					mask = 0x80; 
					while (mask>0) {
					printf("%d", (md5_temp_result[r] & mask) > 0 );
					mask >>= 1; 
					}*/
					
					bool bin_t[8];
					InitializeBinaryArray(bin_t,8);
					for(int h=0;h<8;h++)
					{
						bin_t[h] = !!((k >> h) & 0x01);
						if (bin_t[h]==1)
						{flip++;}
						//printf("\n flip = %0.3f", flip);
					}
					//printf("\n no of flips = %0.3f",flip);
				}
				
			}
			//printf("\n");
		}
		
		float single_avg = flip/(128*(float)bit_length);
		cum_avg = cum_avg + single_avg;
	}
	
	//note that the md5 output has 128 bits divided into 32 hex values each of 4 bits 
	cum_avg = cum_avg/1000;
	printf("\n Length %d - %0.5f", length, cum_avg);  
	return 0;
}

int diffRateSHA1(int l)
{
	SHA1Context sha;
	SHA1Context sha_temp;
   int i;
	int length=l; 			//Length is given in terms of number of characters.The number of bits is length*8.
	int bit_length = l*8;
	float cum_avg = 0;
	
	time_t t;
	srand((unsigned) time(&t));
	
	for(int p=0; p<500; p++)
	{
		char s[length+1];
		gen_random(s,length);
		printf("\n random string = %s", s);
		SHA1Reset(&sha);
		SHA1Input(&sha, (const unsigned char *) s, strlen(s));
		if (!SHA1Result(&sha))
		{fprintf(stderr, "ERROR--could not compute message digest\n"); return 0;}
			
		bool bin_hash[5][32];
		/*printf("\n\t");*/
		for(i = 0; i < 5 ; i++)
		{
			InitializeBinaryArray(bin_hash[i],32);
			Decimal2Binary(sha.Message_Digest[i], bin_hash[i]);
			//printf("%X ", sha.Message_Digest[i]);
		}
		
		float flip = 0;
		for(int j=0; j<length; j++)
		{
			for(i=0; i<8;i++)
			{
				char s_temp[length+1];
				for(int p=0; p<length+1; p++)
				{s_temp[p] = s[p];}
				unsigned char k = 1<<i;
				s_temp[j] = s[j] ^ k;
				//printf("\n new string is = %s", s_temp);
				SHA1Reset(&sha_temp);
				SHA1Input(&sha_temp, (const unsigned char *) s_temp, strlen(s_temp));
				
				for(int t=0;t<5;t++)
				{
					int c = sha_temp.Message_Digest[t];
					bool bin_c[32];
					InitializeBinaryArray(bin_c,32);
					Decimal2Binary(sha_temp.Message_Digest[t], bin_c);
					for(int k=0;k<32;k++)
					{
						if(bin_c[k]!=bin_hash[t][k])
						{flip++;}
					}
				}
			}
			//printf("\n");
		}
		
		float single_avg = flip/(160*(float)bit_length);
		cum_avg = cum_avg + single_avg;
	}
	//note that the SHA1 output has 160 bits divided into 5 int values: Message_Digest[0],....,Message_Digest[4]
	cum_avg = cum_avg/500;
	printf("\n Length %d - %0.3f", length, cum_avg);  
	return 0;
}

/*to avoid overflows, l should be at most 16 bits.
 *if x is 16 bits, and y is 16 bits then the input is 32 bits long. the output of multiplication will be 32 bits as well.
 *for each of the 32 bit positions, we randomly assign the remaining input bits, and see how many output bits flip when the specific bit flips
 *for each bit position we do the above experiment 100 times and average it out over 100 trials
 */
int diffRateMultiplication(int l)
{
		time_t t;
		int length = l;
		float per_bit_diff[2*length];
		srand((unsigned) time(&t));
		
		for (int i = 0; i<2*length;i++)
		{
			float flip = 0;
				
			  for(int j = 0; j<1000; j++)
					{
									unsigned int x1;
									unsigned int y1;
									unsigned int x2;
									unsigned int y2;
									unsigned int result1;
									unsigned int result2;
									//int lo = 0;
									unsigned int hi = (1<<length);
								   //printf("\n hi = %d", hi); fflush(stdout);
								
									
									x1 = rand() % hi;
									y1 = rand() % hi;
							
									
									result1 = x1*y1;
							
									//printf("\n x1 = %u, y1 = %u, result1 = %u", x1, y1, result1);
									
									bool bin_result1[2*length];
									InitializeBinaryArray(bin_result1,2*length);
									Decimal2Binary(result1, bin_result1);
									
									if(i<l) //the bit is in x; y stays the same
									{
											unsigned int k = i;
											y2 = y1;
											bool bin_x1[length];
											InitializeBinaryArray(bin_x1,length);
											Decimal2Binary(x1, bin_x1);
											if(bin_x1[i]==1)
											{x2 = x1 - (1<<k);}
											else
											{x2 = x1 + (1<<k);}
									}
									
									else //the bit is in y; x stays the same
									{
											unsigned int k = i-l;
										   x2 = x1;
											bool bin_y1[length];
											InitializeBinaryArray(bin_y1,length);
											Decimal2Binary(y1, bin_y1);
											if(bin_y1[k]==1)
											{y2 = y1 - (1<<k);}
											else
											{y2 = y1 + (1<<k);}	
									}
									
									result2 = x2*y2;
									bool bin_result2[2*length];
									InitializeBinaryArray(bin_result2,2*length);
									Decimal2Binary(result2, bin_result2);
									
									//printf("\n x2 = %u, y2 = %u, result2 = %u", x2, y2, result2);
									
									for(int h=0; h<length;h++)
									{
											if(bin_result1[h]!=bin_result2[h])
											{flip = flip + 1;}
									}
									
					}
					
					//printf("\n %dth bit number of flips = %.3f", i, flip);
					per_bit_diff[i] = flip/(((float)length) * 1000);
					//printf("\n %dth bit diffusion = %.3f", i, per_bit_diff[i]);
		}
		
		float average = 0;
		for (int i = 0; i< 2*length; i++)
		{
				average = average + per_bit_diff[i];
		}
		average = average/(2*length);
		//printf("\n the average diffusion for length %d is = %.3f", length, average); fflush(stdout);
		printf("\n %d - %.3f", length, average); fflush(stdout);
	 return 0;	
}

int diffRateAddition(int l)
{
		time_t t;
		int length = l;
		float per_bit_diff[2*length];
		srand((unsigned) time(&t));
		
		for (int i = 0; i<2*length;i++)
	 {
				float flip = 0;
				
			  for(int j = 0; j<1000; j++)
					{
									unsigned int x1;
									unsigned int y1;
									unsigned int x2;
									unsigned int y2;
									unsigned int result1;
									unsigned int result2;
									//int lo = 0;
									unsigned int hi = (1<<length);
								   //printf("\n hi = %d", hi); fflush(stdout);
								
									
									x1 = rand() % hi;
									y1 = rand() % hi;
							
									
									result1 = x1+y1;
							
									//printf("\n x1 = %u, y1 = %u, result1 = %u", x1, y1, result1);
									
									bool bin_result1[length+1];
									InitializeBinaryArray(bin_result1,length+1);
									Decimal2Binary(result1, bin_result1);
									
									if(i<l) //the bit is in x; y stays the same
									{
											unsigned int k=i;
											y2 = y1;
											bool bin_x1[length];
											InitializeBinaryArray(bin_x1,length);
											Decimal2Binary(x1, bin_x1);
											if(bin_x1[i]==1)
											{x2 = x1 - (1<<k);}
											else
											{x2 = x1 + (1<<k);}
									}
									
									else //the bit is in y; x stays the same
									{
											unsigned int k = i-l;
										   x2 = x1;
											bool bin_y1[length];
											InitializeBinaryArray(bin_y1,length);
											Decimal2Binary(y1, bin_y1);
											if(bin_y1[k]==1)
											{y2 = y1 - (1<<k);}
											else
											{y2 = y1 + (1<<k);}	
									}
									
									result2 = x2+y2;
									bool bin_result2[length+1];
									InitializeBinaryArray(bin_result2,length+1);
									Decimal2Binary(result2, bin_result2);
									
									//printf("\n x2 = %u, y2 = %u, result2 = %u", x2, y2, result2);
									
									for(int h=0; h<length;h++)
									{
											if(bin_result1[h]!=bin_result2[h])
											{flip = flip + 1;}
									}
									
					}
					
					/*if(length==1)
					{printf("\n %dth bit number of flips = %.3f", i, flip);}*/
					per_bit_diff[i] = flip/(((float)length)*1000);
					//printf("\n %dth bit diffusion = %.3f", i, per_bit_diff[i]);
		}
		
		float average = 0;
		for (int i = 0; i< 2*length; i++)
		{
				average = average + per_bit_diff[i];
		}
		average = average/(2*length);
		//printf("\n the average diffusion for length %d is = %.3f", length, average); fflush(stdout);
		printf("\n %d - %.3f", length, average); fflush(stdout);
	 return 0;	
}

//input = composite, output = smallest prime factor
/*int diffRateFactoring (int l)
{
	int length = l; //max number of bits in each prime
	
	time_t t;
	float per_bit_diff[2*length];
	srand((unsigned) time(&t));
	
	unsigned int hi = (1<<length) - 1;
	unsigned int p;
	unsigned int q;
	unsigned int output;
	unsigned int input;
	int isPrime = 0;
	
	while(isPrime==0)
	{
		p = (rand() % hi) + 1;
		isPrime = Miller(p,25);
	}
	
	isPrime = 0;
	while(isPrime==0)
	{
		q = (rand() % hi) + 1;
		isPrime = Miller(p,25);
	}
	
	input = p*q;
	if(p<q){output=p;}
	else{output=q;}
	
	
	
	
	return 0;
}*/

//SHA1, MD5 etc.

int main ( int argc, char *argv[] )
{
		//xor are hard to invert although their diffusion is only 1/n where n is the number of output bits!
	 
		int op = 5; 						// 1-multiplication, 2-addition,3-SHA1, 4-MD5, 5-SHA256
		
		int length;
		
		if(op==1) // multiplication
		{
			for(int i=2; i<32; i++)
			{
				length = i; 				//number of bits in an integer. total input length is twice of this
				diffRateMultiplication(length);
			}
		}
		
		else if (op==2) 					// addition
		{
			for(int i=2; i<32; i++)
			{
				length = i; 				//number of bits in an integer. total output length is one more than this.
				diffRateAddition(length);
			}
		}
		
		else if (op==3)
		{
			for(int length = 50; length <= 1000; length = length + 50)
			{diffRateSHA1(length);}
		}
		
		else if (op==4)
		{
			for(int length = 50; length <= 1000; length = length + 50)
			{diffRateMD5(length);}
		}
		
		else if (op==5)
		{
			for(int length = 50; length <= 1000; length = length + 50)
			{diffRateSHA256(length);}
		}
	
		return 0;
		
}


