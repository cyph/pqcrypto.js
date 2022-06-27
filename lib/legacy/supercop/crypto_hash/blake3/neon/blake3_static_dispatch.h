#define SIMD_DEGREE 4
#define blake3_compress_in_place blake3_compress_in_place_portable
#define blake3_hash_many blake3_hash_many_neon

void blake3_compress_in_place_portable(
  uint32_t cv[8], const uint8_t block[BLAKE3_BLOCK_LEN], uint8_t block_len,
  uint64_t counter, uint8_t flags);

void blake3_hash_many_neon(
  const uint8_t *const *inputs, size_t num_inputs, size_t blocks,
  const uint32_t key[8], uint64_t counter, bool increment_counter,
  uint8_t flags, uint8_t flags_start, uint8_t flags_end, uint8_t *out);

// Normally the NEON build requires setting BLAKE3_USE_NEON=1. Since we're not
// going to set that here, explicitly override the max SIMD degree. This works
// because this header gets included last.
#define MAX_SIMD_DEGREE 4
#define MAX_SIMD_DEGREE_OR_2 4
