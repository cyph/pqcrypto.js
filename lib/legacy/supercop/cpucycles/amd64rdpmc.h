/*
cpucycles amd64rdpmc.h version 20210108
D. J. Bernstein
Public domain.
*/

#ifndef CPUCYCLES_amd64rdpmc_h
#define CPUCYCLES_amd64rdpmc_h

#ifdef __cplusplus
extern "C" {
#endif

extern long long cpucycles_amd64rdpmc(void);
extern long long cpucycles_amd64rdpmc_persecond(void);

#ifdef __cplusplus
}
#endif

#ifndef cpucycles_implementation
#define cpucycles_implementation "amd64rdpmc"
#define cpucycles cpucycles_amd64rdpmc
#define cpucycles_persecond cpucycles_amd64rdpmc_persecond
#endif

#endif
