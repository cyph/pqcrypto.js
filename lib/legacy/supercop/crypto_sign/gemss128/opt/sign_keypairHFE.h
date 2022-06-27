#ifndef _SIGN_KEYPAIRHFE_H
#define _SIGN_KEYPAIRHFE_H

#include "prefix_name.h"
#include "arch.h"

/* Generates a keypair - pk is the public key and sk is the secret key. */
int PREFIX_NAME(sign_keypairHFE)(UINT* pk,UINT* sk);
#define sign_keypairHFE PREFIX_NAME(sign_keypairHFE)


#endif
