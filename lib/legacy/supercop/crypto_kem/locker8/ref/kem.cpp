/** 
 * \file kem.cpp
 * \brief Implementation of api.h
 */

#include "hash.h"
#include "rng.h"
#include "api.h"
#include "parameters.h"
#include "decoder.h"
#include "parsing.h"
#include "ffi_field.h"
#include "ffi_vec.h"
#include "locker_types.h"
#include "crypto_kem.h"


/** 
 * \fn int crypto_kem_keypair(unsigned char* pk, unsigned char* sk)
 * \brief Keygen of the LOCKER scheme
 *
 * \param[out] pk String containing the public key
 * \param[out] sk String containing the secret key
 * \return 0 if keygen is sucessfull
 */
int crypto_kem_keypair(unsigned char* pk, unsigned char* sk) {
  #ifdef VERBOSE
    printf("\n\n\n### KEYGEN ###\n\n");
  #endif

  ffi_field_init();
  ffi_vec_init_mulmod();

  publicKey pk_tmp;
  secretKey sk_tmp;

  long invStatus = 1;
  ffi_vec invx;

  // Create seed expanders for public key and secret key
  unsigned char sk_seed[SEEDEXPANDER_SEED_BYTES];

  do {
    randombytes(sk_seed, SEEDEXPANDER_SEED_BYTES);

    locker_secret_key_from_string(sk_tmp, sk_seed);

    //Try to invert x
    invStatus = ffi_vec_inv(invx, sk_tmp.x);
    //If not invertible change the private key
  } while(invStatus == 1);

  //Computes h=x^-1.y the public key
  ffi_vec_mul(pk_tmp.h, invx, sk_tmp.y, PARAM_N);

  // Parse keys to string
  locker_public_key_to_string(pk, pk_tmp);
  locker_public_key_to_string(sk + SEEDEXPANDER_SEED_BYTES, pk_tmp);
  locker_secret_key_to_string(sk, sk_seed);

  #ifdef VERBOSE
    printf("\n\nsk(seed): "); for(int i = 0 ; i < SEEDEXPANDER_SEED_BYTES ; ++i) printf("%02x", sk_seed[i]);
    printf("\n\nF: "); ffi_vec_print(sk_tmp.F, PARAM_D);
    printf("\n\nx: "); ffi_vec_print(sk_tmp.x, PARAM_N);
    printf("\n\ny: "); ffi_vec_print(sk_tmp.y, PARAM_N);

    printf("\n\nh = x^-1.y: "); ffi_vec_print(pk_tmp.h, PARAM_N);

    printf("\n\npk: "); for(int i = 0 ; i < PUBLIC_KEY_BYTES ; ++i) printf("%02x", pk[i]);
  #endif

  return 0;
}



/** 
 * \fn int crypto_kem_enc(unsigned char* ct, unsigned char* ss, const unsigned char* pk)
 * \brief Encapsulation of the LOCKER scheme
 *
 * \param[out] ct String containing the ciphertext
 * \param[out] ss String containing the shared secret
 * \param[in] pk String containing the public key
 * \return 0 if encapsulation is sucessfull
 */
int crypto_kem_enc(unsigned char* ct, unsigned char* ss, const unsigned char* pk) {
  #ifdef VERBOSE
    printf("\n\n\n\n### ENCAPS ###");
  #endif

  ffi_field_init();
  ffi_vec_init_mulmod();

  publicKey pk_tmp;

  locker_public_key_from_string(pk_tmp, pk);

  //Computing m
  unsigned char m[CRYPTO_BYTES];
  randombytes(m, CRYPTO_BYTES);
  
  // Generating G function
  unsigned char seed_G[CRYPTO_BYTES];
  memcpy(seed_G, m, CRYPTO_BYTES);
  AES_XOF_struct* G_seedexpander = (AES_XOF_struct*) malloc(sizeof(AES_XOF_struct));
  seedexpander_init(G_seedexpander, seed_G, seed_G + 32, SEEDEXPANDER_MAX_LENGTH);

  // Computing theta
  unsigned char theta[SEEDEXPANDER_SEED_BYTES];
  seedexpander(G_seedexpander, theta, SEEDEXPANDER_SEED_BYTES);

  //Seedexpander used to encrypt
  AES_XOF_struct* encSeedexpander = (AES_XOF_struct*) malloc(sizeof(AES_XOF_struct));
  seedexpander_init(encSeedexpander, theta, theta + 32, SEEDEXPANDER_MAX_LENGTH);

  //Random error vectors
  ffi_vec E;
  ffi_vec_set_random_full_rank_using_seedexpander(E, PARAM_R, encSeedexpander);

  //Polynomials
  ffi_vec polyE1, polyE2;
  ffi_vec_set_random_from_support_using_seedexpander(polyE1, PARAM_N, E, PARAM_R, encSeedexpander);
  ffi_vec_set_random_from_support_using_seedexpander(polyE2, PARAM_N, E, PARAM_R, encSeedexpander);

  //Compute ciphertext
  ciphertext c_tmp;

  //Left part : we multiply with the generator matrix by the message and then add right part of the error
  ffi_vec_mul(c_tmp.u, polyE2, pk_tmp.h, PARAM_N);
  ffi_vec_add(c_tmp.u, c_tmp.u, polyE1, PARAM_N);

  ffi_vec_echelonize(E, PARAM_R);

  unsigned char support[PARAM_R * GF2MBYTES], hashSupp[CRYPTO_BYTES];
  ffi_vec_to_string(support, E, PARAM_R);
  sha512(hashSupp, support, PARAM_R * GF2MBYTES);

  for(int i=0 ; i<CRYPTO_BYTES ; i++) {
    c_tmp.v[i] = m[i] ^ hashSupp[i];
  }

  sha512(c_tmp.d, m, CRYPTO_BYTES);

  //Compute shared secret
  //Derive shared secret from m, u and v
  unsigned char mc[CIPHERTEXT_BYTES];
  memcpy(mc, m, SHA512_BYTES);
  ffi_vec_to_string(mc + SHA512_BYTES, c_tmp.u, PARAM_N);
  memcpy(mc + SHA512_BYTES + PARAM_N * GF2MBYTES, c_tmp.v, SHA512_BYTES);
  sha512(ss, mc, CIPHERTEXT_BYTES);

  //Ciphertext parsing
  locker_ciphertext_to_string(ct, c_tmp);

  #ifdef VERBOSE
    printf("\n\nm: "); for(int i=0 ; i<CRYPTO_BYTES ; i++) printf("%02x", m[i]);
    printf("\n\ntheta: "); for(int i=0 ; i<SEEDEXPANDER_SEED_BYTES ; i++) printf("%02x", theta[i]);
    printf("\n\nE: "); ffi_vec_print(E, PARAM_R);
    printf("\n\nE1: "); ffi_vec_print(polyE1, PARAM_N);
    printf("\n\nE2: "); ffi_vec_print(polyE2, PARAM_N);
    printf("\n\nu(syndrom): "); ffi_vec_print(c_tmp.u, PARAM_N);
    printf("\n\nv: "); for(int i = 0 ; i < CRYPTO_BYTES ; ++i) printf("%02x", c_tmp.v[i]);
    printf("\n\nd: "); for(int i = 0 ; i < CRYPTO_BYTES ; ++i) printf("%02x", c_tmp.d[i]);
    printf("\n\nss: "); for(int i = 0 ; i < SHARED_SECRET_BYTES ; ++i) printf("%02x", ss[i]);
    printf("\n\nh = x^-1.y: "); ffi_vec_print(pk_tmp.h, PARAM_N);
  #endif

  free(G_seedexpander);
  free(encSeedexpander);

  return 0;
}



/** 
 * \fn int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk)
 * \brief Decapsulation of the LOCKER scheme
 *
 * \param[out] ss String containing the shared secret
 * \param[in] ct String containing the ciphertext
 * \param[in] sk String containing the secret key
 * \return 0 if decapsulation is successfull
 */
int crypto_kem_dec(unsigned char* ss, const unsigned char* ct, const unsigned char* sk) {
  #ifdef VERBOSE
    printf("\n\n\n\n### DECAPS ###\n");
  #endif

  ffi_field_init();
  ffi_vec_init_mulmod();

  ciphertext c;
  secretKey sk_tmp;

  //Retrieve ciphertext and secret key
  locker_ciphertext_from_string(ct, c);
  locker_secret_key_from_string(sk_tmp, sk);

  //Syndrom computation
  ffi_vec xc;
  ffi_vec_mul(xc, sk_tmp.x, c.u, PARAM_N);
  #ifdef VERBOSE
    printf("\n\nxc: "); ffi_vec_print(xc, PARAM_N);
  #endif

  // Retrieve support_r by cyclic-support error decoding
  ffi_vec E;
  int E_dim = RS_recover(E, PARAM_R, sk_tmp.F, PARAM_D, xc, PARAM_N);

  unsigned char decryptedE[SHA512_BYTES];

  // Compute shared secret
  if(E_dim != 0) {
    unsigned char support[E_dim * GF2MBYTES];
    ffi_vec_to_string(support, E, PARAM_R);
    sha512(decryptedE, support, E_dim * GF2MBYTES);
  } else {
    memset(decryptedE, 0, sizeof(SHARED_SECRET_BYTES));
  }

  #ifdef VERBOSE
    printf("\n\nE: "); ffi_vec_print(E, E_dim);
  #endif

  unsigned char m2[CRYPTO_BYTES];

  for(int i=0 ; i<CRYPTO_BYTES ; i++) {
    m2[i] = decryptedE[i] ^ c.v[i];
  }

  // Generating G function
  unsigned char seed_G[CRYPTO_BYTES];
  memcpy(seed_G, m2, CRYPTO_BYTES);
  AES_XOF_struct* G_seedexpander = (AES_XOF_struct*) malloc(sizeof(AES_XOF_struct));
  seedexpander_init(G_seedexpander, seed_G, seed_G + 32, SEEDEXPANDER_MAX_LENGTH);

  // Computing theta
  unsigned char theta[SEEDEXPANDER_SEED_BYTES];
  seedexpander(G_seedexpander, theta, SEEDEXPANDER_SEED_BYTES);

  //Seedexpander used to encrypt
  AES_XOF_struct* encSeedexpander = (AES_XOF_struct*) malloc(sizeof(AES_XOF_struct));
  seedexpander_init(encSeedexpander, theta, theta + 32, SEEDEXPANDER_MAX_LENGTH);

  /******** Re encrypt part ***********/
  publicKey pk_tmp;
  locker_public_key_from_string(pk_tmp, sk + SEEDEXPANDER_SEED_BYTES);

  //Random error vectors
  ffi_vec E2;
  ffi_vec_set_random_full_rank_using_seedexpander(E2, PARAM_R, encSeedexpander);

  //Polynomials
  ffi_vec polyE1, polyE2;
  ffi_vec_set_random_from_support_using_seedexpander(polyE1, PARAM_N, E2, PARAM_R, encSeedexpander);
  ffi_vec_set_random_from_support_using_seedexpander(polyE2, PARAM_N, E2, PARAM_R, encSeedexpander);

  //Compute ciphertext
  ciphertext c2;

  //Left part : we multiply with the generator matrix by the message and then add right part of the error
  ffi_vec_mul(c2.u, polyE2, pk_tmp.h, PARAM_N);
  ffi_vec_add(c2.u, c2.u, polyE1, PARAM_N);

  ffi_vec_echelonize(E2, PARAM_R);

  #ifdef VERBOSE
    printf("\n\nE': "); ffi_vec_print(E2, PARAM_R);
    printf("\n\nE1': "); ffi_vec_print(polyE1, PARAM_N);
    printf("\n\nE2': "); ffi_vec_print(polyE2, PARAM_N);
  #endif

  unsigned char support[PARAM_R * GF2MBYTES], hashSupp[CRYPTO_BYTES];
  ffi_vec_to_string(support, E, PARAM_R);
  sha512(hashSupp, support, PARAM_R * GF2MBYTES);

  for(int i=0 ; i<CRYPTO_BYTES ; i++) {
    c2.v[i] = m2[i] ^ hashSupp[i];
  }

  sha512(c2.d, m2, CRYPTO_BYTES);

  /************************************/

  #ifdef VERBOSE
    printf("\n\nu'(syndrom): "); ffi_vec_print(c2.u, PARAM_N);
    printf("\n\nv': "); for(int i = 0 ; i < CRYPTO_BYTES ; ++i) printf("%02x", c2.v[i]);
    printf("\n\nd': "); for(int i = 0 ; i < CRYPTO_BYTES ; ++i) printf("%02x", c2.d[i]);
  #endif

  //d = d' ?
  if(memcmp(c.d, c2.d, SHA512_BYTES)) return 1;
  //v = v' ?
  if(memcmp(c.v, c2.v, SHA512_BYTES)) return 1;
  //u = u' ?
  if(!ffi_vec_cmp(c.u, c2.u)) return 1;

  //If everything is OK
  //Compute shared secret
  //Derive shared secret from m, u and v
  unsigned char mc[CIPHERTEXT_BYTES];
  memcpy(mc, m2, SHA512_BYTES);
  ffi_vec_to_string(mc + SHA512_BYTES, c.u, PARAM_N);
  memcpy(mc + SHA512_BYTES + PARAM_N * GF2MBYTES, c.v, SHA512_BYTES);
  sha512(ss, mc, CIPHERTEXT_BYTES);

  #ifdef VERBOSE
    printf("\n\nss: "); for(int i = 0 ; i < SHARED_SECRET_BYTES ; ++i) printf("%02x", ss[i]);
  #endif

  free(G_seedexpander);
  free(encSeedexpander);

  return 0;
}

