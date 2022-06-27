#define ECRYPT_VARIANT 1


#ifndef ECRYPT_SYNC
#define ECRYPT_SYNC

#include "ecrypt-portable.h"

/* Cipher parameters */

/*
 * The name of your cipher.
 */
#define ECRYPT_NAME "CryptMT-v3"        /* [edit] */
#define ECRYPT_PROFILE "S3___"

/*
 * Specify which key and IV sizes are supported by your cipher. A user
 * should be able to enumerate the supported sizes by running the
 * following code:
 *
 * for (i = 0; ECRYPT_KEYSIZE(i) <= ECRYPT_MAXKEYSIZE; ++i)
 *   {
 *     keysize = ECRYPT_KEYSIZE(i);
 *
 *     ...
 *   }
 *
 * All sizes are in bits.
 */

#define ECRYPT_MAXKEYSIZE 2048  /* [edit] */
#define ECRYPT_KEYSIZE(i) (128 + (i)*128)       /* [edit] */

#define ECRYPT_MAXIVSIZE 2048   /* [edit] */
#define ECRYPT_IVSIZE(i) (128 + (i)*128)        /* [edit] */

/* Data structures */

/*
 * ECRYPT_ctx is the structure containing the representation of the
 * internal state of your cipher.
 */

typedef struct {
#if defined(__ALTIVEC__)
    vector unsigned int dummy;
#else
    ALIGN(u8, dummy, 1);
#endif
    u32 sfmt[156 + 2 + ((ECRYPT_MAXKEYSIZE + ECRYPT_MAXIVSIZE) * 3) / 128][4];
                                /* the array for the state vector  */
    u32 accum[4];               /* filter */
    u32 lung[4];                /* booter */
    u32 *psfmt;                 /* pointer to sfmt internal state */
    u32 length;                 /* length of first block */
    u32 key[ECRYPT_MAXKEYSIZE / 32];
    s32 keysize;                /* size in 16bit words (bits / 32) */
    s32 ivsize;                 /* size in 16bit words (bits / 32) */
    s32 first;                  /* first flag */
} ECRYPT_ctx;

#undef ECRYPT_GENERATES_KEYSTREAM

#define ECRYPT_USES_DEFAULT_ALL_IN_ONE  /* [edit] */


#define ECRYPT_BLOCKLENGTH (624 * 2)    /* [edit] */

/* #define ECRYPT_USES_DEFAULT_BLOCK_MACROS *//* [edit] */
#define ECRYPT_MAXVARIANT 1     /* [edit] */



#endif

#include "estream-functions.h"
