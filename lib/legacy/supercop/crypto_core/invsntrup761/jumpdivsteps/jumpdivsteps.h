#ifndef _JUMPDIVSTEPS_H_
#define _JUMPDIVSTEPS_H_


#include <immintrin.h>


#define jump32divsteps CRYPTO_NAMESPACE(jump32divsteps)
#define jump64divsteps CRYPTO_NAMESPACE(jump64divsteps)
#define jump128divsteps CRYPTO_NAMESPACE(jump128divsteps)
#define jump256divsteps CRYPTO_NAMESPACE(jump256divsteps)
#define jump16xdivsteps CRYPTO_NAMESPACE(jump16xdivsteps)
#define jump32xdivsteps CRYPTO_NAMESPACE(jump32xdivsteps)
#define jump64xdivsteps CRYPTO_NAMESPACE(jump64xdivsteps)
#define jump128xdivsteps CRYPTO_NAMESPACE(jump128xdivsteps)
#define jump256xdivsteps CRYPTO_NAMESPACE(jump256xdivsteps)



#ifdef  __cplusplus
extern  "C" {
#endif


int jump32divsteps(int delta, __m256i *f, __m256i *g, __m256i *uvqr);

int jump64divsteps(int delta, __m256i *f, __m256i *g, __m256i *uvqr);

int jump128divsteps(int delta, __m256i *f, __m256i *g, __m256i *uvqr);

int jump256divsteps(int delta, __m256i *f, __m256i *g, __m256i *uvqr);


int jump32xdivsteps(int delta, __m256i *f, __m256i *g, __m256i *uvqr, int t);

int jump64xdivsteps(int delta, __m256i *f, __m256i *g, __m256i *uvqr, int t);

int jump128xdivsteps(int delta, __m256i *f, __m256i *g, __m256i *uvqr, int t);

int jump256xdivsteps(int delta, __m256i *f, __m256i *g, __m256i *uvqr, int t);



#ifdef  __cplusplus
}
#endif


#endif // _JUMPDIVSTEPS_H_
