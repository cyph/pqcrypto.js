#include <string.h>
#include "bin-lwe.h"
#include "rand.h"
#include "lac_param.h"
#include <stdio.h>
#include <stdint.h>

//generate the public parameter a from seed
int gen_a(unsigned char *a,  const unsigned char *seed)
{
	int i,j;
	unsigned char buf[MESSAGE_LEN];
	//check the pointers
	if(a==NULL || seed==NULL)
	{
		return 1;
	}
	
	pseudo_random_bytes(a,DIM_N,seed);
	
	hash(seed,SEED_LEN,buf);
	j=0;
	for(i=0;i<DIM_N;i++)
	{
		while(a[i]>=Q)
		{
			memcpy(a+i,buf+(j++),1);//replace a[i] with buf[j]
			if(j>=MESSAGE_LEN)
			{
				hash(buf,MESSAGE_LEN,buf);//use hash chain to refresh buf
				j=0;
			}
		}
	}
	
	return 0;
}
 
//generate the small random vector for secret and error
int gen_psi(char *e, unsigned int vec_num, unsigned char *seed)
{
	int i;
	if(e==NULL)
	{
		return 1;
	}
	
	#if defined PSI_SQUARE
	//Pr[e[i]=-1]=1/8,Pr[s[i]=0]=3/4,Pr[e[i]=1]=1/8,
	unsigned char r[vec_num/2],*p1,*p2,*p3;
	int e_0,e_1;
	//generate vec_num/2 bytes, use 4 bits to generate one error item
	pseudo_random_bytes(r,vec_num/2,seed);
	//COMPUTE e from r
	p1=r+vec_num/8;
	p2=p1+vec_num/8;
	p3=p2+vec_num/8;
	for(i=0;i<vec_num;i++)
	{
		
		e_0=((r[i/8]>>(i%8))&1)-((p1[i/8]>>(i%8))&1);
		e_1=((p2[i/8]>>(i%8))&1)-((p3[i/8]>>(i%8))&1);
		e[i]=e_0*e_1;
	}
	#else
	//Pr[e[i]=-1]=1/4,Pr[s[i]=0]=1/2,Pr[e[i]=1]=1/4,
	unsigned char r[vec_num/4],*p;
	//generate vec_num/4 bytes, use two bits to generate one error item
	pseudo_random_bytes(r,vec_num/4,seed);
	//COMPUTE e from r
	p=r+vec_num/8;
	for(i=0;i<vec_num;i++)
	{
		e[i]=((r[i/8]>>(i%8))&1)-((p[i/8]>>(i%8))&1);
	}
	#endif
	
	return 0;

}

// poly_mul  b=as
int poly_mul(const unsigned char *a, const char *s, unsigned char *b, unsigned int vec_num)
{
	int i,j;
	int16_t v[DIM_N+DIM_N],s0[DIM_N],s1[DIM_N];
	int64_t *v_p,*s0_p,*s1_p;
	int64_t sum0,sum1;
	int32_t gather0,gather1;
	
	//construct matrix of a
	memset(s0,0,DIM_N*2);
	memset(s1,0,DIM_N*2);
	for(i=0;i<DIM_N;i++)
	{
		v[i]=a[DIM_N-1-i];
		v[i+DIM_N]=Q-v[i];
		if(s[i]==-1)
			s0[i]=0xffff;
		if(s[i]==1)
			s1[i]=0xffff;
	}	
	
	for(i=0;i<vec_num;i++)
	{
		v_p=(int64_t*)(v+DIM_N-i-1);
		s0_p=(int64_t*)s0;
		s1_p=(int64_t*)s1;
		sum0=0;
		sum1=0;
		for(j=0;j<DIM_N;j+=32)
		{
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
			
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
			
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
			
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
			
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
			
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
			
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
			
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
		}
		gather0=((sum0&0xffff)+((sum0>>16)&0xffff)+((sum0>>32)&0xffff)+((sum0>>48)&0xffff));
		gather1=((sum1&0xffff)+((sum1>>16)&0xffff)+((sum1>>32)&0xffff)+((sum1>>48)&0xffff));
		b[i]=(gather1-gather0+BIG_Q)%Q;
	}
	
	return 0;
}
//b=as+e 
int poly_aff(const unsigned char *a, const char *s, char *e, unsigned char *b, unsigned int vec_num)
{
	int i,j;
	int16_t v[DIM_N+DIM_N],s0[DIM_N],s1[DIM_N];
	int64_t *v_p,*s0_p,*s1_p;
	int64_t sum0,sum1;
	int32_t gather0,gather1;
	
	//construct matrix of a
	memset(s0,0,DIM_N*2);
	memset(s1,0,DIM_N*2);
	for(i=0;i<DIM_N;i++)
	{
		v[i]=a[DIM_N-1-i];
		v[i+DIM_N]=Q-v[i];
		if(s[i]==-1)
			s0[i]=0xffff;
		if(s[i]==1)
			s1[i]=0xffff;
	}	
	
	for(i=0;i<vec_num;i++)
	{
		v_p=(int64_t*)(v+DIM_N-i-1);
		s0_p=(int64_t*)s0;
		s1_p=(int64_t*)s1;
		sum0=0;
		sum1=0;
		for(j=0;j<DIM_N;j+=32)
		{
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
			
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
			
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
			
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
			
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
			
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
			
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
			
			sum0+=((*v_p)&(*s0_p++));
			sum1+=((*v_p++)&(*s1_p++));
		}
		gather0=((sum0&0xffff)+((sum0>>16)&0xffff)+((sum0>>32)&0xffff)+((sum0>>48)&0xffff));
		gather1=((sum1&0xffff)+((sum1>>16)&0xffff)+((sum1>>32)&0xffff)+((sum1>>48)&0xffff));
		b[i]=(gather1-gather0+e[i]+BIG_Q)%Q;
	}
	
	return 0;
}

