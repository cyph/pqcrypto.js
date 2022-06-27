#include "crypto_encrypt.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "crypt.h"
#include "rng.h"
#include "sha3.h"
#include "conv.h"

/* you may change these three parameters */
short blk = 21;
short d = 3;
short r = 7;

extern short _m;

#define CONST_LEN 5

const unsigned char Const_bytes[CONST_LEN] = { 0xD9, 0xA5, 0xEE, 0xA5, 0x2E };


void free_LRP(struct LRP *H)
{
	short i;
	short n,k,nkd;

	n = blk * 4;
	k = blk * 2;
	nkd = (n - k) * d;
	
	for( i=0 ; i<nkd ; i++ ) free(H->bin[i].data);
	free(H->bin);
	free(H->basis);
	free_matrix(&H->M);
}

/* construct a random 4-quasi LRPC ( 8 blocks of circulant ) */
struct LRP gen_4Q(void)
{
	short i,j,t,info;
	short n,k,nk,nkd;
	short blk2,blk3,blk4;
	struct LRP H;
	Bvec *dec;

	n = blk * 4;
	k = blk * 2;
	nk = n - k;
	nkd = nk * d;

	blk2 = blk * 2;
	blk3 = blk * 3;
	blk4 = blk * 4;

	H.M = gen_matrix(nk,n);
	H.basis = gen_indep_vec(d);
	H.bin = (Bvec *)malloc(nkd * sizeof(*H.bin));
	dec = (Bvec *)malloc(nkd * sizeof(*dec));

	while( 1 )
	{
		for( i=0 ; i<nkd ; i+=blk )
		{
			H.bin[i] = gen_vec_rand(n);

			for( j=1 ; j<blk ; j++ )
			{
				H.bin[i+j] = gen_vec_copy(H.bin[i+j-1]);
				Shift(&H.bin[i+j]);

				put_elt(&H.bin[i+j],0,get_elt(H.bin[i+j-1],blk-1));
				put_elt(&H.bin[i+j],blk,get_elt(H.bin[i+j-1],blk2-1));
				put_elt(&H.bin[i+j],blk2,get_elt(H.bin[i+j-1],blk3-1));
				put_elt(&H.bin[i+j],blk3,get_elt(H.bin[i+j-1],blk4-1));
			}
		}

		/* check the rank of decoding matrix */
		for( i=0 ; i<nkd ; i++ ) dec[i] = gen_vec_copy(H.bin[i]);
		gauss_elm(dec,nkd,n);

		for( i=0, info=1 ; i<nkd && i<n ; i++ )
		  if( !get_elt(dec[i],i) ) { info = 0; break; }

		for( i=0 ; i<nkd ; i++ ) free(dec[i].data);

		if( !info )
		{
			for( i=0 ; i<nkd ; i++ ) free(H.bin[i].data);
			continue;
		}

		/* recover matrix H from H.bin and H.basis */
		for( i=0 ; i<nk ; i++ )
		  for( j=0 ; j<n ; j++ )
			H.M.dat[i][j] = 0;

		for( t=0 ; t<d ; t++ )
		  for( i=0 ; i<nk ; i++ )
			for( j=0 ; j<n ; j++ )
			  if( get_elt(H.bin[t*nk+i],j) )
				H.M.dat[i][j] ^= H.basis[t];

		/* check the rank of parity check matrix */
		if( mat_rank(H.M)==nk ) break;

		for( i=0 ; i<nkd ; i++ ) free(H.bin[i].data);
	}

	free(dec);

	return H;
}


int crypto_encrypt_keypair(unsigned char *pk, unsigned char *sk)
{
	short i,j,t,info;
	short n,k,l,nk,nkd,bh;
	Gmat G0,F,F0,S,St,Sinv;
	struct LRP H;

	long gf_size,bin_size,pk_size,sk_size;
	uint64 *bin_dat;
	gf *gf_dat;
	short blk2,blk3,blk4;

	n = blk * 4;
	k = blk * 2;
	l = blk * 3;
	nk = n - k;
	nkd = nk * d;

	blk2 = blk * 2;
	blk3 = blk * 3;
	blk4 = blk * 4;

	/* improper parameter error */
	if( r*d >= nk )    return -1;
	if( r*(d+1) >= _m) return -1;

	while( 2 )
	{
		H = gen_4Q();

		/* construct a random code (generator matrix) */
		G0 = gen_matrix_id(l,n);
		gf_dat = gen_rand_vec(blk3);

		for( i=0 ; i<blk ; i++ )
		{
			G0.dat[0][l+i] = gf_dat[i];
			G0.dat[blk][l+i] = gf_dat[blk+i];
			G0.dat[blk2][l+i] = gf_dat[blk2+i];
		}

		for( i=1 ; i<blk ; i++ ) /* circulant */
		{
			G0.dat[i][l] = G0.dat[i-1][n-1];
			G0.dat[blk+i][l] = G0.dat[blk+i-1][n-1];
			G0.dat[blk2+i][l] = G0.dat[blk2+i-1][n-1];

			for( j=l+1 ; j<n ; j++ )
			{
				G0.dat[i][j] = G0.dat[i-1][j-1];
				G0.dat[blk+i][j] = G0.dat[blk+i-1][j-1];
				G0.dat[blk2+i][j] = G0.dat[blk2+i-1][j-1];
			}
		}

		free(gf_dat);

		/* compute F = G' * H^T (6 blocks of circulant) */
		F = gen_matrix(l,nk);

		for( i=0 ; i<l ; i+=blk )
		  for( j=0 ; j<nk ; j++ )
			for( t=0, F.dat[i][j]=0 ; t<n ; t++ )
			  F.dat[i][j] ^= gf_mul(G0.dat[i][t],H.M.dat[j][t]);

		for( i=1 ; i<blk ; i++ )
		{
			F.dat[i][0] = F.dat[i-1][blk-1];
			F.dat[i][blk] = F.dat[i-1][nk-1];
			F.dat[blk+i][0] = F.dat[blk+i-1][blk-1];
			F.dat[blk+i][blk] = F.dat[blk+i-1][nk-1];
			F.dat[blk2+i][0] = F.dat[blk2+i-1][blk-1];
			F.dat[blk2+i][blk] = F.dat[blk2+i-1][nk-1];

			for( j=1 ; j<blk ; j++ )
			{
				F.dat[i][j] = F.dat[i-1][j-1];
				F.dat[i][blk+j] = F.dat[i-1][blk+j-1];
				F.dat[blk+i][j] = F.dat[blk+i-1][j-1];
				F.dat[blk+i][blk+j] = F.dat[blk+i-1][blk+j-1];
				F.dat[blk2+i][j] = F.dat[blk2+i-1][j-1];
				F.dat[blk2+i][blk+j] = F.dat[blk2+i-1][blk+j-1];
			}
		}

		/* generate a random scrambling matrix */
		S = gen_matrix(nk,nk);
		gf_dat = gen_rand_vec(blk4);

		for( j=0 ; j<nk ; j++ )
		{
			S.dat[0][j] = gf_dat[j];
			S.dat[blk][j] = gf_dat[blk2+j];
		}

		for( i=1 ; i<blk ; i++ ) /* circulant */
		{
			S.dat[i][0] = S.dat[i-1][blk-1];
			S.dat[i][blk] = S.dat[i-1][blk2-1];
			S.dat[blk+i][0] = S.dat[blk+i-1][blk-1];
			S.dat[blk+i][blk] = S.dat[blk+i-1][blk2-1];

			for( j=1 ; j<blk ; j++ )
			{
				S.dat[i][j] = S.dat[i-1][j-1];
				S.dat[i][blk+j] = S.dat[i-1][blk+j-1];
				S.dat[blk+i][j] = S.dat[blk+i-1][j-1];
				S.dat[blk+i][blk+j] = S.dat[blk+i-1][blk+j-1];
			}
		}

		free(gf_dat);


		F0 = mat_mul_t(S,F);
		gf_gauss_elm2(&F0,&S);


		/* check the rank of F */
		for( i=0, info=1 ; i<nk ; i++ )
		  if( !F0.dat[i][i] ) { info=0 ; break; }

		if( !info )
		{
			free_LRP(&H);
			free_matrix(&G0);
			free_matrix(&F);
			free_matrix(&F0);
			free_matrix(&S);
			continue;
		}

		/* St = S^T */
		St = gen_matrix(nk,nk);
		for( i=0 ; i<nk ; i++ )
		  for( j=0 ; j<nk ; j++ )
			St.dat[j][i] = S.dat[i][j];

		/* if inverse does not exist, then reconstruct key */
		if( !mat_inv(St,&Sinv) )
		{
			free_LRP(&H);
			free_matrix(&G0);
			free_matrix(&F);
			free_matrix(&F0);
			free_matrix(&S);
			free_matrix(&St);
			continue;
		}

		for( i=0 ; i<l ; i++ )
		  for( j=0 ; j<nk ; j++ )
			F.dat[i][j] = F0.dat[j][i];

		free_matrix(&F0);
		free_matrix(&St);
		free_matrix(&S);
		break;
	}


	/* save public key */
	gf_size = blk * 5;
	gf_dat = (gf *)malloc(gf_size * sizeof(*gf_dat));

	for( i=0 ; i<blk ; i++ )
	{
		gf_dat[i] = G0.dat[0][l+i];
		gf_dat[blk+i] = G0.dat[blk][l+i];
		gf_dat[blk2+i] = G0.dat[blk2][l+i];
		gf_dat[blk3+i] = F.dat[nk][i];
		gf_dat[blk4+i] = F.dat[nk][blk+i];
	}

	bin_dat = gf2bin(gf_dat,gf_size,&bin_size);
	pk_size = (_m * blk * 5 + 7)/8;
	memcpy(pk,bin_dat,pk_size);

	free(bin_dat); free(gf_dat);


	/* save secret key */
	gf_size = blk * 4 + d;
	gf_dat = (gf *)malloc(gf_size * sizeof(*gf_dat));

	for( i=0 ; i<nk ; i++ )
	{
		gf_dat[i] = Sinv.dat[0][i];
		gf_dat[nk+i] = Sinv.dat[blk][i];
	}

	for( i=0 ; i<d ; i++ )
		gf_dat[n+i] = H.basis[i];

	bin_dat = gf2bin(gf_dat,gf_size,&bin_size);
	bh = (blk + 1) / 2;
	sk_size = (_m * gf_size + 7) / 8;
	memcpy(sk,bin_dat,sk_size);

	for( i=j=0 ; i<nkd ; i+=blk, j+=bh )
		memcpy(sk + sk_size + j, H.bin[i].data, bh);

	free(bin_dat); free(gf_dat);


	/* delete allocated memory */
	free_LRP(&H);
	free_matrix(&G0); free_matrix(&F); free_matrix(&Sinv);

	return 0;
}



void encrypt_one_block(
	unsigned char *c, const unsigned char *m, const unsigned char *pk)
{
	short i,j;
	short n,k,l,nk;
	unsigned long lin,flag;
	long bin_len;
	uint64 *bin_dat;
	size_t cipher_len;
	gf *gf_dat,*gf_msg,*cipher;
	gf *er_basis;  /* basis of error vector */
	gf *er;        /* error vector */
	Gmat G0,F;
	short blk2,blk3;

	n = blk * 4;
	k = blk * 2;
	l = blk * 3;
	nk = n - k;

	blk2 = blk * 2;
	blk3 = blk * 3;

	/* load public key */
	gf_dat = bin2gf(pk,blk*5);

	G0 = gen_matrix_id(l,n);

	for( j=0 ; j<blk ; j++ )
	{
		G0.dat[0][l+j] = gf_dat[j];
		G0.dat[blk][l+j] = gf_dat[blk+j];
		G0.dat[blk2][l+j] = gf_dat[blk2+j];
	}

	for( i=1 ; i<blk ; i++ ) /* circulant */
	{
		G0.dat[i][l] = G0.dat[i-1][n-1];
		G0.dat[blk+i][l] = G0.dat[blk+i-1][n-1];
		G0.dat[blk2+i][l] = G0.dat[blk2+i-1][n-1];

		for( j=l+1 ; j<n ; j++ )
		{
			G0.dat[i][j] = G0.dat[i-1][j-1];
			G0.dat[blk+i][j] = G0.dat[blk+i-1][j-1];
			G0.dat[blk2+i][j] = G0.dat[blk2+i-1][j-1];
		}
	}

	F = gen_matrix_id(l,nk);

	for( j=0 ; j<nk ; j++ )
	  F.dat[nk][j] = gf_dat[blk3+j];

	for( i=nk+1 ; i<l ; i++ )  /* circulant */
	{
		F.dat[i][0] = F.dat[i-1][blk-1];
		F.dat[i][blk] = F.dat[i-1][nk-1];

		for( j=1 ; j<blk ; j++ )
		{
			F.dat[i][j] = F.dat[i-1][j-1];
			F.dat[i][blk+j] = F.dat[i-1][blk+j-1];
		}
	}
	
	/* load message */
	gf_msg = bin2gf(m,l);

	/* generate an error vector with rank metric r */
	er_basis = gen_indep_vec(r);
	er = (gf *)malloc(n * sizeof(*er));
	flag = (1UL << r) - 1;

	for( i=0 ; i<n ; i++ )
	{
		xrandombytes((unsigned char *)&lin,4);
		lin &= flag;

		for( j=0, er[i]=0 ; j<r ; j++, lin>>=1 )
		  if( lin & 1 )
			er[i] ^= er_basis[j];
	}

	/* cipher = ( c_1 | c_2 ) */
	cipher = (gf *)malloc((n + nk) * sizeof(*cipher));
	
	/* compute c_1 =  m * G' + e */
	for( j=0 ; j<n ; j++ )
	  for( i=0, cipher[j]=0 ; i<l ; i++ )
		cipher[j] ^= gf_mul(gf_msg[i],G0.dat[i][j]);

	for( j=0 ; j<n ; j++ )
		cipher[j] ^= er[j];

	/* copmute c_2 = m * F */
	for( j=0 ; j<nk ; j++ )
	  for( i=0, cipher[n+j]=0 ; i<l ; i++ )
		cipher[n+j] ^= gf_mul(gf_msg[i],F.dat[i][j]);


	/* convert gf to binary */
	bin_dat = gf2bin(cipher,n+nk,&bin_len);

	cipher_len = (_m * blk * 6 + 7) / 8;
	memcpy(c,bin_dat,cipher_len);

	/* delete allocated memory */
	free_matrix(&G0); free_matrix(&F);
	free(gf_dat); free(bin_dat);
	free(gf_msg); free(cipher);
	free(er_basis); free(er);
}


void encrypt_one_block_given_error(
	unsigned char *c, const unsigned char *m, const unsigned char *pk, gf *er)
{
	short i,j;
	short n,k,l,nk;
	long bin_len;
	uint64 *bin_dat;
	size_t cipher_len;
	gf *gf_dat,*gf_msg,*cipher;
	Gmat G0,F;
	short blk2,blk3;

	n = blk * 4;
	k = blk * 2;
	l = blk * 3;
	nk = n - k;

	blk2 = blk * 2;
	blk3 = blk * 3;

	/* load public key */
	gf_dat = bin2gf(pk,blk*5);

	G0 = gen_matrix_id(l,n);

	for( j=0 ; j<blk ; j++ )
	{
		G0.dat[0][l+j] = gf_dat[j];
		G0.dat[blk][l+j] = gf_dat[blk+j];
		G0.dat[blk2][l+j] = gf_dat[blk2+j];
	}

	for( i=1 ; i<blk ; i++ ) /* circulant */
	{
		G0.dat[i][l] = G0.dat[i-1][n-1];
		G0.dat[blk+i][l] = G0.dat[blk+i-1][n-1];
		G0.dat[blk2+i][l] = G0.dat[blk2+i-1][n-1];

		for( j=l+1 ; j<n ; j++ )
		{
			G0.dat[i][j] = G0.dat[i-1][j-1];
			G0.dat[blk+i][j] = G0.dat[blk+i-1][j-1];
			G0.dat[blk2+i][j] = G0.dat[blk2+i-1][j-1];
		}
	}

	F = gen_matrix_id(l,nk);

	for( j=0 ; j<nk ; j++ )
	  F.dat[nk][j] = gf_dat[blk3+j];

	for( i=nk+1 ; i<l ; i++ )  /* circulant */
	{
		F.dat[i][0] = F.dat[i-1][blk-1];
		F.dat[i][blk] = F.dat[i-1][nk-1];

		for( j=1 ; j<blk ; j++ )
		{
			F.dat[i][j] = F.dat[i-1][j-1];
			F.dat[i][blk+j] = F.dat[i-1][blk+j-1];
		}
	}
	
	/* load message */
	gf_msg = bin2gf(m,l);

	/* cipher = ( c_1 | c_2 ) */
	cipher = (gf *)malloc((n + nk) * sizeof(*cipher));
	
	/* compute c_1 =  m * G' + e */
	for( j=0 ; j<n ; j++ )
	  for( i=0, cipher[j]=0 ; i<l ; i++ )
		cipher[j] ^= gf_mul(gf_msg[i],G0.dat[i][j]);

	for( j=0 ; j<n ; j++ )
		cipher[j] ^= er[j];

	/* copmute c_2 = m * F */
	for( j=0 ; j<nk ; j++ )
	  for( i=0, cipher[n+j]=0 ; i<l ; i++ )
		cipher[n+j] ^= gf_mul(gf_msg[i],F.dat[i][j]);


	/* convert gf to binary */
	bin_dat = gf2bin(cipher,n+nk,&bin_len);

	cipher_len = (_m * blk * 6 + 7) / 8;
	memcpy(c,bin_dat,cipher_len);

	/* delete allocated memory */
	free_matrix(&G0); free_matrix(&F);
	free(gf_dat); free(bin_dat);
	free(gf_msg); free(cipher);
}




int decrypt_one_block(
	unsigned char *m, const unsigned char *c, const unsigned char *sk)
{
	int result = 0;
	short i,j,t;
	short n,k,l,nk,nkd,bh;
	long gf_size,bin_size;
	uint64 *bin_dat;
	gf *gf_dat;
	gf *msg,*syn,*c3,*ee;
	Gmat Sinv;
	struct LRP H;
	short blk2,blk3,blk4;

	n = blk * 4;
	k = blk * 2;
	l = blk * 3;
	nk = n - k;
	nkd = nk * d;
	bh = (blk + 1) >> 1;

	blk2 = blk * 2;
	blk3 = blk * 3;
	blk4 = blk * 4;


	gf_size = blk * 4 + d;
	bin_size = (_m * gf_size + 7)/8;

	gf_dat = bin2gf(sk,gf_size);

	Sinv = gen_matrix(nk,nk);
		
	/* recover S^-1*/
	for( i=0 ; i<nk ; i++ )
	{
		Sinv.dat[0][i] = gf_dat[i];
		Sinv.dat[blk][i] = gf_dat[nk+i];
	}

	for( i=1 ; i<blk ; i++ )
	{
		Sinv.dat[i][0] = Sinv.dat[i-1][blk-1];
		Sinv.dat[i][blk] = Sinv.dat[i-1][nk-1];
		Sinv.dat[blk+i][0] = Sinv.dat[blk+i-1][blk-1];
		Sinv.dat[blk+i][blk] = Sinv.dat[blk+i-1][nk-1];
			
		for( j=1 ; j<blk ; j++ )
		{
			Sinv.dat[i][j] = Sinv.dat[i-1][j-1];
			Sinv.dat[i][blk+j] = Sinv.dat[i-1][blk+j-1];
			Sinv.dat[blk+i][j] = Sinv.dat[blk+i-1][j-1];
			Sinv.dat[blk+i][blk+j] = Sinv.dat[blk+i-1][blk+j-1];
		}
	}

	/* recover parity check matrix (8 blocks of circulant) */
	H.M = gen_matrix(nk,n);
	H.basis = (gf *)malloc(d * sizeof(*H.basis));
	H.bin = (Bvec *)malloc(nkd * sizeof(*H.bin));

	for( i=0 ; i<d ; i++ )
	  H.basis[i] = gf_dat[blk4+i];

	free(gf_dat);

	for( i=j=0 ; i<nkd ; i+=blk,j+=bh )
	{
		H.bin[i] = gen_vec_zero(n);		
		memcpy(H.bin[i].data, sk + bin_size + j, bh);
	}

	for( i=0 ; i<nkd ; i+=blk )
	{
		for( j=1 ; j<blk ; j++ )
		{
			H.bin[i+j] = gen_vec_copy(H.bin[i+j-1]);
			Shift(&H.bin[i+j]);

			put_elt(&H.bin[i+j],0,get_elt(H.bin[i+j-1],blk-1));
			put_elt(&H.bin[i+j],blk,get_elt(H.bin[i+j-1],blk2-1));
			put_elt(&H.bin[i+j],blk2,get_elt(H.bin[i+j-1],blk3-1));
			put_elt(&H.bin[i+j],blk3,get_elt(H.bin[i+j-1],blk4-1));
		}
	}

	H.M = gen_matrix(nk,n);

	for( i=0 ; i<nk ; i++ )
	  for( j=0 ; j<n ; j++ )
		H.M.dat[i][j] = 0;

	for( t=0 ; t<d ; t++ )
	  for( i=0 ; i<nk ; i++ )
		for( j=0 ; j<n ; j++ )
		  if( get_elt(H.bin[t*nk+i],j) )
			H.M.dat[i][j] ^= H.basis[t];

	/* load ciphertext */
	gf_dat = bin2gf(c,n+nk);

	/* compute c1 * H^T */
	syn = (gf *)malloc(nk * sizeof(*syn));
	for( i=0 ; i<nk ; i++ )
	  for( j=0, syn[i]=0 ; j<n ; j++ )
		syn[i] ^= gf_mul(H.M.dat[i][j],gf_dat[j]);

	/* compute c * S^(-1) */
	c3 = (gf *)malloc(nk * sizeof(*c3));
	for( j=0 ; j<nk ; j++ )
	  for( i=0,c3[j]=0 ; i<nk ; i++ )
		  c3[j] ^= gf_mul(gf_dat[n+i],Sinv.dat[i][j]);

	/* compute syn = c1 * H^T - c2 * S^(-1)  */
	for( i=0 ; i<nk ; i++ ) syn[i] ^= c3[i];

	if( bin_rank(syn,nk) < r*d )
	{
		result = -1;
		goto end_lable;
	}
	
	ee = syndrome_decode(syn,&H,r);
	if( ee==NULL ) 
	{
		result = -2;
		goto end_lable;
	}

	msg = (gf *)malloc(l * sizeof(*msg));
	for( i=0 ; i<l ; i++ ) msg[i] = gf_dat[i] ^= ee[i];

	/* convert gf to binary */
	bin_dat = gf2bin(msg,l,&bin_size);
	bin_size = _m * l / 8;
	memcpy(m,bin_dat,bin_size);

	/* delete allocated memory */
	free(ee); free(msg); free(bin_dat);

end_lable:
	free(syn); free(c3); free(gf_dat); 
	free_LRP(&H);

	return result;
}


int decrypt_one_block_return_error(
	unsigned char *m, const unsigned char *c, const unsigned char *sk, gf *er)
{
	int result = 0;
	short i,j,t;
	short n,k,l,nk,nkd,bh;
	long gf_size,bin_size;
	uint64 *bin_dat;
	gf *gf_dat;
	gf *msg,*syn,*c3,*ee;
	Gmat Sinv;
	struct LRP H;
	short blk2,blk3,blk4;

	n = blk * 4;
	k = blk * 2;
	l = blk * 3;
	nk = n - k;
	nkd = nk * d;
	bh = (blk + 1) >> 1;

	blk2 = blk * 2;
	blk3 = blk * 3;
	blk4 = blk * 4;


	gf_size = blk * 4 + d;
	bin_size = (_m * gf_size + 7)/8;

	gf_dat = bin2gf(sk,gf_size);

	Sinv = gen_matrix(nk,nk);
		
	/* recover S^-1*/
	for( i=0 ; i<nk ; i++ )
	{
		Sinv.dat[0][i] = gf_dat[i];
		Sinv.dat[blk][i] = gf_dat[nk+i];
	}

	for( i=1 ; i<blk ; i++ )
	{
		Sinv.dat[i][0] = Sinv.dat[i-1][blk-1];
		Sinv.dat[i][blk] = Sinv.dat[i-1][nk-1];
		Sinv.dat[blk+i][0] = Sinv.dat[blk+i-1][blk-1];
		Sinv.dat[blk+i][blk] = Sinv.dat[blk+i-1][nk-1];
			
		for( j=1 ; j<blk ; j++ )
		{
			Sinv.dat[i][j] = Sinv.dat[i-1][j-1];
			Sinv.dat[i][blk+j] = Sinv.dat[i-1][blk+j-1];
			Sinv.dat[blk+i][j] = Sinv.dat[blk+i-1][j-1];
			Sinv.dat[blk+i][blk+j] = Sinv.dat[blk+i-1][blk+j-1];
		}
	}

	/* recover parity check matrix (8 blocks of circulant) */
	H.M = gen_matrix(nk,n);
	H.basis = (gf *)malloc(d * sizeof(*H.basis));
	H.bin = (Bvec *)malloc(nkd * sizeof(*H.bin));

	for( i=0 ; i<d ; i++ )
	  H.basis[i] = gf_dat[blk4+i];

	free(gf_dat);

	for( i=j=0 ; i<nkd ; i+=blk,j+=bh )
	{
		H.bin[i] = gen_vec_zero(n);		
		memcpy(H.bin[i].data, sk + bin_size + j, bh);
	}

	for( i=0 ; i<nkd ; i+=blk )
	{
		for( j=1 ; j<blk ; j++ )
		{
			H.bin[i+j] = gen_vec_copy(H.bin[i+j-1]);
			Shift(&H.bin[i+j]);

			put_elt(&H.bin[i+j],0,get_elt(H.bin[i+j-1],blk-1));
			put_elt(&H.bin[i+j],blk,get_elt(H.bin[i+j-1],blk2-1));
			put_elt(&H.bin[i+j],blk2,get_elt(H.bin[i+j-1],blk3-1));
			put_elt(&H.bin[i+j],blk3,get_elt(H.bin[i+j-1],blk4-1));
		}
	}

	H.M = gen_matrix(nk,n);

	for( i=0 ; i<nk ; i++ )
	  for( j=0 ; j<n ; j++ )
		H.M.dat[i][j] = 0;

	for( t=0 ; t<d ; t++ )
	  for( i=0 ; i<nk ; i++ )
		for( j=0 ; j<n ; j++ )
		  if( get_elt(H.bin[t*nk+i],j) )
			H.M.dat[i][j] ^= H.basis[t];

	/* load ciphertext */
	gf_dat = bin2gf(c,n+nk);

	/* compute c1 * H^T */
	syn = (gf *)malloc(nk * sizeof(*syn));
	for( i=0 ; i<nk ; i++ )
	  for( j=0, syn[i]=0 ; j<n ; j++ )
		syn[i] ^= gf_mul(H.M.dat[i][j],gf_dat[j]);

	/* compute c * S^(-1) */
	c3 = (gf *)malloc(nk * sizeof(*c3));
	for( j=0 ; j<nk ; j++ )
	  for( i=0,c3[j]=0 ; i<nk ; i++ )
		  c3[j] ^= gf_mul(gf_dat[n+i],Sinv.dat[i][j]);

	/* compute syn = c1 * H^T - c2 * S^(-1)  */
	for( i=0 ; i<nk ; i++ ) syn[i] ^= c3[i];

	if( bin_rank(syn,nk) < r*d )
	{
		result = -1;
		goto end_lable;
	}
	
	ee = syndrome_decode(syn,&H,r);
	if( ee==NULL ) 
	{
		result = -2;
		goto end_lable;
	}

	msg = (gf *)malloc(l * sizeof(*msg));
	for( i=0 ; i<l ; i++ ) msg[i] = gf_dat[i] ^= ee[i];

	/* copy error vector */
	memcpy(er,ee,n<<3);

	/* convert gf to binary */
	bin_dat = gf2bin(msg,l,&bin_size);
	bin_size = _m * l / 8;
	memcpy(m,bin_dat,bin_size);

	/* delete allocated memory */
	free(ee); free(msg); free(bin_dat);

end_lable:
	free(syn); free(c3); free(gf_dat); 
	free_LRP(&H);

	return result;
}



void encrypt_one_block_INDCCA2(unsigned char *c, const unsigned char *m, const unsigned char *pk)
{
	int i;
	short n,l,k;
	unsigned long long msg_len,cip_len,er_dat_len,y_len;
	unsigned char *y0;
	unsigned char rand_seed[48];
	unsigned char *rand_value;
	gf *error;
	SHA3_CTX sha_ctx;

	n = blk*4;
	l = blk*3;
	k = blk*2;

	/* length of one block message, ciphertext, error data */
	er_dat_len = r*(r-1)/2 + r*(n-r) + r*(_m-r);
	er_dat_len >>= 3;
	msg_len = (_m * l) >> 3;
	msg_len += er_dat_len - 48;           /* hash byte : 48 */
	cip_len = (_m * (n*2 - k) + 7) >> 3;
	cip_len += CONST_LEN;                 /* y5 byte */

	randombytes(rand_seed,48);   /* random vector of length 384 bit */

	/* generate the random value from given seed */
	xrandombytes_init(rand_seed,NULL,48);
	rand_value = (unsigned char *)malloc((size_t)msg_len+CONST_LEN);
	xrandombytes(rand_value,msg_len+CONST_LEN);

	/* y0 = ( y2 || y1 ) */
	y_len = msg_len + CONST_LEN + 48;
	y0 = (unsigned char *)malloc((size_t)y_len + 1);  /* more 1 byte for remaining bits */
	y0[y_len] = 0;

	/* y_1 = rand_value + ( msg || const ) */
	memcpy(y0+48,m,(size_t)msg_len);
	memcpy(y0+48+msg_len,Const_bytes,CONST_LEN);
	for( i=0 ; i<msg_len+CONST_LEN ; i++ ) y0[i+48] ^= rand_value[i];

	/* y_2 = rand_seed + hash(y1)	*/
	SHA3_Init(&sha_ctx,SHA3_384);
	SHA3_Update(&sha_ctx,y0+48,(size_t)msg_len+CONST_LEN);
	SHA3_Final(y0,&sha_ctx);
	for( i=0 ; i<48 ; i++ ) y0[i] ^= rand_seed[i];

	/* construct the error vector using conversion function */
	error = (gf *)malloc(n * sizeof(*error));
	conv(y0+CONST_LEN,n,r,error);

	encrypt_one_block_given_error(c+CONST_LEN, y0+CONST_LEN+er_dat_len, pk, error);
	memcpy(c,y0,CONST_LEN);   /* copy y5 data */

	free(rand_value);
	free(y0); free(error);
}


int decrypt_one_block_INDCCA2(unsigned char *m, const unsigned char *c, const unsigned char *sk)
{
	int i;
	int rst;
	short n,l,k;
	unsigned long long msg_len,cip_len,er_dat_len;
	unsigned char *y0,y2[48];
	unsigned char *er_dat;
	unsigned char rand_seed[48];
	unsigned char *rand_value;
	gf *error;
	SHA3_CTX sha_ctx;

	n = blk*4;
	l = blk*3;
	k = blk*2;

	/* length of one block message, ciphertext, error data */
	er_dat_len = r*(r-1)/2 + r*(n-r) + r*(_m-r);
	er_dat_len >>= 3;
	msg_len = (_m * l) >> 3;
	msg_len += er_dat_len - 48;           /* hash byte : 48 */
	cip_len = (_m * (n*2 - k) + 7) >> 3;
	cip_len += CONST_LEN;                 /* y5 byte */

	y0 = (unsigned char *)malloc((size_t)msg_len+CONST_LEN+48);
	er_dat = (unsigned char *)malloc((size_t)er_dat_len+1);  /* more 1 byte for remaining bits */
	error = (gf *)malloc(n * sizeof(*error));
	
	memcpy(y0,c,CONST_LEN);  /* copy y5 data */
	
	rst = decrypt_one_block_return_error(y0+CONST_LEN+er_dat_len, c+CONST_LEN, sk, error);
	if( rst!=0 )
	{
		free(y0); free(er_dat); free(error);
		return rst;
	}

	inv_conv(error,n,r,er_dat);
	memcpy(y0+CONST_LEN,er_dat,(size_t)er_dat_len);  /* copy y4 data */

	/* hash(y1) */
	SHA3_Init(&sha_ctx,SHA3_384);
	SHA3_Update(&sha_ctx,y0+48,(size_t)msg_len+CONST_LEN);  /* y0 = ( y2 || y1 ) */
	SHA3_Final(y2,&sha_ctx);

	/* recover rand_seed and random_value */
	for( i=0 ; i<48 ; i++ ) rand_seed[i] = y2[i] ^ y0[i];

	xrandombytes_init(rand_seed,NULL,48);
	rand_value = (unsigned char *)malloc((size_t)msg_len+CONST_LEN);
	xrandombytes(rand_value,msg_len+CONST_LEN);

	/* recover ( m || const ) using rand_value */
	for( i=0 ; i<msg_len+CONST_LEN ; i++ ) y0[i+48] ^= rand_value[i];

	/* const bytes check */
	if( memcmp(y0+msg_len+48,Const_bytes,CONST_LEN) )
	{
		free(y0); free(er_dat); free(error);
		free(rand_value);
		return -4;
	}

	memcpy(m,y0+48,(size_t)msg_len);

	free(rand_value);
	free(y0); free(er_dat); free(error);

	return 0;
}





/* encrypt (arbitrary length message) */
int crypto_encrypt(
	unsigned char *c, unsigned long long *clen,
	const unsigned char *m, const unsigned long long mlen,
	const unsigned char *pk)
{
	short n,l,k;
	unsigned char *msg;
	unsigned long long i;
	unsigned long long msg_len,cip_len,er_dat_len;
	unsigned long long num_block,res_bytes,real_mlen;

	n = blk*4;  l = blk*3;  k = blk*2;

	/* length of one block message, ciphertext, error data */
	er_dat_len = r*(r-1)/2 + r*(n-r) + r*(_m-r);
	er_dat_len >>= 3;
	msg_len = (_m * l) >> 3;
	msg_len += er_dat_len - 48;           /* hash byte : 48 */
	cip_len = (_m * (n*2 - k) + 7) >> 3;
	cip_len += CONST_LEN;                 /* y5 byte */

	num_block = (mlen + 4 + msg_len - 1) / msg_len;
	res_bytes = (mlen + 4) % msg_len;
	real_mlen = msg_len * num_block;

	msg = (unsigned char *)malloc((size_t)real_mlen);

	memcpy(msg,&mlen,4);
	memcpy(msg+4,m,(size_t)mlen);

	/* if the length of message is not multiple of block length, 
	   then fill the extra bytes with random value */
	if( real_mlen > mlen+4 )
		xrandombytes(msg + 4 + mlen, real_mlen - mlen - 4);

	for( i=0 ; i<num_block ; i++ )
		encrypt_one_block_INDCCA2(c + cip_len * i, msg + msg_len * i, pk);

	*clen = cip_len * num_block;

	free(msg);

	return 0;
}




/* decrypt */
int crypto_encrypt_open(
	unsigned char *m, unsigned long long *mlen,
	const unsigned char *c, unsigned long long clen,
	const unsigned char *sk)
{
	short n,l,k;
	int result;
	unsigned long long msg_len,cip_len,er_dat_len;
	unsigned long long num_block;
	unsigned long long i;
	unsigned char *tmp_msg;
	uint32_t *ptr32;

	n = blk*4;  l = blk*3;  k = blk*2;

	/* length of one block message, ciphertext, error data */
	er_dat_len = r*(r-1)/2 + r*(n-r) + r*(_m-r);
	er_dat_len >>= 3;
	msg_len = (_m * l) >> 3;
	msg_len += er_dat_len - 48;           /* hash byte : 48 */
	cip_len = (_m * (n*2 - k) + 7) >> 3;
	cip_len += CONST_LEN;                 /* y5 byte */

	/* clen should be multiple of cip_len  */
	if( clen % cip_len ) return -3;

	num_block = clen / cip_len;

	tmp_msg = (unsigned char *)malloc((size_t)(msg_len * num_block));

	for( i=0 ; i<num_block ; i++ )
	{
		result = decrypt_one_block_INDCCA2(tmp_msg + msg_len * i, c + cip_len * i, sk);

		if( result ) { free(tmp_msg); return result; }
	}

	ptr32 = (uint32_t *)tmp_msg;
	*mlen = *ptr32;  /* length of original message */

	/* message length error */
	if( *mlen > msg_len * num_block ) { free(tmp_msg); return -3; }

	memcpy(m,tmp_msg+4,(size_t)*mlen);  /* original message data */
	
	return 0;
}



/* find the basis of dual space of the space generated by given n vectors
   *r = (rank of dual space) */
gf *dual_space(gf *x, short n, short *rank)
{
	short i,j,t,tt;
	short info[64],other[64];
	gf flag;
	gf *y,*dual;

	/* copy whole vectors */
	y = (gf *)malloc( n * sizeof(*y) );
	for( i=0 ; i<n ; i++ ) y[i] = x[i];

	Gauss_elimination(y,n,_m);

	for( i=n-1 ; i>=0 ; i-- )
	  if( y[i] ) break;

	n = i+1;  /* n = (rank of the matrix) */

	*rank = _m - n;

	/* find the information set (colum) */
	t = tt = 0;
	flag = 1;
	for( i=0,j=0 ; i<n ; j++,flag<<=1 )
		if( y[i] & flag ) { info[t++] = j; i++; }
		else                other[tt++] = j;

	for( ; j<_m ; j++ ) other[tt++] = j;

	/* memory for the basis of dual space */
	dual = (gf *)malloc( (*rank) * sizeof(*dual));
	
	for( i=0 ; i<*rank ; i++ ) dual[i] = 0;
	

	for( j=0 ; j<*rank ; j++ )
	  for( i=0 ; i<n ; i++ )
		if( vec_get_elt(y[i],other[j]) )
		  vec_put_elt(&dual[j],info[i],1);

	for( i=0 ; i<*rank ; i++ )
	  vec_put_elt(&dual[i],other[i],1);
	  
	free(y);

	return dual;
}


/* intersection of two vector spaces V,W 
   find a basis of the space */
gf *vec_sp_inter(gf *V, short Vdim, gf *W, short Wdim, short *rank)
{
	short i;
	short Vnul,Wnul;
	gf *Vpub,*Wpub;
	gf *sum,*dual;

	Vpub = dual_space(V,Vdim,&Vnul);
	Wpub = dual_space(W,Wdim,&Wnul);

	/* Vpub + Wpub */

	sum = (gf *)malloc((Vnul+Wnul) * sizeof(*sum));

	for( i=0 ; i<Vnul ; i++ ) sum[i] = Vpub[i];
	for( i=0 ; i<Wnul ; i++ ) sum[Vnul+i] = Wpub[i];

	free(Vpub); free(Wpub);

	dual = dual_space(sum,Vnul+Wnul,rank);

	free(sum);

	return dual;
}

/* find error space using a syndrome vector with length N and the basis of parity check matrix */
gf *error_space(gf *synd, short N, gf *F, short *rank)
{
	short i,j;
	short rr,rr2;
	gf inv;
	gf **Fs;
	gf *er,*er2;

	/* S_i^(-1) F */
	Fs = (gf **)malloc(d * sizeof(*Fs));
	for( i=0 ; i<d ; i++ ) 
		Fs[i] = (gf *)malloc(N * sizeof(**Fs));

	for( j=0 ; j<d ; j++ )
	{
		inv = gf_inverse(F[j]);
		for( i=0 ; i<N ; i++ ) Fs[j][i] = gf_mul(synd[i],inv);
	}

	er = Fs[0];  rr = N;

	for( i=1 ; i<d ; i++ )
	{
		er2 = er; rr2 = rr;
		er = vec_sp_inter(er2,rr2,Fs[i],N,&rr);
		free(er2);

		if( rr==r ) break;
	}

	*rank = rr;

	/* delete allocated memory */
	for( i=1 ; i<d ; i++ ) free(Fs[i]);
	free(Fs);

	return er;
}


gf *syndrome_decode(gf *syn, struct LRP *H, short r)
{
	short i,j,t;
	short n,nk,nkd,rd;
	short m_size,rank,info;
	gf flag;
	gf *Eb,*EF,*ee;
	Bvec *M;
	uint32_t *syn_co,ff;
	uint32_t *tm, tm0;

	n = H->M.col;
	nk = H->M.row;;
	nkd = nk * d;
	rd = r * d;

	Eb = error_space(syn,nk,H->basis,&rank);

	if( rank!=r ) { free(Eb); return NULL; }

	/* calculate EF (basis of syndrome vector) */
	EF = (gf *)malloc(rd * sizeof(*EF));
	for( i=0 ; i<r ; i++ )
	  for( j=0 ; j<d ; j++ )
		EF[i*d+j] = gf_mul(Eb[i],H->basis[j]);
	
	/* generate _m binary vectors (initial value 0) */
	M = (Bvec *)malloc(_m * sizeof(*M));
	m_size = nk + rd;
	for( i=0 ; i<_m ; i++ ) M[i] = gen_vec_zero(m_size);

	/* define the matrix for linear equation */
	for( i=0 ; i<rd ; i++ )
	  for( j=0,flag=1 ; j<_m ; j++,flag<<=1 )
		if( EF[i] & flag )
		  put_elt(&M[j],i,1);

	for( i=0 ; i<nk ; i++ )
	  for( j=0,flag=1 ; j<_m ; j++,flag<<=1 )
		if( syn[i] & flag )
		  put_elt(&M[j],i+rd,1);
	
	free(EF);

	gauss_elm(M,_m,m_size);

	/* rank of the matrix M */
	for( i=_m-1 ; i>=0 ; i-- )
	  if( !chk_zero(M[i]) )
		break;

	rank = i+1;

	if( rank!=rd ) { free(M); return NULL; }


	/* check that M contains identity matrix */
	for( i=0,info=1 ; i<rank ; i++ )
	  if( !get_elt(M[i],i) ) { info = 0; break; }

	if( info==0 ) { free(M); return NULL; }


	/* representation by EF basis */
	syn_co = (uint32_t *)malloc(nk*d * sizeof(*syn_co));
	
	for( i=0 ; i<nkd ; i++ ) syn_co[i] = 0;

	for( i=0,ff=1 ; i<r ; i++,ff<<=1 )
	  for( j=0 ; j<d ; j++ )
		for( t=0 ; t<nk ; t++ )
		  if( get_elt(M[i*d+j],rank+t) )
			syn_co[nk*j+t] |= ff;

	for( i=0 ; i<_m ; i++ ) free(M[i].data);   
	free(M);


	/* copy coefficient data */
	M = (Bvec *)malloc(nkd * sizeof(*M));
	for( i=0 ; i<nkd ; i++ ) M[i] = gen_vec_copy(H->bin[i]);

	/* solve the linear equation */
	for( i=0,j=0 ; i<n && j<n ; j++ )
	{
		for( t=i ; t<nkd ; t++ )
		  if( get_elt(M[t],j) )
			break;

		if( t==nkd ) continue;

		if( t!=i )
		{
			tm = M[i].data;  M[i].data = M[t].data;  M[t].data = tm;
			tm0 = syn_co[i];  syn_co[i] = syn_co[t];  syn_co[t] = tm0;
		}

		for( t=0 ; t<nkd ; t++ )
		{
			if( t==i ) continue;
			if( get_elt(M[t],j) )
			{
				vec_add(&M[t],M[i]);
				syn_co[t] ^= syn_co[i];
			}
		}

		i++;
	}

	for( i=0 ; i<nkd ; i++ ) free(M[i].data);
	free(M);

	/* recover error vector using coefficients */
	ee = (gf *)malloc(n * sizeof(*ee));

	for( i=0 ; i<n ; i++ )
	{
		ee[i] = 0;
		for( j=0,ff=1 ; j<r ; j++,ff<<=1 )
		  if( syn_co[i] & ff )
			ee[i] ^= Eb[j];
	}

	free(syn_co); free(Eb);

	return ee;
}


