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

#include "crypto_hash_sha384.h"
#include "randombytes.h"
#include "rng.h"

#include "field.h"

Kfield K;


#include "EdonK192.h"
#include "crypto_kem.h"


// This is Fisher Yates shuffle 
// It returns a random permutation of numbers 0, 1, ..., N-1, 
// based on a source of uniformly distributed source of random bytes Pool.
// The value of SwapPos might not be always perfectly uniformly distributed
static void randomPermutation(int16_t buf[], int16_t N, struct PoolOfRandomBytes *Pool)
{
	int16_t i, SwapPos;
	int16_t temp;
    
    for(i = 0; i < N; i++) buf[i] = i;

	for(i = N; i > 0; i--)
	{
		SwapPos =  Pool->pool[Pool->poolsize] % i;
        Pool->poolsize--;
		temp = buf[SwapPos];
		buf[SwapPos] = buf[i-1];
		buf[i-1] = temp;
	}
}


static void randomOrthogonalBinMatrix(mpz_t BinMatrix[], int n, struct PoolOfRandomBytes *Pool)
{
    int16_t i, j, jj, pp, perm[n];
    int16_t Rotations, Rot;
    mpz_t tempMatrix[n], temp;
    
    for(i = 0; i < n; i++) {
        mpz_init2(tempMatrix[i], n);
    }
    mpz_init2(temp, n);

    
    if (OrthoPairsRotationsRot[n-1][0] == 0) {
        printf("I can not produce random orthogonal binary matrix for these parameters! \n");
        exit(1);
    }
    Rotations = OrthoPairsRotationsRot[n-1][0];
    Rot = OrthoPairsRotationsRot[n-1][1];
    
    /* Seting up with the first permutation */
    randomPermutation(perm, n, Pool);
    for(j=0; j<n; j++){
        for(i=0; i<Rotations; i++){
            jj = (j + i*Rot) % n;
            mpz_setbit(BinMatrix[j], perm[jj]);
        }
    }
    
    /* Now proceed with et least 5 new permutations */
    for(pp=0; pp<5; pp++){
        randomPermutation(perm, n, Pool);
        for(j=0; j<n; j++){
            mpz_set_ui(temp, 0);
            for(i=0; i<Rotations; i++){
                jj = (j + i*Rot) % n;
                mpz_xor(temp, temp, BinMatrix[ perm[jj] ]);
            }
            mpz_set(tempMatrix[j],temp);
        }
        for(j=0; j<n; j++)
            mpz_swap(tempMatrix[j],BinMatrix[j]);
    }


    /* Finaly clear the temporary GMP variables */
    mpz_clear(temp);
    for(i = n-1; i >= 0; i--) {
        mpz_clear(tempMatrix[i]);
    }
}

int crypto_kem_keypair(unsigned char *pk, unsigned char *sk)
{
    Kfield K;
    Kelt a, b, c, d, aabb;
    Kelt randomConstant[BinarySpan];
    int i, j, ii, flag, filledRows, filledOccupied;
    unsigned long int popcount, position;
    uint8_t row;
    #ifdef KAT
        char *bp;
        size_t byteswritten;
        FILE *stream;
    #endif
    
    unsigned char tempbuf[NrColumns8];


    mpz_t HBottom[ProjectionDim], HBottomTransposed[ProjectionDim];
    mpz_t HTop[ProjectionDim];
    mpz_t H[ProjectionDim];
    mpz_t temp, temp2, temp3, temp4, temp4a, temp4b;
    mpz_t OneRowOfGLeftPart[BinarySpan], TempMatrix[BinarySpan], OneRowOfGRightPart[BinarySpan];
    mpz_t OneRowOfG[BinarySpan];
    mpz_t ShortenedPermutation[ProjectionDim];
    mpz_t ccbin, ddbin, ccbinxorddbin;

    uint8_t cosetLeader[NrRows], occupied[1<<BinarySpan];
    uint8_t linearCombination[NrRows][NrColumns];

    struct PublicKeyStruct PublicKey;
    
    struct PoolOfRandomBytes Pool;
    
    mpz_t P[NrColumns];

    unsigned char diversifier[8];
    AES_XOF_struct ctx;
    
    /* Initialization of GMP and MPFQ variables */
    for(i=0; i<NrRows; i++)
        cosetLeader[i] = 0;
    for(i=0; i<(1<<BinarySpan); i++)
        occupied[i] = 0;
    
    
    Kfield_init();
    
    for(i=0; i<NrColumns; i++)
        mpz_init2(P[i], NrColumns);
    
    Kinit(a);
    Kinit(b);
    Kinit(c);
    Kinit(d);
    Kinit(aabb);
    
    Kvec_init(randomConstant, BinarySpan);
    Kvec_init(PublicKey.p, BinarySpan*2);

    for(i=0; i<ProjectionDim; i++)
        mpz_init2(HBottom[i], ProjectionDim);
    for(i=0; i<ProjectionDim; i++)
        mpz_init2(ShortenedPermutation[i], NrColumns);    
    for(i = 0; i < ProjectionDim; i++)
        mpz_init2(HBottomTransposed[i], ProjectionDim);
    for(i=0; i<ProjectionDim; i++)
        mpz_init2(HTop[i], NrColumns - ProjectionDim);

    mpz_init2(temp, NrColumns);
    mpz_init2(temp2, NrColumns);
    mpz_init2(temp3, NrColumns - ProjectionDim);
    mpz_init2(temp4, ProjectionDim);
    mpz_init2(temp4a, ProjectionDim);
    mpz_init2(temp4b, ProjectionDim);

    for(i=0; i<ProjectionDim; i++)
        mpz_init2(H[i], NrColumns);
    for(i=0; i<BinarySpan; i++)
        mpz_init2(OneRowOfGLeftPart[i], NrColumns - ProjectionDim);
    for(i=0; i<BinarySpan; i++)
        mpz_init2(TempMatrix[i], ProjectionDim);
    for(i=0; i<BinarySpan; i++)
        mpz_init2(OneRowOfGRightPart[i], ProjectionDim);
    for(i=0; i<BinarySpan; i++)
        mpz_init2(OneRowOfG[i], NrColumns);
    mpz_init2(ccbin, BinarySpan);
    mpz_init2(ddbin, BinarySpan);
    mpz_init2(ccbinxorddbin, BinarySpan);


    /* Private key sk is fetched from an external source of randomnes randombytes() */
    randombytes(sk, 32);
    
    /* The private key will be extended by a NIST approved deterministic seedexpander */
    /* in order to generate the values in  Public key */
    memset(diversifier, 0x00, 8);
    seedexpander_init(&ctx, sk, diversifier, 0x01<<15);
    
    /* Generate 4 variables a, b, c and d used to define a quasi-binary orthogonal matrix P */
    /* For a and b chosen uniformly at random, c = a / (a + b)^2, d = b / (a + b)^2 */
    seedexpander(&ctx, (unsigned char *)a, sizeof(Kelt));
    seedexpander(&ctx, (unsigned char *)b, sizeof(Kelt));
     
    Kadd(aabb, a, b);
    Ksqr(aabb, aabb);
    Kinv(aabb, aabb);
    Kmul(c, a, aabb);
    Kmul(d, b, aabb);
#ifdef KAT
    printf("\n");
    printf("\nGenerate 4 variables a, b, c and d used to define a quasi-binary orthogonal matrix P");
    printf("\nFor a and b chosen uniformly at random, c = a / (a + b)^2, d = b / (a + b)^2\n");
    printf("a = "); Kprint(a); printf("\n");
    printf("b = "); Kprint(b); printf("\n");
    printf("c = "); Kprint(c); printf("\n");
    printf("d = "); Kprint(d); printf("\n");
#endif


    seedexpander(&ctx, (unsigned char *) &Pool.pool, 6*NrColumns + 7*ProjectionDim );
    Pool.poolsize = 6*NrColumns + 7*ProjectionDim - 1;

    
    /* Generate one random orthogonal binary matrix P of size NrColumns x NrColumns */
    randomOrthogonalBinMatrix(P, NrColumns, &Pool);
    
#ifdef KAT
    printf("\nAn orthogonal binary matrix P of size %d x %d:\n",NrColumns, NrColumns);
    for(i = 0; i < NrColumns; i++) {
        stream = open_memstream(&bp, &byteswritten);
        mpz_out_str(stream, 2, P[i]);
        fflush(stream);
        fclose(stream);
        for(int ii=0; ii<NrColumns-byteswritten; ii++) 
            printf("0");
        printf("%s\n", bp);
    }
    printf("\n");
#endif


    /* Generate one random binary orthogonal matrix HBottom of size ProjectionDim x ProjectionDim */
    randomOrthogonalBinMatrix(HBottom, ProjectionDim, &Pool);
    /* a slow computation of a transposition of HBottom to produce HBottomTransposed */
    for(i = 0; i < ProjectionDim; i++) {
        mpz_set_ui(HBottomTransposed[i], 0);
        for(j=0; j<ProjectionDim; j++) {
            if (mpz_tstbit(HBottom[j],i)==1)
                mpz_setbit(HBottomTransposed[i],j);
        }
    }
#ifdef KAT
    printf("\nAn orthogonal binary matrix HBottom of size %d x %d:\n", ProjectionDim, ProjectionDim);
    for(i = 0; i < ProjectionDim; i++) {
        stream = open_memstream(&bp, &byteswritten);
        mpz_out_str(stream, 2, HBottom[i]);
        fflush(stream);
        fclose(stream);
        for(int ii=0; ii<ProjectionDim-byteswritten; ii++) 
            printf("0");
        printf("%s\n", bp);
    }
    printf("\n");
#endif

    /* Generate a random matrix HTop where every column has an even Hamming weight */
    for(i=0; i<ProjectionDim; i++) {
        seedexpander(&ctx, tempbuf, (NrColumns - ProjectionDim)>>3 );
        gmp_import(HTop[i], tempbuf, (NrColumns - ProjectionDim)>>3);
        popcount = mpz_popcount(HTop[i]);
        if (popcount % 2 == 1) {
            position = Pool.pool[Pool.poolsize--] % (NrColumns - ProjectionDim);
            if (mpz_tstbit(HTop[i], position)) {
                mpz_clrbit(HTop[i], position);
            }
            else {
                mpz_setbit(HTop[i], position);
            }
        }
    }

    /* Compose a larger matrix H = [HTop HBottom] */
    for(i=0; i<ProjectionDim; i++) {
        mpz_mul_2exp(temp, HTop[i], ProjectionDim);
        mpz_add(H[i], temp, HBottom[i]);
    }


    /* Generate an array randomConstant of BinarySpan elements from Kfield */
    /* The randomness used here can be, but does not need to be from the initial sk seed */
    for (i = 0; i < BinarySpan; i++) randombytes((unsigned char *)randomConstant[i], sizeof(Kelt));

    /* Multiply those elements by c and d and place the results in 2*BinarySpan variables p[] in PublicKey */
    for (i = 0; i < BinarySpan; i++) {
        Kmul(PublicKey.p[i], randomConstant[i], c);
        Kmul(PublicKey.p[i+BinarySpan], randomConstant[i], d);
    }

    
    /* Finally produce the public matrix G and the coset leaders */
    filledRows = 0;
    filledOccupied = 0;
    do {
        /* Generate OneRowOfGLeftPart */
        for(i=0; i<BinarySpan; i++) {
            mpz_set_ui(OneRowOfGLeftPart[i], 0);
            randombytes(tempbuf, (NrColumns - ProjectionDim)>>3 );
            gmp_import(OneRowOfGLeftPart[i], tempbuf, (NrColumns - ProjectionDim)>>3);
        }
        
        /* Multiply OneRowOfGLeftPart by HTop and place the result in TempMatrix */
        for(i=0; i<BinarySpan; i++) {
            mpz_set_ui(TempMatrix[i], 0);
            for(j=0; j<ProjectionDim; j++) {
                mpz_and(temp3, OneRowOfGLeftPart[i], HTop[j]);
                if (mpz_popcount(temp3) & 1)
                    mpz_setbit(TempMatrix[i], j);
            }
        }
    
        /* Multiply TempMatrix by HBottomTransposed and place the result in OneRowOfGRightPart */
        for(i=0; i<BinarySpan; i++) {
            mpz_set_ui(OneRowOfGRightPart[i], 0);
            for(j=0; j<ProjectionDim; j++) {
                mpz_and(temp4, TempMatrix[i], HBottomTransposed[j]);
                if (mpz_popcount(temp4) & 1)
                    mpz_setbit(OneRowOfGRightPart[i], j);
            }
        }
        
        /* Compose a larger matrix OneRowOfG = [OneRowOfGLeftPart OneRowOfGRightPart] */
        for(i=0; i<BinarySpan; i++) {
            mpz_set_ui(OneRowOfG[i], 0);
            mpz_mul_2exp(temp, OneRowOfGLeftPart[i], ProjectionDim);
            mpz_add(OneRowOfG[i], temp, OneRowOfGRightPart[i]);
        }
        
        mpz_set_ui(ccbin, 0);
        mpz_set_ui(ddbin, 0);
        mpz_com(temp, P[0]);
        for(i=0; i<BinarySpan; i++) {
            mpz_and(temp2, OneRowOfG[i], temp);
            if(mpz_popcount(temp2) & 1)
                mpz_setbit(ccbin, i);
        }
        for(i=0; i<BinarySpan; i++) {
            mpz_and(temp2, OneRowOfG[i], P[0]);
            if(mpz_popcount(temp2) & 1)
                mpz_setbit(ddbin, i);
        }
        mpz_xor(ccbinxorddbin, ccbin, ddbin);
        row = (uint8_t)mpz_get_ui(ccbinxorddbin);
        
        flag = 0;
        for(ii=0; ii<filledRows; ii++) {
            if(occupied[ii] == row) {
                flag = 1;
                break;
            }
        }
        if (flag == 0) {
            cosetLeader[filledRows] = row;
            linearCombination[filledRows][0] = (uint8_t)mpz_get_ui(ddbin);
    
            for(j=1; j<NrColumns; j++) {
                mpz_set_ui(ddbin, 0);
                for(i=0; i<BinarySpan; i++) {
                    mpz_and(temp2, OneRowOfG[i], P[j]);
                    if(mpz_popcount(temp2) & 1)
                        mpz_setbit(ddbin, i);
                }
                
                linearCombination[filledRows][j] = (uint8_t)mpz_get_ui(ddbin);
            }
            filledRows++;
            filledOccupied++;
            if(filledOccupied==(1<<BinarySpan)) {
                filledOccupied = 0;
                for(ii=0; ii<(1<<BinarySpan); ii++)
                    occupied[ii] = 0;
            }
        }
        
    } while (filledRows < NrRows);

    if(BinarySpan==4) {
        /* Compress the structure such that 2 x 4 bits go to 1 byte */
        for(i=0; i<NrRows/(8/BinarySpan); i++) {
            PublicKey.cosetLeader[i] = (cosetLeader[2*i]<<4) | cosetLeader[2*i + 1];
        }
        for(i=0; i<NrRows; i++) {
            for(j=0; j<NrColumns/(8/BinarySpan); j++) {
                PublicKey.linearCombination[i][j] = (linearCombination[i][2*j]<<4) | linearCombination[i][2*j + 1];
            }
        }
    }
    else {
        for(i=0; i<NrRows; i++) {
            PublicKey.cosetLeader[i] = cosetLeader[i];
        }
        for(i=0; i<NrRows; i++) {
            for(j=0; j<NrColumns; j++) {
                PublicKey.linearCombination[i][j] = linearCombination[i][j];
            }
        }        
    }

    memcpy(pk, &PublicKey, sizeof(struct PublicKeyStruct));
#ifdef KAT
        printf("\nPublicKey\n");
        printf("PublicKey.cosetLeader = "); for(i=0; i<NrRows/(8/BinarySpan); i++) printf("%02x ",PublicKey.cosetLeader[i]); printf("\n");
        printf("PublicKey %d x %d matrix\n", NrRows, NrColumns);
        for(i=0; i<NrRows; i++) {
            for(j=0; j<NrColumns/(8/BinarySpan); j++) {
                printf("%02x ",PublicKey.linearCombination[i][j]);
            }
            printf("\n");
        }
        printf("PublicKey GF numbers:\n");
        for(i=0; i<2*BinarySpan; i++) {
            printf("p[%2d] = ",i); Kprint(PublicKey.p[i]); printf("\n");
        }
#endif    
    
 
    /* Clear the variables */
    for(i = 0; i<NrColumns; i++) mpz_clear(P[i]);
    
    Kclear(a);
    Kclear(b);
    Kclear(c);
    Kclear(d);
    Kclear(aabb);

    Kvec_clear(randomConstant, BinarySpan);
    Kvec_clear(PublicKey.p, BinarySpan*2);

    for(i=0; i<ProjectionDim; i++) mpz_clear(HBottom[i]);
    for(i=0; i<ProjectionDim; i++) mpz_clear(ShortenedPermutation[i]);    
    for(i = 0; i < ProjectionDim; i++) mpz_clear(HBottomTransposed[i]);
    for(i=0; i<ProjectionDim; i++) mpz_clear(HTop[i]);

    mpz_clear(temp);
    mpz_clear(temp2);
    mpz_clear(temp3);
    mpz_clear(temp4);
    mpz_clear(temp4a);
    mpz_clear(temp4b);

    for(i=0; i<ProjectionDim; i++) mpz_clear(H[i]);
    for(i=0; i<BinarySpan; i++) mpz_clear(OneRowOfGLeftPart[i]);
    for(i=0; i<BinarySpan; i++) mpz_clear(TempMatrix[i]);
    for(i=0; i<BinarySpan; i++) mpz_clear(OneRowOfGRightPart[i]);
    for(i=0; i<BinarySpan; i++) mpz_clear(OneRowOfG[i]);
    mpz_clear(ccbin);
    mpz_clear(ddbin);
    mpz_clear(ccbinxorddbin);

    return 0;
}

int checkhash(  Kelt candidate1, 
                Kelt candidate2, 
                unsigned char *ss, 
                const unsigned char HashedSecretShare[],
                const unsigned char SHA2ofC[]
)
{
    int i;
    unsigned char hash[crypto_hash_sha384_BYTES], hash2[crypto_hash_sha384_BYTES], temphash[2*crypto_hash_sha384_BYTES];
    
    memcpy(temphash, candidate1, sizeof(Kelt));
    memcpy(temphash + sizeof(Kelt), candidate2, sizeof(Kelt));
    crypto_hash_sha384(hash, temphash, crypto_hash_sha384_BYTES);
    for(i=0; i<ErrorBasisDimension; i+=2) {
        /* check does it match the final hash */ 
        memcpy(temphash, &hash[sizeof(Kelt)], sizeof(Kelt));
        memcpy(temphash + sizeof(Kelt), &hash, sizeof(Kelt));
        memcpy(temphash + crypto_hash_sha384_BYTES, SHA2ofC, crypto_hash_sha384_BYTES);
        
        crypto_hash_sha384(hash2, temphash, 2*crypto_hash_sha384_BYTES);

        if(memcmp(hash2, HashedSecretShare, crypto_hash_sha384_BYTES) == 0) {
            memcpy(temphash, &hash, crypto_hash_sha384_BYTES);
            memcpy(temphash + crypto_hash_sha384_BYTES, SHA2ofC, crypto_hash_sha384_BYTES);
            
            crypto_hash_sha384(ss, temphash, 2*crypto_hash_sha384_BYTES);
            return 0;
        }
        else {
            memcpy(temphash, &hash, crypto_hash_sha384_BYTES);
            crypto_hash_sha384(hash, temphash, crypto_hash_sha384_BYTES);
        }
    }

    memcpy(temphash, candidate2, sizeof(Kelt));
    memcpy(temphash + sizeof(Kelt), candidate1, sizeof(Kelt));
    crypto_hash_sha384(hash, temphash, crypto_hash_sha384_BYTES);
    for(i=0; i<ErrorBasisDimension; i+=2) {
        /* check does it match the final hash */ 
        memcpy(temphash, &hash[sizeof(Kelt)], sizeof(Kelt));
        memcpy(temphash + sizeof(Kelt), &hash, sizeof(Kelt));
        memcpy(temphash + crypto_hash_sha384_BYTES, SHA2ofC, crypto_hash_sha384_BYTES);
        
        crypto_hash_sha384(hash2, temphash, 2*crypto_hash_sha384_BYTES);

        if(memcmp(hash2, HashedSecretShare, crypto_hash_sha384_BYTES) == 0) {
            memcpy(temphash, &hash, crypto_hash_sha384_BYTES);
            memcpy(temphash + crypto_hash_sha384_BYTES, SHA2ofC, crypto_hash_sha384_BYTES);
            
            crypto_hash_sha384(ss, temphash, 2*crypto_hash_sha384_BYTES);
            return 0;
        }
        else {
            memcpy(temphash, &hash, crypto_hash_sha384_BYTES);
            crypto_hash_sha384(hash, temphash, crypto_hash_sha384_BYTES);
        }
    }
    

    return 1;
}

int addcandidate(  Kelt allcandidates[], 
                    Kelt candidate, 
                    int *NumberOfAllCandidates
)
{
    int ii, jj, jj2, flag;
    Kelt NewCandidate;
    Kinit(NewCandidate);


    flag = 0;
    for(ii=0; ii < *NumberOfAllCandidates; ii++) {
        if(!(Kcmp(allcandidates[ii], candidate) )) {
            flag = 1;
            break;
        }
    }
    if (flag == 0) {
        Kset(allcandidates[*NumberOfAllCandidates], candidate);
        jj2 = (*NumberOfAllCandidates);
        (*NumberOfAllCandidates) = (*NumberOfAllCandidates) + 1;

        for(jj=0; jj<jj2; jj++) {
            Kadd(NewCandidate, allcandidates[jj], candidate);
            
            flag = 0;
            for(ii=0; ii < jj2; ii++) {
                if(!(Kcmp(allcandidates[ii], NewCandidate) )) {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) {
                Kset(allcandidates[*NumberOfAllCandidates], NewCandidate);
                (*NumberOfAllCandidates) = (*NumberOfAllCandidates) + 1;
            }
        }
    }
    
    Kclear(NewCandidate);
    return 0;
}

int crypto_kem_dec(unsigned char *ss, const unsigned char *ct, const unsigned char *sk)
{
    int i, j, NumberOfAllCandidates;
    unsigned long int popcount, position;
    unsigned char tempsk[32];
    unsigned char SHA2ofC[crypto_hash_sha384_BYTES];
    
    mpz_t ShortenedPermutation[ProjectionDim];
    mpz_t HBottom[ProjectionDim], HBottomTransposed[ProjectionDim];
    mpz_t HTop[ProjectionDim];
    mpz_t H[ProjectionDim];
    mpz_t temp;
    mpz_t P[NrColumns];

    Kfield_init();

    struct CiphertextStruct Ciphertext;
    Kelt a, b, aasum, bbsum, aabbinv;
    Kelt candidates[ProjectionDim], allcandidates[(1<<(ErrorBasisDimension+1))+2];
    unsigned char diversifier[8];
    AES_XOF_struct ctx;
    struct PoolOfRandomBytes Pool;
    unsigned char tempbuf[NrColumns8];

    for(i=0; i<ProjectionDim; i++) Kinit(candidates[i]);
    for(i=0; i<(1<<(ErrorBasisDimension+1))+2; i++) Kinit(allcandidates[i]);
    Kinit(aasum);
    Kinit(bbsum);
    Kinit(aabbinv);
        
    for(i=0; i<NrColumns; i++) Kinit(Ciphertext.cph[i]);
    Kinit(a);
    Kinit(b);
    for(i=0; i<ProjectionDim; i++)
        mpz_init2(HBottom[i], ProjectionDim);
    for(i=0; i<ProjectionDim; i++)
        mpz_init2(ShortenedPermutation[i], NrColumns);    
    for(i = 0; i < ProjectionDim; i++)
        mpz_init2(HBottomTransposed[i], ProjectionDim);
    for(i=0; i<ProjectionDim; i++)
        mpz_init2(HTop[i], NrColumns - ProjectionDim);

    mpz_init2(temp, NrColumns);

    for(i=0; i<ProjectionDim; i++)
        mpz_init2(H[i], NrColumns);
    for(i=0; i<NrColumns; i++)
        mpz_init2(P[i], NrColumns);

    memcpy(&Ciphertext, ct, sizeof(struct CiphertextStruct));
    crypto_hash_sha384(SHA2ofC, ct, sizeof(struct CiphertextStruct) - crypto_hash_sha384_BYTES);
    
    memcpy(tempsk, sk, 32);

    memset(diversifier, 0x00, 8);
    seedexpander_init(&ctx, tempsk, diversifier, 0x01<<15);

    seedexpander(&ctx, (unsigned char *)a, sizeof(Kelt));
    seedexpander(&ctx, (unsigned char *)b, sizeof(Kelt));

    Kadd(aabbinv, a, b);
    Kinv(aabbinv, aabbinv);

    seedexpander(&ctx, (unsigned char *) &Pool.pool, 6*NrColumns + 7*ProjectionDim );
    Pool.poolsize = 6*NrColumns + 7*ProjectionDim - 1;

    /* Recreate the random orthogonal binary matrix P of size NrColumns x NrColumns */
    randomOrthogonalBinMatrix(P, NrColumns, &Pool);

    /* Recreate the random binary orthogonal matrix HBottom of size ProjectionDim x ProjectionDim */
    randomOrthogonalBinMatrix(HBottom, ProjectionDim, &Pool);
    /* a slow computation of a transposition of HBottom to produce HBottomTransposed */
    for(i = 0; i < ProjectionDim; i++) {
        mpz_set_ui(HBottomTransposed[i], 0);
        for(j=0; j<ProjectionDim; j++) {
            if (mpz_tstbit(HBottom[j],i)==1)
                mpz_setbit(HBottomTransposed[i],j);
        }
    }

    /* Generate a random matrix HTop where every column has an even Hamming weight */
    for(i=0; i<ProjectionDim; i++) {
        seedexpander(&ctx, tempbuf, (NrColumns - ProjectionDim)>>3 );
        gmp_import(HTop[i], tempbuf, (NrColumns - ProjectionDim)>>3);
        popcount = mpz_popcount(HTop[i]);
        if (popcount % 2 == 1) {
            position = Pool.pool[Pool.poolsize--] % (NrColumns - ProjectionDim);
            if (mpz_tstbit(HTop[i], position)) {
                mpz_clrbit(HTop[i], position);
            }
            else {
                mpz_setbit(HTop[i], position);
            }
        }
    }


    /* Compose a larger matrix H = [HTop HBottom] */
    for(i=0; i<ProjectionDim; i++) {
        mpz_mul_2exp(temp, HTop[i], ProjectionDim);
        mpz_add(H[i], temp, HBottom[i]);
    }


    /* Compute ShortenedPermutation as a product P^-1 x H */
    for(j=0; j<ProjectionDim; j++) {
        for(i=0; i<NrColumns; i++) {
            mpz_and(temp, P[i], H[j]);
            if (mpz_popcount(temp) % 2 == 1)
                mpz_setbit(ShortenedPermutation[j], i);
        }
    }

    
    NumberOfAllCandidates = 0;
    /* project ciphertext to ProjectionDim variables in GF with the ShortenedPermutation */
    for(i=0; i<ProjectionDim; i++) {
        Kset_ui(aasum,0);
        Kset_ui(bbsum,0);
        for(j=0; j<NrColumns; j++) {
            if(mpz_tstbit(ShortenedPermutation[i], j)==0) {
                Kadd(aasum, aasum, Ciphertext.cph[j]);
            }
            else {
                Kadd(bbsum, bbsum, Ciphertext.cph[j]);
            }
        }

        Kmul(aasum, aasum, a);
        Kmul(bbsum, bbsum, b);

        Kadd(candidates[i], aasum, bbsum);
        Kmul(candidates[i], candidates[i], aabbinv);
        addcandidate(allcandidates, candidates[i], &NumberOfAllCandidates);
        if(NumberOfAllCandidates>(1<<(ErrorBasisDimension+1))) {
            return CorruptedCiphertextError;
        }
    }
    
    for(i=0; i<NumberOfAllCandidates-1; i++) {
        for(j=i+1; j<NumberOfAllCandidates; j++) {
            if(checkhash(allcandidates[i], allcandidates[j], ss, Ciphertext.HashedSecretShare, SHA2ofC) == 0) {
                
                /* Clear variables before successful return */
                for(i=0; i<ProjectionDim; i++) Kclear(candidates[i]);
                for(i=0; i<(1<<(ErrorBasisDimension+1))+2; i++) Kclear(allcandidates[i]);
                Kclear(a);
                Kclear(b);
                Kclear(aasum);
                Kclear(bbsum);
                Kclear(aabbinv);
                for(i=0; i<ProjectionDim; i++) mpz_clear(HBottom[i]);
                for(i=0; i<ProjectionDim; i++) mpz_clear(ShortenedPermutation[i]);    
                for(i = 0; i < ProjectionDim; i++) mpz_clear(HBottomTransposed[i]);
                for(i=0; i<ProjectionDim; i++) mpz_clear(HTop[i]);
                mpz_clear(temp);
                for(i=0; i<ProjectionDim; i++) mpz_clear(H[i]);
                for(i = 0; i<NrColumns; i++) mpz_clear(P[i]);
                
                return 0;
            }
                
        }
    }
    
    

    /* Decoding FAILURE!!! */
    return DecodingFailureError;
}

