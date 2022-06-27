#define ECRYPT_VARIANT 1


#ifndef ECRYPT_SYNC
#define ECRYPT_SYNC

#include "ecrypt-portable.h"

/* Cipher parameters */

/*
 * The name of your cipher.
 */
#define ECRYPT_NAME "TPypy"    /* [edit] */
#define ECRYPT_PROFILE "_____"

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

#define ECRYPT_MAXKEYSIZE (256*8)             /* [edit] */
#define ECRYPT_KEYSIZE(i) ((i+1)*8)           /* [edit] */

#define ECRYPT_MAXIVSIZE (64*8)                 /* [edit] */
#define ECRYPT_IVSIZE(i) ((i+1)*8)              /* [edit] */

/* Data structures */

/*
 * ECRYPT_ctx is the structure containing the representation of the
 * internal state of your cipher.
 */

#define PYSIZE 260
#define YMININD (-3)
#define YMAXIND (256)

typedef struct
{
  /*
   * [edit]
   *
   * Put here all state variable needed during the encryption process.
   */
  u32 KPY0[PYSIZE];
  /* KPY and PY must be consecutive, as we use them as one large array in IVsetup */
  u32 PY0[PYSIZE+3];
  /* If there are accesses after PY, dummy should take these values and ignore them */
  u32 dummy0[3][2];
  u32 KPY1[PYSIZE];
  /* KPY and PY must be consecutive, as we use them as one large array in IVsetup */
  u32 PY1[PYSIZE+3];
  /* If there are accesses after PY, dummy should take these values and ignore them */
  u32 dummy1[3][2];
  u32 s;
  int keysize;
  int ivsize;
} ECRYPT_ctx;


#define ECRYPT_HAS_SINGLE_BYTE_FUNCTION       /* [edit] */
#define ECRYPT_GENERATES_KEYSTREAM

#define ECRYPT_USES_DEFAULT_ALL_IN_ONE        /* [edit] */

/*
 * Undef ECRYPT_HAS_SINGLE_PACKET_FUNCTION if you want to provide
 * separate packet encryption and decryption functions.
 */
#define ECRYPT_HAS_SINGLE_PACKET_FUNCTION     /* [edit] */

#define ECRYPT_BLOCKLENGTH 8                  /* [edit] */

#define ECRYPT_USES_DEFAULT_BLOCK_MACROS      /* [edit] */
#define ECRYPT_MAXVARIANT 1                   /* [edit] */



#endif

#include "estream-functions.h"
