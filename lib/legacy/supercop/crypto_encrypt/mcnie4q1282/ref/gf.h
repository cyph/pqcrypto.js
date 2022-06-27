
#ifndef gf_h
#define gf_h

typedef unsigned long long gf;
typedef unsigned long long uint64;

short vec_size(gf x);
gf gf_sum(gf x, gf y);     /* addition x + y */
void gf_add(gf *x, gf y);  /* addition x += y */
gf gf_mul(gf a, gf b);     /* multiplication x * y */
gf gf_inverse(gf a);
void vec_print(gf x);

short vec_get_elt(gf x, short cur);
void vec_put_elt(gf *x, short cur, short d);

void Gauss_elimination(gf *M, short row, short col);
short bin_rank(gf *x, short n);  /* rank metric of the vector (x1, ..., x_n) */

gf rand_gf(void);            /* generate a random gf element */
gf *gen_rand_vec(short n);   /* generate random n gf elements */
gf *gen_indep_vec(short n);  /* generate GF(2)-independent random n gf elements */

uint64 *gf2bin(const gf *x, const long xlen, long *dlen);  /* convert gf elements to binary data */
gf *bin2gf(const unsigned char *d, long xlen);             /* read gf elements from given binary data */

#endif
