#define ECRYPT_VARIANT 4


#ifndef ECRYPT_SYNC
#define ECRYPT_SYNC

#include "ecrypt-portable.h"

/* Cipher parameters */

/*
 * The name of your cipher.
 */
#define ECRYPT_NAME "Rabbit"
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

#define ECRYPT_MAXKEYSIZE 128
#define ECRYPT_KEYSIZE(i) (128 + (i)*32)

#define ECRYPT_MAXIVSIZE 64
#define ECRYPT_IVSIZE(i) (64 + (i)*64)

/* Data structures */

/*
 * ECRYPT_ctx is the structure containing the representation of the
 * internal state of your cipher.
 */

typedef struct RABBIT_ctx_
{
   u32 x0, x1, x2, x3, x4, x5, x6, x7;
   u32 c0, c1, c2, c3, c4, c5, c6, c7;
   u32 carry;
} RABBIT_ctx;

typedef struct
{
  /*
   * Put here all state variable needed during the encryption process.
   */
   RABBIT_ctx master_ctx;
   RABBIT_ctx work_ctx;
} ECRYPT_ctx;


#define ECRYPT_HAS_SINGLE_BYTE_FUNCTION
#define ECRYPT_GENERATES_KEYSTREAM

#define ECRYPT_USES_DEFAULT_ALL_IN_ONE

/*
 * Undef ECRYPT_HAS_SINGLE_PACKET_FUNCTION if you want to provide
 * separate packet encryption and decryption functions.
 */
#define ECRYPT_HAS_SINGLE_PACKET_FUNCTION

#define ECRYPT_BLOCKLENGTH 16

#undef ECRYPT_USES_DEFAULT_BLOCK_MACROS

#define ECRYPT_MAXVARIANT 4

#ifndef ECRYPT_VARIANT
#define ECRYPT_VARIANT 4
#endif

#if (ECRYPT_VARIANT > ECRYPT_MAXVARIANT)
#error this variant does not exist
#endif

#endif

#include "estream-functions.h"
