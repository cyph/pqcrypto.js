/** 
 * \file kem.c
 * \brief Implementation of api.h
 */


#include "crypto_kem.h"
#include "rbc_qre.h"
#include "rbc_vec.h"
#include "hash.h"
#include "parameters.h"
#include "string.h"
#include "lrpc.h"
#include "types.h"
#include "parsing.h"

int crypto_kem_keypair(uint8_t* pk, uint8_t* sk) {
  rolloII_secretKey skTmp;
  rolloII_publicKey pkTmp;

  rbc_qre invX;

  rbc_qre_init_modulus(ROLLOII_PARAM_N);

  uint8_t sk_seed[SEEDEXPANDER_SEED_BYTES];
  randombytes(sk_seed, SEEDEXPANDER_SEED_BYTES);

  rolloII_secret_key_from_string(&skTmp, sk_seed);

  rbc_qre_init(&invX);
  rbc_qre_inv(invX, skTmp.x);

  rbc_qre_init(&(pkTmp.h));
  rbc_qre_mul(pkTmp.h, invX, skTmp.y);

  rolloII_secret_key_to_string(sk, sk_seed);
  rolloII_public_key_to_string(sk + SEEDEXPANDER_SEED_BYTES, &pkTmp);
  rolloII_public_key_to_string(pk, &pkTmp);

  #ifdef VERBOSE
    printf("\n\nsk_seed: "); for(size_t i = 0 ; i < SEEDEXPANDER_SEED_BYTES ; ++i) printf("%02x", sk_seed[i]);
    printf("\n\nx: "); rbc_qre_print(skTmp.x);
    printf("\n\ny: "); rbc_qre_print(skTmp.y);
    printf("\n\nx^-1: "); rbc_qre_print(invX);
    printf("\n\nh: "); rbc_qre_print(pkTmp.h);
    printf("\n\nsk: "); for(size_t i = 0 ; i < ROLLOII_SECRET_KEY_BYTES ; ++i) printf("%02x", sk[i]);
    printf("\n\npk: "); for(size_t i = 0 ; i < ROLLOII_PUBLIC_KEY_BYTES ; ++i) printf("%02x", pk[i]);
  #endif

  rbc_qre_clear(invX);
  rbc_vspace_clear(skTmp.F);
  rbc_qre_clear(skTmp.x);
  rbc_qre_clear(skTmp.y);
  rbc_qre_clear(pkTmp.h);
  rbc_qre_clear_modulus();

  return 0;
}

int crypto_kem_enc(uint8_t* ct, uint8_t* ss, const uint8_t* pk) {
  rolloII_publicKey pkTmp;
  rolloII_ciphertext ctTmp;

  rbc_vspace E;
  rbc_qre E1, E2;

  uint8_t m[SHA512_BYTES];
  uint8_t theta[SHA512_BYTES] = {0};
  AES_XOF_struct encSeedexpander;

  uint8_t support[ROLLOII_RBC_VEC_R_BYTES];
  uint8_t hashSupp[SHA512_BYTES];
  uint8_t mc[ROLLOII_CIPHERTEXT_BYTES];

  rbc_field_init();
  rbc_qre_init_modulus(ROLLOII_PARAM_N);

  rolloII_public_key_from_string(&pkTmp, pk);

  rbc_vspace_init(&E, ROLLOII_PARAM_R);

  //Computing m
  randombytes(m, SHA512_BYTES);

  // Computing theta
  sha3_512(theta, m, SHA512_BYTES);

  //Seedexpander used to encrypt
  seedexpander_init(&encSeedexpander, theta, theta + 32, SEEDEXPANDER_MAX_LENGTH);

  //Support
  rbc_vspace_set_random_full_rank(&encSeedexpander, E, ROLLOII_PARAM_R);

  rbc_qre_init(&E1);
  rbc_qre_init(&E2);
  rbc_qre_init(&(ctTmp.syndrom));

  //Random error vectors
  rbc_qre_set_random_pair_from_support(&encSeedexpander, E1, E2, E, ROLLOII_PARAM_R);

  rbc_qre_mul(ctTmp.syndrom, E2, pkTmp.h);
  rbc_qre_add(ctTmp.syndrom, ctTmp.syndrom, E1);

  rbc_vec_echelonize(E, ROLLOII_PARAM_R);

  rbc_vec_to_string(support, E, ROLLOII_PARAM_R);
  sha512(hashSupp, support, ROLLOII_RBC_VEC_R_BYTES);

  for(size_t i = 0 ; i < SHA512_BYTES ; i++) {
    ctTmp.v[i] = m[i] ^ hashSupp[i];
  }

  sha512(ctTmp.d, m, SHA512_BYTES);

  //Compute shared secret
  //Derive shared secret from m, syndrom and v
  memcpy(mc, m, SHA512_BYTES);
  rbc_qre_to_string(mc + SHA512_BYTES, ctTmp.syndrom);
  memcpy(mc + SHA512_BYTES + ROLLOII_RBC_VEC_N_BYTES, ctTmp.v, SHA512_BYTES);
  sha512(ss, mc, ROLLOII_CIPHERTEXT_BYTES);

  //Ciphertext parsing
  rolloII_ciphertext_to_string(ct, &ctTmp);

  #ifdef VERBOSE
    printf("\n\nh: "); rbc_qre_print(pkTmp.h);
    printf("\n\nm: "); for(size_t i=0 ; i<SHA512_BYTES ; i++) printf("%02x", m[i]);
    printf("\n\ntheta: "); for(size_t i=0 ; i<SEEDEXPANDER_SEED_BYTES ; i++) printf("%02x", theta[i]);
    printf("\n\nE: "); rbc_vspace_print(E, ROLLOII_PARAM_R);
    printf("\n\nE1: "); rbc_qre_print(E1);
    printf("\n\nE2: "); rbc_qre_print(E2);
    printf("\n\nsyndrom: "); rbc_qre_print(ctTmp.syndrom);
    printf("\n\nv: "); for(size_t i = 0 ; i < SHA512_BYTES ; ++i) printf("%02x", ctTmp.v[i]);
    printf("\n\nd: "); for(size_t i = 0 ; i < SHA512_BYTES ; ++i) printf("%02x", ctTmp.d[i]);
    printf("\n\nss: "); for(size_t i = 0 ; i < ROLLOII_SHARED_SECRET_BYTES ; ++i) printf("%02x", ss[i]);
  #endif

  rbc_vspace_clear(E);
  rbc_qre_clear(E1);
  rbc_qre_clear(E2);
  rbc_qre_clear(pkTmp.h);
  rbc_qre_clear(ctTmp.syndrom);
  rbc_qre_clear_modulus();

  return 0;
}

int crypto_kem_dec(uint8_t* ss, const uint8_t* ct, const uint8_t* sk) {
  rolloII_secretKey skTmp;
  rolloII_ciphertext ctTmp;
  rolloII_publicKey pkTmp;
  rolloII_ciphertext cprime;

  rbc_qre xc;
  rbc_vspace E;
  uint32_t dimE = 0;
  rbc_vspace Eprime;
  rbc_qre E1prime, E2prime;

  uint8_t theta[SHA512_BYTES] = {0};
  uint8_t decryptedE[SHA512_BYTES];
  int8_t result = -1;
  uint8_t m2[SHA512_BYTES];
  uint8_t support[ROLLOII_RBC_VEC_R_BYTES];
  AES_XOF_struct encSeedexpander;
  uint8_t hashSupp[SHA512_BYTES];
  uint8_t mc[ROLLOII_CIPHERTEXT_BYTES];

  rbc_field_init();
  rbc_qre_init_modulus(ROLLOII_PARAM_N);

  rolloII_secret_key_from_string(&skTmp, sk);
  rolloII_ciphertext_from_string(&ctTmp, ct);

  rbc_qre_init(&xc);

  rbc_qre_mul(xc, skTmp.x, ctTmp.syndrom);

  rbc_vspace_init(&E, ROLLOII_PARAM_N);

  dimE = rbc_lrpc_RSR(E, ROLLOII_PARAM_R, skTmp.F, ROLLOII_PARAM_D, xc, ROLLOII_PARAM_N);

  if(dimE != 0) {
    rbc_vec_to_string(support, E, ROLLOII_PARAM_R);
    sha512(decryptedE, support, ROLLOII_RBC_VEC_R_BYTES);
  } else {
    memset(decryptedE, 0, ROLLOII_SHARED_SECRET_BYTES);
  }

  for(size_t i = 0 ; i < SHA512_BYTES ; i++) {
    m2[i] = decryptedE[i] ^ ctTmp.v[i];
  }

  // Computing theta
  sha3_512(theta, m2, SHA512_BYTES);

  //Seedexpander used to encrypt
  seedexpander_init(&encSeedexpander, theta, theta + 32, SEEDEXPANDER_MAX_LENGTH);

  /******** Re encrypt part ***********/
  rolloII_public_key_from_string(&pkTmp, sk + SEEDEXPANDER_SEED_BYTES);

  rbc_vspace_init(&Eprime, ROLLOII_PARAM_R);
  rbc_vspace_set_random_full_rank(&encSeedexpander, Eprime, ROLLOII_PARAM_R);

  rbc_qre_init(&E1prime);
  rbc_qre_init(&E2prime);
  rbc_qre_init(&(cprime.syndrom));

  //Random error vectors
  rbc_qre_set_random_pair_from_support(&encSeedexpander, E1prime, E2prime, Eprime, ROLLOII_PARAM_R);

  rbc_qre_mul(cprime.syndrom, E2prime, pkTmp.h);
  rbc_qre_add(cprime.syndrom, cprime.syndrom, E1prime);

  rbc_vec_echelonize(Eprime, ROLLOII_PARAM_R);

  rbc_vec_to_string(support, Eprime, ROLLOII_PARAM_R);
  sha512(hashSupp, support, ROLLOII_RBC_VEC_R_BYTES);

  for(size_t i = 0 ; i < SHA512_BYTES ; i++) {
    cprime.v[i] = m2[i] ^ hashSupp[i];
  }

  sha512(cprime.d, m2, SHA512_BYTES);

  // Computing shared secret
  memcpy(mc, m2, SHA512_BYTES);
  rbc_qre_to_string(mc + SHA512_BYTES, ctTmp.syndrom);
  memcpy(mc + SHA512_BYTES + ROLLOII_RBC_VEC_N_BYTES, ctTmp.v, SHA512_BYTES);
  sha512(ss, mc, ROLLOII_CIPHERTEXT_BYTES);

  //Abort if differences in reencryption
  result = (rbc_qre_is_equal_to(ctTmp.syndrom, cprime.syndrom) == 1 && memcmp(ctTmp.d, cprime.d, SHA512_BYTES) == 0 && memcmp(ctTmp.v, cprime.v, SHA512_BYTES) == 0);
  for(size_t i = 0 ; i < ROLLOII_SHARED_SECRET_BYTES ; i++) ss[i] = result * ss[i];
  result--;

  #ifdef VERBOSE
    printf("\n\nsk(seed): "); for(size_t i = 0 ; i < SEEDEXPANDER_SEED_BYTES ; ++i) printf("%02x", sk[i]);
    printf("\n\nF: "); rbc_vspace_print(skTmp.F, ROLLOII_PARAM_D);
    printf("\n\nx: "); rbc_qre_print(skTmp.x);
    printf("\n\ny: "); rbc_qre_print(skTmp.y);
    printf("\n\nsyndrom: "); rbc_qre_print(ctTmp.syndrom);
    printf("\n\nv: "); for(size_t i = 0 ; i < SHA512_BYTES ; ++i) printf("%02x", ctTmp.v[i]);
    printf("\n\nd: "); for(size_t i = 0 ; i < SHA512_BYTES ; ++i) printf("%02x", ctTmp.d[i]);
    printf("\n\nxc: "); rbc_qre_print(xc);
    printf("\n\nE': "); rbc_vspace_print(Eprime, ROLLOII_PARAM_R);
    printf("\n\nE1': "); rbc_qre_print(E1prime);
    printf("\n\nE2': "); rbc_qre_print(E2prime);
    printf("\n\nsyndrom': "); rbc_qre_print(cprime.syndrom);
    printf("\n\nv': "); for(size_t i = 0 ; i < SHA512_BYTES ; ++i) printf("%02x", cprime.v[i]);
    printf("\n\nd': "); for(size_t i = 0 ; i < SHA512_BYTES ; ++i) printf("%02x", cprime.d[i]);
    printf("\n\nss: "); for(size_t i = 0 ; i < ROLLOII_SHARED_SECRET_BYTES ; ++i) printf("%02x", ss[i]);
  #endif

  rbc_vspace_clear(E);
  rbc_vspace_clear(Eprime);
  rbc_vspace_clear(skTmp.F);
  rbc_qre_clear(E1prime);
  rbc_qre_clear(E2prime);
  rbc_qre_clear(xc);
  rbc_qre_clear(skTmp.x);
  rbc_qre_clear(skTmp.y);
  rbc_qre_clear(ctTmp.syndrom);
  rbc_qre_clear(cprime.syndrom);
  rbc_qre_clear(pkTmp.h);
  rbc_qre_clear_modulus();

  return result;

  return 0;
}
