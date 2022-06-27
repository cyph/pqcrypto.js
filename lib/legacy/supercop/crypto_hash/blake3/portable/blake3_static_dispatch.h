#define SIMD_DEGREE 1
#define blake3_compress_in_place blake3_compress_in_place_portable
#define blake3_hash_many blake3_hash_many_portable

void blake3_compress_in_place_portable(
  uint32_t cv[8], const uint8_t block[BLAKE3_BLOCK_LEN], uint8_t block_len,
  uint64_t counter, uint8_t flags);

void blake3_hash_many_portable(
  const uint8_t *const *inputs, size_t num_inputs, size_t blocks,
  const uint32_t key[8], uint64_t counter, bool increment_counter,
  uint8_t flags, uint8_t flags_start, uint8_t flags_end, uint8_t *out);
