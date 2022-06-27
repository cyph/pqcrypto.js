/*
cpucycles arm1176.h version 20200114
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_arm1176_h
#define CPUCYCLES_arm1176_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_arm1176(void);
extern long long cpucycles_arm1176_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "arm1176"
#define cpucycles cpucycles_arm1176
#define cpucycles_persecond cpucycles_arm1176_persecond
#endif

#endif
