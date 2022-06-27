#include "merge_sort.h"
#include <iostream>
#include <stdlib.h>
#include <cassert>
#include <algorithm>
#include <cstring>

masks global_masks;

inline __m256i reverse(__m256i& v) {
  return _mm256_permutevar8x32_epi32(v, global_masks.rev_idx_mask);
}

inline __m256i interleave_low(__m256i& a, __m256i& b) {
  return _mm256_unpacklo_epi32(a,b);
}

inline __m256i interleave_high(__m256i& a, __m256i& b) {
  return _mm256_unpackhi_epi32(a,b);
}

inline void minmax(const __m256i& a, const __m256i& b, 
    __m256i& minab, __m256i& maxab){
  minab = _mm256_min_epi32(a, b);
  maxab = _mm256_max_epi32(a, b);
  return;
}

inline void minmax(__m256i& a, __m256i& b){
  auto t = a;
  a = _mm256_min_epi32(a, b);
  b = _mm256_max_epi32(t, b);
  return;
}

inline __m256i shuffle(__m256i& a, int* idx_array) {
  __m256i idx = _mm256_load_si256((__m256i *)idx_array);
  return _mm256_permutevar8x32_epi32(a, idx);
}

inline __m256i register_shuffle(const __m256i& a, const __m256i& mask) {
  return _mm256_permutevar8x32_epi32(a, mask);
}

inline void transpose8(__m256* row0, __m256* row1, __m256* row2, __m256* row3,
                       __m256* row4, __m256* row5, __m256* row6, __m256* row7) {
  __m256 __t0, __t1, __t2, __t3, __t4, __t5, __t6, __t7;
  __m256 __tt0, __tt1, __tt2, __tt3, __tt4, __tt5, __tt6, __tt7;
  static const int mask0 = _MM_SHUFFLE(1,0,1,0);
  static const int mask1 = _MM_SHUFFLE(3,2,3,2);
  __t0 = _mm256_unpacklo_ps(*row0, *row1);
  __t1 = _mm256_unpackhi_ps(*row0, *row1);
  __t2 = _mm256_unpacklo_ps(*row2, *row3);
  __t3 = _mm256_unpackhi_ps(*row2, *row3);
  __t4 = _mm256_unpacklo_ps(*row4, *row5);
  __t5 = _mm256_unpackhi_ps(*row4, *row5);
  __t6 = _mm256_unpacklo_ps(*row6, *row7);
  __t7 = _mm256_unpackhi_ps(*row6, *row7);
  __tt0 = _mm256_shuffle_ps(__t0,__t2, mask0);
  __tt1 = _mm256_shuffle_ps(__t0,__t2, mask1);
  __tt2 = _mm256_shuffle_ps(__t1,__t3, mask0);
  __tt3 = _mm256_shuffle_ps(__t1,__t3, mask1);
  __tt4 = _mm256_shuffle_ps(__t4,__t6, mask0);
  __tt5 = _mm256_shuffle_ps(__t4,__t6, mask1);
  __tt6 = _mm256_shuffle_ps(__t5,__t7, mask0);
  __tt7 = _mm256_shuffle_ps(__t5,__t7, mask1);
  *row0 = _mm256_permute2f128_ps(__tt0, __tt4, 0x20);
  *row1 = _mm256_permute2f128_ps(__tt1, __tt5, 0x20);
  *row2 = _mm256_permute2f128_ps(__tt2, __tt6, 0x20);
  *row3 = _mm256_permute2f128_ps(__tt3, __tt7, 0x20);
  *row4 = _mm256_permute2f128_ps(__tt0, __tt4, 0x31);
  *row5 = _mm256_permute2f128_ps(__tt1, __tt5, 0x31);
  *row6 = _mm256_permute2f128_ps(__tt2, __tt6, 0x31);
  *row7 = _mm256_permute2f128_ps(__tt3, __tt7, 0x31);
}

inline void sort_columns(__m256i& row0, __m256i& row1, __m256i& row2, __m256i& row3,
                  __m256i& row4, __m256i& row5, __m256i& row6, __m256i& row7) {

    minmax(row0,row1);
    minmax(row2,row3);
    minmax(row4,row5);
    minmax(row6,row7);

    minmax(row0,row2);
    minmax(row1,row3);
    minmax(row4,row6);
    minmax(row5,row7);

    minmax(row1,row2);
    minmax(row0,row4);
    minmax(row5,row6);
    minmax(row3,row7);

    minmax(row1,row5);
    minmax(row2,row6);

    minmax(row1,row4);
    minmax(row3,row6);

    minmax(row2,row4);
    minmax(row3,row5);

    minmax(row3,row4);
}

inline void sort64(__m256i& row0, __m256i& row1, __m256i& row2, __m256i& row3,
                   __m256i& row4, __m256i& row5, __m256i& row6, __m256i& row7) {
  sort_columns(row0, row1, row2, row3, row4, row5, row6, row7);
  transpose8((__m256 *)&row0, (__m256 *)&row1, (__m256 *)&row2, (__m256 *)&row3,
             (__m256 *)&row4, (__m256 *)&row5, (__m256 *)&row6, (__m256 *)&row7);
}

inline void sort64(__m256i* row) {
  sort64(row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7]);
}

inline void bitonic_merge(__m256i& a, __m256i& b) {
  // phase 1 - 8 against 8
  b = reverse(b);
  minmax(a,b);
  intra_register_sort(a,b);
}

inline void intra_register_sort(__m256i& a8, __m256i& b8) {
  __m256i mina, maxa, minb, maxb;
  // phase 1
  auto a8_1 = register_shuffle(a8, global_masks.swap_128);
  auto b8_1 = register_shuffle(b8, global_masks.swap_128);

  minmax(a8, a8_1, mina, maxa);
  minmax(b8, b8_1, minb, maxb);

  auto a4 = _mm256_blend_epi32(mina, maxa, 0xf0);
  auto b4 = _mm256_blend_epi32(minb, maxb, 0xf0);

  // phase 2
  auto a4_1 = _mm256_shuffle_epi32(a4, 0x4e);
  auto b4_1 = _mm256_shuffle_epi32(b4, 0x4e);

  minmax(a4, a4_1, mina, maxa);
  minmax(b4, b4_1, minb, maxb);

  auto a2 = _mm256_unpacklo_epi64(mina, maxa);
  auto b2 = _mm256_unpacklo_epi64(minb, maxb);
  // phase 3
  auto a2_1 = _mm256_shuffle_epi32(a2, 0xb1);
  auto b2_1 = _mm256_shuffle_epi32(b2, 0xb1);

  minmax(a2, a2_1, mina, maxa);
  minmax(b2, b2_1, minb, maxb);

  a8 = _mm256_blend_epi32(mina, maxa, 0xaa);
  b8 = _mm256_blend_epi32(minb, maxb, 0xaa);
}

void initialize() {
  // directly set load store mask in 32B aligned memory
  alignas(32) int load_store_mask[8] = 
    {1<<31,1<<31,1<<31,1<<31,1<<31,1<<31,1<<31,1<<31};
  global_masks.load_store_mask = 
      _mm256_load_si256((__m256i *) &load_store_mask[0]);

  // load the remaining masks
  int rev_idx_mask[8] = {7,6,5,4,3,2,1,0};
  int swap_128[8] = {4,5,6,7,0,1,2,3};
  global_masks.rev_idx_mask = load_reg256(&rev_idx_mask[0]);
  global_masks.swap_128 = load_reg256(&swap_128[0]);
}

void merge_phase(int *a, int *out, int start, int mid, int end) {
  int i=start, j=mid+1, k=start;
  int i_end = i + mid - start + 1;
  int j_end = j + end - mid;

  auto ra = load_reg256(&a[i]);
  auto rb = load_reg256(&a[j]);

  i += SIMD_SIZE;
  j += SIMD_SIZE;

  // 8-by-8 merge
  if (mid-start+1 == SIMD_SIZE) {
    bitonic_merge(ra, rb);
    // save the smaller half
    store_reg256(&out[k], ra);
    k += SIMD_SIZE;
    // then save the larger half
    store_reg256(&out[k], rb);
    k += SIMD_SIZE;
    return;
  }

  do {
    bitonic_merge(ra, rb);
    
    // save the smaller half
    store_reg256(&out[k], ra);
    k += SIMD_SIZE;
    
    // use the larger half for the next comparison
    ra = rb;

    // select the input with the lowest value at the current pointer
    if (a[i] < a[j]) {
      rb = load_reg256(&a[i]);
      i += SIMD_SIZE;
    } else {
      rb = load_reg256(&a[j]);
      j += SIMD_SIZE;
    }
  } while (i < i_end && j < j_end);

  // merge the final pair of registers from each input
  bitonic_merge(ra, rb);
  store_reg256(&out[k], ra);
  k += SIMD_SIZE;
  ra = rb;

  // consume remaining data from a, if left
  while (i < i_end) {
    rb = load_reg256(&a[i]);
    i += SIMD_SIZE;
    bitonic_merge(ra, rb);
    store_reg256(&out[k], ra);
    k += SIMD_SIZE;
    ra = rb;
  }

  // consume remaining data from b, if left
  while (j < j_end) {
    rb = load_reg256(&a[j]);
    j += SIMD_SIZE;
    bitonic_merge(ra, rb);
    store_reg256(&out[k], ra);
    k += SIMD_SIZE;
    ra = rb;
  }

  // store the final batch
  store_reg256(&out[k], ra);
  k += SIMD_SIZE;
}

// minimum merge_size=16, minimum n=2*merge_size
void merge_pass(int *in, int *out, int n, int merge_size) {
  for (int i=0; i < n-1; i+=2*merge_size) {
    auto mid = i + merge_size - 1;
    auto end = std::min(i+2*merge_size-1, n-1);
    // check if there are 2 sub-arrays to merge
    if (mid < end) {
      // merge two merge_size arrays per iteration
      merge_phase(in, out, i, mid, end);
    } else {
      // copy the leftover data to output
      std::memcpy(out+i, in+i, (n-i)*sizeof(int));
    }
  }
}

// assume first sort phase has finished
std::pair<int *, int *> merge(int *a, int *b, size_t len) {
  int i=0;
  /*
   * even iterations: a->b
   * odd iterations: b->a
   */
  // start from 16-16 merge
  for (size_t pass_size=SIMD_SIZE; pass_size<len; pass_size*=2, i++) {
    if (i%2 == 0) {
      merge_pass(a, b, len, pass_size);
    } else {
      merge_pass(b, a, len, pass_size);
    }
  }

  if (i%2 == 0)
    return std::make_pair(a,b);
  return std::make_pair(b,a);
}

std::pair<int *, int *> merge_sort(int *a, int *b, size_t len) {
    __m256i rows[SIMD_SIZE];
  // if (len%64!=0) {
  //   // add padding
  //   auto i = a.size();
  //   auto end = ((i+64)/64)*64;
  //   while (i<end) {
  //     a.push_back(INT_MAX);
  //     i++;
  //   }
  //   // adjust b's size as well
  //   b.resize(a.size());
  // }

  assert(len%64 == 0);
  // assert(b.size() == a.size());

  for (size_t i=0; i < len; i+=SORT_SIZE) {
    for (int j=0; j<SORT_SIZE/SIMD_SIZE; j++) {
      rows[j] = load_reg256(&a[i+j*SIMD_SIZE]);
    }
    sort64(rows);
    for (int j=0; j<SORT_SIZE/SIMD_SIZE; j++) {
      store_reg256(&a[i+j*SIMD_SIZE], rows[j]);
    }
  }

  return merge(a, b, len);
}

// void print_test_array(__m256i res, const std::string& msg) {
//   std::cout << msg << std::endl;
//   for (int i=0; i<8; i++)
//     std::cout << ((int *)&res)[i] << "\t";
//   std::cout << std::endl;
// }

// void test_basic() {
//   __m256i min, max;
//   int test_arr1[8] = {1,2,3,4,15,16,17,18};
//   int test_arr2[8] = {11,12,13,14,5,6,7,8};
//   int idx[8] = {4,5,6,7,0,1,2,3};
//   __m256i test1 = load_reg256(&test_arr1[0]);
//   __m256i test2 = load_reg256(&test_arr2[0]);
//   __m256i mask = load_reg256(&idx[0]);
//   print_test_array(test1, "test1");
//   print_test_array(test2, "test2");
//   print_test_array(reverse(test1), "Reverse Output");
//   print_test_array(register_shuffle(test1, mask), "Register shuffle");
//   print_test_array(interleave_low(test1, test2), "interleave_low");
//   print_test_array(interleave_high(test1, test2), "interleave_high");
//   minmax(test1, test2, min, max);
//   print_test_array(min, "Minimum");
//   print_test_array(max, "Maximum");
// }
