/*
Implementation by the Keccak Team, namely, Guido Bertoni, Joan Daemen,
Michaël Peeters, Gilles Van Assche and Ronny Van Keer, 
hereby denoted as "the implementer".

For more information, feedback or questions, please refer to our website:
https://keccak.team/

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include <string.h>
#include "brg_endian.h"

int crypto_core_keccakf160064bits(unsigned char *out, const unsigned char *in, const unsigned char *k, const unsigned char *c);

#ifdef ALIGN
#undef ALIGN
#endif

#if defined(__GNUC__)
#define ALIGN(x) __attribute__ ((aligned(x)))
#elif defined(_MSC_VER)
#define ALIGN(x) __declspec(align(x))
#elif defined(__ARMCC_VERSION)
#define ALIGN(x) __align(x)
#else
#define ALIGN(x)
#endif

typedef unsigned long long UINT64;

static void KeccakP1600_AddLanes(void *state, const unsigned char *data, unsigned int laneCount)
{
#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN)
    unsigned int i = 0;
    for( ; (i+8)<=laneCount; i+=8) {
        ((UINT64*)state)[i+0] ^= ((UINT64*)data)[i+0];
        ((UINT64*)state)[i+1] ^= ((UINT64*)data)[i+1];
        ((UINT64*)state)[i+2] ^= ((UINT64*)data)[i+2];
        ((UINT64*)state)[i+3] ^= ((UINT64*)data)[i+3];
        ((UINT64*)state)[i+4] ^= ((UINT64*)data)[i+4];
        ((UINT64*)state)[i+5] ^= ((UINT64*)data)[i+5];
        ((UINT64*)state)[i+6] ^= ((UINT64*)data)[i+6];
        ((UINT64*)state)[i+7] ^= ((UINT64*)data)[i+7];
    }
    for( ; (i+4)<=laneCount; i+=4) {
        ((UINT64*)state)[i+0] ^= ((UINT64*)data)[i+0];
        ((UINT64*)state)[i+1] ^= ((UINT64*)data)[i+1];
        ((UINT64*)state)[i+2] ^= ((UINT64*)data)[i+2];
        ((UINT64*)state)[i+3] ^= ((UINT64*)data)[i+3];
    }
    for( ; (i+2)<=laneCount; i+=2) {
        ((UINT64*)state)[i+0] ^= ((UINT64*)data)[i+0];
        ((UINT64*)state)[i+1] ^= ((UINT64*)data)[i+1];
    }
    if (i<laneCount) {
        ((UINT64*)state)[i+0] ^= ((UINT64*)data)[i+0];
    }
#else
    unsigned int i, j;
    unsigned char *pState = (unsigned char *)state;
    unsigned char *pData = data;
    for(i=0; i<laneCount; i++) {
        for(j=0; j<8; j++)
            pState[j] ^= pData[j];
        pState += 8;
        pData += 8;
    }
#endif
}

static void KeccakP1600_AddBytes(void *state, const unsigned char *data, unsigned int offset, unsigned int length)
{
    unsigned int i;

    if (offset == 0) {
        if (length >= 8)
            KeccakP1600_AddLanes(state, data, length/8);
        for(i=(length/8)*8; i<length; i++)
            ((unsigned char *)state)[i+offset] ^= data[i];
    }
    else {
        for(i=0; i<length; i++)
            ((unsigned char *)state)[i+offset] ^= data[i];
    }
}

#define KeccakP1600_AddByte(state, byte, offset) \
    ((unsigned char*)(state))[(offset)] ^= (byte)

#define KeccakP1600_Permute_24rounds(state) \
    crypto_core_keccakf160064bits(state, state, 0, 0)

#define KeccakP1600_ExtractBytes(state, data, offset, length) \
    memcpy(data, (const unsigned char *)(state)+(offset), length)

static int KeccakWidth1600_Sponge(unsigned int rate, unsigned int capacity, const unsigned char *input, size_t inputByteLen, unsigned char suffix, unsigned char *output, size_t outputByteLen)
{
    ALIGN(32) unsigned char state[200];
    unsigned int partialBlock;
    const unsigned char *curInput = input;
    unsigned char *curOutput = output;
    unsigned int rateInBytes = rate/8;

    if (rate+capacity != 1600)
        return 1;
    if ((rate <= 0) || (rate > 1600) || ((rate % 8) != 0))
        return 1;
    if (suffix == 0)
        return 1;

    /* Initialize the state */
    memset(state, 0, sizeof(state));

    /* First, absorb whole blocks */
    while(inputByteLen >= (size_t)rateInBytes) {
        KeccakP1600_AddBytes(state, curInput, 0, rateInBytes);
        KeccakP1600_Permute_24rounds(state);
        curInput += rateInBytes;
        inputByteLen -= rateInBytes;
    }

    /* Then, absorb what remains */
    partialBlock = (unsigned int)inputByteLen;
    KeccakP1600_AddBytes(state, curInput, 0, partialBlock);

    /* Finally, absorb the suffix */
    /* Last few bits, whose delimiter coincides with first bit of padding */
    KeccakP1600_AddByte(state, suffix, partialBlock);
    /* If the first bit of padding is at position rate-1, we need a whole new block for the second bit of padding */
    if ((suffix >= 0x80) && (partialBlock == (rateInBytes-1)))
        KeccakP1600_Permute_24rounds(state);
    /* Second bit of padding */
    KeccakP1600_AddByte(state, 0x80, rateInBytes-1);
    KeccakP1600_Permute_24rounds(state);

    /* First, output whole blocks */
    while(outputByteLen > (size_t)rateInBytes) {
        KeccakP1600_ExtractBytes(state, curOutput, 0, rateInBytes);
        KeccakP1600_Permute_24rounds(state);
        curOutput += rateInBytes;
        outputByteLen -= rateInBytes;
    }

    /* Finally, output what remains */
    partialBlock = (unsigned int)outputByteLen;
    KeccakP1600_ExtractBytes(state, curOutput, 0, partialBlock);

    return 0;
}
