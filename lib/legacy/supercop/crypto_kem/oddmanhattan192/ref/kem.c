#include "crypto_kem.h"
#include "gmp.h"
#include "rng.h"
#include "string.h"
#include "randombytes.h"


void om_random_generator(mpz_t a, mpz_t b, AES_XOF_struct *ctx)
{
  // Sample a random from [0,b] 
  unsigned long n=mpz_sizeinbase(b,2);
  unsigned long xlen=(n+7)/8;
  unsigned char x[xlen];
  mpz_t r;
  mpz_init(r);
  do {
    memset(x, 0, xlen);
    seedexpander(ctx,x,xlen);
    mpz_import(r,xlen,-1,1,-1,0,x);
    mpz_fdiv_r_2exp(r,r,n);
  } while (mpz_cmp(r,b)>0);
  mpz_set(a,r);
  mpz_clear(r);
}


int crypto_kem_keypair(unsigned char *pk, unsigned char *sk)
{
  char seed[32]={0};
  randombytes(seed,crypto_kem_BYTES);

  AES_XOF_struct ctx[1];
  unsigned char diversifier[8]={0};
  unsigned long maxlen=4294967295;  
  seedexpander_init(ctx,seed,diversifier,maxlen);

  mpz_t keys[D];

  // Compute determinant 2^N-C
  mpz_t det;
  mpz_init(det);
  mpz_ui_pow_ui(det,2,N);
  mpz_sub_ui(det,det,C);

  // Compute l=det/(2B)
  mpz_t l1;
  mpz_init(l1);
  mpz_fdiv_q_ui(l1,det,2*B);

  // Compute a random combination of D elements of [1,l]
  mpz_add_ui(l1,l1,D);
  mpz_fdiv_q_ui(l1,l1,2);
  for (long i=0;i<D;i++)
    {
      mpz_sub_ui(l1,l1,1);
      mpz_init(keys[i]);
      om_random_generator(keys[i],l1, ctx);
      mpz_add_ui(keys[i],keys[i],1);
      for (long j=0;j<i;j++)
	if (mpz_cmp(keys[j],keys[i])<=0)
	  mpz_add_ui(keys[i],keys[i],1);
	else
	  mpz_swap(keys[j],keys[i]);
    }

  // Compute a random positive vector of Manhattan norm <=(l+d)/2
  for (long i=D-1;i>0;i--)
    mpz_sub(keys[i],keys[i],keys[i-1]);

  // Compute a random positive odd vector of Manhattan norm <=l
  for (long i=0;i<D;i++)
    {
      mpz_mul_ui(keys[i],keys[i],2);
      mpz_sub_ui(keys[i],keys[i],1);
    }

  // Compute a random signed odd vector of Manhattan norm <=l
  mpz_t random_signs;
  mpz_init(random_signs);
  mpz_ui_pow_ui(random_signs,2,D-1);
  mpz_sub_ui(random_signs,random_signs,1);
  om_random_generator(random_signs,random_signs, ctx);
  
  for (long i=1;i<D;i++)
    if (mpz_fdiv_q_ui(random_signs,random_signs,2)==0)
      mpz_sub(keys[i],det,keys[i]);

  // Compute keys
  mpz_t invert_secret_key;
  mpz_init(invert_secret_key);
  mpz_invert(invert_secret_key,keys[0],det);
  for (long i=1;i<D;i++)
    {
      mpz_mul(keys[i],keys[i],invert_secret_key);
      mpz_mod(keys[i],keys[i],det);
    }

  // Set keys
  memset(sk, 0, crypto_kem_SECRETKEYBYTES);
  for (long i=0;i<D;i++)
    mpz_export(&(sk[i*P]),NULL,-1,1,-1,0,keys[i]);
  
  memset(pk, 0, crypto_kem_PUBLICKEYBYTES);
  for (long i=1;i<D;i++)
    mpz_export(&(pk[(i-1)*P]),NULL,-1,1,-1,0,keys[i]);

  // Free memory
  mpz_clear(invert_secret_key);
  mpz_clear(random_signs);
  mpz_clear(l1);
  mpz_clear(det);
  
  for (long i=0;i<D;i++)
    mpz_clear(keys[i]);

  return (0);
}


void om_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk, unsigned char *seed)
{
  AES_XOF_struct ctx[1];
  char diversifier[8]={0};
  unsigned long maxlen=4294967295;
  seedexpander_init(ctx,seed,diversifier,maxlen);

  // Expand seed to a share secret
  memset(ss, 0, crypto_kem_BYTES);
  seedexpander(ctx,ss,crypto_kem_BYTES);

  // Compute determinant
  mpz_t det;
  mpz_init(det);
  mpz_ui_pow_ui(det,2,N);
  mpz_sub_ui(det,det,C);

  // Set public key 
  mpz_t sum_of_key;
  mpz_init_set_ui(sum_of_key,1);
  mpz_t public_key[D];
  mpz_init_set_ui(public_key[0],1);  
  for (long i=1;i<D;i++)
    {
      mpz_init(public_key[i]);
      mpz_import(public_key[i],P,-1,1,-1,0,&(pk[(i-1)*P]));
      mpz_add(sum_of_key,sum_of_key,public_key[i]);
    }
  mpz_mul_ui(sum_of_key,sum_of_key,B+1);
  mpz_mod(sum_of_key,sum_of_key,det);
  mpz_sub(sum_of_key,det,sum_of_key);

  // Precomputation for side channel attack resistance
  mpz_t precomputed_keys[D*(2*B+1)];
  for (long i=0;i<D;i++)
    for (long j=0;j<2*B+1;j++)
      {
        mpz_init(precomputed_keys[i+D*j]);
	mpz_mul_ui(precomputed_keys[i+D*j],public_key[i],j+1);
	mpz_mod(precomputed_keys[i+D*j],precomputed_keys[i+D*j],det);
      }
  
  memset(ct,0,crypto_kem_CIPHERTEXTBYTES);  
  mpz_t random_vector,ciphertext,quotient;
  mpz_init(random_vector);
  mpz_init(ciphertext);
  mpz_init(quotient);
  for (long i=0,k=0;i<crypto_kem_BYTES;i++)
    {
      unsigned char s0=seed[i];
      for (long j=0;j<8;j++,k++)
	{

	  // Create a random integer corresponding to random vector u
	  mpz_ui_pow_ui(random_vector,2*B+1,D);
	  mpz_fdiv_q_ui(random_vector,random_vector,2);
	  mpz_sub_ui(random_vector,random_vector,1);
	  om_random_generator(random_vector,random_vector,ctx);
	  mpz_mul_ui(random_vector,random_vector,2);
	  if ((D*B)%2==0)
	    mpz_add_ui(random_vector,random_vector,s0 %2);
	  else
	    mpz_add_ui(random_vector,random_vector,(s0+1)%2);
	  s0/=2;

	  // Compute <u,h>
	  mpz_set(ciphertext,sum_of_key);
	  for (long d=0;d<D;d++)
	    mpz_add(ciphertext,ciphertext,precomputed_keys[d+D*mpz_fdiv_q_ui(random_vector,random_vector,2*B+1)]);
	  mpz_mod(ciphertext,ciphertext,det);

	  mpz_export(&(ct[k*P]),NULL,-1,1,-1,0,ciphertext);
	}
    }

  // Free memory
  mpz_clear(quotient);
  mpz_clear(ciphertext);
  mpz_clear(random_vector);
  for (long i=0;i<D;i++)
    mpz_clear(public_key[i]);
  for (long i=0;i<D;i++)
    for (long j=0;j<2*B+1;j++)
      mpz_clear(precomputed_keys[i+D*j]);  
  mpz_clear(det);
}


int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk)
{
  unsigned char seed[32]={0};
  randombytes(seed,crypto_kem_BYTES);

  // Encrypt using seed
  om_kem_enc(ct,ss,pk,seed);
  return (0);
}


void om_kem_dec(unsigned char *message, const unsigned char *ct, const unsigned char *sk)
{
  // Compute determinant
  mpz_t det;
  mpz_init(det);
  mpz_ui_pow_ui(det,2,N);
  mpz_sub_ui(det,det,C);
  mpz_t secret_key,quotient,remainder,ciphertext;
  mpz_init(secret_key);
  mpz_import(secret_key,P,-1,1,-1,0,sk);
  mpz_init(quotient);
  mpz_init(remainder);
  mpz_init(ciphertext);
  for (long i=0,k=0;i<crypto_kem_BYTES;i++)
    {
      unsigned char r0=0;
      unsigned char b0=1;
      for (long j=0;j<8;j++,k++)
	{
	  mpz_import(ciphertext,P,-1,1,-1,0,&(ct[k*P]));
	  
	  // Compute cw_1 mod p
	  mpz_mul(ciphertext,ciphertext,secret_key);
	  mpz_mod(ciphertext,ciphertext,det);

	  // Extract m
	  mpz_add_ui(quotient,ciphertext,C/2);
	  if (mpz_sizeinbase(quotient,2)>=N)
	    r0+=(((char)(mpz_odd_p(ciphertext)==0))<<j);
	  else
	    r0+=(((char)(mpz_even_p(ciphertext)==0))<<j);
	}
      message[i]=r0;
    }
  
  // Free memory
  mpz_clear(ciphertext);
  mpz_clear(remainder);
  mpz_clear(quotient);
  mpz_clear(secret_key);
  mpz_clear(det);
}


int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk)
{
  // Extract seed
  unsigned char seed[32]={0};
  om_kem_dec(seed, ct, sk);

  // Encrypt using seed
  unsigned char ct_cca[crypto_kem_CIPHERTEXTBYTES];
  unsigned char ss2[crypto_kem_BYTES];
  om_kem_enc(ct_cca, ss2, &(sk[P]),seed);
  
  // Check if cyphertexts are equivalent
  int cca_check=0;
  for (long i=0;i<crypto_kem_BYTES;i++)
    if (ct_cca[i]!=ct[i])
      cca_check=-1;
  memcpy(ss,ss2,sizeof ss2);
  return (cca_check);
}
