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
#include <sys/syscall.h>
//#include <sys/random.h>

unsigned long long getRand(unsigned long long max) {
    /* Get a rand int in [0, max] */

    if(max == 0) return 0;

    int fd = open("/dev/urandom", O_RDONLY);
    if(fd < 0) {
		perror("open(urandom)"); exit(0);
    }

    unsigned long long i = 0;
    if(read(fd, &i, sizeof(unsigned long long)) < sizeof(unsigned long long)) {
	perror("read(urandom)"); exit(0);
    }

    close(fd);

    i %= (max+1); //printf("\n max = %llu and i=%llu", max,i);
    return i;
}


int Decimal2Binary(unsigned long long dec, unsigned int binary[],int max)
{
			int i=0;
			unsigned long long quotient = 0;
			quotient = dec;
			unsigned long long remainder = 0;
			
			//printf("\n dec = %llu : ", dec);
			
			while(quotient!=0)
			{
					remainder = quotient % 2;
					quotient = (unsigned long long)quotient/2;
					binary[i] = remainder;
                    //if(dec==0)
					//{printf("\n binary[%d] = %u", i, binary[i]); fflush(stdout);}
                    i++;
			}
			
			//fflush(stdout);
			return i;
}

int diffRateAddition(unsigned int l)
{
		
		//printf("\n MAX LENGTH = %d",l);
		int no_samples = 10000;
		unsigned int max_length = l;
		unsigned long long max = 0;
		max = (1ULL<<l)-1;
        //printf("\n max = %llu", max);
        float flips_total = 0;
		
        float diffusion = 0;
        
		for(int i=0; i< no_samples;i++)
        {
            unsigned int x_bin[max_length];
            unsigned int y_bin[max_length];
            unsigned int result_bin[max_length+1];
            
            for(int j=0;j<max_length;j++)
            {
                x_bin[j]=0;
                y_bin[j]=0;
                result_bin[j]=0;
            }
            result_bin[max_length]=0;
            
            unsigned long long x = getRand(max);
            unsigned long long y = getRand(max);
            unsigned long long result = x+y;
            
            int total_input_length=0;
            int size_result;
            int size_operand;
            
            int size_x = Decimal2Binary(x,x_bin,max_length);
            int size_y = Decimal2Binary(y,y_bin,max_length);
            Decimal2Binary(result,result_bin,max_length+1);
            
            if(size_x>size_y)
            {
                size_operand = size_x;
                size_result = size_x+1;
                total_input_length = 2*size_x;
            }
            else
            {
                size_operand = size_y;
                size_result = size_y+1;
                total_input_length = 2*size_y;
            }
            
            float flips = 0;
            
            for(int h=0;h<total_input_length;h++)
            {
                unsigned long long x_new=0;
                unsigned long long y_new=0;
                if(h<size_operand) //working with x
                {
                    if(x_bin[h]==1)
					{x_new = x - (unsigned long long)(1ULL<<h);}
					else
					{x_new = x + (unsigned long long)(1ULL<<h);}
					y_new = y;
                }
                else //working with y
                {
                    int r = h - size_operand;
                    if(y_bin[r]==1)
					{y_new = y - (unsigned long long)(1ULL<<r);}
					else
					{y_new = y + (unsigned long long)(1ULL<<r);}
					x_new = x;
                }
                
                unsigned long long result_new = 0;
                result_new = x_new+y_new;
                
                unsigned int result_new_bin[max_length+1];
                
                for(int t=0;t<max_length+1;t++)
                {result_new_bin[t]=0;}
                
                unsigned long long max2 = (1ULL<<(max_length+1))-1;
                if(result_new>max2)
                {
                    printf("\nsize_x = %d, size_y=%d, total_input_length=%d, \nbit no = %d, \nx=%llu, \ny=%llu, \nx_new=%llu, \ny_new=%llu, \noverflowing result = %llu, \n max2 = %llu",size_x,size_y,total_input_length,h, x,y,x_new, y_new, result_new,max2); fflush(stdout);
                }
                
                if(result_new>max2)
                {return 0;}
                Decimal2Binary(result_new,result_new_bin,max_length+1);
                
                for(int t=0;t<size_result;t++)
                {
                    if(result_bin[t]!=result_new_bin[t])
                    {flips++;}
                }
                
            }
            
            flips_total = flips_total + flips/(float)total_input_length;
            diffusion = diffusion + (flips/(float)(total_input_length*size_result));
        }
		
        //diffusion = diffusion/(float)no_samples;
        //printf("\n%0.2f",diffusion);
        printf("\n average no. of flips is %0.2f", flips_total/(float)no_samples);
        
        return 0;	
}

//going to look at bit independence between bits 0-16. So operands should be at least 16 bits long.
//that is, operands should be at least 2^15 in value.
//we consider a maximum size of 32 bits
int bitind(int l)
{
		//printf("\n MAX LENGTH = %d",l);
		int no_samples = 10000;
		unsigned int max_length = l;
		unsigned long long max1 = 0;
		max1 = (1ULL<<l)-1;
        unsigned long long max2 = 0;
        max2 = (1ULL<<l)-1-(1ULL<<15);
        //printf("\n max = %llu", max);
        float flips_per_bit[17];
        float flips_per_pair[17][17];
        float per_bit[17];
        float per_pair[17][17];
        
        for(int i=0;i<17;i++)
        {
            flips_per_bit[i]=0;
            per_bit[i]=0;
           for(int j=0; j<17; j++)
           {
                per_pair[i][j]=0;
                flips_per_pair[i][j]=0;
           }
        }
        
		for(int i=0; i< no_samples;i++)
        {
            unsigned int x_bin[max_length];
            unsigned int y_bin[max_length];
            unsigned int result_bin[max_length+1];
            
            for(int s=0;s<17;s++)
            {
                flips_per_bit[s]=0;
                for(int r=0;r<17;r++)
                {flips_per_pair[s][r]=0;}
            }
            
            for(int j=0;j<max_length;j++)
            {
                x_bin[j]=0;
                y_bin[j]=0;
                result_bin[j]=0;
            }
            result_bin[max_length]=0;
            
            unsigned long long x = getRand(max2)+(1ULL<<15);
            unsigned long long y = getRand(max1);
            unsigned long long result = x+y;
            
            int total_input_length=0;
            int size_result;
            int size_operand;
            
            int size_x = Decimal2Binary(x,x_bin,max_length);
            int size_y = Decimal2Binary(y,y_bin,max_length);
            Decimal2Binary(result,result_bin,max_length+1);
            
            if(size_x>size_y)
            {
                size_operand = size_x;
                size_result = size_x+1;
                total_input_length = 2*size_x;
            }
            else
            {
                size_operand = size_y;
                size_result = size_y+1;
                total_input_length = 2*size_y;
            }
            
            for(int h=0;h<total_input_length;h++)
            {
                unsigned long long x_new=0;
                unsigned long long y_new=0;
                if(h<size_operand) //working with x
                {
                    if(x_bin[h]==1)
					{x_new = x - (unsigned long long)(1ULL<<h);}
					else
					{x_new = x + (unsigned long long)(1ULL<<h);}
					y_new = y;
                }
                else //working with y
                {
                    int r = h - size_operand;
                    if(y_bin[r]==1)
					{y_new = y - (unsigned long long)(1ULL<<r);}
					else
					{y_new = y + (unsigned long long)(1ULL<<r);}
					x_new = x;
                }
                
                unsigned long long result_new = 0;
                result_new = x_new+y_new;
                
                unsigned int result_new_bin[max_length+1];
                
                for(int t=0;t<max_length+1;t++)
                {result_new_bin[t]=0;}
                
                Decimal2Binary(result_new,result_new_bin,max_length+1);
                
                for(int t=0;t<17;t++)
                {
                    if(result_bin[t]!=result_new_bin[t])
                    {flips_per_bit[t]++;}
                    for(int s=t+1;s<17;s++)
                    {
                        if(result_bin[t]!=result_new_bin[t] && result_bin[s]!=result_new_bin[s])
                        {flips_per_pair[t][s]++;}
                    }
                }
            
            }
            
            for(int t=0;t<17;t++)
            {
                flips_per_bit[t] = flips_per_bit[t]/(float)total_input_length;
                per_bit[t] = per_bit[t] + flips_per_bit[t];
                for(int s=t+1;s<17;s++)
                {
                    flips_per_pair[t][s] =  flips_per_pair[t][s]/(float)total_input_length;
                    per_pair[t][s] = per_pair[t][s] + flips_per_pair[t][s];
                }
            }
        }
		
        for(int t=0;t<17;t++)
        {
            per_bit[t] = per_bit[t]/(float)no_samples;
            for(int s=t+1;s<17;s++)
            {
                per_pair[t][s] = per_pair[t][s]/(float)no_samples;
            }
        }
        
        for(int t=0;t<17;t++)
        {
            for(int s=t+1;s<17;s++)
            {
                printf("\n[%d][%d] = %0.2f",t,s,1-fabs(per_pair[t][s]-(per_bit[t]*per_bit[s])));
            }
        }
        //diffusion = diffusion/(float)no_samples;
        //printf("\n%0.2f",diffusion)
        
        return 0;	
}

int main ( int argc, char *argv[] )
{
		
		unsigned int length=32;
		/*for (int i=2;i<63;i++)
		{
				length=(unsigned int)i;
				diffRateAddition(length);
		}*/
        bitind(length);
		return 0;
}
