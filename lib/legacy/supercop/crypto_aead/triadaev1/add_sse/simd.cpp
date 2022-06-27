
#include "crypto_aead.h"
#include "api.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>

#ifdef _MSC_VER
  #include <Windows.h>
  #include <intrin.h>
  #include <immintrin.h>
  #define MIE_ALIGN(x) __declspec(align(x))
#else
  #include <x86intrin.h>
  #define MIE_ALIGN(x) __attribute__((aligned(x)))
#endif


/*==== display function ====*/
static void disp(__m128i in) {
  MIE_ALIGN(128)
  unsigned char tmp[16] = {0};
  _mm_store_si128((__m128i *)(tmp), in);

  //Output
  printf("O \t");
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      printf("%2x ", tmp[4 * i + j]);
    }
    printf(" | ");
  }
  printf("\n");
}

static void dispReg(__m128i in) {
  MIE_ALIGN(128)
  unsigned char tmp[16] = {0};
  _mm_store_si128((__m128i *)(tmp), in);

  //Output
  printf("O \t0x");
  for (int i = 3; i >= 0; i--) {
    for (int j = 3; j >= 0; j--) {
      printf("%02X", tmp[4 * i + j]);
    }
    if(i == 2) printf(" ");
  }
  printf("\n");
}

static void dispReg(__m128i inL, __m128i inR) {
  MIE_ALIGN(128)
  unsigned char tmpL[16] = {0};
  _mm_store_si128((__m128i *)(tmpL), inL);
  
  MIE_ALIGN(128)
  unsigned char tmpR[16] = {0};
  _mm_store_si128((__m128i *)(tmpR), inR);


  //Output
  printf("O \t0x");
  for (int i = 3; i >= 2; i--) {
    for (int j = 3; j >= 0; j--) {
      printf("%02X", tmpL[4 * i + j]);
    }

    if(i == 2) printf(" ");
  }


  //Output
  for (int i = 3; i >= 2; i--) {
    for (int j = 3; j >= 0; j--) {
      printf("%02X", tmpR[4 * i + j]);
    }

    if(i == 2) printf(" ");
  }
  
  //Output
  for (int i = 1; i >= 0; i--) {
    for (int j = 3; j >= 0; j--) {
      printf("%02X", tmpL[4 * i + j]);
    }

    if(i == 0) printf(" ");
  }


  //Output
  for (int i = 1; i >= 0; i--) {
    for (int j = 3; j >= 0; j--) {
      printf("%02X", tmpR[4 * i + j]);
    }

    if(i == 2) printf(" ");
  }

  printf("\n");
}

/*==== Triad Update for XMM Register ====*/
static inline void triad_update_encryption(const __m128i inL[3], const __m128i inR[3], __m128i outL[3]) {
  // REG B
  __m128i newb = _mm_xor_si128(_mm_slli_epi64(inL[1], 2), _mm_srli_epi64(inR[1], 64 - 2));
  __m128i ksb  = _mm_xor_si128(_mm_slli_epi64(inL[1], 24), _mm_srli_epi64(inR[1], 64 - 24));
  __m128i ffb  = inL[1];
  ksb = _mm_xor_si128(ksb, ffb);

  __m128i nlb1 = _mm_xor_si128(_mm_slli_epi64(inL[1], 23), _mm_srli_epi64(inR[1], 64 - 23));
  __m128i nlb2 = _mm_xor_si128(_mm_slli_epi64(inL[1], 1), _mm_srli_epi64(inR[1], 64 - 1));
  __m128i nlb3 = _mm_xor_si128(_mm_slli_epi64(inL[1], 21), _mm_srli_epi64(inR[1], 64 - 21));
  __m128i nlb = _mm_and_si128(nlb1, nlb2);

  __m128i outB = _mm_xor_si128(ksb, nlb);


  // REG C
  __m128i newc = _mm_xor_si128(_mm_slli_epi64(inL[2], 20), _mm_srli_epi64(inR[2], 64 - 20));
  outL[2] = _mm_xor_si128(newc, outB);
  __m128i ksc  = _mm_xor_si128(_mm_slli_epi64(inL[2], 24), _mm_srli_epi64(inR[2], 64 - 24));
  __m128i ffc  = _mm_xor_si128(_mm_slli_epi64(inL[2], 4), _mm_srli_epi64(inR[2], 64 - 4));
  ksc = _mm_xor_si128(ksc, ffc);

  __m128i nlc1 = _mm_xor_si128(_mm_slli_epi64(inL[2], 23), _mm_srli_epi64(inR[2], 64 - 23));
  __m128i nlc2 = _mm_xor_si128(_mm_slli_epi64(inL[2], 13), _mm_srli_epi64(inR[2], 64 - 13));
  __m128i nlc3 = _mm_xor_si128(_mm_slli_epi64(inL[2], 21), _mm_srli_epi64(inR[2], 64 - 21));
  __m128i nlc = _mm_and_si128(nlc1, nlc2);
  __m128i nld = _mm_and_si128(nlb3, nlc3);

  __m128i outC = _mm_xor_si128(ksc, nlc);


  // REG A
  __m128i newa = _mm_xor_si128(_mm_slli_epi64(inL[0], 10), _mm_srli_epi64(inR[0], 64 - 10));
  outL[0] = _mm_xor_si128(newa, outC);
  __m128i ksa  = _mm_xor_si128(_mm_slli_epi64(inL[0], 16), _mm_srli_epi64(inR[0], 64 - 16));
  __m128i ffa  = _mm_xor_si128(_mm_slli_epi64(inL[0], 4), _mm_srli_epi64(inR[0], 64 - 4));
  ksa = _mm_xor_si128(ksa, ffa);
  ksa = _mm_xor_si128(ksa, nld);

  __m128i nla1 = _mm_xor_si128(_mm_slli_epi64(inL[0], 15), _mm_srli_epi64(inR[0], 64 - 15));
  __m128i nla2 = _mm_xor_si128(_mm_slli_epi64(inL[0], 9), _mm_srli_epi64(inR[0], 64 - 9));
  __m128i nla = _mm_and_si128(nla1, nla2);

  __m128i outA = _mm_xor_si128(ksa, nla);

  //
  outL[1] = _mm_xor_si128(newb, outA);

  return;
}

/*==== Triad Update and Key Stream Generation for XMM Register ====*/
static inline void triad_update_encryption(const __m128i inL[3], const __m128i inR[3], __m128i outL[3], __m128i &ks) {
  // REG B
  __m128i newb = _mm_xor_si128(_mm_slli_epi64(inL[1], 2), _mm_srli_epi64(inR[1], 64 - 2));
  __m128i ksb  = _mm_xor_si128(_mm_slli_epi64(inL[1], 24), _mm_srli_epi64(inR[1], 64 - 24));
  __m128i ffb  = inL[1];
  ksb = _mm_xor_si128(ksb, ffb);

  __m128i nlb1 = _mm_xor_si128(_mm_slli_epi64(inL[1], 23), _mm_srli_epi64(inR[1], 64 - 23));
  __m128i nlb2 = _mm_xor_si128(_mm_slli_epi64(inL[1], 1), _mm_srli_epi64(inR[1], 64 - 1));
  __m128i nlb3 = _mm_xor_si128(_mm_slli_epi64(inL[1], 21), _mm_srli_epi64(inR[1], 64 - 21));
  __m128i nlb = _mm_and_si128(nlb1, nlb2);

  __m128i outB = _mm_xor_si128(ksb, nlb);


  // REG C
  __m128i newc = _mm_xor_si128(_mm_slli_epi64(inL[2], 20), _mm_srli_epi64(inR[2], 64 - 20));
  outL[2] = _mm_xor_si128(newc, outB);
  __m128i ksc  = _mm_xor_si128(_mm_slli_epi64(inL[2], 24), _mm_srli_epi64(inR[2], 64 - 24));
  __m128i ffc  = _mm_xor_si128(_mm_slli_epi64(inL[2], 4), _mm_srli_epi64(inR[2], 64 - 4));
  ksc = _mm_xor_si128(ksc, ffc);

  __m128i nlc1 = _mm_xor_si128(_mm_slli_epi64(inL[2], 23), _mm_srli_epi64(inR[2], 64 - 23));
  __m128i nlc2 = _mm_xor_si128(_mm_slli_epi64(inL[2], 13), _mm_srli_epi64(inR[2], 64 - 13));
  __m128i nlc3 = _mm_xor_si128(_mm_slli_epi64(inL[2], 21), _mm_srli_epi64(inR[2], 64 - 21));
  __m128i nlc = _mm_and_si128(nlc1, nlc2);
  __m128i nld = _mm_and_si128(nlb3, nlc3);

  __m128i outC = _mm_xor_si128(ksc, nlc);


  // REG A
  __m128i newa = _mm_xor_si128(_mm_slli_epi64(inL[0], 10), _mm_srli_epi64(inR[0], 64 - 10));
  outL[0] = _mm_xor_si128(newa, outC);
  __m128i ksa  = _mm_xor_si128(_mm_slli_epi64(inL[0], 16), _mm_srli_epi64(inR[0], 64 - 16));
  __m128i ffa  = _mm_xor_si128(_mm_slli_epi64(inL[0], 4), _mm_srli_epi64(inR[0], 64 - 4));
  ksa = _mm_xor_si128(ksa, ffa);
  ksa = _mm_xor_si128(ksa, nld);

  __m128i nla1 = _mm_xor_si128(_mm_slli_epi64(inL[0], 15), _mm_srli_epi64(inR[0], 64 - 15));
  __m128i nla2 = _mm_xor_si128(_mm_slli_epi64(inL[0], 9), _mm_srli_epi64(inR[0], 64 - 9));
  __m128i nla = _mm_and_si128(nla1, nla2);

  __m128i outA = _mm_xor_si128(ksa, nla);

  //
  outL[1] = _mm_xor_si128(newb, outA);

  ksa = _mm_xor_si128(ksa, ksb);
  ks = _mm_xor_si128(ksa, ksc);
  
  return;
}


/*==== AEAD Encryption ====*/
void triad_high_speed_encryption(unsigned char *c, const unsigned char *m, unsigned long long mlen, const unsigned char *ad, unsigned long long adlen, const unsigned char *npub, const unsigned char *k){
  unsigned char c_enc[4] = {0xFEU, 0xFFU, 0xFFU, 0xFFU};
  int i;

  MIE_ALIGN(128)
  unsigned char tmp0[16] = {0};
  memcpy(tmp0, k, 16);
  MIE_ALIGN(128)
  unsigned char tmp1[16] = {0};
  memcpy(tmp1 + 4, npub, 12);
  memcpy(tmp1, c_enc, 4);

  __m128i in[3];
  in[2] = _mm_load_si128((const __m128i *)(tmp0));
  in[1] = _mm_load_si128((const __m128i *)(tmp1));
  in[0] = _mm_unpacklo_epi8(in[2], in[1]);
  in[0] = _mm_slli_si128(in[0], 6);

  __m128i inL[3], inR[3];
  inL[0] = _mm_shuffle_epi32(in[0], 0xEE);
  inL[1] = _mm_shuffle_epi32(in[1], 0xEE);
  inL[2] = _mm_shuffle_epi32(in[2], 0xEE);
  inR[0] = _mm_broadcastq_epi64(in[0]);
  inR[1] = _mm_broadcastq_epi64(in[1]);
  inR[2] = _mm_broadcastq_epi64(in[2]);
  
  
  
  // initialization (64 + 15 * 64 = 1024 rounds)
  __m128i p_bar_input = _mm_set_epi8(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01);
  __m128i outL[3];
  triad_update_encryption(inL, inR, outL);
  memcpy(inR, inL, sizeof(inL));
  memcpy(inL, outL, sizeof(outL));

  inL[0] = _mm_xor_si128(inL[0], p_bar_input);
  inL[1] = _mm_xor_si128(inL[1], p_bar_input);
  inL[2] = _mm_xor_si128(inL[2], p_bar_input);

  for (i = 0; i < 15; i++){
    triad_update_encryption(inL, inR, outL);
    memcpy(inR, inL, sizeof(inL));
    memcpy(inL, outL, sizeof(outL));
  }
  
  __m128i s1024L[3] = {inL[0], inL[1], inL[2]};
  __m128i s1024R[3] = {inR[0], inR[1], inR[2]};

  

  // absorb associated data
  for (i = 0; i < (int)(adlen)-7; i += 8) {
    __m128i xmm_a = _mm_loadu_si128((const __m128i *)(ad + i));
    //xmm_a = _mm_and_si128(xmm_a, and_mask);

    triad_update_encryption(inL, inR, outL);
    memcpy(inR, inL, sizeof(inL));
    memcpy(inL, outL, sizeof(outL));

    inL[0] = _mm_xor_si128(inL[0], xmm_a);
    inL[1] = _mm_xor_si128(inL[1], xmm_a);
    inL[2] = _mm_xor_si128(inL[2], xmm_a);
  }

  // absorb last block of associated data
  unsigned char last_ad_block[16];
  memcpy(last_ad_block, ad + i, adlen - i);
  memcpy(last_ad_block + adlen - i, &adlen, 7);
  __m128i xmm_a = _mm_loadu_si128((const __m128i *)(last_ad_block));
  int last_block_length = adlen - i + 7;

  //
  if(last_block_length > 7){
    triad_update_encryption(inL, inR, outL);
    outL[0] = _mm_xor_si128(outL[0], xmm_a);
    outL[1] = _mm_xor_si128(outL[1], xmm_a);
    outL[2] = _mm_xor_si128(outL[2], xmm_a);

    memcpy(inR, inL, sizeof(inL));
    memcpy(inL, outL, sizeof(outL));

    xmm_a = _mm_srli_si128(xmm_a, 8);
    last_block_length -= 8;
  }

  //
  if(last_block_length > 0){
    triad_update_encryption(inL, inR, outL);
    outL[0] = _mm_xor_si128(outL[0], xmm_a);
    outL[1] = _mm_xor_si128(outL[1], xmm_a);
    outL[2] = _mm_xor_si128(outL[2], xmm_a);

    inR[0] = _mm_xor_si128(_mm_slli_epi64(inL[0], 64 - 8 * last_block_length), _mm_srli_epi64(inR[0], 8 * last_block_length) );
    inR[1] = _mm_xor_si128(_mm_slli_epi64(inL[1], 64 - 8 * last_block_length), _mm_srli_epi64(inR[1], 8 * last_block_length) );
    inR[2] = _mm_xor_si128(_mm_slli_epi64(inL[2], 64 - 8 * last_block_length), _mm_srli_epi64(inR[2], 8 * last_block_length) );

    inL[0] = _mm_xor_si128(_mm_slli_epi64(outL[0], 64 - 8 * last_block_length), _mm_srli_epi64(inL[0], 8 * last_block_length) );
    inL[1] = _mm_xor_si128(_mm_slli_epi64(outL[1], 64 - 8 * last_block_length), _mm_srli_epi64(inL[1], 8 * last_block_length) );
    inL[2] = _mm_xor_si128(_mm_slli_epi64(outL[2], 64 - 8 * last_block_length), _mm_srli_epi64(inL[2], 8 * last_block_length) );
  }

  // blend
  inL[0] = _mm_blend_epi32(inL[0], s1024L[0], 0xC);
  inL[1] = _mm_blend_epi32(inL[1], s1024L[1], 0xC);
  inL[2] = _mm_blend_epi32(inL[2], s1024L[2], 0xC);

  inR[0] = _mm_blend_epi32(inR[0], s1024R[0], 0xC);
  inR[1] = _mm_blend_epi32(inR[1], s1024R[1], 0xC);
  inR[2] = _mm_blend_epi32(inR[2], s1024R[2], 0xC);

  
  // absorb and encrypt message
  __m128i and_mask = _mm_set_epi8(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
  __m128i ks;
  for (i = 0; i < (int)(mlen)-8; i += 8)
  {
    triad_update_encryption(inL, inR, outL, ks);
    ks = _mm_srli_si128(ks, 8);

    // encrypt
    __m128i xmm_m = _mm_loadu_si128((const __m128i *)(m + i));
    __m128i xmm_c = _mm_xor_si128(xmm_m, ks);
    _mm_storeu_si128((__m128i *)(c + i), xmm_c);

    // absorb
    xmm_m = _mm_and_si128(xmm_m, and_mask);
    outL[0] = _mm_xor_si128(outL[0], xmm_m);
    outL[1] = _mm_xor_si128(outL[1], xmm_m);
    outL[2] = _mm_xor_si128(outL[2], xmm_m);

    memcpy(inR, inL, sizeof(inL));
    memcpy(inL, outL, sizeof(outL));
  }

  
  unsigned char last_m_block[16];
  memcpy(last_m_block, m + i, mlen - i);
  __m128i xmm_m = _mm_loadu_si128((const __m128i *)(last_m_block));
  last_block_length = mlen - i;

  if(last_block_length > 0){
    triad_update_encryption(inL, inR, outL, ks);
    ks = _mm_srli_si128(ks, 8);

    // encrypt
    __m128i xmm_c = _mm_xor_si128(xmm_m, ks);
    unsigned char last_ct[16];
    _mm_storeu_si128((__m128i *)(last_ct), xmm_c);
    memcpy(c + i, last_ct, last_block_length);


    // absorb
    xmm_m = _mm_and_si128(xmm_m, and_mask);
    outL[0] = _mm_xor_si128(outL[0], xmm_m);
    outL[1] = _mm_xor_si128(outL[1], xmm_m);
    outL[2] = _mm_xor_si128(outL[2], xmm_m);

    inR[0] = _mm_xor_si128(_mm_slli_epi64(inL[0], 64 - 8 * last_block_length), _mm_srli_epi64(inR[0], 8 * last_block_length) );
    inR[1] = _mm_xor_si128(_mm_slli_epi64(inL[1], 64 - 8 * last_block_length), _mm_srli_epi64(inR[1], 8 * last_block_length) );
    inR[2] = _mm_xor_si128(_mm_slli_epi64(inL[2], 64 - 8 * last_block_length), _mm_srli_epi64(inR[2], 8 * last_block_length) );

    inL[0] = _mm_xor_si128(_mm_slli_epi64(outL[0], 64 - 8 * last_block_length), _mm_srli_epi64(inL[0], 8 * last_block_length) );
    inL[1] = _mm_xor_si128(_mm_slli_epi64(outL[1], 64 - 8 * last_block_length), _mm_srli_epi64(inL[1], 8 * last_block_length) );
    inL[2] = _mm_xor_si128(_mm_slli_epi64(outL[2], 64 - 8 * last_block_length), _mm_srli_epi64(inL[2], 8 * last_block_length) );
  }
  
  // finalization
  triad_update_encryption(inL, inR, outL);
  memcpy(inR, inL, sizeof(inL));
  memcpy(inL, outL, sizeof(outL));

  inL[0] = _mm_xor_si128(inL[0], p_bar_input);
  inL[1] = _mm_xor_si128(inL[1], p_bar_input);
  inL[2] = _mm_xor_si128(inL[2], p_bar_input);

  for (i = 0; i < 15; i++){
    triad_update_encryption(inL, inR, outL);
    memcpy(inR, inL, sizeof(inL));
    memcpy(inL, outL, sizeof(outL));
  }
  

  // tag generation
  unsigned char tag[32];
  triad_update_encryption(inL, inR, outL, ks);
  _mm_storeu_si128((__m128i *)(tag), ks);

  memcpy(c + mlen, tag, 8);
}

/*==== AEAD Decryption ====*/
void triad_high_speed_decryption(unsigned char *tag, unsigned char *rm, const unsigned char *c, unsigned long long clen, const unsigned char *ad, unsigned long long adlen, const unsigned char *npub, const unsigned char *k){
  unsigned char c_enc[4] = {0xFEU, 0xFFU, 0xFFU, 0xFFU};
  int i;

  MIE_ALIGN(128)
  unsigned char tmp0[16] = {0};
  memcpy(tmp0, k, 16);
  MIE_ALIGN(128)
  unsigned char tmp1[16] = {0};
  memcpy(tmp1 + 4, npub, 12);
  memcpy(tmp1, c_enc, 4);

  __m128i in[3];
  in[2] = _mm_load_si128((const __m128i *)(tmp0));
  in[1] = _mm_load_si128((const __m128i *)(tmp1));
  in[0] = _mm_unpacklo_epi8(in[2], in[1]);
  in[0] = _mm_slli_si128(in[0], 6);

  __m128i inL[3], inR[3];
  inL[0] = _mm_shuffle_epi32(in[0], 0xEE);
  inL[1] = _mm_shuffle_epi32(in[1], 0xEE);
  inL[2] = _mm_shuffle_epi32(in[2], 0xEE);
  inR[0] = _mm_broadcastq_epi64(in[0]);
  inR[1] = _mm_broadcastq_epi64(in[1]);
  inR[2] = _mm_broadcastq_epi64(in[2]);
  
  
  
  // initialization (64 + 15 * 64 = 1024 rounds)
  __m128i p_bar_input = _mm_set_epi8(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01);
  __m128i outL[3];
  triad_update_encryption(inL, inR, outL);
  memcpy(inR, inL, sizeof(inL));
  memcpy(inL, outL, sizeof(outL));

  inL[0] = _mm_xor_si128(inL[0], p_bar_input);
  inL[1] = _mm_xor_si128(inL[1], p_bar_input);
  inL[2] = _mm_xor_si128(inL[2], p_bar_input);

  for (i = 0; i < 15; i++){
    triad_update_encryption(inL, inR, outL);
    memcpy(inR, inL, sizeof(inL));
    memcpy(inL, outL, sizeof(outL));
  }
  
  __m128i s1024L[3] = {inL[0], inL[1], inL[2]};
  __m128i s1024R[3] = {inR[0], inR[1], inR[2]};

  

  // absorb associated data
  for (i = 0; i < (int)(adlen)-7; i += 8) {
    __m128i xmm_a = _mm_loadu_si128((const __m128i *)(ad + i));
    //xmm_a = _mm_and_si128(xmm_a, and_mask);

    triad_update_encryption(inL, inR, outL);
    memcpy(inR, inL, sizeof(inL));
    memcpy(inL, outL, sizeof(outL));

    inL[0] = _mm_xor_si128(inL[0], xmm_a);
    inL[1] = _mm_xor_si128(inL[1], xmm_a);
    inL[2] = _mm_xor_si128(inL[2], xmm_a);
  }

  // absorb last block of associated data
  unsigned char last_ad_block[16];
  memcpy(last_ad_block, ad + i, adlen - i);
  memcpy(last_ad_block + adlen - i, &adlen, 7);
  __m128i xmm_a = _mm_loadu_si128((const __m128i *)(last_ad_block));
  int last_block_length = adlen - i + 7;

  //
  if(last_block_length > 7){
    triad_update_encryption(inL, inR, outL);
    outL[0] = _mm_xor_si128(outL[0], xmm_a);
    outL[1] = _mm_xor_si128(outL[1], xmm_a);
    outL[2] = _mm_xor_si128(outL[2], xmm_a);

    memcpy(inR, inL, sizeof(inL));
    memcpy(inL, outL, sizeof(outL));

    xmm_a = _mm_srli_si128(xmm_a, 8);
    last_block_length -= 8;
  }

  //
  if(last_block_length > 0){
    triad_update_encryption(inL, inR, outL);
    outL[0] = _mm_xor_si128(outL[0], xmm_a);
    outL[1] = _mm_xor_si128(outL[1], xmm_a);
    outL[2] = _mm_xor_si128(outL[2], xmm_a);

    inR[0] = _mm_xor_si128(_mm_slli_epi64(inL[0], 64 - 8 * last_block_length), _mm_srli_epi64(inR[0], 8 * last_block_length) );
    inR[1] = _mm_xor_si128(_mm_slli_epi64(inL[1], 64 - 8 * last_block_length), _mm_srli_epi64(inR[1], 8 * last_block_length) );
    inR[2] = _mm_xor_si128(_mm_slli_epi64(inL[2], 64 - 8 * last_block_length), _mm_srli_epi64(inR[2], 8 * last_block_length) );

    inL[0] = _mm_xor_si128(_mm_slli_epi64(outL[0], 64 - 8 * last_block_length), _mm_srli_epi64(inL[0], 8 * last_block_length) );
    inL[1] = _mm_xor_si128(_mm_slli_epi64(outL[1], 64 - 8 * last_block_length), _mm_srli_epi64(inL[1], 8 * last_block_length) );
    inL[2] = _mm_xor_si128(_mm_slli_epi64(outL[2], 64 - 8 * last_block_length), _mm_srli_epi64(inL[2], 8 * last_block_length) );
  }

  // blend
  inL[0] = _mm_blend_epi32(inL[0], s1024L[0], 0xC);
  inL[1] = _mm_blend_epi32(inL[1], s1024L[1], 0xC);
  inL[2] = _mm_blend_epi32(inL[2], s1024L[2], 0xC);

  inR[0] = _mm_blend_epi32(inR[0], s1024R[0], 0xC);
  inR[1] = _mm_blend_epi32(inR[1], s1024R[1], 0xC);
  inR[2] = _mm_blend_epi32(inR[2], s1024R[2], 0xC);

  // absorb and decrypt message
  __m128i and_mask = _mm_set_epi8(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
  __m128i ks;
  for (i = 0; i < (int)(clen)-16; i += 8)
  {
    triad_update_encryption(inL, inR, outL, ks);
    ks = _mm_srli_si128(ks, 8);

    // decrypt
    __m128i xmm_c = _mm_loadu_si128((const __m128i *)(c + i));
    __m128i xmm_m = _mm_xor_si128(xmm_c, ks);
    _mm_storeu_si128((__m128i *)(rm + i), xmm_m);

    // absorb
    xmm_m = _mm_and_si128(xmm_m, and_mask);
    outL[0] = _mm_xor_si128(outL[0], xmm_m);
    outL[1] = _mm_xor_si128(outL[1], xmm_m);
    outL[2] = _mm_xor_si128(outL[2], xmm_m);

    memcpy(inR, inL, sizeof(inL));
    memcpy(inL, outL, sizeof(outL));
  }

  
  unsigned char last_c_block[16];
  memcpy(last_c_block, c + i, clen - 8 - i);
  __m128i xmm_c = _mm_loadu_si128((const __m128i *)(last_c_block));
  last_block_length = clen - 8 - i;

  if(last_block_length > 0){
    triad_update_encryption(inL, inR, outL, ks);
    ks = _mm_srli_si128(ks, 8);

    // decrypt
    __m128i xmm_m = _mm_xor_si128(xmm_c, ks);
    unsigned char last_rm[16];
    _mm_storeu_si128((__m128i *)(last_rm), xmm_m);
    memcpy(rm + i, last_rm, last_block_length);


    // absorb
    xmm_m = _mm_and_si128(xmm_m, and_mask);
    outL[0] = _mm_xor_si128(outL[0], xmm_m);
    outL[1] = _mm_xor_si128(outL[1], xmm_m);
    outL[2] = _mm_xor_si128(outL[2], xmm_m);

    inR[0] = _mm_xor_si128(_mm_slli_epi64(inL[0], 64 - 8 * last_block_length), _mm_srli_epi64(inR[0], 8 * last_block_length) );
    inR[1] = _mm_xor_si128(_mm_slli_epi64(inL[1], 64 - 8 * last_block_length), _mm_srli_epi64(inR[1], 8 * last_block_length) );
    inR[2] = _mm_xor_si128(_mm_slli_epi64(inL[2], 64 - 8 * last_block_length), _mm_srli_epi64(inR[2], 8 * last_block_length) );

    inL[0] = _mm_xor_si128(_mm_slli_epi64(outL[0], 64 - 8 * last_block_length), _mm_srli_epi64(inL[0], 8 * last_block_length) );
    inL[1] = _mm_xor_si128(_mm_slli_epi64(outL[1], 64 - 8 * last_block_length), _mm_srli_epi64(inL[1], 8 * last_block_length) );
    inL[2] = _mm_xor_si128(_mm_slli_epi64(outL[2], 64 - 8 * last_block_length), _mm_srli_epi64(inL[2], 8 * last_block_length) );
  }
  
  // finalization
  triad_update_encryption(inL, inR, outL);
  memcpy(inR, inL, sizeof(inL));
  memcpy(inL, outL, sizeof(outL));

  inL[0] = _mm_xor_si128(inL[0], p_bar_input);
  inL[1] = _mm_xor_si128(inL[1], p_bar_input);
  inL[2] = _mm_xor_si128(inL[2], p_bar_input);

  for (i = 0; i < 15; i++){
    triad_update_encryption(inL, inR, outL);
    memcpy(inR, inL, sizeof(inL));
    memcpy(inL, outL, sizeof(outL));
  }
  

  // tag generation
  unsigned char tmp[32];
  triad_update_encryption(inL, inR, outL, ks);
  _mm_storeu_si128((__m128i *)(tmp), ks);

  memcpy(tag, tmp, 8);

}



int crypto_aead_encrypt(
  unsigned char *c,
  unsigned long long *clen,
  const unsigned char *m,
  unsigned long long mlen,
  const unsigned char *ad,
  unsigned long long adlen,
  const unsigned char *nsec,
  const unsigned char *npub,
  const unsigned char *k)
{

  triad_high_speed_encryption(c, m, mlen, ad, adlen, npub, k);
  *clen = mlen + CRYPTO_ABYTES;

  return 0;
}

int crypto_aead_decrypt(
  unsigned char *m,
  unsigned long long *mlen,
  unsigned char *nsec,
  const unsigned char *c,
  unsigned long long clen,
  const unsigned char *ad,
  unsigned long long adlen,
  const unsigned char *npub,
  const unsigned char *k)
{
  //printf("\n");

  unsigned char tag[CRYPTO_ABYTES];
  int i;
  int flag = 0;
  *mlen = clen - CRYPTO_ABYTES;


  triad_high_speed_decryption(tag, m, c, clen, ad, adlen, npub, k);


  for (i = 0; i < 8; i++) {
    if (tag[i] != c[*mlen + i]) {
      flag = -1;
    }
  }
  return(flag);
}
