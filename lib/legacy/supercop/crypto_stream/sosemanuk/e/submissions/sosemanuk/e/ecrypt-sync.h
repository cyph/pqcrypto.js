#define ECRYPT_VARIANT 1

#ifndef ECRYPT_SYNC
#define ECRYPT_SYNC

#include "ecrypt-portable.h"

/* Cipher parameters */

/*
 * The name of your cipher.
 */
#define ECRYPT_NAME "SOSEMANUK"    /* [edit] */
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

#define ECRYPT_MAXKEYSIZE 256                 /* [edit] */
#define ECRYPT_KEYSIZE(i) (8 + (i)*8)         /* [edit] */

#define ECRYPT_MAXIVSIZE 128                  /* [edit] */
#define ECRYPT_IVSIZE(i) (8 + (i)*8)          /* [edit] */

/* Data structures */

/*
 * ECRYPT_ctx is the structure containing the representation of the
 * internal state of your cipher.
 */

typedef struct
{
  /*
   * [edit]
   *
   * Put here all state variable needed during the encryption process.
   */

        /*
         * Sub-keys (computed from the key).
         */
        u32 sk[100];

        /*
         * IV length (in bytes).
         */
        u32 ivlen;

        /*
         * Internal state.
         */
        u32 s00, s01, s02, s03, s04, s05, s06, s07, s08, s09;
        u32 r1, r2;

} ECRYPT_ctx;

#define ECRYPT_HAS_SINGLE_BYTE_FUNCTION       /* [edit] */
#define ECRYPT_GENERATES_KEYSTREAM
#define ECRYPT_USES_DEFAULT_ALL_IN_ONE        /* [edit] */
#define ECRYPT_HAS_SINGLE_PACKET_FUNCTION     /* [edit] */
#define ECRYPT_BLOCKLENGTH 80                /* [edit] */
#undef ECRYPT_USES_DEFAULT_BLOCK_MACROS      /* [edit] */
#define ECRYPT_HAS_SINGLE_BLOCK_FUNCTION      /* [edit] */
#define ECRYPT_MAXVARIANT 1                   /* [edit] */

#endif

#include "estream-functions.h"
