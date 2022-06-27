#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gmp.h>
#include <openssl/sha.h>
#include "rng.h"
#include "randombytes.h"
#include "crypto_kem.h"

#define SECURITYLVL 256

#if (SECURITYLVL==256)
 #define CRYPTO_ALGNAME "Mersenne756839" 
 #define CRYPTO_BASIC_SECRETKEYBYTES 94624
 #define CRYPTO_SECRETKEYBYTES (SECURITYLVL/8)
 #define CRYPTO_PUBLICKEYBYTES (2*CRYPTO_BASIC_SECRETKEYBYTES)
 #define CRYPTO_BYTES (SECURITYLVL/8)
 #define CRYPTO_RepetitionLength 2048
 #define CRYPTO_CIPHERTEXTBYTES_EXTRA (CRYPTO_BYTES*CRYPTO_RepetitionLength)
 #define CRYPTO_CIPHERTEXTBYTES (CRYPTO_BASIC_SECRETKEYBYTES+CRYPTO_CIPHERTEXTBYTES_EXTRA)
#endif

#if (SECURITYLVL==256)
 #define Psize 756839
 #define Pmask 1048575
#endif

#define NbBytesEncodeCpy (CRYPTO_RepetitionLength/8)
#define Hvalue SECURITYLVL

#define HammingWeight(x) __builtin_popcount(x)

/******************************************************/
/* Wrapper part for the XOF allowing to change easily */
/******************************************************/
typedef AES_XOF_struct ExpandableState;

void InitExpandableState(unsigned char *input,
			ExpandableState *State)
{
  unsigned char diversifier[8]={0,0,0,0,0,0,0,0};
  seedexpander_init(State, input, diversifier, 1UL<<30);
}

void GetExpandableOutput(unsigned char *output,
			 unsigned int outputByteLen,
			 ExpandableState *State)
{
  seedexpander(State, output, outputByteLen);
}

/******************************************************/
/*                   End of XOF wrapper               */
/******************************************************/


/* Rejection sampling for random number modulo a modulus close to Psize */

#define PrandomBytesize 3
int RandomMod(int modulus, ExpandableState *Hstate)
{
  unsigned char Prandom[PrandomBytesize];
  int val,k;
  
  do {
    GetExpandableOutput(Prandom, PrandomBytesize, Hstate);
    val=0;
    for(k=PrandomBytesize-1;k>=0;k--) {
      val<<=8;
      val+=Prandom[k];
    }
    val&=Pmask;
  } while (val>=modulus);

  return val;
}


/******************************************************/
/*    Generate random string of Hamming weight H      */
/******************************************************/
void GenerateHSparseString(unsigned char *string,
			   unsigned int stringlen,
			   ExpandableState *Hstate,
			   unsigned int Hcount)
{
  int i,pos,b1pos,b2pos;
  unsigned char b1val,b2val,bxor;

  if (stringlen*8<Psize) {
    fprintf(stderr,"String too short to store a %ud-bit number\n",Psize);
    exit(1);
  }
  
  memset(string, 0, stringlen);
  for(i=0;i<Hcount/8;i++) {
    string[i]=0xff;
  }
  string[i]=(1U<<(Hcount%8))-1;

  for(i=Hcount-1;i>=0;i--) {
    pos=RandomMod(Psize-i, Hstate);
    /* Exchange the two bits at position i and i+pos */
    b1pos=i;
    b2pos=i+pos;
    b1val=(string[b1pos/8]>>(b1pos%8))&1;
    b2val=(string[b2pos/8]>>(b2pos%8))&1;
    bxor=b1val^b2val;
    string[b1pos/8]^=bxor<<(b1pos%8);
    string[b2pos/8]^=bxor<<(b2pos%8);
  }
}


int crypto_kem_keypair_fromseed(unsigned char *pk,
				unsigned char *sk, unsigned char *seed)
{
  unsigned char error[CRYPTO_BASIC_SECRETKEYBYTES];
  ExpandableState Hstate;
  mpz_t f,g,R,T;
  size_t countp;

  InitExpandableState(seed,&Hstate);

  /* Generate H-sparse strings sk and error */
  GenerateHSparseString(sk,CRYPTO_BASIC_SECRETKEYBYTES,
			&Hstate, Hvalue);

  GenerateHSparseString(error,CRYPTO_BASIC_SECRETKEYBYTES,
			&Hstate, Hvalue);

  /* Generate first half of public key at random (i.e. R) */
  GetExpandableOutput(pk, CRYPTO_BASIC_SECRETKEYBYTES, &Hstate);

  mpz_inits(f,g,R,T,NULL);
  /* Import char tabs into GMP (least significant byte first) */
  mpz_import(R, CRYPTO_BASIC_SECRETKEYBYTES, -1, 1, 0, 0, pk);
  /* For R : also do reduction modulo Mersenne prime */
  mpz_fdiv_q_2exp (f, R, Psize);
  mpz_fdiv_r_2exp (g, R, Psize);
  mpz_add(R, f, g);
  mpz_fdiv_q_2exp (f, R, Psize);
  mpz_fdiv_r_2exp (g, R, Psize);
  mpz_add(R, f, g);
  /* Direct import for f and g */
  mpz_import(f, CRYPTO_BASIC_SECRETKEYBYTES, -1, 1, 0, 0, sk);
  mpz_import(g, CRYPTO_BASIC_SECRETKEYBYTES, -1, 1, 0, 0, error);

  /* Let T=f*R+g */
  mpz_mul(T, f, R);
  mpz_add(T, T, g);

  /* Do reduction of T modulo Mersenne prime (f and g re-used as tmp values) */
  mpz_fdiv_q_2exp (f, T, Psize);
  mpz_fdiv_r_2exp (g, T, Psize);
  mpz_add(T, f, g);
  mpz_fdiv_q_2exp (f, T, Psize);
  mpz_fdiv_r_2exp (g, T, Psize);
  mpz_add(T, f, g);

  /* Store R, T as public key */
  memset(pk, 0, 2*CRYPTO_BASIC_SECRETKEYBYTES);
  mpz_export(pk, &countp, -1, 1, 0, 0, R);
  mpz_export(pk+CRYPTO_BASIC_SECRETKEYBYTES, &countp, -1, 1, 0, 0, T);

  mpz_clears(f,g,R,T,NULL);

  return 0;
}


int crypto_kem_keypair(unsigned char *pk,
		       unsigned char *sk)
{
  unsigned char seed[32];
  unsigned char basic_sk[CRYPTO_BASIC_SECRETKEYBYTES];

  /* If any of the two pointers is NULL return an error */
  if ((pk==NULL) || (sk==NULL)) {
    return -1;
  }
  

  /*Choose random seed (possibly pad with 0 to 256 bits is seclvl<256)*/
  memset(seed,0,32);
  randombytes(sk,SECURITYLVL/8);
  memcpy(seed,sk,SECURITYLVL/8);

  /* Generate expanded keypair from seed */
  return crypto_kem_keypair_fromseed(pk, basic_sk, seed);
}


/* Create a kem exchange message from public key and seed value */
void mersenne_kem_enc_seed(unsigned char *ct,
			  unsigned char *ss,
			  const unsigned char *pk,
			  const unsigned char *inputseed)
{
  unsigned char seed[32];
  unsigned char error[CRYPTO_BASIC_SECRETKEYBYTES];
  ExpandableState Hstate;
  size_t countp;
  int i,j,pos;
  mpz_t a,b1,b2,C1,C2,R,T;


  /* Initialize PRNG from seed (padded if 256 bits if seclvl<256) */
  memset(seed,0,32);
  memcpy(seed, inputseed, SECURITYLVL/8);
  InitExpandableState(seed,&Hstate);

  /* Get Shared Secret */
  GetExpandableOutput(ss, CRYPTO_BYTES, &Hstate);
  
  mpz_inits(a,b1,b2,C1,C2,R,T,NULL);

  /* Generate H-sparse strings for a, b1 and b2 */
  GenerateHSparseString(error,CRYPTO_BASIC_SECRETKEYBYTES,
			&Hstate, Hvalue);
  mpz_import(a, CRYPTO_BASIC_SECRETKEYBYTES, -1, 1, 0, 0, error);

  GenerateHSparseString(error,CRYPTO_BASIC_SECRETKEYBYTES,
			&Hstate, Hvalue);
  mpz_import(b1, CRYPTO_BASIC_SECRETKEYBYTES, -1, 1, 0, 0, error);

  GenerateHSparseString(error,CRYPTO_BASIC_SECRETKEYBYTES,
			&Hstate, Hvalue);
  mpz_import(b2, CRYPTO_BASIC_SECRETKEYBYTES, -1, 1, 0, 0, error);


  /* Get R and T from the public key array */
  mpz_import(R, CRYPTO_BASIC_SECRETKEYBYTES, -1, 1, 0, 0, pk);
  mpz_import(T, CRYPTO_BASIC_SECRETKEYBYTES, -1, 1, 0, 0, pk+CRYPTO_BASIC_SECRETKEYBYTES);

  /* Compute C1=aR+b1 and C2=aT+b2 (modulo the Mersenne) */
  mpz_mul(C1, a, R);
  mpz_add(C1, C1, b1);

  mpz_mul(C2, a, T);
  mpz_add(C2, C2, b2);
  
  /* Reduction of C1 and C2 modulo Mersenne prime (b1 and b2 re-used as tmp values) */
  mpz_fdiv_q_2exp (b1, C1, Psize);
  mpz_fdiv_r_2exp (b2, C1, Psize);
  mpz_add(C1, b1, b2);
  mpz_fdiv_q_2exp (b1, C1, Psize);
  mpz_fdiv_r_2exp (b2, C1, Psize);
  mpz_add(C1, b1, b2);

  mpz_fdiv_q_2exp (b1, C2, Psize);
  mpz_fdiv_r_2exp (b2, C2, Psize);
  mpz_add(C2, b1, b2);
  mpz_fdiv_q_2exp (b1, C2, Psize);
  mpz_fdiv_r_2exp (b2, C2, Psize);
  mpz_add(C2, b1, b2);

  /* Export C1 as first part of ciphertext  */
  memset(ct,0,CRYPTO_CIPHERTEXTBYTES);
  mpz_export(ct, &countp, -1, 1, 0, 0, C1);

  /* Extract Mask in Second Part from C2 */
  mpz_export(error, &countp, -1, 1, 0, 0, C2);

  /* XOR a repetition encoding of inputseed with mask */
  pos=0;
  for(i=0;i<SECURITYLVL;i++) {
    if ((inputseed[i/8]>>(i%8))&1) {
      for(j=0;j<NbBytesEncodeCpy;j++) {
	ct[CRYPTO_BASIC_SECRETKEYBYTES+pos]=
	  error[pos]^0xff;
	pos++;
      }
    }
    else {
      for(j=0;j<NbBytesEncodeCpy;j++) {
	ct[CRYPTO_BASIC_SECRETKEYBYTES+pos]=
	  error[pos];
	pos++;
      }
    }
  }
  
  mpz_clears(a,b1,b2,C1,C2,R,T,NULL);
}


int crypto_kem_enc(unsigned char *ct,
		   unsigned char *ss,
		   const unsigned char *pk)
{
  unsigned char kemseed[SECURITYLVL/8];

  /* If any of the three pointers is NULL return an error */
  if ((ss==NULL) || (ct==NULL) || (pk==NULL)) {
    return -1;
  }
  

  /* Choose a random kem seed and call the seeded kem_enc function */
  randombytes(kemseed,SECURITYLVL/8);
  mersenne_kem_enc_seed(ct, ss, pk, kemseed);

  return 0;
}

int crypto_kem_dec(unsigned char *ss,
		   const unsigned char *ct,
		   const unsigned char *sk)
{
  unsigned char kemseed[SECURITYLVL/8]={0};
  unsigned char ct2[CRYPTO_CIPHERTEXTBYTES];
  unsigned char ss2[CRYPTO_BYTES];
  int retcode, i, j, pos;
  size_t countp;
  mpz_t tmp,f,C1,C2bis;
  unsigned char seed[32];
  static int has_stored_sk=0;
  static unsigned char basic_sk[3*CRYPTO_BASIC_SECRETKEYBYTES];
  static unsigned char stored_sk[CRYPTO_SECRETKEYBYTES];
  
  /* If any of the three pointers is NULL*/
  /* Delete any stored sk and return an error */
  if ((ss==NULL) || (ct==NULL) || (sk==NULL)) {
    memset(stored_sk,0,CRYPTO_SECRETKEYBYTES);
    memset(basic_sk,0,3*CRYPTO_BASIC_SECRETKEYBYTES);
    has_stored_sk=0;
    return -1;
  }

  if (has_stored_sk) {
    if (memcmp(sk, stored_sk, CRYPTO_SECRETKEYBYTES)) {
      has_stored_sk=0;
    }
  }
  
  /* If needed: Re-generate expanded key from secret key in seed form */
  if (has_stored_sk==0) {
    memset(seed,0,32);
    memcpy(seed,sk,SECURITYLVL/8);
    crypto_kem_keypair_fromseed(basic_sk+CRYPTO_BASIC_SECRETKEYBYTES, basic_sk, seed);
    memcpy(stored_sk,sk,CRYPTO_SECRETKEYBYTES);
    has_stored_sk=1;
  }
  
  mpz_inits(tmp,f,C1,C2bis,NULL);
  mpz_import(f, CRYPTO_BASIC_SECRETKEYBYTES, -1, 1, 0, 0, basic_sk);
  mpz_import(C1, CRYPTO_BASIC_SECRETKEYBYTES, -1, 1, 0, 0, ct);

  /* Compute secret key Times first part of ciphertext modulo Mersenne prime */
  mpz_mul(C2bis, f, C1);
  /* Reduction of C2bis modulo Mersenne prime (tmp and f used as tmp values) */
  mpz_fdiv_q_2exp (tmp, C2bis, Psize);
  mpz_fdiv_r_2exp (f, C2bis, Psize);
  mpz_add(C2bis, tmp, f);
  mpz_fdiv_q_2exp (tmp, C2bis, Psize);
  mpz_fdiv_r_2exp (f, C2bis, Psize);
  mpz_add(C2bis, tmp, f);

  /* Extract Mask in Second Part from C2bis */
  mpz_export(ct2, &countp, -1, 1, 0, 0, C2bis);
  
  /* Xor with second half of ciphertext to obtain (noisy) encoded kemseed */
  for(i=0;i<CRYPTO_CIPHERTEXTBYTES_EXTRA;i++) {
    ct2[i]^=ct[i+CRYPTO_BASIC_SECRETKEYBYTES];
  }

  /* Decode the repetition code bit by bit to obtain kemseed */
  pos=0;
  for(i=0;i<SECURITYLVL;i++) {
    int total_weight;
    total_weight=0;
    for(j=0;j<NbBytesEncodeCpy;j++,pos++) {
      total_weight+=HammingWeight(ct2[pos]);
    }
    if (total_weight>4*NbBytesEncodeCpy) kemseed[i/8]^=(1<<(i%8));
  }
    
  mpz_clears(tmp,f,C1,C2bis,NULL);

  /* Redo kem_enc from the recovered seed */
  mersenne_kem_enc_seed(ct2, ss2, basic_sk+CRYPTO_BASIC_SECRETKEYBYTES, kemseed);

  /* Compare the results to check that they are fully identical */
  retcode=0;
  for(i=0;i<CRYPTO_CIPHERTEXTBYTES;i++) {
    if (ct[i]!=ct2[i]) {
      retcode=-1;
    }
  }
  // Erase shared secret in case of error
  if (retcode) memset(ss2,0,CRYPTO_BYTES);
  memcpy(ss,ss2,CRYPTO_BYTES);
  
  return retcode;
}

