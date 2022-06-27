// hlia5_endian.h
// 2017-09-25  Markku-Juhani O. Saarinen <mjos@iki.fi>

// Flips 64-bit words in in vector v[n] around if this appears to be a
// big-endian platform. A no-op otherwise.

#ifndef HILA5_ENDIAN_FLIP64

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

// nop
#define HILA5_ENDIAN_FLIP64(v, n) {;}

#else

#define HILA5_ENDIAN_FLIP64(v, n) {                 \
    for (int i = 0; i < n; i++) {                   \
        uint64_t t;                                 \
        t = ((uint64_t *) v)[i];                    \
        t = (t << 32) | (t >> 32);                  \
        t = ((t &  0x0000FFFF0000FFFFllu) << 16) |  \
            ((t >> 16) & 0x0000FFFF0000FFFFllu);    \
        t = ((t & 0x00FF00FF00FF00FFllu) << 8) |    \
            ((t >> 8) & 0x00FF00FF00FF00FFllu);     \
        ((uint64_t *) v)[i] = t;                    \
    }                                               \
}

#endif

#endif


