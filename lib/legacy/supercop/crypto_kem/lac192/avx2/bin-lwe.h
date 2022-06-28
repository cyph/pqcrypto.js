#include <stdint.h>

//generate the public parameter a from seed
int gen_a(unsigned char *a,  const unsigned char *seed);
//generate the small random vector for secret and error
int gen_psi(char *e, unsigned int vec_num, unsigned char *seed);
// poly_mul  b=[as]
int poly_mul(const unsigned char *a, const char *s, unsigned char *b, unsigned int  vec_num);
// poly_aff  b=as+e 
int poly_aff(const unsigned char *a, const  char *s, char *e, unsigned char *b, unsigned int vec_num);
