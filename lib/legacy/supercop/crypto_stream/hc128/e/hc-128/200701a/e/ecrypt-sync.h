#define ECRYPT_VARIANT 1

#ifndef ECRYPT_SYNC
#define ECRYPT_SYNC

#include "ecrypt-portable.h"

/* Cipher parameters */

/*
 * The name of your cipher.
 */
#define ECRYPT_NAME "HC-128"    /* [edit] */
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

/*
*Remarks:  One key size is supported: 128 bits
*          One IV size is supported: 128 bits
*
*          The other key, IV sizes can also be used in HC-128,
*          but not recommended:
*          1) For any key with size not equal to 128,
*          the key needs to be concatenated to a 128-bit key
*          before being used in HC-128.
*          2) For any IV with size not equal to 128,
*          the IV needs to be concatenated to a 128-bit IV
*          before being used in HC-128
*
*Caution:  Two keys with different sizes should be independently generated
*          Two IVs with different sizes should not be used with the same key
*
*Recommended: 128-bit IV for 128-bit key;
*/

#define ECRYPT_MAXKEYSIZE 128                  /* [edit] */
#define ECRYPT_KEYSIZE(i) (128 + (i)*128)      /* [edit] */

#define ECRYPT_MAXIVSIZE 128                   /* [edit] */
#define ECRYPT_IVSIZE(i) (128 + (i)*128)       /* [edit] */



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
  u32 T[1024];       /* P[i] = T[i]; Q[i] = T[1024+i];*/
  u32 X[16];
  u32 Y[16];
  u32 counter1024;   /*counter1024 = i mod 1024 at the i-th step */
  u32 key[8];
  u32 iv[8];
  u32 keysize;       /* key size in bits */
  u32 ivsize;        /* iv size in bits*/
} ECRYPT_ctx;

/*-------------------------------------
Added functions
---------------------------------------*/

void generate_keystream(ECRYPT_ctx* ctx, u32* keystream);

void setup_update(ECRYPT_ctx* ctx);


#define ECRYPT_HAS_SINGLE_BYTE_FUNCTION       /* [edit] */
#undef ECRYPT_GENERATES_KEYSTREAM

#define ECRYPT_USES_DEFAULT_ALL_IN_ONE        /* [edit] */

/*
 * Undef ECRYPT_HAS_SINGLE_PACKET_FUNCTION if you want to provide
 * separate packet encryption and decryption functions.
 */
#define ECRYPT_HAS_SINGLE_PACKET_FUNCTION     /* [edit] */

#define ECRYPT_BLOCKLENGTH 64                 /* [edit] */

#define ECRYPT_USES_DEFAULT_BLOCK_MACROS      /* [edit] */
#define ECRYPT_MAXVARIANT 1                   /* [edit] */



#endif



#include "estream-functions.h"
