#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <inttypes.h>

#include <sys/types.h>
#include <stdint.h>

#include <gmp.h>
#include <assert.h>
extern int gmp_export(unsigned char *,unsigned long long,mpz_t);
extern void gmp_import(mpz_t,const unsigned char *,unsigned long long);

#include "crypto_hash_sha256.h"
#include "randombytes.h"

#include "field.h"

Kfield K;


#include "EdonK128.h"
#include "crypto_kem.h"

int crypto_kem_enc(unsigned char *ct, unsigned char *ss, const unsigned char *pk)
{
    int i, j, row;
    Kelt e[ErrorBasisDimension], m[NrRows];
    Kelt tempproducts01[BinarySpan], tempproducts02[BinarySpan];
    unsigned char hash[crypto_hash_sha256_BYTES], temphash[2*crypto_hash_sha256_BYTES];
    unsigned char SHA2ofC[crypto_hash_sha256_BYTES];
    unsigned char s0s1[crypto_hash_sha256_BYTES], s1s0[crypto_hash_sha256_BYTES];
    unsigned char allrandombytes[NrColumns];
    unsigned char temp01;

    Kfield_init();
    struct PublicKeyStruct PublicKey;
    struct CiphertextStruct Ciphertext;
    
    memcpy(&PublicKey, pk, sizeof(struct PublicKeyStruct));

    for(i=0; i<ErrorBasisDimension; i++) Kinit(e[i]);
    for(i=0; i<NrRows; i++) Kinit(m[i]);
    for(i=0; i<NrColumns; i++) Kinit(Ciphertext.cph[i]);
        
    /* Fetch some random m */
#ifdef KAT
    printf("\nEncapsulation phase:\nMessage m:\n");
#endif
    for(j=0; j<NrRows; j++) {
        randombytes(temphash, sizeof(Kelt));
        memcpy(m[j], &temphash, sizeof(Kelt));
#ifdef KAT
    printf("m[%2d] = ",j); Kprint(m[j]); printf("\n");    
#endif
    }    

    /* Fetch some random e[0] and e[1] of the error masking basis */
    randombytes(temphash, crypto_hash_sha256_BYTES);
    memcpy(e[0], &temphash, sizeof(Kelt));
#ifdef KAT
    printf("\nError base:\ne[ 0] = "); Kprint(e[0]); printf("\n");    
#endif
    memcpy(e[1], &temphash[sizeof(Kelt)], sizeof(Kelt));
#ifdef KAT
    printf("e[ 1] = "); Kprint(e[1]); printf("\n");    
#endif
    
    for(j=0; j<ErrorBasisDimension - 2; j+=2) {
        crypto_hash_sha256(hash, temphash, crypto_hash_sha256_BYTES);
        memcpy(e[j + 2], &hash, sizeof(Kelt));
        memcpy(e[j + 3], &hash[sizeof(Kelt)], sizeof(Kelt));
        memcpy(temphash, &hash, crypto_hash_sha256_BYTES);
#ifdef KAT
    printf("e[%2d] = ",j + 2); Kprint(e[j + 2]); printf("\n");    
    printf("e[%2d] = ",j + 3); Kprint(e[j + 3]); printf("\n");    
#endif
    }

    /* Compute s0s1 */
    crypto_hash_sha256(s0s1, temphash, crypto_hash_sha256_BYTES);
    /* Obtain s1s0 */
    memcpy(s1s0, &s0s1[sizeof(Kelt)], sizeof(Kelt));
    memcpy(&s1s0[sizeof(Kelt)], s0s1, sizeof(Kelt));



    randombytes(allrandombytes, NrColumns);

    /* Generate the error mask from the error base e[] and allrandombytes. */
    /* Put it in the initial Ciphertext.cph[ ] */
    for(i=0; i<NrColumns; i++) {
        Kset_ui(Ciphertext.cph[i],0);
        temp01 = allrandombytes[i];
        for(j=0; j<ErrorBasisDimension; j++) {
            if((temp01>>j) & 1)
                Kadd(Ciphertext.cph[i], Ciphertext.cph[i], e[j]);
        }
    }

    /* Finally compute Ciphertext.cph = m * PublicKey + error */
    for(row=0; row<NrRows; row++) {
        for(j=0; j<BinarySpan; j++) {
            Kmul(tempproducts01[j], PublicKey.p[j], m[row]);
        }
        for(j=0; j<BinarySpan; j++) {
            Kmul(tempproducts02[j], PublicKey.p[j + BinarySpan], m[row]);
        }
        
        for(i=0; i<NrColumns; i++) {
            temp01 = PublicKey.cosetLeader[row] ^ PublicKey.linearCombination[row][i];
            for(j=0; j<BinarySpan; j++) {
                if((temp01>>j) & 1)
                    Kadd(Ciphertext.cph[i], Ciphertext.cph[i], tempproducts01[j]);
            }
            temp01 = PublicKey.linearCombination[row][i];
            for(j=0; j<BinarySpan; j++) {
                if((temp01>>j) & 1)
                    Kadd(Ciphertext.cph[i], Ciphertext.cph[i], tempproducts02[j]);
            }
        }
    }
    
    /* Finalizing the computation of the ciphertext pair (C, h) */
    memcpy(ct, &Ciphertext, sizeof(struct CiphertextStruct) - crypto_hash_sha256_BYTES);
    crypto_hash_sha256(SHA2ofC, ct, sizeof(struct CiphertextStruct) - crypto_hash_sha256_BYTES);

    memcpy(temphash, &s1s0, crypto_hash_sha256_BYTES);
    memcpy(&temphash[crypto_hash_sha256_BYTES], &SHA2ofC, crypto_hash_sha256_BYTES);
    crypto_hash_sha256(hash, temphash, 2*crypto_hash_sha256_BYTES);

    memcpy(Ciphertext.HashedSecretShare, &hash, crypto_hash_sha256_BYTES);
    memcpy(ct, &Ciphertext, sizeof(struct CiphertextStruct));
#ifdef KAT
    printf("\nCiphertext:\n");        
    for(i=0; i<NrColumns; i++) {
        printf("cph[%3d] = ",i); Kprint(Ciphertext.cph[i]); printf("\n");
    }
    printf("\nHash of the shared secret (a part of the Ciphertext):\n"); 
    for(i=0; i<crypto_hash_sha256_BYTES; i++) printf("%02x", Ciphertext.HashedSecretShare[i]); 
    printf("\n");
#endif

    /* Computing the SharedSecret from s0s1 and SHA2ofC */
    memcpy(temphash, &s0s1, crypto_hash_sha256_BYTES);
    memcpy(&temphash[crypto_hash_sha256_BYTES], &SHA2ofC, crypto_hash_sha256_BYTES);
    crypto_hash_sha256(ss, temphash, 2*crypto_hash_sha256_BYTES);

#ifdef KAT
    printf("\n\nEncapsulating the following shared secret:\n"); 
    for(i=0; i<crypto_hash_sha256_BYTES; i++) printf("%02x", ss[i]); 
    printf("\n");
#endif


    

    /* Clear the variables */
    for(i=0; i<ErrorBasisDimension; i++) Kclear(e[i]);
    for(i=0; i<NrRows; i++) Kclear(m[i]);
    for(i=0; i<NrColumns; i++) Kclear(Ciphertext.cph[i]);
    
    return 0;
}
