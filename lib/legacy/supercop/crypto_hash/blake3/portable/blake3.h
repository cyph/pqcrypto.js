#ifndef BLAKE3_H
#define BLAKE3_H

#include <stddef.h>
#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define BLAKE3_KEY_LEN 32
#define BLAKE3_OUT_LEN 32
#define BLAKE3_BLOCK_LEN 64
#define BLAKE3_CHUNK_LEN 1024
#define BLAKE3_MAX_DEPTH 54
#define BLAKE3_MAX_SIMD_DEGREE 16

void blake3_default_hash(const uint8_t *input, size_t input_len, uint8_t output[BLAKE3_OUT_LEN]);

#ifdef  __cplusplus
}
#endif

#endif /* BLAKE3_H */
