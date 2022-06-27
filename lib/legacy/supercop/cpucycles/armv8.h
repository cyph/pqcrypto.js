/*
cpucycles armv8.h version 20190803
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_armv8_h
#define CPUCYCLES_armv8_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_armv8(void);
extern long long cpucycles_armv8_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "armv8"
#define cpucycles cpucycles_armv8
#define cpucycles_persecond cpucycles_armv8_persecond
#endif

#endif
