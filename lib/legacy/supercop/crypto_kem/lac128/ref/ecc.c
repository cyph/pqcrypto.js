#include "bch.h"
#include "ecc.h"
#include <string.h>

struct bch_control *ecc_bch;

//error corretion encode
int ecc_enc(const unsigned char *d, unsigned char *c)
{
	unsigned char ecc[ECCBUF_LEN];
	//init bch
	ecc_bch=init_bch(LOG_CODE_LEN,MAX_ERROR,0);
	//init ecc to be 0 as requited by encode_bch function
	memset(ecc,0,ECCBUF_LEN);
	//encoode
	encode_bch(ecc_bch,d,DATA_LEN,ecc);
	//copy data to the first part of code
	memcpy(c,d,DATA_LEN);
	// compy ecc to the second part of code
	memcpy(c+DATA_LEN,ecc,ECC_LEN);
	//free bch
	free_bch(ecc_bch);
	
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
	//init bch
	ecc_bch=init_bch(LOG_CODE_LEN,MAX_ERROR,0);
	//compy correction code to ecc
	memcpy(ecc,c+DATA_LEN,ECC_LEN);
	memset(ecc+ECC_LEN,0,ECCBUF_LEN-ECC_LEN);
	//decode
	error_num=decode_bch(ecc_bch,d,DATA_LEN,ecc,NULL,NULL,error_loc);
	//correct errors
	if(error_num>0)
    {
		for (i=0;i<error_num;i++)
        {
			if(error_loc[i]<DATA_LEN*8)
                d[(error_loc[i])/8] ^= (1 << ((error_loc[i]) % 8));
        }
    }
	//free bch
	free_bch(ecc_bch);
	#endif
	
	return error_num;
}