#include <stdio.h>
#include <string.h>
#include <immintrin.h>
#include "api.h"
#include "isap.h"

typedef unsigned char u8;
typedef unsigned long long u64;
typedef unsigned long u32;
typedef long long i64;

const u8 ISAP_IV1[] = {0x01,ISAP_K,ISAP_rH,ISAP_rB,ISAP_sH,ISAP_sB,ISAP_sE,ISAP_sK};
const u8 ISAP_IV2[] = {0x02,ISAP_K,ISAP_rH,ISAP_rB,ISAP_sH,ISAP_sB,ISAP_sE,ISAP_sK};
const u8 ISAP_IV3[] = {0x03,ISAP_K,ISAP_rH,ISAP_rB,ISAP_sH,ISAP_sB,ISAP_sE,ISAP_sK};

#define RATE (64 / 8)
#define PA_ROUNDS 12
#define PB_ROUNDS 6

#define ROTR(x,n) (((x)>>(n))|((x)<<(64-(n))))
#define EXT_BYTE(x,n) ((u8)((u64)(x)>>(8*(7-(n)))))
#define INS_BYTE(x,n) ((u64)(x)<<(8*(7-(n))))

#define U64BIG(x) \
    ((ROTR(x, 8) & (0xFF000000FF000000ULL)) | \
     (ROTR(x,24) & (0x00FF000000FF0000ULL)) | \
     (ROTR(x,40) & (0x0000FF000000FF00ULL)) | \
     (ROTR(x,56) & (0x000000FF000000FFULL)))

#define ROUND(C) ({\
    x2 ^= C;\
    x0 ^= x4;\
    x4 ^= x3;\
    x2 ^= x1;\
    t0 = x0;\
    t4 = x4;\
    t3 = x3;\
    t1 = x1;\
    t2 = x2;\
    x0 = t0 ^ ((~t1) & t2);\
    x2 = t2 ^ ((~t3) & t4);\
    x4 = t4 ^ ((~t0) & t1);\
    x1 = t1 ^ ((~t2) & t3);\
    x3 = t3 ^ ((~t4) & t0);\
    x1 ^= x0;\
    t1  = x1;\
    x1 = ROTR(x1, R[1][0]);\
    x3 ^= x2;\
    t2  = x2;\
    x2 = ROTR(x2, R[2][0]);\
    t4  = x4;\
    t2 ^= x2;\
    x2 = ROTR(x2, R[2][1] - R[2][0]);\
    t3  = x3;\
    t1 ^= x1;\
    x3 = ROTR(x3, R[3][0]);\
    x0 ^= x4;\
    x4 = ROTR(x4, R[4][0]);\
    t3 ^= x3;\
    x2 ^= t2;\
    x1 = ROTR(x1, R[1][1] - R[1][0]);\
    t0  = x0;\
    x2 = ~x2;\
    x3 = ROTR(x3, R[3][1] - R[3][0]);\
    t4 ^= x4;\
    x4 = ROTR(x4, R[4][1] - R[4][0]);\
    x3 ^= t3;\
    x1 ^= t1;\
    x0 = ROTR(x0, R[0][0]);\
    x4 ^= t4;\
    t0 ^= x0;\
    x0 = ROTR(x0, R[0][1] - R[0][0]);\
    x0 ^= t0;\
})

#define ROUNDAVX(C1,C2) ({\
    x2a = _mm_ternarylogic_epi64(x2a, _mm_set_epi64x (C1, C2), x1a, 0x96);\
    x0a =_mm_xor_si128 (x0a, x4a);\
    x4a =_mm_xor_si128 (x4a, x3a);\
    t0a = x0a;\
    t4a = x4a;\
    t3a = x3a;\
    t1a = x1a;\
    t2a = x2a;\
    x0a = _mm_ternarylogic_epi64(t0a, t1a, t2a, 0xd2);\
    x2a = _mm_ternarylogic_epi64(t2a, t3a, t4a, 0xd2);\
    x4a = _mm_ternarylogic_epi64(t4a, t0a, t1a, 0xd2);\
    x1a = _mm_ternarylogic_epi64(t1a, t2a, t3a, 0xd2);\
    x3a = _mm_ternarylogic_epi64(t3a, t4a, t0a, 0xd2);\
    x1a =_mm_xor_si128 (x1a, x0a);\
    x3a =_mm_xor_si128 (x3a, x2a);\
    x0a =_mm_xor_si128 (x0a, x4a);\
    t0a = _mm_ror_epi64(x0a, R[0][0]);\
    t1a = _mm_ror_epi64(x0a, R[0][1]);\
    t2a = _mm_ror_epi64(x1a, R[1][0]);\
    t3a = _mm_ror_epi64(x1a, R[1][1]);\
    x0a = _mm_ternarylogic_epi64(x0a, t0a, t1a, 0x96);\
    x1a = _mm_ternarylogic_epi64(x1a, t2a, t3a, 0x96);\
    t0a = _mm_ror_epi64(x2a, R[2][0]);\
    t1a = _mm_ror_epi64(x2a, R[2][1]);\
    t2a = _mm_ror_epi64(x3a, R[3][0]);\
    t3a = _mm_ror_epi64(x3a, R[3][1]);\
    x2a = _mm_ternarylogic_epi64(x2a, t0a, t1a, 0x69);\
    x3a = _mm_ternarylogic_epi64(x3a, t2a, t3a, 0x96);\
    t0a = _mm_ror_epi64(x4a, R[4][0]);\
    t1a = _mm_ror_epi64(x4a, R[4][1]);\
    x4a = _mm_ternarylogic_epi64(x4a, t0a, t1a, 0x96);\
})

#define P12 ({\
    ROUND(0xf0);\
    ROUND(0xe1);\
    ROUND(0xd2);\
    ROUND(0xc3);\
    ROUND(0xb4);\
    ROUND(0xa5);\
    ROUND(0x96);\
    ROUND(0x87);\
    ROUND(0x78);\
    ROUND(0x69);\
    ROUND(0x5a);\
    ROUND(0x4b);\
})

#define P6 ({\
    ROUND(0x96);\
    ROUND(0x87);\
    ROUND(0x78);\
    ROUND(0x69);\
    ROUND(0x5a);\
    ROUND(0x4b);\
})

#define P6_avx_first ({\
    ROUNDAVX(0xf0,0x96);\
    ROUNDAVX(0xe1,0x87);\
    ROUNDAVX(0xd2,0x78);\
    ROUNDAVX(0xc3,0x69);\
    ROUNDAVX(0xb4,0x5a);\
    ROUNDAVX(0xa5,0x4b);\
})

#define P6_avx_second ({\
    ROUNDAVX(0x96,0x96);\
    ROUNDAVX(0x87,0x87);\
    ROUNDAVX(0x78,0x78);\
    ROUNDAVX(0x69,0x69);\
    ROUNDAVX(0x5a,0x5a);\
    ROUNDAVX(0x4b,0x4b);\
})

#define P1 ({\
    ROUND(0x4b);\
})

static const int R[5][2] = {
    {19, 28}, {39, 61}, {1, 6}, {10, 17}, {7, 41}
};

#define ABSORB_LANES(src, len) ({ \
    u32 rem_bytes = len; \
    u64 *src64 = (u64 *)src; \
    u32 idx64 = 0; \
    while(1){ \
        if(rem_bytes>ISAP_rH_SZ){ \
            x0 ^= U64BIG(src64[idx64]); \
            idx64++; \
            P12; \
            rem_bytes -= ISAP_rH_SZ; \
        } else if(rem_bytes==ISAP_rH_SZ){ \
            x0 ^= U64BIG(src64[idx64]); \
            P12; \
            x0 ^= 0x8000000000000000ULL; \
            break; \
        } else { \
            u64 lane64; \
            u8 *lane8 = (u8 *)&lane64; \
            u32 idx8 = idx64*8; \
            for (u32 i = 0; i < 8; i++) { \
                if(i<(rem_bytes)){ \
                    lane8[i] = src[idx8]; \
                    idx8++; \
                } else if(i==rem_bytes){ \
                    lane8[i] = 0x80; \
                } else { \
                    lane8[i] = 0x00; \
                } \
            } \
            x0 ^= U64BIG(lane64); \
            break; \
        } \
    } \
})

/******************************************************************************/
/*                                   IsapRk                                   */
/******************************************************************************/

void isap_rk(
	const u8 *k,
	const u8 *iv,
	const u8 *y,
	const u64 ylen,
	u8 *out,
	const u64 outlen
){
    const u64 *k64 = (u64 *)k;
    const u64 *iv64 = (u64 *)iv;
    u64 *out64 = (u64 *)out;
    u64 x0, x1, x2, x3, x4;
    u64 t0, t1, t2, t3, t4;

    // Init state
    t0 = t1 = t2 = t3 = t4 = 0;
    x0 = U64BIG(k64[0]);
    x1 = U64BIG(k64[1]);
    x2 = U64BIG(iv64[0]);
    x3 = x4 = 0;
    P12;

    // Absorb Y
    for (size_t i = 0; i < ylen*8-1; i++){
        size_t cur_byte_pos = i/8;
        size_t cur_bit_pos = 7-(i%8);
        u8 cur_bit = ((y[cur_byte_pos] >> (cur_bit_pos)) & 0x01) << 7;
        x0 ^= ((u64)cur_bit) << 56;
        P1;
    }
    u8 cur_bit = ((y[ylen-1]) & 0x01) << 7;
    x0 ^= ((u64)cur_bit) << 56;
    P12;

    // Extract K*
    out64[0] = U64BIG(x0);
    out64[1] = U64BIG(x1);
    if(outlen == 24){
        out64[2] = U64BIG(x2);
    }
}

/******************************************************************************/
/*                                  IsapMac                                   */
/******************************************************************************/

void isap_mac(
    const u8 *k,
    const u8 *npub,
    const u8 *ad, const u64 adlen,
    const u8 *c, const u64 clen,
    u8 *tag
){
    u8 state[ISAP_STATE_SZ];
    const u64 *npub64 = (u64 *)npub;
    u64 *state64 = (u64 *)state;
    u64 x0, x1, x2, x3, x4;
    u64 t0, t1, t2, t3, t4;
    t0 = t1 = t2 = t3 = t4 = 0;

    // Init state
    x0 = U64BIG(npub64[0]);
    x1 = U64BIG(npub64[1]);
    x2 = U64BIG(((u64 *)ISAP_IV1)[0]);
    x3 = x4 = 0;
    P12;

    // Absorb AD
    ABSORB_LANES(ad,adlen);

    // Domain seperation
    P12;
    x4 ^= 0x0000000000000001ULL;

    // Absorb C
    ABSORB_LANES(c,clen);
    P12;

    // Derive K*
    state64[0] = U64BIG(x0);
    state64[1] = U64BIG(x1);
    state64[2] = U64BIG(x2);
    state64[3] = U64BIG(x3);
    state64[4] = U64BIG(x4);
    isap_rk(k,ISAP_IV2,(u8 *)state64,CRYPTO_KEYBYTES,(u8 *)state64,CRYPTO_KEYBYTES);
    x0 = U64BIG(state64[0]);
    x1 = U64BIG(state64[1]);
    x2 = U64BIG(state64[2]);
    x3 = U64BIG(state64[3]);
    x4 = U64BIG(state64[4]);

    // Squeeze tag
    P12;
    unsigned long long *tag64 = (u64 *)tag;
    tag64[0] = U64BIG(x0);
    tag64[1] = U64BIG(x1);
}

/******************************************************************************/
/*                                  IsapEnc                                   */
/******************************************************************************/

void isap_enc(
	const u8 *k,
	const u8 *npub,
	const u8 *m,
    const u64 mlen,
	u8 *c
){
    u8 state[ISAP_STATE_SZ];

    // Init state
    u64 *state64 = (u64 *)state;
    u64 *npub64 = (u64 *)npub;
    isap_rk(k,ISAP_IV3,npub,CRYPTO_NPUBBYTES,state,ISAP_STATE_SZ-CRYPTO_NPUBBYTES);
    u64 x0, x1, x2, x3, x4;
    u64 t0, t1, t2, t3, t4;
    t0 = t1 = t2 = t3 = t4 = 0;
    x0 = U64BIG(state64[0]);
    x1 = U64BIG(state64[1]);
    x2 = U64BIG(state64[2]);
    x3 = U64BIG(npub64[0]);
    x4 = U64BIG(npub64[1]);
    P6;

    // Squeeze key stream
    u64 rem_bytes = mlen;
    u64 *m64 = (u64 *)m;
    u64 *c64 = (u64 *)c;
    u32 idx64 = 0;
    while(1){
        if(rem_bytes>ISAP_rH_SZ){
            // Squeeze full lane
            c64[idx64] = U64BIG(x0) ^ m64[idx64];
            idx64++;
            P6;
            rem_bytes -= ISAP_rH_SZ;
        } else if(rem_bytes==ISAP_rH_SZ){
            // Squeeze full lane and stop
            c64[idx64] = U64BIG(x0) ^ m64[idx64];
            break;
        } else {
            // Squeeze partial lane and stop
            u64 lane64 = U64BIG(x0);
            u8 *lane8 = (u8 *)&lane64;
            u32 idx8 = idx64*8;
            for (u32 i = 0; i < rem_bytes; i++) {
                c[idx8] = lane8[i] ^ m[idx8];
                idx8++;
            }
            break;
        }
    }
}


/******************************************************************************/
/*                             IsapMac and Enc                                */
/******************************************************************************/

void isap_mac_enc(
    const u8 *k,
    const u8 *npub,
    const u8 *ad, const u64 adlen,
    u8 *c, const u64 clen,
    const u8 *m, const u64 mlen,
    u8 *tag
){
    u8 state_mac[ISAP_STATE_SZ];
    const u64 *npub64 = (u64 *)npub;
    u64 *state_mac64 = (u64 *)state_mac;
    u64 x0, x1, x2, x3, x4;
    u64 t0, t1, t2, t3, t4;
    __m128i x0a, x1a, x2a, x3a, x4a;
    __m128i t0a, t1a, t2a, t3a, t4a;
    t0 = t1 = t2 = t3 = t4 = 0;
    
    u8 state_enc[ISAP_STATE_SZ];

    // Init state_enc
    u64 *state_enc64 = (u64 *)state_enc;
    isap_rk(k,ISAP_IV3,npub,CRYPTO_NPUBBYTES,state_enc,ISAP_STATE_SZ-CRYPTO_NPUBBYTES);
    t0 = t1 = t2 = t3 = t4 = 0;
     
    state_enc64[3] = npub64[0];
    state_enc64[4] = npub64[1];
    
    // Init state_mac
    x0 = U64BIG(npub64[0]);
    x1 = U64BIG(npub64[1]);
    x2 = U64BIG(((u64 *)ISAP_IV1)[0]);
    x3 = x4 = 0;
    P12;

    // Absorb AD
    ABSORB_LANES(ad,adlen);
    
        
   //Initialize AVX registers
   t0 = U64BIG(state_enc64[0]);
   t1 = U64BIG(state_enc64[1]);
   t2 = U64BIG(state_enc64[2]);
   t3 = U64BIG(state_enc64[3]);
   t4 = U64BIG(state_enc64[4]);
        
        
   u64 tmp[2];
   tmp[1] = x0;
   tmp[0] = t0;
   x0a = _mm_maskz_loadu_epi64 (3, tmp);
   tmp[1] = x1;
   tmp[0] = t1;
   x1a = _mm_maskz_loadu_epi64 (3, tmp);
   tmp[1] = x2;
   tmp[0] = t2;
   x2a = _mm_maskz_loadu_epi64 (3, tmp);
   tmp[1] = x3;
   tmp[0] = t3;
   x3a = _mm_maskz_loadu_epi64 (3, tmp);
   tmp[1] = x4;
   tmp[0] = t4;
   x4a = _mm_maskz_loadu_epi64 (3, tmp);
    


    // Squeeze key stream while absorbing ciphertext
    long long rem_enc_bytes = mlen;
    u64 tmpc1,tmpc2;
    u64 tmpm1,tmpm2;
    u64 encbytes1,encbytes2;
    u32 idx8_enc = 0;
    u32 idx8_mac = 0; 
    u64 tmpc_mac[2];
    __m128i domain_separation = _mm_set_epi64x (0x0000000000000001ULL, 0);
    long long rem_mac_bytes = clen; 
    do{
        //prepare plaintext to encrypt
        tmpm1 = 0;
        tmpm2 = 0;
        if(rem_enc_bytes>=ISAP_rH_SZ)
          encbytes1 = 8;
        else
          encbytes1 = rem_enc_bytes;
        if((rem_enc_bytes-encbytes1)>=ISAP_rH_SZ)
          encbytes2 = 8;
        else
          if(encbytes1 < 8)
            encbytes2 = 0;
          else
            encbytes2 = rem_enc_bytes - encbytes1;
        for (u32 i = 0; i < encbytes1; i++) 
          tmpm1 = (tmpm1 << 8) | ((u64)m[idx8_enc+(encbytes1-i-1)]);
        for (u32 i = 0; i < encbytes2; i++) 
          tmpm2 = (tmpm2 << 8) | ((u64)m[idx8_enc+encbytes1+(encbytes2-i-1)]);
          
        //prepare ciphertext to authenticate
        tmpc_mac[1] = 0; 
        if(rem_enc_bytes < mlen){
          u8 *lane8 = (u8 *)&tmpc_mac[1];  
          for (u32 i = 0; i < 8; i++) { 
            if(i<(rem_mac_bytes)){ 
                lane8[i] = c[idx8_mac]; 
                idx8_mac++; 
            } else if(i==rem_mac_bytes){ 
                lane8[i] = 0x80; 
            } else { 
                lane8[i] = 0x00; 
            } 
          }
          tmpc_mac[1] = U64BIG(tmpc_mac[1]);
          rem_mac_bytes -= ISAP_rH_SZ;
        }
        
        
        
        x0a = _mm_xor_si128 (x0a, _mm_maskz_loadu_epi64 (2, tmpc_mac));
        P6_avx_first;
        _mm_mask_storeu_epi64 (tmp, 1, x0a);
        tmpc1 = U64BIG(tmp[0]) ^ tmpm1;
        P6_avx_second;
        _mm_mask_storeu_epi64 (tmp, 1, x0a);
        tmpc2 = U64BIG(tmp[0]) ^ tmpm2;
        x4a = _mm_xor_si128 (x4a, domain_separation);
        domain_separation = _mm_andnot_si128 (domain_separation,domain_separation);
       
        

        // Squeeze  lane 
        u8 *lane8 = (u8 *)&tmpc1;
        for (u32 i = 0; i < encbytes1; i++) {
            c[idx8_enc] = lane8[i];
            idx8_enc++;
        }
        lane8 = (u8 *)&tmpc2;
        for (u32 i = 0; i < encbytes2; i++) {
            c[idx8_enc] = lane8[i];
            idx8_enc++;
        }
        rem_enc_bytes -= 2*ISAP_rH_SZ;
        
    }while(rem_enc_bytes>0);
    //end encrypting
    
    
    //Restore MAC state
    _mm_mask_storeu_epi64 (tmp, 2, x0a);
    x0 = tmp[1];
    _mm_mask_storeu_epi64 (tmp, 2, x1a);
    x1 = tmp[1];
    _mm_mask_storeu_epi64 (tmp, 2, x2a);
    x2 = tmp[1];
    _mm_mask_storeu_epi64 (tmp, 2, x3a);
    x3 = tmp[1];
    _mm_mask_storeu_epi64 (tmp, 2, x4a);
    x4 = tmp[1];
  

    // Absorb  rest of C
    while(rem_mac_bytes>=0){ 
        tmpc_mac[1] = 0; 
        u8 *lane8 = (u8 *)&tmpc_mac[1];  
        for (u32 i = 0; i < 8; i++) { 
          if(i<(rem_mac_bytes)){ 
              lane8[i] = c[idx8_mac]; 
              idx8_mac++; 
          } else if(i==rem_mac_bytes){ 
              lane8[i] = 0x80; 
          } else { 
              lane8[i] = 0x00; 
          } 
        } 
        x0 ^= U64BIG(tmpc_mac[1]);
        P12;
        rem_mac_bytes -= ISAP_rH_SZ;
    } 


    // Derive K*
    state_mac64[0] = U64BIG(x0);
    state_mac64[1] = U64BIG(x1);
    state_mac64[2] = U64BIG(x2);
    state_mac64[3] = U64BIG(x3);
    state_mac64[4] = U64BIG(x4);
    isap_rk(k,ISAP_IV2,(u8 *)state_mac64,CRYPTO_KEYBYTES,(u8 *)state_mac64,CRYPTO_KEYBYTES);
    x0 = U64BIG(state_mac64[0]);
    x1 = U64BIG(state_mac64[1]);
    x2 = U64BIG(state_mac64[2]);
    x3 = U64BIG(state_mac64[3]);
    x4 = U64BIG(state_mac64[4]);

    // Squeeze tag
    P12;
    unsigned long long *tag64 = (u64 *)tag;
    tag64[0] = U64BIG(x0);
    tag64[1] = U64BIG(x1);
}
