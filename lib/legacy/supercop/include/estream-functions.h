#define ECRYPT_API

#include "ecrypt-portable.h"

#define ECRYPT_init CRYPTO_NAMESPACE(ECRYPT_init)
extern void ECRYPT_init(void);

#define ECRYPT_keysetup CRYPTO_NAMESPACE(ECRYPT_keysetup)
void ECRYPT_keysetup(
  ECRYPT_ctx* ctx, 
  const u8* key, 
  u32 keysize,                /* Key size in bits. */ 
  u32 ivsize);                /* IV size in bits. */ 

#define ECRYPT_ivsetup CRYPTO_NAMESPACE(ECRYPT_ivsetup)
void ECRYPT_ivsetup(
  ECRYPT_ctx* ctx, 
  const u8* iv);

#ifdef ECRYPT_HAS_SINGLE_BYTE_FUNCTION

#define ECRYPT_encrypt_bytes(ctx, plaintext, ciphertext, msglen)   \
  ECRYPT_process_bytes(0, ctx, plaintext, ciphertext, msglen)

#define ECRYPT_decrypt_bytes(ctx, ciphertext, plaintext, msglen)   \
  ECRYPT_process_bytes(1, ctx, ciphertext, plaintext, msglen)

#define ECRYPT_process_bytes CRYPTO_NAMESPACE(ECRYPT_process_bytes)
void ECRYPT_process_bytes(
  int action,                 /* 0 = encrypt; 1 = decrypt; */
  ECRYPT_ctx* ctx, 
  const u8* input, 
  u8* output, 
  u32 msglen);                /* Message length in bytes. */ 

#else

#define ECRYPT_encrypt_bytes CRYPTO_NAMESPACE(ECRYPT_encrypt_bytes)
void ECRYPT_encrypt_bytes(
  ECRYPT_ctx* ctx, 
  const u8* plaintext, 
  u8* ciphertext, 
  u32 msglen);                /* Message length in bytes. */ 

#define ECRYPT_decrypt_bytes CRYPTO_NAMESPACE(ECRYPT_decrypt_bytes)
void ECRYPT_decrypt_bytes(
  ECRYPT_ctx* ctx, 
  const u8* ciphertext, 
  u8* plaintext, 
  u32 msglen);                /* Message length in bytes. */ 

#endif

#ifdef ECRYPT_GENERATES_KEYSTREAM

#define ECRYPT_keystream_bytes CRYPTO_NAMESPACE(ECRYPT_keystream_bytes)
void ECRYPT_keystream_bytes(
  ECRYPT_ctx* ctx,
  u8* keystream,
  u32 length);                /* Length of keystream in bytes. */

#endif

/* ------------------------------------------------------------------------- */

#ifdef ECRYPT_HAS_SINGLE_PACKET_FUNCTION

#define ECRYPT_encrypt_packet(                                        \
    ctx, iv, plaintext, ciphertext, mglen)                            \
  ECRYPT_process_packet(0,                                            \
    ctx, iv, plaintext, ciphertext, mglen)

#define ECRYPT_decrypt_packet(                                        \
    ctx, iv, ciphertext, plaintext, mglen)                            \
  ECRYPT_process_packet(1,                                            \
    ctx, iv, ciphertext, plaintext, mglen)

#define ECRYPT_process_packet CRYPTO_NAMESPACE(ECRYPT_process_packet)
void ECRYPT_process_packet(
  int action,                 /* 0 = encrypt; 1 = decrypt; */
  ECRYPT_ctx* ctx, 
  const u8* iv,
  const u8* input, 
  u8* output, 
  u32 msglen);

#else

#define ECRYPT_encrypt_packet CRYPTO_NAMESPACE(ECRYPT_encrypt_packet)
void ECRYPT_encrypt_packet(
  ECRYPT_ctx* ctx, 
  const u8* iv,
  const u8* plaintext, 
  u8* ciphertext, 
  u32 msglen);

#define ECRYPT_decrypt_packet CRYPTO_NAMESPACE(ECRYPT_decrypt_packet)
void ECRYPT_decrypt_packet(
  ECRYPT_ctx* ctx, 
  const u8* iv,
  const u8* ciphertext, 
  u8* plaintext, 
  u32 msglen);

#endif

#ifdef ECRYPT_USES_DEFAULT_BLOCK_MACROS

#define ECRYPT_encrypt_blocks(ctx, plaintext, ciphertext, blocks)  \
  ECRYPT_encrypt_bytes(ctx, plaintext, ciphertext,                 \
    (blocks) * ECRYPT_BLOCKLENGTH)

#define ECRYPT_decrypt_blocks(ctx, ciphertext, plaintext, blocks)  \
  ECRYPT_decrypt_bytes(ctx, ciphertext, plaintext,                 \
    (blocks) * ECRYPT_BLOCKLENGTH)

#ifdef ECRYPT_GENERATES_KEYSTREAM

#define ECRYPT_keystream_blocks(ctx, keystream, blocks)            \
  ECRYPT_keystream_bytes(ctx, keystream,                           \
    (blocks) * ECRYPT_BLOCKLENGTH)

#endif

#else

#define ECRYPT_HAS_SINGLE_BLOCK_FUNCTION      /* [edit] */
#ifdef ECRYPT_HAS_SINGLE_BLOCK_FUNCTION

#define ECRYPT_encrypt_blocks(ctx, plaintext, ciphertext, blocks)     \
  ECRYPT_process_blocks(0, ctx, plaintext, ciphertext, blocks)

#define ECRYPT_decrypt_blocks(ctx, ciphertext, plaintext, blocks)     \
  ECRYPT_process_blocks(1, ctx, ciphertext, plaintext, blocks)

#define ECRYPT_process_blocks CRYPTO_NAMESPACE(ECRYPT_process_blocks)
void ECRYPT_process_blocks(
  int action,                 /* 0 = encrypt; 1 = decrypt; */
  ECRYPT_ctx* ctx, 
  const u8* input, 
  u8* output, 
  u32 blocks);                /* Message length in blocks. */

#else

#define ECRYPT_encrypt_blocks CRYPTO_NAMESPACE(ECRYPT_encrypt_blocks)
void ECRYPT_encrypt_blocks(
  ECRYPT_ctx* ctx, 
  const u8* plaintext, 
  u8* ciphertext, 
  u32 blocks);                /* Message length in blocks. */ 

#define ECRYPT_decrypt_blocks CRYPTO_NAMESPACE(ECRYPT_decrypt_blocks)
void ECRYPT_decrypt_blocks(
  ECRYPT_ctx* ctx, 
  const u8* ciphertext, 
  u8* plaintext, 
  u32 blocks);                /* Message length in blocks. */ 

#endif

#ifdef ECRYPT_GENERATES_KEYSTREAM

#define ECRYPT_keystream_blocks CRYPTO_NAMESPACE(ECRYPT_keystream_blocks)
void ECRYPT_keystream_blocks(
  ECRYPT_ctx* ctx,
  u8* keystream,
  u32 blocks);                /* Keystream length in blocks. */ 

#endif

#endif

#if (ECRYPT_VARIANT > ECRYPT_MAXVARIANT)
#error this variant does not exist
#endif
