#include <immintrin.h>
#include <string.h>
#include "bin-lwe.h"
#include "rand.h"
#include "lac_param.h"
#include <stdio.h>

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
	unsigned char v[DIM_N+DIM_N],*v_p;
	__m256i tmp0, tmp1, tmp2, tmp_one;
	int32_t sum_all;
	int32_t r[8];
	
	tmp_one=_mm256_set_epi16(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1);
	//construct vector of a
	for(i=0;i<DIM_N;i++)
	{
		v[i]=a[DIM_N-1-i];
		v[i+DIM_N]=Q-v[i];
	}	

	//compute b[i] 
	for(i=0;i<vec_num;i++)
	{
		v_p=(v+DIM_N-i-1);
		tmp2 =_mm256_setzero_si256();
		for(j=0;j<DIM_N;j+=128)
		{
			tmp0 = _mm256_loadu_si256((__m256i *)(v_p+j));
			tmp1 = _mm256_loadu_si256((__m256i *)(s+j));
			tmp0 = _mm256_maddubs_epi16(tmp0, tmp1);
			tmp2 = _mm256_add_epi16(tmp2, tmp0);
			
			tmp0 = _mm256_loadu_si256((__m256i *)(v_p+j+32));
			tmp1 = _mm256_loadu_si256((__m256i *)(s+j+32));
			tmp0 = _mm256_maddubs_epi16(tmp0, tmp1);
			tmp2 = _mm256_add_epi16(tmp2, tmp0);
			
			tmp0 = _mm256_loadu_si256((__m256i *)(v_p+j+64));
			tmp1 = _mm256_loadu_si256((__m256i *)(s+j+64));
			tmp0 = _mm256_maddubs_epi16(tmp0, tmp1);
			tmp2 = _mm256_add_epi16(tmp2, tmp0);
			
			tmp0 = _mm256_loadu_si256((__m256i *)(v_p+j+96));
			tmp1 = _mm256_loadu_si256((__m256i *)(s+j+96));
			tmp0 = _mm256_maddubs_epi16(tmp0, tmp1);
			tmp2 = _mm256_add_epi16(tmp2, tmp0);
		}

		tmp2 = _mm256_madd_epi16(tmp2,tmp_one);
		_mm256_storeu_si256((__m256i*)r,tmp2);
		sum_all=r[0]+r[1]+r[2]+r[3]+r[4]+r[5]+r[6]+r[7];
		
		b[i]=(sum_all+BIG_Q)%Q;
	}
	
	return 0;
}
//b=as+e 
int poly_aff(const unsigned char *a, const char *s, char *e, unsigned char *b, unsigned int vec_num)
{
	int i,j;
	unsigned char v[DIM_N+DIM_N],*v_p;
	__m256i tmp0, tmp1, tmp2, tmp_one;
	int32_t sum_all;
	int32_t r[8];
	
	tmp_one=_mm256_set_epi16(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1);
	//construct vector of a
	for(i=0;i<DIM_N;i++)
	{
		v[i]=a[DIM_N-1-i];
		v[i+DIM_N]=Q-v[i];
	}	

	//compute b[i] 
	for(i=0;i<vec_num;i++)
	{
		v_p=(v+DIM_N-i-1);
		tmp2 =_mm256_setzero_si256();
		for(j=0;j<DIM_N;j+=128)
		{
			tmp0 = _mm256_loadu_si256((__m256i *)(v_p+j));
			tmp1 = _mm256_loadu_si256((__m256i *)(s+j));
			tmp0 = _mm256_maddubs_epi16(tmp0, tmp1);
			tmp2 = _mm256_add_epi16(tmp2, tmp0);
			
			tmp0 = _mm256_loadu_si256((__m256i *)(v_p+j+32));
			tmp1 = _mm256_loadu_si256((__m256i *)(s+j+32));
			tmp0 = _mm256_maddubs_epi16(tmp0, tmp1);
			tmp2 = _mm256_add_epi16(tmp2, tmp0);
			
			tmp0 = _mm256_loadu_si256((__m256i *)(v_p+j+64));
			tmp1 = _mm256_loadu_si256((__m256i *)(s+j+64));
			tmp0 = _mm256_maddubs_epi16(tmp0, tmp1);
			tmp2 = _mm256_add_epi16(tmp2, tmp0);
			
			tmp0 = _mm256_loadu_si256((__m256i *)(v_p+j+96));
			tmp1 = _mm256_loadu_si256((__m256i *)(s+j+96));
			tmp0 = _mm256_maddubs_epi16(tmp0, tmp1);
			tmp2 = _mm256_add_epi16(tmp2, tmp0);
		}

		tmp2 = _mm256_madd_epi16(tmp2,tmp_one);
		_mm256_storeu_si256((__m256i*)r,tmp2);
		sum_all=r[0]+r[1]+r[2]+r[3]+r[4]+r[5]+r[6]+r[7];
		
		b[i]=(sum_all+e[i]+BIG_Q)%Q;
	}
	
	return 0;
}

