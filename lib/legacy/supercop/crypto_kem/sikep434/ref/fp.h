//
// Supersingular Isogeny Key Encapsulation Ref. Library
//
// InfoSec Global Inc., 2017-2020
// Basil Hess <basil.hess@infosecglobal.com>
//

/** @file
Prime field F_p arithmetic API
*/

#ifndef ISOGENY_REF_FP_H
#define ISOGENY_REF_FP_H

#include <gmp.h>

/**
 * Type for multi-precision arithmetic
 */
typedef mpz_t mp;

/**
 * Finite field parameters and arithmetic, given the modulus.
 */
typedef struct _ff_Params ff_Params;

struct _ff_Params {

  /* The modulus */
  mp mod;

  void ( *init )(const ff_Params *p, mp a);

  void ( *add )(const ff_Params *p, const mp a, const mp b, mp c);

  void ( *clear )(const ff_Params *p, mp a);

  void ( *constant )(const ff_Params *p, unsigned long a, mp b);

  void ( *copy )(const ff_Params *p, mp dst, const mp src);

  int ( *isEqual )(const ff_Params *p, const mp a, const mp b);

  void ( *invert )(const ff_Params *p, const mp a, mp b);

  int ( *isBitSet )(const ff_Params *p, const mp a, const unsigned long index);

  int ( *isConstant )(const ff_Params *p, const mp a, const size_t constant);

  void ( *multiply )(const ff_Params *p, const mp a, const mp b, mp c);

  void ( *negative )(const ff_Params *p, const mp a, mp b);

  void ( *pow )(const ff_Params *p, const mp a, const mp b, mp c);

  int ( *rand )(const ff_Params *p, mp a);

  void ( *square )(const ff_Params *d, const mp a, mp b);

  void ( *subtract )(const ff_Params *p, const mp a, const mp b, mp c);

  void ( *unity )(const ff_Params *p, mp b);

  void ( *zero )(const ff_Params *p, mp a);

};

/**
 * Initializes the Finite field parameters with GMP implementations.
 * @param params Finite field parameters to be initialized.
 */
void
set_gmp_fp_params(ff_Params *params);

/**
 * Imports a string to a multi-precision type
 */
void
mp_import(mp rop, size_t count, int order, size_t size, int endian, size_t nails, const void *op);

/**
 * Exports a multi-precision type to a string
 */
void
mp_export(void *rop, size_t *countp, int order, size_t size, int endian, size_t nails, const mp op);

void
mp_mod(const mp a, const mp mod, mp b);

/**
 * Size of a multi-precision value in a given base.
 * @param a Multi-precision value
 * @param base base
 * @return size of `a` in `base`
 */
size_t mp_sizeinbase(const mp a, int base);

/**
 * `c` = `a`^`b`
 * @param a
 * @param b
 * @param c
 */
void
mp_pow(const unsigned long a, const unsigned long b, mp c);


/**
 * Addition
 *
 * @param p Finite field parameters
 * @param a
 * @param b
 * @param c =a+b (mod p)
 */
void
fp_Add(const ff_Params *p, const mp a, const mp b, mp c);

/**
 * Clearing/deinitialization of an fp element
 *
 * @param p Finite field parameters
 * @param a To be cleared
 */
void
fp_Clear(const ff_Params *p, mp a);

/**
 * Set to an integer constant
 *
 * @param p Finite field parameters
 * @param a integer constant
 * @param b MP element to be set
 */
void
fp_Constant(const ff_Params *p, unsigned long a, mp b);

/**
 * Copy one fp element to another.
 * dst = src
 *
 * @param p Finite field parameters
 * @param dst Destination
 * @param src Source
 */
void
fp_Copy(const ff_Params *p, mp dst, const mp src);

/**
 * Initialization
 *
 * @param p Finite field parameters
 * @param a Element to be intiialized
 */
void
fp_Init(const ff_Params* p, mp a);

/**
 * Checking for equality
 *
 * @param p Finite field parameters
 * @param a
 * @param b
 * @return 1 if a equals b, 0 otherwise
 */
int
fp_IsEqual(const ff_Params *p, const mp a, const mp b);

/**
 * Inversion
 *
 * @param p Finite field parameters
 * @param a
 * @param b =a^-1 (mod p)
 */
void
fp_Invert(const ff_Params *p, const mp a, mp b);

/**
 * Checks if the i'th bit is set
 *
 * @param p Finite field parameters
 * @param a
 * @param i index
 * @return 1 if i'th bit in a is set, 0 otherwise
 */
int
fp_IsBitSet(const ff_Params *p, const mp a, const unsigned long i);

/**
 * Checks equality with an integer constant
 *
 * @param p Finite field parameters
 * @param a
 * @param constant
 * @return 1 if a == constant, 0 otherwise
 */
int
fp_IsConstant(const ff_Params *p, const mp a, const size_t constant);

/**
 * Multiplication
 *
 * @param p Finite field parameters
 * @param a
 * @param b
 * @param c =a*b (mod p)
 */
void
fp_Multiply(const ff_Params *p, const mp a, const mp b, mp c);

/**
 * Negation
 *
 * @param p Finite field parameters
 * @param a
 * @param b =-a (mod p)
 */
void
fp_Negative(const ff_Params *p, const mp a, mp b);

/**
 * Exponentiation
 *
 * @param p Finite field parameters
 * @param a
 * @param b
 * @param c = a^b (mod p)
 */
void
fp_Pow(const ff_Params *p, const mp a, const mp b, mp c);

/**
 * Generation of a random element in {0, ..., p->modulus - 1}
 *
 * @param p Finite field parameters
 * @param a Random element in {0, ..., p->modulus - 1}
 */
void
fp_Rand(const ff_Params *p, mp a);

/**
 * Squaring
 *
 * @param p Finite field parameters
 * @param a
 * @param b =a^2 (mod p)
 */
void
fp_Square(const ff_Params *p, const mp a, mp b);

/**
 * Subtraction
 *
 * @param p Finite field parameters
 * @param a
 * @param b
 * @param c =a-b (mod p)
 */
void
fp_Subtract(const ff_Params *p, const mp a, const mp b, mp c);

/**
 * Set to unity (1)
 *
 * @param p Finite field parameters
 * @param b = 1
 */
void
fp_Unity(const ff_Params *p, mp b);

/**
 * Set to zero
 *
 * @param p Finite field parameters
 * @param a = 0
 */
void
fp_Zero(const ff_Params *p, mp a);

/**
 * Decodes and sets an element to an hex value
 *
 * @param hexStr
 * @param a = hexString (decoded)
 */
void
fp_ImportHex(const char *hexStr, mp a);

#endif //ISOGENY_REF_FP_H
