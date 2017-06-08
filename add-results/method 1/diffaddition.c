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
#include <sys/random.h>

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

    i %= (max+1);
				//printf("\n max = %llu and i=%llu", max,i);
    return i;
}


int Decimal2Binary(unsigned long long dec, unsigned int binary[])
{
			int i=0;
			unsigned long long quotient = 0;
			quotient = dec;
			unsigned long long remainder = 0;
			
			//printf("\n dec = %llu : ", dec);
			
			while(quotient!=0)
			{
					remainder = quotient % 2;
					quotient = quotient/2;
					binary[i] = remainder;
                    /*if(dec==0)
					{printf("\n binary[%d] = %u", i, binary[i]); fflush(stdout);}*/
                    i++;
			}
			
			fflush(stdout);
			return 0;
}

int diffRateAddition(unsigned int l)
{
		
		printf("\n LENGTH = %d",l);
		int no_samples = 10000;
		unsigned int length = l;
		unsigned long long max = 0;
		max = (1ULL<<l)-1;
        printf("\n max = %llu", max);
		
		//first choose a 10000 input samples and calculate the original outputs.
		unsigned int output[no_samples][length+1];
		unsigned int input_x[no_samples][length];
		unsigned int input_y[no_samples][length];														
		unsigned long long x[no_samples];
		unsigned long long y[no_samples];
        
		for(int i=0; i<no_samples;i++)
		{
			 x[i]=0;
			 y[i]=0;
             for(int j=0;j<length;j++)
             {
                input_x[i][j]=0;
                input_y[i][j]=0;
             }
		}
		
        for(int i=0;i<no_samples;i++)
        {
            for(int j=0;j<length+1;j++)
            {
                output[i][j]=0;
            }
        }
        
		for(int i=0;i<no_samples;i++)
		{
			unsigned long long x_temp=0;
			unsigned long long y_temp=0;
			x_temp = getRand(max);
			y_temp = getRand(max);
			x[i]=x_temp;
			y[i]=y_temp;
			unsigned long long result = 0;
			result = x_temp+y_temp;
			//printf("\n x=%llu, y=%llu, result=%llu",x[i],y[i],result);
			Decimal2Binary(result, output[i]);
			Decimal2Binary(x_temp, input_x[i]);
			Decimal2Binary(y_temp, input_y[i]);
		}
		
		unsigned long long x_new=0;
		unsigned long long y_new=0;
		unsigned long long result_new=0;
		unsigned int output_new[length+1];
		float flips[2*length];
		
		for(int i=0;i<2*length;i++)
		{
				flips[i]=0;
		}
		
		//now for each bit position from 0 to 2*length-1, flip the input bit and find number of flips over samples
		for (int i=0;i<(2*length);i++)
		{
				for(int j=0; j<no_samples;j++)
				{
					x_new = 0;
					y_new = 0;
					if(i<length) //working with x value
					{
							if(input_x[j][i]==1)
							{x_new = x[j] - (unsigned long long)(1ULL<<i);}
							else
							{x_new = x[j] + (unsigned long long)(1ULL<<i);}
							y_new = y[j];
							if(x_new>max || y_new>max)
							{
								printf("\n max = %llu, x_new = %llu, y_new = %llu, x[j]=%llu, y[j]=%llu, i=%d, 1<<i is %llu",max, x_new,y_new,x[j],y[j],i,(unsigned long long) 1<<i);
                                printf("\n input_x[j][i]=%u",input_x[j][i]);
							}
					}
					else //working with y value
					{
							int r=i-length;
							if(input_y[j][r]==1)
							{y_new = y[j] - (unsigned long long)(1ULL<<r);}
							else
							{y_new = y[j] + (unsigned long long)(1ULL<<r);}
							x_new = x[j];
							if(x_new>max || y_new>max)
							{
								printf("\n max = %llu, x_new = %llu, y_new %llu = , x[j]=%llu, y[j]=%llu, r=%d, 1<<r is %llu", max, x_new,y_new,x[j],y[j],r,(unsigned long long)1<<r);
                                printf("\n input_y[j][r]=%u",input_y[j][r]);
							}
					}
					
					result_new=0;
					result_new = x_new+y_new;
					//printf("\n x_new = %llu, y_new = %llu, result_new = %llu", x_new,y_new, result_new);
					Decimal2Binary(result_new, output_new);
					for(int k=0;k<length+1;k++)
					{
							if(output_new[k]!=output[j][k])
							{flips[i]++;}
					}
				}
				
		//print per bit diffusion
		//printf("\n no. of flips for input bit %d is %0.5f",i,flips[i]); fflush(stdout);
        if(length==8||length==16||length==32||length==63)
		{printf("\n input bit %d = %0.5f",i,flips[i]/(float)((no_samples*(length+1)))); fflush(stdout);}
		}
		
		//print average diffusion
		float avg = 0;
        float total_flipped=0;
		for(int i=0;i<2*length;i++)
		{
				avg = avg + flips[i];
		}
        total_flipped = avg/(2*length*no_samples);
		avg = avg/(float)(((2*length)*no_samples*(length+1)));
		
		//printf("\n average diffusion for length %d = %0.3f, avg. number of bits that flip = %0.3f",length,avg,total_flipped); fflush(stdout);
        return 0;	
}

int bitind(int l)
{
		int no_samples = 10000;
		unsigned int length = l;
		unsigned long long max = 0;
		max = (1ULL<<l)-1;
        printf("\n max = %llu", max);
		
		//first choose a 100000 input samples and calculate the original outputs.
		unsigned int output[no_samples][length+1];
		unsigned int input_x[no_samples][length];
		unsigned int input_y[no_samples][length];														
		unsigned long long x[no_samples];
		unsigned long long y[no_samples];
        
		for(int i=0; i<no_samples;i++)
		{
			 x[i]=0;
			 y[i]=0;
             for(int j=0;j<length;j++)
             {
                input_x[i][j]=0;
                input_y[i][j]=0;
             }
		}
		
        for(int i=0;i<no_samples;i++)
        {
            for(int j=0;j<length+1;j++)
            {
                output[i][j]=0;
            }
        }
        
          
		for(int i=0;i<no_samples;i++)
		{
			unsigned long long x_temp=0;
			unsigned long long y_temp=0;
			x_temp = getRand(max);
			y_temp = getRand(max);
			x[i]=x_temp;
			y[i]=y_temp;
			unsigned long long result = 0;
			result = x_temp+y_temp;
			//printf("\n x=%llu, y=%llu, result=%llu",x[i],y[i],result);
			Decimal2Binary(result, output[i]);
			Decimal2Binary(x_temp, input_x[i]);
			Decimal2Binary(y_temp, input_y[i]);
		}
        
        unsigned long long x_new=0;
		unsigned long long y_new=0;
		unsigned long long result_new=0;
		unsigned int output_new[length+1];
		int flips_per_bit[2*length][length+1];
        int flips_per_pair[2*length][length+1][length+1];
        
        for(int i=0;i<2*length;i++)
        {
            for(int j=0;j<length+1;j++)
            {
                flips_per_bit[i][j]=0;
            }
        }
        
         for(int i=0;i<2*length;i++)
        {
            for(int j=0;j<length+1;j++)
            {
                for(int k=0;k<length+1;k++)
                {flips_per_pair[i][j][k]=0;}
            }
        }
        
        for (int i=0;i<(2*length);i++)
        {
			for(int j=0; j<no_samples;j++)
			{
				x_new = 0;
				y_new = 0;
				if(i<length) //working with x value
				{
					if(input_x[j][i]==1)
					{x_new = x[j] - (unsigned long long)(1ULL<<i);}
					else
					{x_new = x[j] + (unsigned long long)(1ULL<<i);}
					y_new = y[j];
					if(x_new>max || y_new>max)
					{
						printf("\n max = %llu, x_new = %llu, y_new = %llu, x[j]=%llu, y[j]=%llu, i=%d, 1<<i is %llu",max, x_new,y_new,x[j],y[j],i,(unsigned long long) 1<<i);
                        printf("\n input_x[j][i]=%u",input_x[j][i]);
					}
				}
				else //working with y value
				{
					int r=i-length;
					if(input_y[j][r]==1)
					{y_new = y[j] - (unsigned long long)(1ULL<<r);}
					else
					{y_new = y[j] + (unsigned long long)(1ULL<<r);}
					x_new = x[j];
					if(x_new>max || y_new>max)
					{
						printf("\n max = %llu, x_new = %llu, y_new %llu = , x[j]=%llu, y[j]=%llu, r=%d, 1<<r is %llu", max, x_new,y_new,x[j],y[j],r,(unsigned long long)1<<r);
                        printf("\n input_y[j][r]=%u",input_y[j][r]);
					}
				}
					
				result_new=0;
				result_new = x_new+y_new;
				//printf("\n x_new = %llu, y_new = %llu, result_new = %llu", x_new,y_new, result_new);
				Decimal2Binary(result_new, output_new);
				for(int k=0;k<length+1;k++)
				{
					if(output_new[k]!=output[j][k])
					{flips_per_bit[i][k]++;}
				}
                
                for(int h=0;h<length+1;h++)
                {
                    for(int e=h+1;e<length+1;e++)
                    {
                        if(output_new[h]!=output[j][h]&&output_new[e]!=output[j][e])
                        {
                            flips_per_pair[i][h][e]++;
                        }
                    }
                }
			}
        }
        
        //calculate the probabilities and the independence now
        float ind_per_pair[2*length][length+1][length+1];
        
        for(int i=0;i<2*length;i++)
        {
            for(int j=0;j<length+1;j++)
            {
                for(int k=j+1;k<length+1;k++)
                {
                    float p_ij = flips_per_bit[i][j]/(float)no_samples;
                    float p_ik = flips_per_bit[i][k]/(float)no_samples;
                    float p_ijk = flips_per_pair[i][j][k]/(float)no_samples;
                    float difference = fabs((p_ij*p_ik) - p_ijk);
                    ind_per_pair[i][j][k] = 1-difference;
                }
            }
        }
        
        float avg_ind_per_pair[length+1][length+1];
        for(int i=0; i<length+1;i++)
        {
            for(int j=i+1;j<length+1;j++)
            {
                float r = 0;
                for(int k=0;k<2*length;k++)
                {
                    r = r+ind_per_pair[k][i][j];
                }
                avg_ind_per_pair[i][j]=r/(2*length);
                printf("\n the avg ind for pair[%d][%d] = %0.3f",i,j,avg_ind_per_pair[i][j]);
            }
        }
        
        
        return 0;
}

int main ( int argc, char *argv[] )
{
		
		int length=16;
		for (int i=1;i<64;i++)
		{
				length=i;
				diffRateAddition(length);
		}
        //bitind(length);
		return 0;
}