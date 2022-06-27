/// @file blas_comm.h
/// @brief Common functions for linear algebra.
///
#ifndef _BLAS_COMM_H_
#define _BLAS_COMM_H_

#include <stdint.h>

//IF_CRYPTO_CORE:define CRYPTO_NAMESPACE


/// @brief get an element from GF(16) vector .
///
/// @param[in]  a         - the input vector a.
/// @param[in]  i         - the index in the vector a.
/// @return  the value of the element.
///
static inline uint8_t gf16v_get_ele(const uint8_t *a, unsigned i) {
    uint8_t r = a[i >> 1];
    uint8_t r0 = r&0xf;
    uint8_t r1 = r>>4;
    uint8_t m = (uint8_t)(-(i&1));
    return (r1&m)|((~m)&r0);
}

/// @brief set an element for a GF(16) vector .
///
/// @param[in,out]   a   - the vector a.
/// @param[in]  i        - the index in the vector a.
/// @param[in]  v        - the value for the i-th element in vector a.
/// @return  the value of the element.
///
static inline uint8_t gf16v_set_ele(uint8_t *a, unsigned i, uint8_t v) {
    uint8_t m = 0xf ^ (-(i&1));   ///  1--> 0xf0 , 0--> 0x0f
    uint8_t ai_remaining = a[i>>1] & (~m);   /// erase
    a[i>>1] = ai_remaining | (m&(v<<4))|(m&v&0xf);  /// set
    return v;
}


/// @brief get an element from GF(256) vector .
///
/// @param[in]  a         - the input vector a.
/// @param[in]  i         - the index in the vector a.
/// @return  the value of the element.
///
static inline uint8_t gf256v_get_ele(const uint8_t *a, unsigned i) { return a[i]; }


/// @brief set an element for a GF(256) vector .
///
/// @param[in,out]   a   - the vector a.
/// @param[in]  i        - the index in the vector a.
/// @param[in]  v        - the value for the i-th element in vector a.
/// @return  the value of the element.
///
static inline uint8_t gf256v_set_ele(uint8_t *a, unsigned i, uint8_t v) { a[i]=v; return v; }


#ifdef  __cplusplus
extern  "C" {
#endif


/// @brief set a vector to 0.
///
/// @param[in,out]   b      - the vector b.
/// @param[in]  _num_byte   - number of bytes for the vector b.
///
void gf256v_set_zero(uint8_t *b, unsigned _num_byte);


/// @brief check if a vector is 0.
///
/// @param[in]   a          - the vector a.
/// @param[in]  _num_byte   - number of bytes for the vector a.
/// @return  1(true) if a is 0. 0(false) else.
///
unsigned gf256v_is_zero(const uint8_t *a, unsigned _num_byte);




#ifdef  __cplusplus
}
#endif

#endif  // _BLAS_COMM_H_

