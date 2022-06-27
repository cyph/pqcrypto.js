#ifndef ROMULUS_H_
#define ROMULUS_H_

#include "skinny128.h"

#define TAGBYTES    16
#define KEYBYTES    TWEAKEYBYTES

#define SET_DOMAIN(tk1, domain) (tk1[7] = (domain))

//G as defined in the Romulus specification in a 32-bit word-wise manner
#define G(x,y) ({                                                                       \
    tmp = ((uint32_t*)(y))[0];                                                          \
    ((uint32_t*)(x))[0] = (tmp >> 1 & 0x7f7f7f7f) ^ ((tmp ^ (tmp << 7)) & 0x80808080);  \
    tmp = ((uint32_t*)(y))[1];                                                          \
    ((uint32_t*)(x))[1] = (tmp >> 1 & 0x7f7f7f7f) ^ ((tmp ^ (tmp << 7)) & 0x80808080);  \
    tmp = ((uint32_t*)(y))[2];                                                          \
    ((uint32_t*)(x))[2] = (tmp >> 1 & 0x7f7f7f7f) ^ ((tmp ^ (tmp << 7)) & 0x80808080);  \
    tmp = ((uint32_t*)(y))[3];                                                          \
    ((uint32_t*)(x))[3] = (tmp >> 1 & 0x7f7f7f7f) ^ ((tmp ^ (tmp << 7)) & 0x80808080);  \
})

//update the counter in tk1 in a 32-bit word-wise manner
#define UPDATE_CTR(tk1) ({                                  \
    tmp = ((uint32_t*)(tk1))[1];                            \
    ((uint32_t*)(tk1))[1] = (tmp << 1) & 0x00ffffff;        \
    ((uint32_t*)(tk1))[1] |= (((uint32_t*)(tk1))[0] >> 31); \
    ((uint32_t*)(tk1))[1] |= tmp & 0xff000000;              \
    ((uint32_t*)(tk1))[0] <<= 1;                            \
    if ((tmp >> 23) & 0x01)                                 \
        ((uint32_t*)(tk1))[0] ^= 0x95;                      \
})

//x <- y ^ z for 128-bit blocks
#define XOR_BLOCK(x,y,z) ({                                             \
    ((uint32_t*)(x))[0] = ((uint32_t*)(y))[0] ^ ((uint32_t*)(z))[0];    \
    ((uint32_t*)(x))[1] = ((uint32_t*)(y))[1] ^ ((uint32_t*)(z))[1];    \
    ((uint32_t*)(x))[2] = ((uint32_t*)(y))[2] ^ ((uint32_t*)(z))[2];    \
    ((uint32_t*)(x))[3] = ((uint32_t*)(y))[3] ^ ((uint32_t*)(z))[3];    \
})


//Rho as defined in the Romulus specification
//use pad as a tmp variable in case y = z
#define RHO(x,y,z,tmp) ({       \
    G(tmp,x);                   \
    XOR_BLOCK(y, tmp, z);       \
    XOR_BLOCK(x, x, z);         \
})

//Rho inverse as defined in the Romulus specification
//use pad as a tmp variable in case y = z
#define RHO_INV(x, y, z, tmp) ({    \
    G(tmp, x);                      \
    XOR_BLOCK(z, tmp, y);           \
    XOR_BLOCK(x, x, z);             \
})

// Core Romulus-T functions
void romulust_init(uint8_t *state, uint8_t *tk1);

int romulusht(
    unsigned char out[],
    const unsigned char a[],
    unsigned long long  adlen,
    const unsigned char c[],
    unsigned long long clen,
    const unsigned char npub[],
    unsigned char tk1[]);

void romulust_kdf(
    uint8_t state[],
    uint8_t tk1[],
    const unsigned char npub[],
    const unsigned char k[]);

void romulust_process_msg(
    uint8_t state[],
    uint8_t tk1[],
    const unsigned char npub[],
    unsigned char c[],
    const unsigned char m[],
    unsigned long long mlen);

void romulust_generate_tag(
    uint8_t tag[],
    unsigned char tk1[],
    const unsigned char ad[],
    unsigned long long adlen,
    const unsigned char c[],
    unsigned long long mlen,
    const unsigned char npub[],
    const unsigned char k[]);

#endif  // ROMULUS_H_