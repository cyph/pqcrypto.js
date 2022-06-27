#ifndef casb
#define casb

#define _casb_ENC_ROUND_KEY_GEN(round_keys, key)		(generate_round_keys(round_keys, key))
#define CRYPTO_BC_NUM_ROUNDS (50)
typedef unsigned char u8;

void generate_round_keys(u8 (*round_key_nibbles)[32], const u8 *key_bytes);
void casb_enc(u8 *ct, const u8 (*round_keys)[32], const u8 *pt);

#endif
