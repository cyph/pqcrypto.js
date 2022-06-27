/**
 * @file lwe-arithmetics.c
 * @author Takuya HAYASHI (t-hayashi@eedept.kobe-u.ac.jp)
 * @brief Arithmetics used in LOTUS-CPA-PKE
 */
#include "type.h"
#include "param.h"

/**
 * @brief P -= A * S
 */
void submul(U16 *P, const U16 *A, const U16 *S){
  int i, j, k;
  for(i = 0; i < _LOTUS_LWE_DIM; ++i){
    for(j = 0; j < _LOTUS_LWE_DIM; ++j){
      for(k = 0; k < _LOTUS_LWE_PT; ++k){
	P[i * _LOTUS_LWE_PT + k] -=
	  A[i * _LOTUS_LWE_DIM + j] * S[j * _LOTUS_LWE_PT + k];
      }
    }
  }
}

/**
 * @brief c += e * [A|P]
 */
void addmul_concat(U16 *c, const U16 *e, const U16 *A, const U16 *P){
  int i, j;
  /* c1 += e * A */
  for(i = 0; i < _LOTUS_LWE_DIM; ++i){
    for(j = 0; j < _LOTUS_LWE_DIM; ++j){
      c[j] += e[i] * A[i * _LOTUS_LWE_DIM + j];
    }
  }

  /* c2 += e * P */
  for(i = 0; i < _LOTUS_LWE_DIM; ++i){
    for(j = 0; j < _LOTUS_LWE_PT; ++j){
      c[_LOTUS_LWE_DIM + j] += e[i] * P[i * _LOTUS_LWE_PT + j];
    }
  }
}

/**
 * @brief m += c * S
 */
void addmul(U16 *m, const U16 *c1, const U16 *S){
  int i, j;
  for(i = 0; i < _LOTUS_LWE_DIM; ++i){
    for(j = 0; j < _LOTUS_LWE_PT; ++j){
      m[j] += c1[i] * S[i * _LOTUS_LWE_PT + j];
    }
  }
}

/**
 * @brief v = v mod q, where q = 2^n
 */
void redc(U16 *v, const int len){
  int i;
  for(i = 0; i < len; ++i){
    v[i] &= (_LOTUS_LWE_MOD - 1);
  }
}

/**
 * @brief c2 += sigma * \ceil q / 2 \rceil, where sigma \in \{0, 1}^l and q = 2^n 
 */
void add_sigma(U16 *c2, const U8 *sigma){
  int i, j, idx = 0;
  for(i = 0; i < _LOTUS_LWE_PT_BYTES; ++i){
    for(j = 0; j < 8; ++j){
      /* move j-th bit of sigma[i] to (_LOTUS_LWE_LOG2_MOD - 1)-th bit */
      c2[idx] += (((U16)sigma[i]) << (_LOTUS_LWE_LOG2_MOD - 1 - j)) & (_LOTUS_LWE_MOD >> 1);
      ++idx;
    }
  }
}

/**
 * @brief sigma \in {0, 1}^l, where sigma[i] = 0 when t[i] \in [-q/4, q/4), 1 otherwise.
 */
void reconstruct(U8 *sigma, const U16 *t){
  int i, j, idx = 0;
  /* sigma[i] = 0 if 0 <= t[i] < q / 4 or 3q / 4 <= t[i] < q,
     otherwise sigma[i] = 1, i.e., q / 4 <= t[i] < 3q / 4.
     This means that, for q = 2^k, 
     sigma[i] = b_{k-1} xor b_{k-2} where b_j is a j-th bit of t[idx],
     since {0 <= t[idx] < q / 4} or {3q / 4 <= t[idx] < q} then {b_{k-1} = 0 and b_{k-2} = 0} 
     or {b_{k-1} = 1 and b_{k-2} = 1}, and {q / 4 <= t[idx] < 3q / 4},
     then {b_{k-1} = 1 and b_{k-2} = 0} or {b_{k-1} = 0 and b_{k-2} = 1}. */
  for(i = 0; i < _LOTUS_LWE_PT_BYTES; ++i){
    sigma[i] = 0;
    for(j = 0; j < 8; ++j){
      sigma[i] |= (U8)((((t[idx] >> (_LOTUS_LWE_LOG2_MOD - 1))
			 ^ (t[idx] >> (_LOTUS_LWE_LOG2_MOD - 2))) & 1) << j);
      ++idx;
    }
  }
}
