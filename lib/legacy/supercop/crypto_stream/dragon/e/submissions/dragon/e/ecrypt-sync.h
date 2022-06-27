#define ECRYPT_VARIANT 1


#ifndef ECRYPT_SYNC
#define ECRYPT_SYNC

#include "ecrypt-portable.h"

/* Cipher parameters */

/*
 * The name of your cipher.
 */
#define ECRYPT_NAME "Dragon"    /* [edit] */
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
#define ECRYPT_KEYSIZE(i) (128 + (i)*128)      /* [edit] */

#define ECRYPT_MAXIVSIZE 256                  /* [edit] */
#define ECRYPT_IVSIZE(i) (128 + (i)*128)        /* [edit] */

/* Data structures */

/*
 * ECRYPT_ctx is the structure containing the representation of the
 * internal state of your cipher.
 */

#define DRAGON_NLFSR_SIZE      32 /* size of NLFSR in 32-bit multiples */
#define DRAGON_KEYSTREAM_SIZE  2 /* size of output in 32-bit multiples */

#ifdef _DRAGON_OPT
#define DRAGON_BUFFER_SIZE     16 /* number of keystream blocks to buffer */
#else
#define DRAGON_BUFFER_SIZE      1
#endif

#define DRAGON_BUFFER_BYTES    (DRAGON_BUFFER_SIZE * DRAGON_KEYSTREAM_SIZE * 4)

typedef struct
{
        /* The NLFSR and counter comprise the state of Dragon */
        u32  nlfsr_word[DRAGON_NLFSR_SIZE];

#ifdef _DRAGON_OPT_
        u32  state_counter[2];
#else
        u64  state_counter;
#endif
        /* NLFSR shifting is modelled by the decrement of the nlfsr_offset
         * pointer, which indicates the 0th element of the NLFSR
         */
        u32  nlfsr_offset;

        /* Although key and IV injection are not seperated processes in Dragon
         * the ECRYPT API requires that they are added to the state separately.
         * Thus, to maintain consistency, the state at the end of the key
         * injection needs to be recalled for IV injection.
         */
        u32  init_state[DRAGON_NLFSR_SIZE];
        u32  key_size;


        /* Dragon is a block-cipher but the ECRYPT API mandates that partial
         * blocks must be able to be encrypted. Usually this will involve
         * caller-managed buffering, but the ECRYPT API makes no provision
         * for this, so buffering unforutnately needs to be managed internally
         * to the primitive.
         */
        u8   keystream_buffer[DRAGON_BUFFER_BYTES];
        u32  buffer_index;
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

#undef ECRYPT_USES_DEFAULT_BLOCK_MACROS      /* [edit] */
#define ECRYPT_MAXVARIANT 1                   /* [edit] */



#endif

#include "estream-functions.h"
