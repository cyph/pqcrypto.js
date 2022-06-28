/**
 * \file ffi_elt.cpp
 * \brief Implementation of ffi_elt.h using NTL library
 */

#include "ffi_elt.h"


/** 
 * \fn void ffi_elt_set(ffi_elt& o, const ffi_elt& e)
 * \brief This function copies a finite field element into another one
 *
 * \param[out] o Finite field element
 * \param[in] e Finite field element
 */
void ffi_elt_set(ffi_elt& o, const ffi_elt& e) {
  o = e; 
}



/** 
 * \fn void ffi_elt_set_zero(ffi_elt& o)
 * \brief This function sets a finite field element to zero
 *
 * \param[out] o Finite field element
 */
void ffi_elt_set_zero(ffi_elt& o) {
	o = 0;
}



/** 
 * \fn void ffi_elt_set_one(ffi_elt& o)
 * \brief This function sets a finite field element to one
 *
 * \param[out] o Finite field element
 */
void ffi_elt_set_one(ffi_elt &o) {
	o = 1;
}



/** 
 * \fn void ffi_elt_set_random_using_rng(ffi_elt& o)
 * \brief This function sets a finite field element to a random value using the NIST randombytes function
 *
 * \param[out] o Finite field element
 */
void ffi_elt_set_random_using_rng(ffi_elt& o) {
  ntl_random_using_rng(o._GF2E__rep, FIELD_M);
}



/** 
 * \fn void ffi_elt_set_random_using_seedexpander(ffi_elt& o, AES_XOF_struct* ctx)
 * \brief This function sets a finite field element to a random value using the NIST seed expander
 *
 * \param[out] o Finite field element
 * \param[in] ctx NIST seed expander
 */
void ffi_elt_set_random_using_seedexpander(ffi_elt& o, AES_XOF_struct* ctx) {
  ntl_random_using_seedexpander(o._GF2E__rep, FIELD_M, ctx);
}



/** 
 * \fn int ffi_elt_is_zero(const ffi_elt& e)
 * \brief This functions tests if a finite field element is equal to zero
 *
 * \param[in] e Finite field element
 * \return 1 if <b>e</b> is equal to zero, 0 otherwise
 */
int ffi_elt_is_zero(const ffi_elt& e) {
	return (int) IsZero(e);
}



/** 
 * \fn void ffi_elt_inv(ffi_elt& o, const ffi_elt& e)
 * \brief This function computes the multiplicative inverse of a finite field element
 *
 * \param[out] o Multiplicative inverse of <b>e</b>
 * \param[in] e Finite field element
 */
void ffi_elt_inv(ffi_elt& o, const ffi_elt& e) {
  inv(o, e);
}



/** 
 * \fn void ffi_elt_add(ffi_elt& o, const ffi_elt& e1, const ffi_elt& e2)
 * \brief This function adds two finite field elements
 *
 * \param[out] o Sum of <b>e1</b> and <b>e2</b>
 * \param[in] e1 Finite field element
 * \param[in] e2 Finite field element
 */
void ffi_elt_add(ffi_elt& o, const ffi_elt& e1, const ffi_elt& e2) {
  add(o, e1, e2);
}



/** 
 * \fn void ffi_elt_mul(ffi_elt& o, const ffi_elt& e1, const ffi_elt& e2)
 * \brief This function multiplies two finite field elements
 *
 * \param[out] o Product of <b>e1</b> and <b>e2</b>
 * \param[in] e1 Finite field element
 * \param[in] e2 Finite field element
 */
void ffi_elt_mul(ffi_elt& o, const ffi_elt& e1, const ffi_elt& e2) {
  mul(o, e1, e2);
}



/** 
 * \fn void ffi_elt_to_string(unsigned char* str, const ffi_elt& e)
 * \brief This function parses a finite field element into a string
 *
 * \param[out] str String obtained
 * \param[in] e Finite field element to parse
 */
void ffi_elt_to_string(unsigned char* str, const ffi_elt& e) {
  BytesFromGF2X(str, rep(e), FIELD_ELT_BYTES);
}



/** 
 * \fn void ffi_elt_from_string(ffi_elt& e, const unsigned char* str)
 * \brief This function parses a finite field element from a string
 *
 * \param[out] e Finite field element
 * \param[in] str String to parse
 */
void ffi_elt_from_string(ffi_elt& e, const unsigned char* str) {
  conv(e, GF2XFromBytes(str, FIELD_ELT_BYTES));
}



/** 
 * \fn void ffi_elt_print(const ffi_elt& e)
 * \brief This function prints the value of a finite field element
 *
 * \param[in] e Finite field element
 */
void ffi_elt_print(const ffi_elt& e) {
  std::cout << e << std::endl;
}



/** 
 * \fn static inline unsigned long WordFromBytes(const unsigned char* buf, long n)
 * \brief This function is a copy of the NTL WordFromBytes function
 *
 * \param[in] buf Buffer 
 * \param[in] n Size of the buffer
 * \return unsigned long 
 */
static inline unsigned long ntl_word_from_bytes(const unsigned char* buf, long n) {
  unsigned long res = 0;

  for(long i = n-1 ; i >= 0 ; i--) {
    res = (res << 8) | buf[i];
  }

  return res;
}



/** 
 * \fn unsigned long ntl_random_word_using_rng()
 * \brief This function modifies the NTL RandomWord function in order to use the NIST randombytes function
 *
 * \return unsigned long 
 */
unsigned long ntl_random_word_using_rng() {
  unsigned char buf[NTL_BITS_PER_LONG/8];
  randombytes(buf, NTL_BITS_PER_LONG/8);
  return ntl_word_from_bytes(buf, NTL_BITS_PER_LONG/8);
}



/** 
 * \fn unsigned long ntl_random_word_using_seedexpander(AES_XOF_struct* ctx)
 * \brief This function modifies the NTL RandomWord function in order to use the NIST seed expander
 *
 * \param[in] ctx NIST seed expander
 * \return unsigned long 
 */
unsigned long ntl_random_word_using_seedexpander(AES_XOF_struct* ctx) {
  unsigned char buf[NTL_BITS_PER_LONG/8];
  seedexpander(ctx, buf, NTL_BITS_PER_LONG/8);
  return ntl_word_from_bytes(buf, NTL_BITS_PER_LONG/8);
}



/** 
 * \fn unsigned long ntl_random_bits_ulong_using_rng(long l)
 * \brief This function modifies the NTL RandomBits_ulong function in order to use the NIST randombytes function
 *
 * \param[in] l long
 * \return unsigned long 
 */
unsigned long ntl_random_bits_ulong_using_rng(long l) {
  if(l <= 0) return 0;
  if(l > NTL_BITS_PER_LONG) ResourceError("RandomBits: length too big");

  unsigned char buf[NTL_BITS_PER_LONG/8];
  long nb = (l+7)/8;

  randombytes(buf, nb);
  unsigned long res = ntl_word_from_bytes(buf, nb);
  if(l < NTL_BITS_PER_LONG) res = res & ((1UL << l)-1UL);
  return res;
}



/** 
 * \fn unsigned long ntl_random_bits_ulong_using_seedexpander(long l, AES_XOF_struct* ctx)
 * \brief This function modifies the NTL RandomBits_ulong function in order to use the NIST seed expander
 *
 * \param[in] l long
 * \param[in] ctx NIST seed expander
 * \return unsigned long 
 */
unsigned long ntl_random_bits_ulong_using_seedexpander(long l, AES_XOF_struct* ctx) {
  if(l <= 0) return 0;
  if(l > NTL_BITS_PER_LONG) ResourceError("RandomBits: length too big");

  unsigned char buf[NTL_BITS_PER_LONG/8];
  long nb = (l+7)/8;

  seedexpander(ctx, buf, nb);
  unsigned long res = ntl_word_from_bytes(buf, nb);
  if(l < NTL_BITS_PER_LONG) res = res & ((1UL << l)-1UL);
  return res;
}



/** 
 * \fn void ntl_random_using_rng(GF2X& x, long n)
 * \brief This function modifies the NTL random function in order to use the NIST randombytes function
 *
 * \param[in] x GF2X 
 * \param[in] n long
 */
void ntl_random_using_rng(GF2X& x, long n) {
  if(n < 0) LogicError("GF2X random: negative length");
  if(NTL_OVERFLOW(n, 1, 0)) ResourceError("GF2X random: excessive length");

  long wl = (n+NTL_BITS_PER_LONG-1)/NTL_BITS_PER_LONG;
  x.xrep.SetLength(wl);

  for(long i = 0 ; i < wl-1 ; i++) {
    x.xrep[i] = ntl_random_word_using_rng();
  }

  if(n > 0) {
    long pos = n % NTL_BITS_PER_LONG;
    if (pos == 0) pos = NTL_BITS_PER_LONG;
    x.xrep[wl-1] = ntl_random_bits_ulong_using_rng(pos);
  }

  x.normalize();
}



/** 
 * \fn void ntl_random_using_seedexpander(GF2X& x, long n, AES_XOF_struct* ctx)
 * \brief This function modifies the NTL random function in order to use the NIST seed expander
 *
 * \param[in] x GF2X 
 * \param[in] n long
 * \param[in] ctx NIST seed expander
 */
void ntl_random_using_seedexpander(GF2X& x, long n, AES_XOF_struct* ctx) {
  if(n < 0) LogicError("GF2X random: negative length");
  if(NTL_OVERFLOW(n, 1, 0)) ResourceError("GF2X random: excessive length");

  long wl = (n+NTL_BITS_PER_LONG-1)/NTL_BITS_PER_LONG;
  x.xrep.SetLength(wl);

  for(long i = 0 ; i < wl-1 ; i++) {
    x.xrep[i] = ntl_random_word_using_seedexpander(ctx);
  }

  if(n > 0) {
    long pos = n % NTL_BITS_PER_LONG;
    if (pos == 0) pos = NTL_BITS_PER_LONG;
    x.xrep[wl-1] = ntl_random_bits_ulong_using_seedexpander(pos, ctx);
  }

  x.normalize();
}

