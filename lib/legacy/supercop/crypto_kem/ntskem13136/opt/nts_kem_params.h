/**
 *  nts_kem_params.h
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 136)
 *  Platform: Intel 64-bit
 *
 *  This file is part of the optimized implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#ifndef __NTS_KEM_PARAMS_H
#define __NTS_KEM_PARAMS_H

/**
 *  NTS-KEM(13,136): [8192, 6424, 273]_2, F_{2^13}
 **/

#define NTS_KEM_KEY_SIZE             32
#define NTS_KEM_PARAM_M              13
#define NTS_KEM_PARAM_T              136
#define NTS_KEM_PARAM_N              8192
#define NTS_KEM_PARAM_K              6424
#define NTS_KEM_PARAM_A              6168
#define NTS_KEM_PARAM_B              256
#define NTS_KEM_PARAM_C              1768
#define NTS_KEM_PARAM_BC             2024
#define NTS_KEM_PARAM_BC_DIV_64      32
#define NTS_KEM_PARAM_BC_DIV_128     16
#define NTS_KEM_PARAM_BC_DIV_256     8
#define NTS_KEM_PARAM_R_DIV_64	     28
#define NTS_KEM_PARAM_R_DIV_128      14
#define NTS_KEM_PARAM_R_DIV_256      7
#define NTS_KEM_PARAM_N_DIV_64       128
#define NTS_KEM_PARAM_N_DIV_128      64
#define NTS_KEM_PARAM_N_DIV_256      32
#define NTS_KEM_PARAM_CEIL_N_BYTE    1024
#define NTS_KEM_PARAM_CEIL_K_BYTE    803
#define NTS_KEM_PARAM_CEIL_R_BYTE    221
#define NTS_KEM_PRIVATE_KEY_SIZE     19890
#define NTS_KEM_PUBLIC_KEY_SIZE      1419704
#define NTS_KEM_CIPHERTEXT_SIZE      253

#endif /* __NTS_KEM_PARAMS_H */
