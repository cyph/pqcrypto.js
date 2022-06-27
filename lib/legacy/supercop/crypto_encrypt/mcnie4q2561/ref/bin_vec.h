#ifndef bin_vec_h
#define bin_vec_h

#include <stdint.h>

typedef struct
{
	short size;
	uint32_t *data;
}Bvec;

Bvec gen_vec(short len, uint32_t *dd);  /* generate the binary vector of given data  */
Bvec gen_vec_zero(short len);  /* generate the binary vector whose components are all zero */
Bvec gen_vec_rand(short len);  /* generate a random binary vector */
Bvec gen_vec_copy(Bvec smp);   

short chk_zero(Bvec x);  /* return 1 if it is zero vector */
uint32_t get_elt(Bvec x, short cur);  
void put_elt(Bvec *x, short cur, uint32_t d); 
void print_bin(Bvec x);

void Shift(Bvec *x); /* x <<= 1 */

Bvec vec_sum(Bvec x, Bvec y);
void vec_add(Bvec *x, Bvec y);
void vec_swap(Bvec *x, Bvec *y);

void gauss_elm(Bvec *M, short row, short col);

#endif

