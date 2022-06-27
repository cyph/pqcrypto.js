#include "poly.h"
#include "crypto_core_invhrss701.h"

void poly_S3_inv(poly *r, const poly *a)
{
  crypto_core_invhrss701((void *) r,(const void *) a,0,0);
}
