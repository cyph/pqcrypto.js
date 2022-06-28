/*********************************************************************************************
* DAGS: Key Encapsulation using Dyadic GS Codes.                             *
* This code is exclusively intended for submission to the NIST Post=Quantum Cryptography.    *
* For any other usage , contact the author(s) to ask permission.                             *
**********************************************************************************************/

#include "stdio.h"
#include "string.h"
#include "crypto_kem.h"
#include "apiorig.h"
#include "keygeneration.h"
#include "encapsulation.h"
#include "decapsulation.h"

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk)
{

	return key_pair_generation(pk, sk);
}

int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk)
{

	return encapsulation(ct, ss, pk);
}

int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk)
{

	return decapsulation(ss, ct, sk);
}
