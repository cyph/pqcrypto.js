/*=============================================================================
This file has been adapted from the implementation
(available at, Public Domain https://github.com/KULeuven-COSIC/SABER)
of "Saber: Module-LWR based key exchange, CPA-secure encryption and CCA-secure KEM"
by : Jan-Pieter D'Anvers, Angshuman Karmakar, Sujoy Sinha Roy, and Frederik Vercauteren
Jose Maria Bermudo Mera, Michiel Van Beirendonck, Andrea Basso.

 * Copyright (c) 2020 by Cryptographic Engineering Research Group (CERG)
 * ECE Department, George Mason University
 * Fairfax, VA, U.S.A.
 * Author: Duc Tri Nguyen
=============================================================================*/

#include <arm_neon.h>

#include "pack_unpack.h"
#include "randombytes.h" // Only for SUPERCOP
// #include "rng.h"
#include "cbd.h"
#include "SABER_params.h"
#include "fips202.h"
#include "rq_mul/neon_poly_rq_mul.c"

#define h1 4 //2^(EQ-EP-1)

#define h2 ((1 << (SABER_EP - 2)) - (1 << (SABER_EP - SABER_ET - 1)) + (1 << (SABER_EQ - SABER_EP - 1)))

#if defined(__clang__)

// load c <= a
#define vload(c, a) c = vld1q_u16_x4(a);

// store c <= a
#define vstore(c, a) vst1q_u16_x4(c, a);

#elif defined(__GNUC__)

#define vload(c, a)             \
  c.val[0] = vld1q_u16(a);      \
  c.val[1] = vld1q_u16(a + 8);  \
  c.val[2] = vld1q_u16(a + 16); \
  c.val[3] = vld1q_u16(a + 24);

#define vstore(c, a)           \
  vst1q_u16(c, a.val[0]);      \
  vst1q_u16(c + 8, a.val[1]);  \
  vst1q_u16(c + 16, a.val[2]); \
  vst1q_u16(c + 24, a.val[3]);

#else
#error "Unsupported compiler"
#endif

// c = a << value
#define vsl(c, a, value)                   \
  c.val[0] = vshlq_n_u16(a.val[0], value); \
  c.val[1] = vshlq_n_u16(a.val[1], value); \
  c.val[2] = vshlq_n_u16(a.val[2], value); \
  c.val[3] = vshlq_n_u16(a.val[3], value);

// c = a >> value
#define vsr(c, a, value)                   \
  c.val[0] = vshrq_n_u16(a.val[0], value); \
  c.val[1] = vshrq_n_u16(a.val[1], value); \
  c.val[2] = vshrq_n_u16(a.val[2], value); \
  c.val[3] = vshrq_n_u16(a.val[3], value);

// c = a + b
#define vadd(c, a, b)                       \
  c.val[0] = vaddq_u16(a.val[0], b.val[0]); \
  c.val[1] = vaddq_u16(a.val[1], b.val[1]); \
  c.val[2] = vaddq_u16(a.val[2], b.val[2]); \
  c.val[3] = vaddq_u16(a.val[3], b.val[3]);

// c = a - b
#define vsub(c, a, b)                       \
  c.val[0] = vsubq_u16(a.val[0], b.val[0]); \
  c.val[1] = vsubq_u16(a.val[1], b.val[1]); \
  c.val[2] = vsubq_u16(a.val[2], b.val[2]); \
  c.val[3] = vsubq_u16(a.val[3], b.val[3]);

// c = a + b
#define vadd_const(c, a, b)          \
  c.val[0] = vaddq_u16(a.val[0], b); \
  c.val[1] = vaddq_u16(a.val[1], b); \
  c.val[2] = vaddq_u16(a.val[2], b); \
  c.val[3] = vaddq_u16(a.val[3], b);

// c = a & b
#define vand(c, a, b)                \
  c.val[0] = vandq_u16(a.val[0], b); \
  c.val[1] = vandq_u16(a.val[1], b); \
  c.val[2] = vandq_u16(a.val[2], b); \
  c.val[3] = vandq_u16(a.val[3], b);

void POL2MSG(uint16_t *message_dec_unpacked, unsigned char *message_dec)
{

  int32_t i, j;

  for (j = 0; j < SABER_KEYBYTES; j++)
  {
    message_dec[j] = 0;
    for (i = 0; i < 8; i++)
      message_dec[j] = message_dec[j] | (message_dec_unpacked[j * 8 + i] << i);
  }
}

/*-----------------------------------------------------------------------------------
	This routine generates a=[Matrix K x K] of 256-coefficient polynomials
-------------------------------------------------------------------------------------*/

void BS2POLq(const unsigned char *bytes, uint16_t data[SABER_N])
{

  uint32_t j;
  uint32_t offset_data = 0, offset_byte = 0;

  offset_byte = 0;

  for (j = 0; j < SABER_N / 8; j++)
  {
    offset_byte = 13 * j;
    offset_data = 8 * j;
    data[offset_data + 0] = (bytes[offset_byte + 0] & (0xff)) | ((bytes[offset_byte + 1] & 0x1f) << 8);
    data[offset_data + 1] = (bytes[offset_byte + 1] >> 5 & (0x07)) | ((bytes[offset_byte + 2] & 0xff) << 3) | ((bytes[offset_byte + 3] & 0x03) << 11);
    data[offset_data + 2] = (bytes[offset_byte + 3] >> 2 & (0x3f)) | ((bytes[offset_byte + 4] & 0x7f) << 6);
    data[offset_data + 3] = (bytes[offset_byte + 4] >> 7 & (0x01)) | ((bytes[offset_byte + 5] & 0xff) << 1) | ((bytes[offset_byte + 6] & 0x0f) << 9);
    data[offset_data + 4] = (bytes[offset_byte + 6] >> 4 & (0x0f)) | ((bytes[offset_byte + 7] & 0xff) << 4) | ((bytes[offset_byte + 8] & 0x01) << 12);
    data[offset_data + 5] = (bytes[offset_byte + 8] >> 1 & (0x7f)) | ((bytes[offset_byte + 9] & 0x3f) << 7);
    data[offset_data + 6] = (bytes[offset_byte + 9] >> 6 & (0x03)) | ((bytes[offset_byte + 10] & 0xff) << 2) | ((bytes[offset_byte + 11] & 0x07) << 10);
    data[offset_data + 7] = (bytes[offset_byte + 11] >> 3 & (0x1f)) | ((bytes[offset_byte + 12] & 0xff) << 5);
  }
}

void GenMatrix(polyvec *a, const unsigned char *seed)
{
  unsigned int one_vector = 13 * SABER_N / 8;
  unsigned int byte_bank_length = SABER_K * SABER_K * one_vector;
  unsigned char buf[byte_bank_length];

  uint16_t temp_ar[SABER_N];

  int i, j, k;
  uint16_t mod = (SABER_Q - 1);

  shake128(buf, byte_bank_length, seed, SABER_SEEDBYTES);

  for (i = 0; i < SABER_K; i++)
  {
    for (j = 0; j < SABER_K; j++)
    {
      BS2POLq(buf + (i * SABER_K + j) * one_vector, temp_ar);
      for (k = 0; k < SABER_N; k++)
      {
        a[i].vec[j].coeffs[k] = (temp_ar[k]) & mod;
      }
    }
  }
}

void GenSecret(uint16_t r[SABER_K][SABER_N], const unsigned char *seed)
{

  uint32_t i;

  int32_t buf_size = SABER_MU * SABER_N * SABER_K / 8;

  uint8_t buf[buf_size];

  shake128(buf, buf_size, seed, SABER_NOISESEEDBYTES);

  for (i = 0; i < SABER_K; i++)
  {
    cbd(r[i], buf + i * SABER_MU * SABER_N / 8);
  }
}

void indcpa_kem_keypair(unsigned char *pk, unsigned char *sk)
{

  polyvec a[SABER_K];

  uint16_t skpv1[SABER_K][SABER_N];

  unsigned char seed[SABER_SEEDBYTES];
  unsigned char noiseseed[SABER_COINBYTES];
  int32_t i, j, k;

  //--------------NEON declaration------------------

  uint16x8_t modQ = vdupq_n_u16(SABER_Q - 1);
  uint16x8_t H1_neon = vdupq_n_u16(h1);
  uint16x8x4_t acc_neon;
  uint16x8x4_t res_neon;

  //--------------NEON declaration ends------------------
  uint16_t res[SABER_K][SABER_N];

  randombytes(seed, SABER_SEEDBYTES);

  shake128(seed, SABER_SEEDBYTES, seed, SABER_SEEDBYTES); // for not revealing system RNG state
  randombytes(noiseseed, SABER_COINBYTES);

  GenMatrix(a, seed); //sample matrix A

  GenSecret(skpv1, noiseseed);

  //------------------------do the matrix vector multiplication and rounding------------

  // Matrix-vector multiplication; Matrix in transposed order
  neon_matrix_vector_mul_transpose(res, SABER_Q, a, skpv1);

  // Now truncation
  for (i = 0; i < SABER_K; i++)
  { //shift right EQ-EP bits
    for (j = 0; j < SABER_N; j += 32)
    {
      vload(res_neon, &res[i][j]);
      // res[i][j] += H1_neon
      vadd_const(res_neon, res_neon, H1_neon);
      // res[i][j] >>= (SABER_EQ-SABER_EP)
      vsr(res_neon, res_neon, (SABER_EQ - SABER_EP));
      // res[i][j] &= modQ
      vand(res_neon, res_neon, modQ);
      vstore(&res[i][j], res_neon);
    }
  }

  //------------------Pack sk into byte string-------

  POLVEC2BS(sk, skpv1, SABER_Q);

  //------------------Pack pk into byte string-------
  POLVEC2BS(pk, res, SABER_P); // load the public-key into pk byte string

  for (i = 0; i < SABER_SEEDBYTES; i++)
  { // now load the seedbytes in PK. Easy since seed bytes are kept in byte format.
    pk[SABER_POLYVECCOMPRESSEDBYTES + i] = seed[i];
  }
}

void indcpa_kem_enc(unsigned char *message_received,
                    unsigned char *noiseseed,
                    const unsigned char *pk,
                    unsigned char *ciphertext)
{

  uint32_t i, j, k;
  polyvec a[SABER_K]; // skpv;
  unsigned char seed[SABER_SEEDBYTES];
  uint16_t pkcl[SABER_K][SABER_N]; //public key of received by the client

  uint16_t skpv1[SABER_K][SABER_N];
  uint16_t temp[SABER_K][SABER_N];
  uint16_t message[SABER_KEYBYTES * 8];

  unsigned char msk_c[SABER_SCALEBYTES_KEM];
  //--------------NEON declaration------------------
  uint16x8_t mod = vdupq_n_u16(SABER_Q - 1);
  uint16x8_t mod_p = vdupq_n_u16(SABER_P - 1);
  uint16x8_t H1_neon = vdupq_n_u16(h1);

  uint16x8x4_t tmp1_neon, tmp2_neon;

  //--------------NEON declaration ends------------------
  uint16_t accumulator[SABER_K][SABER_N];
  uint16_t vprime[SABER_N];

  for (i = 0; i < SABER_SEEDBYTES; i++)
  { // Load the seedbytes in the client seed from PK.
    seed[i] = pk[SABER_POLYVECCOMPRESSEDBYTES + i];
  }

  GenMatrix(a, seed);

  GenSecret(skpv1, noiseseed);

  //-------unpack the public_key
  BS2POLVEC(pk, pkcl, SABER_P);

  //-----------------matrix-vector multiplication and rounding

  // Matrix-vector multiplication;
  neon_matrix_vector_mul(accumulator, SABER_Q, a, skpv1);

  // Now truncation
  for (i = 0; i < SABER_K; i++)
  { //shift right EQ-EP bits
    for (j = 0; j < SABER_N; j += 32)
    {
      vload(tmp2_neon, &accumulator[i][j]);
      // accumulator[i][j] += H1_neon
      vadd_const(tmp2_neon, tmp2_neon, H1_neon);
      // accumulator[i][j] >>= (SABER_EQ-SABER_EP)
      vsr(tmp2_neon, tmp2_neon, (SABER_EQ - SABER_EP));
      // accumulator[i][j] &= mod
      vand(tmp2_neon, tmp2_neon, mod);

      //-----this result should be put in b_prime for later use in server.
      vstore(&temp[i][j], tmp2_neon);
    }
  }

  POLVEC2BS(ciphertext, temp, SABER_P); // Pack b_prime into ciphertext byte string

  //**************client matrix-vector multiplication ends******************//

  //------now calculate the v'

  // vector-vector scalar multiplication with mod p
  neon_vector_vector_mul(vprime, SABER_P, pkcl, skpv1);

  // unpack message_received;
  for (j = 0; j < SABER_KEYBYTES; j++)
  {
    for (i = 0; i < 8; i++)
    {
      message[8 * j + i] = ((message_received[j] >> i) & 0x01);
    }
  }

  // message encoding
  for (k = 0; k < SABER_N; k += 32)
  {
    vload(tmp1_neon, &message[k]);
    vsl(tmp1_neon, tmp1_neon, SABER_EP - 1);

    // Computation of v'+h1
    vload(tmp2_neon, &vprime[k]);
    vadd_const(tmp2_neon, tmp2_neon, H1_neon); //adding h1

    // SHIFTRIGHT(v'+h1-m mod p, EP-ET)
    vsub(tmp2_neon, tmp2_neon, tmp1_neon);
    vand(tmp2_neon, tmp2_neon, mod_p);
    vsr(tmp2_neon, tmp2_neon, (SABER_EP - SABER_ET));

    // Unpack
    vstore(&temp[0][k], tmp2_neon);
  }

#if Saber_type == 1
  SABER_pack_3bit(msk_c, temp[0]);
#elif Saber_type == 2
  SABER_pack_4bit(msk_c, temp[0]);
#elif Saber_type == 3
  SABER_pack_6bit(msk_c, temp[0]);
#endif

  for (j = 0; j < SABER_SCALEBYTES_KEM; j++)
  {
    ciphertext[SABER_CIPHERTEXTBYTES + j] = msk_c[j];
  }
}

void indcpa_kem_dec(const unsigned char *sk,
                    const unsigned char *ciphertext,
                    unsigned char message_dec[])
{
  uint32_t i;

  uint16_t sksv[SABER_K][SABER_N]; //secret key of the server

  uint16_t pksv[SABER_K][SABER_N];

  uint16_t message_dec_unpacked[SABER_KEYBYTES * 8]; // one element containes on decrypted bit;

  uint8_t scale_ar[SABER_SCALEBYTES_KEM];

  //--------------NEON declaration------------------
  uint16x8x4_t op_neon;
  uint16x8x4_t v_neon;
  //--------------NEON declaration ends------------------
  uint16_t op[SABER_N];
  uint16_t v[SABER_N];

  //-------unpack the public_key

  BS2POLVEC(sk, sksv, SABER_Q);         //sksv is the secret-key
  BS2POLVEC(ciphertext, pksv, SABER_P); //pksv is the ciphertext

  for (i = 0; i < SABER_SCALEBYTES_KEM; i++)
  {
    scale_ar[i] = ciphertext[SABER_CIPHERTEXTBYTES + i];
  }

#if Saber_type == 1
  SABER_un_pack3bit(scale_ar, op);
#elif Saber_type == 2
  SABER_un_pack4bit(scale_ar, op);
#elif Saber_type == 3
  SABER_un_pack6bit(scale_ar, op);
#endif

  // InnerProduct(b', s, mod p)
  neon_vector_vector_mul(v, SABER_P, pksv, sksv);

  uint16x8_t mod_p = vdupq_n_u16(SABER_P - 1);
  uint16x8_t vh2 = vdupq_n_u16(h2);
  for (i = 0; i < SABER_N; i += 32)
  {
    //addition of h2
    vload(v_neon, &v[i]);
    vadd_const(v_neon, v_neon, vh2);

    vload(op_neon, &op[i]);
    vsl(op_neon, op_neon, (SABER_EP - SABER_ET));

    vsub(v_neon, v_neon, op_neon);
    vand(v_neon, v_neon, mod_p);
    vsr(v_neon, v_neon, (SABER_EP - 1));

    vstore(&message_dec_unpacked[i], v_neon);
  }

  POL2MSG(message_dec_unpacked, message_dec);
}
