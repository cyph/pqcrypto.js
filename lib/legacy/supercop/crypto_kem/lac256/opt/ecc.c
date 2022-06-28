#include "bch.h"
#include "ecc.h"
#include "lac_param.h"
#include <string.h>
#include <stdlib.h>

#if defined(LAC128)
//bch(511,256,61)
#include "bch128.h"
#endif

#if defined(LAC192)
//bch(511,384,29)
#include "bch192.h"
#endif

#if defined(LAC256)
//bch(1023,512,115)
#include "bch256.h"
#endif

# define DIV_ROUND_UP(a, b) ((a + b - 1) / b)
# define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*(arr)))
/*
 * represent a polynomial over GF(2^m)
 */
struct gf_poly {
	unsigned int deg;    /* polynomial degree */
	unsigned int c[0];   /* polynomial terms */
};

/* given its degree, compute a polynomial size in bytes */
#define GF_POLY_SZ(_d) (sizeof(struct gf_poly)+((_d)+1)*sizeof(unsigned int))

/* polynomial of degree 1 */
struct gf_poly_deg1 {
	struct gf_poly poly;
	unsigned int   c[2];
};
//init
int ecc_init()
{
	int i;
	//init the bch encoder
	ecc_bch.elp= malloc((ecc_bch.t+1)*sizeof(struct gf_poly_deg1));
	if(ecc_bch.elp==NULL)
	{
		return -1;
	}
	else
	{
		memset(ecc_bch.elp,0,(ecc_bch.t+1)*sizeof(struct gf_poly_deg1));
	}
	for (i = 0; i < ARRAY_SIZE(ecc_bch.poly_2t); i++)
	{
		ecc_bch.poly_2t[i] = malloc(GF_POLY_SZ(2*ecc_bch.t));
		if(ecc_bch.poly_2t[i]==NULL)
		{
			return -1;
		}
		else
		{
			memset(ecc_bch.poly_2t[i],0,GF_POLY_SZ(2*ecc_bch.t));
		}
	}
	
	return 0;
}

//free
int ecc_free()
{
	int i;
	
	if(ecc_bch.elp!=NULL)
	{
		free(ecc_bch.elp);
		ecc_bch.elp=NULL;
	}
	
	for (i = 0; i < ARRAY_SIZE(ecc_bch.poly_2t); i++)
	{
		if(ecc_bch.poly_2t[i]!=NULL)
		{
			free(ecc_bch.poly_2t[i]);
			ecc_bch.poly_2t[i]=NULL;
		}
	}
	
	return 0;
}

//error corretion encode
int ecc_enc(const unsigned char *d, unsigned char *c)
{
	unsigned char ecc[ECCBUF_LEN];
	
	//ecc init
	ecc_init();
	//init ecc to be 0 as requited by encode_bch function
	memset(ecc,0,ECCBUF_LEN);
	//encoode
	encode_bch(&ecc_bch,d,DATA_LEN,ecc);
	//copy data to the first part of code
	memcpy(c,d,DATA_LEN);
	// compy ecc to the second part of code
	memcpy(c+DATA_LEN,ecc,ECC_LEN);
	//free memory
	ecc_free();
	
	return 0;
}

//error corrction decode
int ecc_dec(unsigned char *d, const unsigned char *c)
{
	int error_num=-1;
	
	memcpy(d,c,DATA_LEN);
	
	#ifndef TEST_ROW_ERROR_RATE
	unsigned char ecc[ECCBUF_LEN];
	int i;
	unsigned int error_loc[MAX_ERROR];
	//init
	ecc_init();
	//compy correction code to ecc
	memcpy(ecc,c+DATA_LEN,ECC_LEN);
	memset(ecc+ECC_LEN,0,ECCBUF_LEN-ECC_LEN);
	//decode
	error_num=decode_bch(&ecc_bch,d,DATA_LEN,ecc,NULL,NULL,error_loc);
	//correct errors
	if(error_num>0)
    {
		for (i=0;i<error_num;i++)
        {
			if(error_loc[i]<DATA_LEN*8)
                d[(error_loc[i])/8] ^= (1 << ((error_loc[i]) % 8));
        }
    }
	//free memory
	ecc_free();
	#endif
	
	return error_num;
}