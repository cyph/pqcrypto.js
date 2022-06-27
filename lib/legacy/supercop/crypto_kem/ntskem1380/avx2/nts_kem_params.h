/**
 *  nts_kem_params.h
 *  NTS-KEM
 *
 *  Parameter: NTS-KEM(13, 80)
 *  Platform: AVX2
 *
 *  This file is part of the additional implemention of NTS-KEM
 *  submitted as part of NIST Post-Quantum Cryptography
 *  Standardization Process.
 **/

#ifndef __NTS_KEM_PARAMS_H
#define __NTS_KEM_PARAMS_H

/**
 *  NTS-KEM(13,80) : [8192, 7152, 161]_2, F_{2^13}
 **/

#define NTS_KEM_KEY_SIZE             32
#define NTS_KEM_PARAM_M              13
#define NTS_KEM_PARAM_T              80
#define NTS_KEM_PARAM_N              8192
#define NTS_KEM_PARAM_K              7152
#define NTS_KEM_PARAM_A              6896
#define NTS_KEM_PARAM_B              256
#define NTS_KEM_PARAM_C              1040
#define NTS_KEM_PARAM_BC             1296
#define NTS_KEM_PARAM_BC_DIV_64      21
#define NTS_KEM_PARAM_BC_DIV_128     11
#define NTS_KEM_PARAM_BC_DIV_256     6
#define NTS_KEM_PARAM_R_DIV_64       17
#define NTS_KEM_PARAM_R_DIV_128      9
#define NTS_KEM_PARAM_R_DIV_256      5
#define NTS_KEM_PARAM_N_DIV_64       128
#define NTS_KEM_PARAM_N_DIV_128      64
#define NTS_KEM_PARAM_N_DIV_256      32
#define NTS_KEM_PARAM_CEIL_N_BYTE    1024
#define NTS_KEM_PARAM_CEIL_K_BYTE    894
#define NTS_KEM_PARAM_CEIL_R_BYTE    130
#define NTS_KEM_PRIVATE_KEY_SIZE     17524
#define NTS_KEM_PUBLIC_KEY_SIZE      929760
#define NTS_KEM_CIPHERTEXT_SIZE      162

#endif /* __NTS_KEM_PARAMS_H */
