/* snow-2.0.c */

/* 
 * SNOW 2.0.  
 *
 * crypto_stream_snow20_e_benchmarks_snow_2_0_ECRYPT wrapper around SNOW 2.0 code written by Patrik Ekdahl &
 * Thomas Johansson.
 * see http://www.it.lth.se/cryptology/snow/
 *
 * Author: Christophe De Canni\`ere, K.U.Leuven.
 */

/* ------------------------------------------------------------------------- */

#include "ecrypt-sync.h"
#include "snow2_fast.c"

/* ------------------------------------------------------------------------- */

void crypto_stream_snow20_e_benchmarks_snow_2_0_ECRYPT_init(void)
{ }

/* ------------------------------------------------------------------------- */

void crypto_stream_snow20_e_benchmarks_snow_2_0_ECRYPT_keysetup(
  crypto_stream_snow20_e_benchmarks_snow_2_0_ECRYPT_ctx* ctx, 
  const u8* key, 
  u32 keysize,
  u32 ivsize)
{ 
  u32 i;

  for (i = 0; i < keysize / 8; ++i)
    ctx->key[i] = key[i];

  ctx->keysize = keysize;
}

/* ------------------------------------------------------------------------- */

void crypto_stream_snow20_e_benchmarks_snow_2_0_ECRYPT_ivsetup(
  crypto_stream_snow20_e_benchmarks_snow_2_0_ECRYPT_ctx* ctx, 
  const u8* iv)
{ 
  snow_loadkey_fast(ctx, 
    U8TO32_LITTLE(iv),
    U8TO32_LITTLE(iv + 4),
    U8TO32_LITTLE(iv + 8),
    U8TO32_LITTLE(iv + 12));
}

/* ------------------------------------------------------------------------- */

void crypto_stream_snow20_e_benchmarks_snow_2_0_ECRYPT_process_bytes(
  int action,
  crypto_stream_snow20_e_benchmarks_snow_2_0_ECRYPT_ctx* ctx, 
  const u8* input, 
  u8* output, 
  u32 msglen)
{ 
  u32 i;
  u32 keystream[16];

  for ( ; msglen >= 64; msglen -= 64, input += 64, output += 64)
    {
      snow_keystream_fast(ctx, keystream);

      for (i = 0; i < 16; ++i)
	((u32*)output)[i] = ((u32*)input)[i] ^ U32TO32_LITTLE(keystream[i]);
    }

  if (msglen > 0)
    {
      snow_keystream_fast(ctx, keystream);

      for (i = 0; i < msglen; i ++)
	output[i] = input[i] ^ ((u8*)keystream)[i];
    }
}

/* ------------------------------------------------------------------------- */
