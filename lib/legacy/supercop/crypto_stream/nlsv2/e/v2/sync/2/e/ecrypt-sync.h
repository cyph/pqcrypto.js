#define ECRYPT_VARIANT 2


#ifndef ECRYPT_SYNC
#define ECRYPT_SYNC

#include "ecrypt-portable.h"

/* Cipher parameters */

/*
 * The name of your cipher.
 */
#define ECRYPT_NAME "NLS-v2"    /* [edit] */
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

#define ECRYPT_MAXKEYSIZE 128                 /* [edit] */
#define ECRYPT_KEYSIZE(i) (80 + (i)*48)       /* [edit] */

#define ECRYPT_MAXIVSIZE 128                  /* [edit] */
#define ECRYPT_IVSIZE(i) (32 + (i)*32)        /* [edit] */

/* Data structures */

/*
 * ECRYPT_ctx is the structure containing the representation of the
 * internal state of your cipher.
 */

#include "nls.h"
typedef struct
{
  /*
   * [edit]
   *
   * Put here all state variable needed during the encryption process.
   */
  nls_ctx ctx;
  u32 ivsize;
} ECRYPT_ctx;


#undef ECRYPT_HAS_SINGLE_BYTE_FUNCTION       /* [edit] */
#define ECRYPT_GENERATES_KEYSTREAM

#define ECRYPT_USES_DEFAULT_ALL_IN_ONE        /* [edit] */

/*
 * Undef ECRYPT_HAS_SINGLE_PACKET_FUNCTION if you want to provide
 * separate packet encryption and decryption functions.
 */
#define ECRYPT_HAS_SINGLE_PACKET_FUNCTION     /* [edit] */

#define ECRYPT_BLOCKLENGTH 4                  /* [edit] */

#define ECRYPT_USES_DEFAULT_BLOCK_MACROS      /* [edit] */
#define ECRYPT_MAXVARIANT 2                   /* [edit] */

#ifndef ECRYPT_VARIANT
#define ECRYPT_VARIANT 2
#endif

#if (ECRYPT_VARIANT > ECRYPT_MAXVARIANT)
#error this variant does not exist
#endif

#endif

#include "estream-functions.h"
