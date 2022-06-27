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

int crypto_core_keccakf160032bits(unsigned char *out, const unsigned char *in, const unsigned char *k, const unsigned char *c);

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

typedef unsigned char UINT8;
typedef unsigned int UINT32;

/* Credit to Henry S. Warren, Hacker's Delight, Addison-Wesley, 2002 */
#define prepareToBitInterleaving(low, high, temp, temp0, temp1) \
        temp0 = (low); \
        temp = (temp0 ^ (temp0 >>  1)) & 0x22222222UL;  temp0 = temp0 ^ temp ^ (temp <<  1); \
        temp = (temp0 ^ (temp0 >>  2)) & 0x0C0C0C0CUL;  temp0 = temp0 ^ temp ^ (temp <<  2); \
        temp = (temp0 ^ (temp0 >>  4)) & 0x00F000F0UL;  temp0 = temp0 ^ temp ^ (temp <<  4); \
        temp = (temp0 ^ (temp0 >>  8)) & 0x0000FF00UL;  temp0 = temp0 ^ temp ^ (temp <<  8); \
        temp1 = (high); \
        temp = (temp1 ^ (temp1 >>  1)) & 0x22222222UL;  temp1 = temp1 ^ temp ^ (temp <<  1); \
        temp = (temp1 ^ (temp1 >>  2)) & 0x0C0C0C0CUL;  temp1 = temp1 ^ temp ^ (temp <<  2); \
        temp = (temp1 ^ (temp1 >>  4)) & 0x00F000F0UL;  temp1 = temp1 ^ temp ^ (temp <<  4); \
        temp = (temp1 ^ (temp1 >>  8)) & 0x0000FF00UL;  temp1 = temp1 ^ temp ^ (temp <<  8);

#define toBitInterleavingAndXOR(low, high, even, odd, temp, temp0, temp1) \
        prepareToBitInterleaving(low, high, temp, temp0, temp1) \
        even ^= (temp0 & 0x0000FFFF) | (temp1 << 16); \
        odd ^= (temp0 >> 16) | (temp1 & 0xFFFF0000);

/* Credit to Henry S. Warren, Hacker's Delight, Addison-Wesley, 2002 */
#define prepareFromBitInterleaving(even, odd, temp, temp0, temp1) \
        temp0 = (even); \
        temp1 = (odd); \
        temp = (temp0 & 0x0000FFFF) | (temp1 << 16); \
        temp1 = (temp0 >> 16) | (temp1 & 0xFFFF0000); \
        temp0 = temp; \
        temp = (temp0 ^ (temp0 >>  8)) & 0x0000FF00UL;  temp0 = temp0 ^ temp ^ (temp <<  8); \
        temp = (temp0 ^ (temp0 >>  4)) & 0x00F000F0UL;  temp0 = temp0 ^ temp ^ (temp <<  4); \
        temp = (temp0 ^ (temp0 >>  2)) & 0x0C0C0C0CUL;  temp0 = temp0 ^ temp ^ (temp <<  2); \
        temp = (temp0 ^ (temp0 >>  1)) & 0x22222222UL;  temp0 = temp0 ^ temp ^ (temp <<  1); \
        temp = (temp1 ^ (temp1 >>  8)) & 0x0000FF00UL;  temp1 = temp1 ^ temp ^ (temp <<  8); \
        temp = (temp1 ^ (temp1 >>  4)) & 0x00F000F0UL;  temp1 = temp1 ^ temp ^ (temp <<  4); \
        temp = (temp1 ^ (temp1 >>  2)) & 0x0C0C0C0CUL;  temp1 = temp1 ^ temp ^ (temp <<  2); \
        temp = (temp1 ^ (temp1 >>  1)) & 0x22222222UL;  temp1 = temp1 ^ temp ^ (temp <<  1);

#define fromBitInterleaving(even, odd, low, high, temp, temp0, temp1) \
        prepareFromBitInterleaving(even, odd, temp, temp0, temp1) \
        low = temp0; \
        high = temp1;

static void KeccakP1600_AddByte(void *state, unsigned char byte, unsigned int offset)
{
    unsigned int lanePosition = offset/8;
    unsigned int offsetInLane = offset%8;
    UINT32 low, high;
    UINT32 temp, temp0, temp1;
    UINT32 *stateAsHalfLanes = (UINT32*)state;

    if (offsetInLane < 4) {
        low = (UINT32)byte << (offsetInLane*8);
        high = 0;
    }
    else {
        low = 0;
        high = (UINT32)byte << ((offsetInLane-4)*8);
    }
    toBitInterleavingAndXOR(low, high, stateAsHalfLanes[lanePosition*2+0], stateAsHalfLanes[lanePosition*2+1], temp, temp0, temp1);
}

static void KeccakP1600_AddBytesInLane(void *state, unsigned int lanePosition, const unsigned char *data, unsigned int offset, unsigned int length)
{
    UINT8 laneAsBytes[8];
    UINT32 low, high;
    UINT32 temp, temp0, temp1;
    UINT32 *stateAsHalfLanes = (UINT32*)state;

    memset(laneAsBytes, 0, 8);
    memcpy(laneAsBytes+offset, data, length);
#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN)
    low = *((UINT32*)(laneAsBytes+0));
    high = *((UINT32*)(laneAsBytes+4));
#else
    low = laneAsBytes[0]
        | ((UINT32)(laneAsBytes[1]) << 8)
        | ((UINT32)(laneAsBytes[2]) << 16)
        | ((UINT32)(laneAsBytes[3]) << 24);
    high = laneAsBytes[4]
        | ((UINT32)(laneAsBytes[5]) << 8)
        | ((UINT32)(laneAsBytes[6]) << 16)
        | ((UINT32)(laneAsBytes[7]) << 24);
#endif
    toBitInterleavingAndXOR(low, high, stateAsHalfLanes[lanePosition*2+0], stateAsHalfLanes[lanePosition*2+1], temp, temp0, temp1);
}

static void KeccakP1600_AddLanes(void *state, const unsigned char *data, unsigned int laneCount)
{
#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN)
    const UINT32 * pI = (const UINT32 *)data;
    UINT32 * pS = (UINT32*)state;
    UINT32 t, x0, x1;
    int i;
    for (i = laneCount-1; i >= 0; --i) {
#ifdef NO_MISALIGNED_ACCESSES
        UINT32 low;
        UINT32 high;
        memcpy(&low, pI++, 4);
        memcpy(&high, pI++, 4);
        toBitInterleavingAndXOR(low, high, *(pS++), *(pS++), t, x0, x1);
#else
        toBitInterleavingAndXOR(*(pI++), *(pI++), *(pS++), *(pS++), t, x0, x1)
#endif
    }
#else
    unsigned int lanePosition;
    for(lanePosition=0; lanePosition<laneCount; lanePosition++) {
        UINT8 laneAsBytes[8];
        memcpy(laneAsBytes, data+lanePosition*8, 8);
        UINT32 low = laneAsBytes[0]
            | ((UINT32)(laneAsBytes[1]) << 8)
            | ((UINT32)(laneAsBytes[2]) << 16)
            | ((UINT32)(laneAsBytes[3]) << 24);
        UINT32 high = laneAsBytes[4]
            | ((UINT32)(laneAsBytes[5]) << 8)
            | ((UINT32)(laneAsBytes[6]) << 16)
            | ((UINT32)(laneAsBytes[7]) << 24);
        UINT32 even, odd, temp, temp0, temp1;
        UINT32 *stateAsHalfLanes = (UINT32*)state;
        toBitInterleavingAndXOR(low, high, stateAsHalfLanes[lanePosition*2+0], stateAsHalfLanes[lanePosition*2+1], temp, temp0, temp1);
    }
#endif
}

static void KeccakP1600_AddBytes(void *state, const unsigned char *data, unsigned int offset, unsigned int length)
{
    if ((offset) == 0) {
        KeccakP1600_AddLanes(state, data, (length)/8);
        KeccakP1600_AddBytesInLane(state, (length)/8, (data)+((length)/8)*8, 0, (length)%8);
    }
    else {
        /* Not needed here, not implemented. */
    }
}

#define KeccakP1600_Permute_24rounds(state) \
    crypto_core_keccakf160032bits(state, state, 0, 0)

static void KeccakP1600_ExtractBytesInLane(const void *state, unsigned int lanePosition, unsigned char *data, unsigned int offset, unsigned int length)
{
    UINT32 *stateAsHalfLanes = (UINT32*)state;
    UINT32 low, high, temp, temp0, temp1;
    UINT8 laneAsBytes[8];

    fromBitInterleaving(stateAsHalfLanes[lanePosition*2], stateAsHalfLanes[lanePosition*2+1], low, high, temp, temp0, temp1);
#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN)
    *((UINT32*)(laneAsBytes+0)) = low;
    *((UINT32*)(laneAsBytes+4)) = high;
#else
    laneAsBytes[0] = low & 0xFF;
    laneAsBytes[1] = (low >> 8) & 0xFF;
    laneAsBytes[2] = (low >> 16) & 0xFF;
    laneAsBytes[3] = (low >> 24) & 0xFF;
    laneAsBytes[4] = high & 0xFF;
    laneAsBytes[5] = (high >> 8) & 0xFF;
    laneAsBytes[6] = (high >> 16) & 0xFF;
    laneAsBytes[7] = (high >> 24) & 0xFF;
#endif
    memcpy(data, laneAsBytes+offset, length);
}

static void KeccakP1600_ExtractLanes(const void *state, unsigned char *data, unsigned int laneCount)
{
#if (PLATFORM_BYTE_ORDER == IS_LITTLE_ENDIAN)
    UINT32 * pI = (UINT32 *)data;
    const UINT32 * pS = ( const UINT32 *)state;
    UINT32 t, x0, x1;
    int i;
    for (i = laneCount-1; i >= 0; --i) {
#ifdef NO_MISALIGNED_ACCESSES
        UINT32 low;
        UINT32 high;
        fromBitInterleaving(*(pS++), *(pS++), low, high, t, x0, x1);
        memcpy(pI++, &low, 4);
        memcpy(pI++, &high, 4);
#else
        fromBitInterleaving(*(pS++), *(pS++), *(pI++), *(pI++), t, x0, x1)
#endif
    }
#else
    unsigned int lanePosition;
    for(lanePosition=0; lanePosition<laneCount; lanePosition++) {
        UINT32 *stateAsHalfLanes = (UINT32*)state;
        UINT32 low, high, temp, temp0, temp1;
        fromBitInterleaving(stateAsHalfLanes[lanePosition*2], stateAsHalfLanes[lanePosition*2+1], low, high, temp, temp0, temp1);
        UINT8 laneAsBytes[8];
        laneAsBytes[0] = low & 0xFF;
        laneAsBytes[1] = (low >> 8) & 0xFF;
        laneAsBytes[2] = (low >> 16) & 0xFF;
        laneAsBytes[3] = (low >> 24) & 0xFF;
        laneAsBytes[4] = high & 0xFF;
        laneAsBytes[5] = (high >> 8) & 0xFF;
        laneAsBytes[6] = (high >> 16) & 0xFF;
        laneAsBytes[7] = (high >> 24) & 0xFF;
        memcpy(data+lanePosition*8, laneAsBytes, 8);
    }
#endif
}

static void KeccakP1600_ExtractBytes(const void *state, unsigned char *data, unsigned int offset, unsigned int length)
{
    if ((offset) == 0) {
        KeccakP1600_ExtractLanes(state, data, (length)/8);
        KeccakP1600_ExtractBytesInLane(state, (length)/8, (data)+((length)/8)*8, 0, (length)%8);
    }
    else {
        /* Not needed here, not implemented. */
    }
}

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
