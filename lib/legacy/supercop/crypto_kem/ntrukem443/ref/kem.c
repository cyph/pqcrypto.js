/*
 * kem.c
 *
 *  Created on: Aug 29, 2017
 *      Author: zhenfei
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "api.h"
#include "NTRUEncrypt.h"
#include "packing.h"
#include "crypto_hash_sha512.h"
#include "fastrandombytes.h"
#include "crypto_kem.h"

/* kem and encryption use a same key gen */
int crypto_kem_keypair(
    unsigned char *pk,
    unsigned char *sk)
{
    uint16_t    *F, *g, *h, *buf, *mem;
    PARAM_SET   *param;

    param   = get_param_set_by_id(TEST_PARAM_SET);

    /* memory for 3 ring elements: f, g and h */
    mem     = malloc (sizeof(uint16_t)*param->padN * 3);
    buf     = malloc (sizeof(uint16_t)*param->padN * 6);
    if (!mem )
    {
        printf("malloc error!\n");
        return -1;
    }

    F = mem;
    g = F   + param->padN;
    h = g   + param->padN;

    keygen(F,g,h,buf,param);

    /* pack h into pk */
    pack_public_key(pk, param, h);
    int i;
    for (i=0;i<param->packsk;i++)
        sk[i] =0;

    /* pack F,h into sk */
    pack_secret_key_CCA(sk, param, F, h);

    memset(mem,0, sizeof(uint16_t)*param->padN*3);
    memset(buf,0, sizeof(uint16_t)*param->padN*6);

    free(mem);
    free(buf);

    return 0;
}


/* encapsulation function */
int crypto_kem_enc(
    unsigned char *ct,
    unsigned char *ss,
    const unsigned char *pk)
{

    PARAM_SET   *param;
    uint16_t    *buf, *mem, *h, *cpoly;
    unsigned char *shared_secret;
    param   = get_param_set_by_id(pk[0]);


    if ( param->id!=NTRU_KEM_443 && param->id != NTRU_KEM_743)
    {
        printf("unsupported parameter sets\n");
        return -1;
    }

    mem     = malloc(sizeof(uint16_t)*param->padN*2);
    buf     = malloc(sizeof(uint16_t)*param->padN*6);
    shared_secret = malloc(CRYPTO_BYTES + LENGTH_OF_HASH);
    if (!mem || !buf || !shared_secret)
    {
        printf("malloc error!\n");
        return -1;
    }


    h       = mem;
    cpoly   = h     + param->padN;


    memset(mem,0, sizeof(uint16_t)*param->padN*2);
    memset(buf,0, sizeof(uint16_t)*param->padN*6);
    memset(shared_secret, 0, CRYPTO_BYTES + LENGTH_OF_HASH);
    memset(ss, 0, CRYPTO_BYTES);

    /* randomly generate a string to be encapsulated */
    fastrandombytes(shared_secret, CRYPTO_BYTES);
    unpack_public_key(pk,param, h);
    encrypt_cca(cpoly, (char*) shared_secret, CRYPTO_BYTES, h,  buf, param);
    pack_public_key (ct, param, cpoly);

    /* ss = Hash (shared_secret | h) */
    crypto_hash_sha512(shared_secret + CRYPTO_BYTES, (unsigned char*)h, sizeof(uint16_t)*param->padN);
    crypto_hash_sha512(shared_secret, shared_secret, LENGTH_OF_HASH + CRYPTO_BYTES);
    memcpy (ss, shared_secret, CRYPTO_BYTES);


    memset(mem,0, sizeof(uint16_t)*param->padN*2);
    memset(buf,0, sizeof(uint16_t)*param->padN*6);
    memset(shared_secret, 0, CRYPTO_BYTES + LENGTH_OF_HASH);
    free(mem);
    free(buf);
    free(shared_secret);
    return 0;
}

/* decapsulation function */
int crypto_kem_dec(
    unsigned char *ss,
    const unsigned char *ct,
    const unsigned char *sk)
{
    PARAM_SET   *param;
    uint16_t    *buf, *mem, *F, *cpoly, *h;
    unsigned long long  mlen;
    unsigned char *shared_secret;
    param   =   get_param_set_by_id(ct[0]);
    if ( param->id!=NTRU_KEM_443 && param->id != NTRU_KEM_743)
    {
        printf("unsupported parameter sets\n");
        return -1;
    }

    mem     = malloc(sizeof(uint16_t)*param->padN*3);
    buf     = malloc(sizeof(uint16_t)*param->padN*8);
    shared_secret = malloc(CRYPTO_BYTES + LENGTH_OF_HASH);
    if(!mem || !buf || !shared_secret)
    {
        printf("malloc error\n");
        return -1;
    }

    F       = mem;
    cpoly   = F     + param->padN;
    h       = cpoly + param->padN;


    memset(mem,0, sizeof(uint16_t)*param->padN*3);
    memset(buf,0, sizeof(uint16_t)*param->padN*8);
    memset(shared_secret, 0, CRYPTO_BYTES + LENGTH_OF_HASH);


    /* decapsulation process */
    unpack_public_key (ct, param, cpoly);
    unpack_secret_key_CCA (sk, param, F, h);
    mlen = decrypt_cca((char*) shared_secret,  F, h, cpoly,  buf, param);
    if (mlen!=CRYPTO_BYTES)
    {
        free(mem);
        free(buf);
        free(shared_secret);
        return -1;
    }

    /* deriving the session key */
    crypto_hash_sha512(shared_secret+CRYPTO_BYTES, (unsigned char*) h, sizeof(uint16_t)*param->padN);
    crypto_hash_sha512(shared_secret, shared_secret, LENGTH_OF_HASH + CRYPTO_BYTES);
    memcpy (ss, shared_secret, CRYPTO_BYTES);

    free(mem);
    free(buf);
    free(shared_secret);
    return 0;
}



