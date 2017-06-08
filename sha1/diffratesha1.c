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
#include "sha1.h"
#include <stdint.h>

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

int diffRateSHA1(int l)
{
	SHA1Context sha;
	SHA1Context sha_temp;
    int i;
	int length=l; 			//Length is given in terms of number of characters.The number of bits is length*8.
	int bit_length = l*8;
	float cum_avg = 0;
    int no_samples = 10000;
	
	time_t t;
	srand((unsigned) time(&t));
	
	for(int p=0; p<no_samples; p++)
	{
		char s[length+1];
		gen_random(s,length);
		//printf("\n random string = %s \n", s);
		SHA1Reset(&sha);
		SHA1Input(&sha, (const unsigned char *) s, strlen(s));
		if (!SHA1Result(&sha))
		{fprintf(stderr, "ERROR--could not compute message digest\n"); return 0;}
			
		bool bin_hash[5][32];
		//printf("\n\t");
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
                int flip_ind=0; //for testing!
				char s_temp[length+1];
				for(int p=0; p<length+1; p++)
				{s_temp[p] = s[p];}
				unsigned char k = 1<<i;
				s_temp[j] = s[j] ^ k;
				//printf("\n new string is = %s\n", s_temp);
				SHA1Reset(&sha_temp);
				SHA1Input(&sha_temp, (const unsigned char *) s_temp, strlen(s_temp));
				
				for(int t=0;t<5;t++)
				{
					int c = sha_temp.Message_Digest[t];
					bool bin_c[32];
					InitializeBinaryArray(bin_c,32);
					Decimal2Binary(sha_temp.Message_Digest[t], bin_c);
                    //printf("%X ", sha.Message_Digest[t]);
					for(int k=0;k<32;k++)
					{
						if(bin_c[k]!=bin_hash[t][k])
						{flip++; flip_ind++;}
					}
				}
                
                //printf("\n flip_ind = %d", flip_ind);
			}
			//printf("\n");
		}
		
		float single_avg = flip/(160*(float)bit_length);
		cum_avg = cum_avg + single_avg;
	}
	//note that the SHA1 output has 160 bits divided into 5 int values: Message_Digest[0],....,Message_Digest[4]
	cum_avg = cum_avg/(float)no_samples;
	printf("\n Length %d - %0.3f", length, cum_avg);  
	return 0;
}

int diffRateSHA1random()
{
	SHA1Context sha;
	SHA1Context sha_temp;
    int i;
	int length; 			//Length is given in terms of number of characters.The number of bits is length*8.
	int bit_length;
	float cum_avg = 0;
    int no_samples = 10000;
	
	time_t t;
	srand((unsigned) time(&t));
	
	for(int p=0; p<no_samples; p++)
	{
        length = (rand() % 996) + 5;
        bit_length = length*8;
		char s[length+1];
		gen_random(s,length);
		//printf("\n random string = %s \n", s);
		SHA1Reset(&sha);
		SHA1Input(&sha, (const unsigned char *) s, strlen(s));
		if (!SHA1Result(&sha))
		{fprintf(stderr, "ERROR--could not compute message digest\n"); return 0;}
			
		bool bin_hash[5][32];
		//printf("\n\t");
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
                int flip_ind=0; //for testing!
				char s_temp[length+1];
				for(int p=0; p<length+1; p++)
				{s_temp[p] = s[p];}
				unsigned char k = 1<<i;
				s_temp[j] = s[j] ^ k;
				//printf("\n new string is = %s\n", s_temp);
				SHA1Reset(&sha_temp);
				SHA1Input(&sha_temp, (const unsigned char *) s_temp, strlen(s_temp));
				
				for(int t=0;t<5;t++)
				{
					int c = sha_temp.Message_Digest[t];
					bool bin_c[32];
					InitializeBinaryArray(bin_c,32);
					Decimal2Binary(sha_temp.Message_Digest[t], bin_c);
                    //printf("%X ", sha.Message_Digest[t]);
					for(int k=0;k<32;k++)
					{
						if(bin_c[k]!=bin_hash[t][k])
						{flip++; flip_ind++;}
					}
				}
                
                //printf("\n flip_ind = %d", flip_ind);
			}
			//printf("\n");
		}
		
		float single_avg = flip/(160*(float)bit_length);
		cum_avg = cum_avg + single_avg;
	}
	//note that the SHA1 output has 160 bits divided into 5 int values: Message_Digest[0],....,Message_Digest[4]
	cum_avg = cum_avg/(float)no_samples;
	printf("\n avg diffusion = %0.3f",cum_avg);  
	return 0;
}

int perbitdiffSHA1(int l)
{
	SHA1Context sha;
	SHA1Context sha_temp;
    int i;
	int length=l; 			//Length is given in terms of number of characters.The number of bits is length*8.
	int bit_length = l*8;
	float cum_avg = 0;
	int no_samples=10000;
    
	time_t t;
	srand((unsigned) time(&t));
	
    float flipperbit[bit_length]; //to track total number of flips per bit
    for(i=0;i<bit_length;i++)
    {flipperbit[i]=0;}
    
	for(int p=0; p<no_samples; p++)
	{
		char s[length+1];
		gen_random(s,length);
		//printf("\n random string = %s \n", s);
		SHA1Reset(&sha);
		SHA1Input(&sha, (const unsigned char *) s, strlen(s));
		if (!SHA1Result(&sha))
		{fprintf(stderr, "ERROR--could not compute message digest\n"); return 0;}
			
		bool bin_hash[5][32];

		for(i = 0; i < 5 ; i++)
		{
			InitializeBinaryArray(bin_hash[i],32);
			Decimal2Binary(sha.Message_Digest[i], bin_hash[i]);
			//printf("%X ", sha.Message_Digest[i]);
		}
		
		//float flip = 0;
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
						{//flip++;
                         flipperbit[(j*8)+i]++;}
					}
				}
			}
			//printf("\n");
		}
		
		//float single_avg = flip/(160*(float)bit_length);
		//cum_avg = cum_avg + single_avg;
	}
	//note that the SHA1 output has 160 bits divided into 5 int values: Message_Digest[0],....,Message_Digest[4]
	//cum_avg = cum_avg/no_samples;
    for(int i=0;i<bit_length;i++)
	{printf("\n Diffusion for bit %d - %0.3f", i, flipperbit[i]/(160*(float)no_samples));}
	return 0;
}

int bitindSHA1()
{
	SHA1Context sha;
	SHA1Context sha_temp;
	float cum_avg = 0;
	int no_samples=100;
    int min = 100;
    
	time_t t;
	srand((unsigned) time(&t));
	
    float prob_ind[160];      //to track the running sum of p(i) over all samples. we will then divide this by no_samples
    float prob_pair[160][160]; //to track the runnin sum of p(i,j) over all samples. we will then divide this by no_samples
    float ind[160][160];
    
    for(int i=0;i<160;i++)
    {
        prob_ind[i]=0;
        for(int j=0;j<160;j++)
        {prob_pair[i][j]=0; ind[i][j]=0;}
    }
    
	for(int p=0; p<no_samples; p++)
	{
        //generate random string. length is a random no. between 50-1000 characters
        int length = random()%951 + 50; 
		char s[length+1];
		gen_random(s,length);
		//printf("\n random string = %s \n", s);
		SHA1Reset(&sha);
		SHA1Input(&sha, (const unsigned char *) s, strlen(s));
		if (!SHA1Result(&sha))
		{fprintf(stderr, "ERROR--could not compute message digest\n"); return 0;}
			
		bool bin_hash[5][32];
		for(int i = 0; i < 5 ; i++)
		{
			InitializeBinaryArray(bin_hash[i],32);
			Decimal2Binary(sha.Message_Digest[i], bin_hash[i]);
			//printf("%X ", sha.Message_Digest[i]);
		}
        
        bool bin_hash160[160];
        
        for(int i=0;i<5;i++)
        {
            for(int j=0;j<32;j++)
            {bin_hash160[i*32+j] = bin_hash[i][j];}
        }
		
        float flip[160];
        float flip_pair[160][160];
        
        for(int i=0;i<160;i++)
        {
            flip[i]=0;
            for(int j=0;j<160;j++)
            {
                flip_pair[i][j]=0;
            }
        }
        
		for(int j=0; j<length; j++)
		{
			for(int i=0; i<8;i++)
			{
				char s_temp[length+1];
				for(int p=0; p<length+1; p++)
				{s_temp[p] = s[p];}
				unsigned char k = 1<<i;
				s_temp[j] = s[j] ^ k;
				//printf("\n new string is = %s", s_temp);
				SHA1Reset(&sha_temp);
				SHA1Input(&sha_temp, (const unsigned char *) s_temp, strlen(s_temp));
				
                
                bool bin_hashtemp160[160];
                  
				for(int t=0;t<5;t++)
				{
					int c = sha_temp.Message_Digest[t];
                    bool bin_c[32];
					InitializeBinaryArray(bin_c,32);
					Decimal2Binary(sha_temp.Message_Digest[t], bin_c);
                    for(int j=0;j<32;j++)
                    {bin_hashtemp160[t*32+j] = bin_c[j];}
                }
                
				for(int k=0;k<160;k++)
				{
					if(bin_hashtemp160[k]!=bin_hash160[k])
					{flip[k]++;}
                    for(int h=k+1;h<160;h++)
                    {
                        if((bin_hashtemp160[k]!=bin_hash160[k]) && (bin_hashtemp160[h]!=bin_hash160[h]))
                        {
                            flip_pair[k][h]++;
                        }
                    }
				}
			}
		}
            
        for(int k=0; k<160;k++)
        {
            flip[k] = flip[k]/(float)(length*8);
            prob_ind[k] = prob_ind[k] + flip[k];
            for(int h=k+1;h<160;h++)
            {
                flip_pair[k][h] = flip_pair[k][h]/(float)(length*8);
                if(flip_pair[k][h]>(length*8))
                {printf("\nerrror: flip_pair > length");}
                prob_pair[k][h] = prob_pair[k][h] + flip_pair[k][h];
            }
            
        }
            
			//printf("\n");
	}
		
		//float single_avg = flip/(160*(float)bit_length);
		//cum_avg = cum_avg + single_avg;
    
    for(int k=0; k<160;k++)
    {
        prob_ind[k] = prob_ind[k]/(float)no_samples;
        if(prob_ind[k]>1)
        {printf("\nerrror: prob_ind > 1");}
              
        for(int h=k+1;h<160;h++)
        {
            prob_pair[k][h] =  prob_pair[k][h]/(float)no_samples;
            if(prob_pair[k][h]>1)
            {printf("\nerrror: prob_pair > 1");}
        }
    }
	//note that the SHA1 output has 160 bits divided into 5 int values: Message_Digest[0],....,Message_Digest[4]
	//cum_avg = cum_avg/no_samples;
    
    /*for(int i=0;i<160;i++)
    {
        printf("%d  ",i);
    }*/
    //printf("\n");
    for(int k=0; k<160;k++)
    {
        //printf("%d  ",k);
        for(int h=k+1;h<160;h++)
        {
            ind[k][h] = 1 - fabs(prob_pair[k][h]-(prob_ind[k]*prob_ind[h]));
            if((int)(ind[k][h] * 100) < min )
            {min = (int)(ind[k][h]*100);}
        }
        //printf("\n");
    }
    
    float avg_ind=0;
    int count=0;
    for(int k=0;k<160;k++)
    {
        for(int h=k+1;h<160;h++)
        {
            avg_ind = avg_ind + ind[k][h];
            count++;
        }
    }
    avg_ind = avg_ind / (float)count;
    printf("\n min = %d", min);
    printf("\n avg = %0.2f",avg_ind);
	return 0;
}

int main()
{
    diffRateSHA1random();
    
	/*for(int length = 50; length <= 500; length = length + 50)
	{diffRateSHA1(length);}*/
	
    //perbitdiffSHA1(500); //5 characters is 40 bits
    //bitindSHA1();
	return 0;
}
