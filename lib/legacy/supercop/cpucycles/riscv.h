/*
cpucycles riscv.h version 20190803
D. J. Bernstein
Romain Dolbeau
Public domain.
*/

#ifndef CPUCYCLES_riscv_h
#define CPUCYCLES_riscv_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_riscv(void);
extern long long cpucycles_riscv_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "riscv"
#define cpucycles cpucycles_riscv
#define cpucycles_persecond cpucycles_riscv_persecond
#endif

#endif
