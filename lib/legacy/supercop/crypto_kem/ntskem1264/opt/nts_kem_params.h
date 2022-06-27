/**
 *  nts_kem_params.h
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(12, 64)
 *  Platform: Intel 64-bit
 *
 *  This file is part of the optimized implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#ifndef __NTS_KEM_PARAMS_H
#define __NTS_KEM_PARAMS_H

/**
 *  NTS-KEM(12, 64) : [4096, 3328, 129]_2, F_{2^12}
 **/

#define NTS_KEM_KEY_SIZE             32
#define NTS_KEM_PARAM_M              12
#define NTS_KEM_PARAM_T              64
#define NTS_KEM_PARAM_N              4096
#define NTS_KEM_PARAM_K              3328
#define NTS_KEM_PARAM_A              3072
#define NTS_KEM_PARAM_B              256
#define NTS_KEM_PARAM_C              768
#define NTS_KEM_PARAM_BC             1024
#define NTS_KEM_PARAM_BC_DIV_64      16
#define NTS_KEM_PARAM_BC_DIV_128     8
#define NTS_KEM_PARAM_BC_DIV_256     4
#define NTS_KEM_PARAM_R_DIV_64	     12
#define NTS_KEM_PARAM_R_DIV_128      6
#define NTS_KEM_PARAM_R_DIV_256      3
#define NTS_KEM_PARAM_N_DIV_64       64
#define NTS_KEM_PARAM_N_DIV_128      32
#define NTS_KEM_PARAM_N_DIV_256      16
#define NTS_KEM_PARAM_CEIL_N_BYTE    512
#define NTS_KEM_PARAM_CEIL_K_BYTE    416
#define NTS_KEM_PARAM_CEIL_R_BYTE    96
#define NTS_KEM_PRIVATE_KEY_SIZE     9216
#define NTS_KEM_PUBLIC_KEY_SIZE      319488
#define NTS_KEM_CIPHERTEXT_SIZE      128

#endif /* __NTS_KEM_PARAMS_H */
