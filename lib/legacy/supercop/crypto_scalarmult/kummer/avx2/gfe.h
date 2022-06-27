#ifndef GFE_H
#define GFE_H

typedef struct{
  unsigned long long v[2];
} gfe;

typedef struct
{
  double v[24];
} __attribute__ ((aligned (32))) gfe4x;

typedef struct
{
  double v[12];
} __attribute__ ((aligned (32))) gfe4x3limb;


#define gfe_mul CRYPTO_NAMESPACE(gfe_mul)
#define gfe_nsquare CRYPTO_NAMESPACE(gfe_nsquare)
#define gfe_invert CRYPTO_NAMESPACE(gfe_invert)
#define gfe_print CRYPTO_NAMESPACE(gfe_print)
#define gfe_pack CRYPTO_NAMESPACE(gfe_pack)
#define gfe_unpack CRYPTO_NAMESPACE(gfe_unpack)
#define gfe4x3limb_freeze CRYPTO_NAMESPACE(gfe4x3limb_freeze)
#define gfe4x3limb_split CRYPTO_NAMESPACE(gfe4x3limb_split)
#define gfe4x3limb_unpack CRYPTO_NAMESPACE(gfe4x3limb_unpack)
#define gfe4x_znegate CRYPTO_NAMESPACE(gfe4x_znegate)
#define gfe4x_split CRYPTO_NAMESPACE(gfe4x_split)
#define gfe4x_unpack CRYPTO_NAMESPACE(gfe4x_unpack)
#define gfe4x_unpack4x CRYPTO_NAMESPACE(gfe4x_unpack4x)
#define gfe4x_mul CRYPTO_NAMESPACE(gfe4x_mul)
#define gfe4x_easymul CRYPTO_NAMESPACE(gfe4x_easymul)
#define gfe4x_square CRYPTO_NAMESPACE(gfe4x_square)
#define gfe4x_mulconst CRYPTO_NAMESPACE(gfe4x_mulconst)
#define gfe4x_hadamard CRYPTO_NAMESPACE(gfe4x_hadamard)
#define gfe4x_negamard CRYPTO_NAMESPACE(gfe4x_negamard)
#define gfe4x_print CRYPTO_NAMESPACE(gfe4x_print)
#define gfe4x_cswap CRYPTO_NAMESPACE(gfe4x_cswap)
#define ladderstep CRYPTO_NAMESPACE(ladderstep)
#define ladder CRYPTO_NAMESPACE(ladder)

void gfe_mul(gfe *r, const gfe *x, const gfe *y);
void gfe_nsquare(gfe *r, const gfe *x, unsigned long long);
//void gfe_square(gfe *r, const gfe *x);
#define gfe_square(R,X) gfe_mul(R,X,X)
void gfe_invert(gfe *r, const gfe *x);
void gfe_print(const gfe *x);
void gfe_pack(unsigned char[16], const gfe *x);
void gfe_unpack(gfe *r, const unsigned char *x);

void gfe4x3limb_freeze(gfe4x *r, const gfe4x *x);
void gfe4x3limb_split(gfe r[4], const gfe4x *a);
void gfe4x3limb_unpack(gfe4x *r, const unsigned char x[16], int pos);

void gfe4x_znegate(gfe4x *r);
void gfe4x_split(gfe r[4], const gfe4x *a);
void gfe4x_unpack(gfe4x *r, const unsigned char x[16], int pos);
void gfe4x_unpack4x(gfe4x *r, const unsigned char x[64]);
void gfe4x_mul(gfe4x *r, const gfe4x *a, const gfe4x *b);
void gfe4x_easymul(gfe4x *r, const gfe4x *a, const gfe4x *b);
void gfe4x_square(gfe4x *r, const gfe4x *a);
void gfe4x_mulconst(gfe4x *r, const gfe4x *a, const double *c);
void gfe4x_hadamard(gfe4x *r);
void gfe4x_negamard(gfe4x *r);
void gfe4x_print(const gfe4x *x, int pos);
void gfe4x_cswap(gfe4x *,gfe4x *,unsigned long long);
void ladderstep(gfe4x *);
void ladder(gfe4x *,const unsigned char *);

#endif
