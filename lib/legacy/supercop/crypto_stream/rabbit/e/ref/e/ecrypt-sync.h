#define ECRYPT_VARIANT 1


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

typedef struct
{
   u32 x[8];
   u32 c[8];
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

#define ECRYPT_MAXVARIANT 1



#endif

#include "estream-functions.h"
