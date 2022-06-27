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
#include "rng.h"

#include "field.h"

Kfield K;


#include "EdonS128.h"
#include "crypto_sign.h"


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

int crypto_sign_keypair(unsigned char *pk, unsigned char *sk)
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

    extern struct PublicKeyStruct PublicKey;
    
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
        for(ii=0; ii<NrColumns-byteswritten; ii++) 
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
        for(ii=0; ii<ProjectionDim-byteswritten; ii++) 
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
//    Kvec_clear(PublicKey.p, BinarySpan*2);

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
        if((*NumberOfAllCandidates)>=(1<<(mu + 1 + ErrorBasisDimension))) {
            return CorruptedCError;
        }

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
                if((*NumberOfAllCandidates)>=(1<<(mu + 1 + ErrorBasisDimension))) {
                    return CorruptedCError;
                }
            }
        }
    }
    
    Kclear(NewCandidate);
    return 0;
}

int compare(const Kelt a, const Kelt b)
{
    return Kcmp(a, b);
}


int crypto_sign( 
    unsigned char *sm, unsigned long long *smlen, 
    const unsigned char *m, unsigned long long mlen, 
    const unsigned char *sk
)
{
    int i, j, jj, counter, NumberOfAllCandidates;
    int nextcandidate, nextbase, cmp;
    unsigned long int popcount, position;
    unsigned char tempsk[32], temphash[crypto_hash_sha256_BYTES];
    unsigned char tmp[256];
    unsigned char SHA2temp[crypto_hash_sha256_BYTES];
    unsigned char temp01;
    unsigned char *tempmsg = 0;
    
    mpz_t ShortenedPermutation[ProjectionDim];
    mpz_t HBottom[ProjectionDim], HBottomTransposed[ProjectionDim];
    mpz_t HTop[ProjectionDim];
    mpz_t H[ProjectionDim];
    mpz_t temp;
    mpz_t P[NrColumns];

    extern struct PublicKeyStruct PublicKey;

    Kfield_init();

    struct SignatureStruct Signature;
    Kelt a, b, tempel;
        
    Kelt M[ProjectionDim], y[NrRows], C[NrColumns], allcandidates[(1<<(mu + 1 + ErrorBasisDimension))+2];
    Kelt e[ErrorBasisDimension], error[NrColumns], mm[mu];
    
    unsigned char diversifier[8];
    AES_XOF_struct ctx;
    struct PoolOfRandomBytes Pool;
    unsigned char tempbuf[NrColumns8];

    for(i=0; i<ErrorBasisDimension; i++) Kinit(e[i]);
    for(i=0; i<NrColumns; i++) Kinit(error[i]);
    for(i=0; i<NrColumns; i++) Kinit(C[i]);

    for(i=0; i<ProjectionDim; i++) Kinit(M[i]);
    for(i=0; i<NrRows; i++) Kinit(y[i]);
    for(i=0; i<mu; i++) Kinit(mm[i]);
        
    for(i=0; i<mu + 1 + ErrorBasisDimension; i++) Kinit(Signature.Cbase[i]);
    for(i=0; i<(1<<(mu + 1 + ErrorBasisDimension))+2; i++) Kinit(allcandidates[i]);
    Kinit(a);
    Kinit(b);
    Kinit(tempel);

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
    
    memcpy(tempsk, sk, 32);

    /* Recreate private structures from the secret key sk via the seedexpander */
    memset(diversifier, 0x00, 8);
    seedexpander_init(&ctx, tempsk, diversifier, 0x01<<15);

    seedexpander(&ctx, (unsigned char *)a, sizeof(Kelt));
    seedexpander(&ctx, (unsigned char *)b, sizeof(Kelt));

    seedexpander(&ctx, (unsigned char *) &Pool.pool, 6*NrColumns + 7*ProjectionDim );
    Pool.poolsize = 6*NrColumns + 7*ProjectionDim - 1;

    /* Recreate the orthogonal binary matrix P of size NrColumns x NrColumns */
    randomOrthogonalBinMatrix(P, NrColumns, &Pool);

    /* Recreate the binary orthogonal matrix HBottom of size ProjectionDim x ProjectionDim */
    randomOrthogonalBinMatrix(HBottom, ProjectionDim, &Pool);
    /* a slow computation of a transposition of HBottom to produce HBottomTransposed */
    for(i = 0; i < ProjectionDim; i++) {
        mpz_set_ui(HBottomTransposed[i], 0);
        for(j=0; j<ProjectionDim; j++) {
            if (mpz_tstbit(HBottom[j],i)==1)
                mpz_setbit(HBottomTransposed[i],j);
        }
    }

    /* Recreate the matrix HTop where every column has an even Hamming weight */
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

LOOP:

    /* Fetch some random e and error */
#ifdef KAT
    printf("\nGenerating a random basis e:\n");
#endif
    for(j=0; j<ErrorBasisDimension; j++) {
        randombytes(tmp, sizeof(Kelt));
        memcpy(e[j], &tmp, sizeof(Kelt));
#ifdef KAT
    printf("e[%2d] = ",j); Kprint(e[j]); printf("\n");    
#endif
    }

#ifdef KAT
    printf("\nFrom the linear binary span of basis e generate a random vector 'error':\n");
#endif
    randombytes(tmp, NrColumns);
    for(i=0; i<NrColumns; i++) {
        temp01 = (tmp[i] % ((1<<ErrorBasisDimension)-1) ) + 1;
        Kset_ui(error[i],0);
        for(j=0; j<ErrorBasisDimension; j++) {
            if((temp01>>j) & 1)
                Kadd(error[i], error[i], e[j]);
        }
#ifdef KAT
    printf("error[%2d] = ",i); Kprint(error[i]); printf("\n");    
#endif
    }


    /* Compute y = PubMat * error */
#ifdef KAT
    printf("\nIn procedure crypto_sign( ), Compute y = PubMat * error :\n");
#endif
    for(i=0; i<NrRows; i++) {
        Kset_ui(y[i],0);
        for(j=0; j<NrColumns; j++) {
            Kset_ui(tempel,0);
            temp01 = PublicKey.cosetLeader[i] ^ PublicKey.linearCombination[i][j];
            for(jj=0; jj<BinarySpan; jj++) {
                if((temp01>>jj) & 1)
                    Kadd(tempel, tempel, PublicKey.p[jj]);
            }
            temp01 = PublicKey.linearCombination[i][j];
            for(jj=0; jj<BinarySpan; jj++) {
                if((temp01>>jj) & 1)
                    Kadd(tempel, tempel, PublicKey.p[jj + BinarySpan]);
            }
            Kmul(tempel, tempel, error[j]);
            Kadd(y[i], y[i], tempel);
        }
#ifdef KAT
    printf("y[%2d] = ",i); Kprint(y[i]); printf("\n");    
#endif
    }
    
    
    /* Compute SHA256(y || m) */
    tempmsg = malloc(mlen + NrRows*sizeof(Kelt));
    memcpy(tempmsg, &y, NrRows*sizeof(Kelt));
    memcpy(&tempmsg[NrRows*sizeof(Kelt)], m, mlen);
#ifdef KATqq
    printf("\nHashing the following message:\n"); 
    for(ii=0; ii<mlen + NrRows*sizeof(Kelt); ii++) printf("%02x", tempmsg[ii]); 
    printf("\n");
#endif

    crypto_hash_sha256(SHA2temp, tempmsg, mlen + NrRows*sizeof(Kelt));
    
    memcpy(Signature.M, &SHA2temp, crypto_hash_sha256_BYTES);
#ifdef KATqq
    printf("\nSignature.M:\n"); 
    for(ii=0; ii<crypto_hash_sha256_BYTES; ii++) printf("%02x", Signature.M[ii]); 
    printf("\n");

    printf("\nSHA2temp:\n"); 
    for(ii=0; ii<crypto_hash_sha256_BYTES; ii++) printf("%02x", SHA2temp[ii]); 
    printf("\n");
#endif


    memcpy(mm[0], &SHA2temp, sizeof(Kelt));
#ifdef KAT
    printf("\nM base:\nmm[ 0] = "); Kprint(mm[0]); printf("\n");    
#endif
    memcpy(mm[1], &SHA2temp[sizeof(Kelt)], sizeof(Kelt));
#ifdef KAT
    printf("mm[ 1] = "); Kprint(mm[1]); printf("\n");    
#endif
    
    
    for(j=0; j<mu - 2; j+=2) {
        crypto_hash_sha256(temphash, SHA2temp, crypto_hash_sha256_BYTES);
        if(j+2 < mu)
            memcpy(mm[j + 2], &temphash, sizeof(Kelt));
        if(j+3 < mu)
            memcpy(mm[j + 3], &temphash[sizeof(Kelt)], sizeof(Kelt));
        memcpy(SHA2temp, &temphash, crypto_hash_sha256_BYTES);
#ifdef KAT
    if(j+2 < mu) {
        printf("mm[%2d] = ",j + 2); Kprint(mm[j + 2]); printf("\n");
    }
    if(j+3 < mu) {
        printf("mm[%2d] = ",j + 3); Kprint(mm[j + 3]); printf("\n");
    }
#endif
    }
    
#ifdef KAT
    printf("\nGenerate a vector M with elements from the linear binary span of the basis mm\n");
#endif
    counter = 0;
    while(counter<ProjectionDim) {
        crypto_hash_sha256(temphash, SHA2temp, crypto_hash_sha256_BYTES);
        memcpy(&tmp[counter], &temphash, crypto_hash_sha256_BYTES);
        counter += crypto_hash_sha256_BYTES;
        memcpy(SHA2temp, &temphash, crypto_hash_sha256_BYTES);
    }
    
    for(i=0; i<ProjectionDim; i++) {
        temp01 = (tmp[i] % ((1<<mu)-1) ) + 1;
        Kset_ui(M[i],0);
        for(j=0; j<mu; j++) {
            if((temp01>>j) & 1)
                Kadd(M[i], M[i], mm[j]);
        }
#ifdef KAT
    printf("M[%2d] = ",i); Kprint(M[i]); printf("\n");    
#endif
    }

    /* Compute C = PrivMat * M + error */
    /* PrivMat in the documentation corresponds to ShortenedPermutation together with a and b */
    
    NumberOfAllCandidates = 0;
    for(i=0; i<NrColumns; i++) {
        Kset(C[i], error[i]);
        for(j=0; j<ProjectionDim; j++) {
            if(mpz_tstbit(ShortenedPermutation[j], i)==0) {
                Kmul(tempel, a, M[j]);
                Kadd(C[i], C[i], tempel);
            }
            else {
                Kmul(tempel, b, M[j]);
                Kadd(C[i], C[i], tempel);
            }            
        }
        addcandidate(allcandidates, C[i], &NumberOfAllCandidates);
        
        if(NumberOfAllCandidates>=(1<<(mu + 1 + ErrorBasisDimension))) {
            return CorruptedCError;
        }
    }
    if(NumberOfAllCandidates == (1<<(mu + 1 + ErrorBasisDimension)) - 1) {
         Kset_ui(allcandidates[NumberOfAllCandidates], 0);
         NumberOfAllCandidates++;
    }
    if(NumberOfAllCandidates < (1<<(mu + 1 + ErrorBasisDimension)) - 1) {
        /* The size of this binary linear span is too small. Try once more. */
#ifdef KAT
    printf("\nUnsuccessful attempt to produce a signature.\n"); 
    printf("The size of the binary linear span is too small. Try once more.\n");
#endif
         free(tempmsg);
         goto LOOP;
    }
    
    /* Compress C */
    
    /* First sort elements */
    qsort(allcandidates, NumberOfAllCandidates, sizeof(Kelt), compare);
    
    /* The first element is always 0 */
    /* Put the next two elements from allcandidates[ ] in the basis of the bin lin span */
    Kset(Signature.Cbase[0], allcandidates[1]);
    Kset(Signature.Cbase[1], allcandidates[2]);
    nextcandidate = 2;
    nextbase = 2;

    /* Continue with the filling of the base by selecting the smallest elements */
    /* from the list allcandidates[ ], and by checking that newly added elements are */
    /* not binary linear combination of the existing elements in the base. */
    while( (nextbase < mu + 1 + ErrorBasisDimension) && (nextcandidate < NumberOfAllCandidates)) {
        nextcandidate++;
        cmp = 1;
        for(j=1; j<(1<<(nextbase)); j++ ) {
            Kset_ui(tempel, 0);
            for(jj=0; jj<nextbase; jj++) {
                if((j>>jj) & 1)
                    Kadd(tempel, tempel, Signature.Cbase[jj]);
            }
            cmp = cmp * Kcmp(tempel, allcandidates[nextcandidate]);
            if(cmp == 0)
                break;
        }
        if(cmp) {
            Kset(Signature.Cbase[nextbase], allcandidates[nextcandidate]);
            nextbase++;
        }
    }
    
    /* Make a new list of 2^(mu + 1 + ErrorBasisDimension) elements in allcandidates[] */
    /* according to the base Signature.Cbase[] */
    for(i=0; i<(1<<(mu + 1 + ErrorBasisDimension)); i++) {
        Kset_ui(tempel, 0);
        for(jj=0; jj<mu + 1 + ErrorBasisDimension; jj++) {
            if((i>>jj) & 1)
                Kadd(tempel, tempel, Signature.Cbase[jj]);
        }
        Kset(allcandidates[i], tempel);
    }
    
    /* Fill the indexes in Signature.index[ ] */
    for(i=0; i<NrColumns; i++) {
        j=0;
        while(Kcmp(C[i], allcandidates[j])) j++;
        Signature.index[i] = j;
    }
    
    /* Finaly produce the signed message sm */
    memcpy(sm, m, mlen);
    memcpy(&sm[mlen], &Signature, sizeof(struct SignatureStruct));
    (*smlen) = mlen + sizeof(Signature);
    
    /* Clear the variables */
    free(tempmsg);
    for(i=0; i<ErrorBasisDimension; i++) Kclear(e[i]);
    for(i=0; i<NrColumns; i++) Kclear(error[i]);
    for(i=0; i<NrColumns; i++) Kclear(C[i]);

    for(i=0; i<ProjectionDim; i++) Kclear(M[i]);
    for(i=0; i<NrRows; i++) Kclear(y[i]);
    for(i=0; i<mu; i++) Kclear(mm[i]);
        
    for(i=0; i<mu + 1 + ErrorBasisDimension; i++) Kclear(Signature.Cbase[i]);
    for(i=0; i<(1<<(mu + 1 + ErrorBasisDimension))+2; i++) Kclear(allcandidates[i]);
    Kclear(a);
    Kclear(b);
    Kclear(tempel);

    for(i = 0; i<NrColumns; i++) mpz_clear(P[i]);
    
    Kvec_clear(randomConstant, BinarySpan);
    Kvec_clear(PublicKey.p, BinarySpan*2);

    for(i=0; i<ProjectionDim; i++) mpz_clear(HBottom[i]);
    for(i=0; i<ProjectionDim; i++) mpz_clear(ShortenedPermutation[i]);    
    for(i = 0; i < ProjectionDim; i++) mpz_clear(HBottomTransposed[i]);
    for(i=0; i<ProjectionDim; i++) mpz_clear(HTop[i]);

    mpz_clear(temp);

    for(i=0; i<ProjectionDim; i++) mpz_clear(H[i]);
    


    return 0;
}

int crypto_sign_open(
    unsigned char *m, unsigned long long *mlen,
    const unsigned char *sm, unsigned long long smlen,
    const unsigned char *pk
)
{
    int i, j, jj, NumberOfAllCandidates;
    int nextcandidate, nextbase, cmp;
    Kelt e[ErrorBasisDimension];
    unsigned char hash[crypto_hash_sha256_BYTES], temphash[2*crypto_hash_sha256_BYTES];
    unsigned char SHA2ofC[crypto_hash_sha256_BYTES];
    unsigned char s0s1[crypto_hash_sha256_BYTES], s1s0[crypto_hash_sha256_BYTES];
    unsigned char allrandombytes[NrColumns];
    unsigned char SHA2temp[crypto_hash_sha256_BYTES];
    unsigned char temp01;
    unsigned char *tempmsg = 0;

    Kfield_init();
    struct PublicKeyStruct PublicKey;
    struct SignatureStruct Signature;

    Kelt M[ProjectionDim], y[NrRows], C[NrColumns], allcandidates[(1<<(mu + 1 + ErrorBasisDimension))+2];
    Kelt mm[mu], Cbase[NrColumns];
    Kelt tempel;
    for(i=0; i<NrColumns; i++) Kinit(C[i]);
    for(i=0; i<NrColumns; i++) Kinit(Cbase[i]);
    for(i=0; i<ProjectionDim; i++) Kinit(M[i]);
    for(i=0; i<NrRows; i++) Kinit(y[i]);
    for(i=0; i<mu; i++) Kinit(mm[i]);
    for(i=0; i<mu + 1 + ErrorBasisDimension; i++) Kinit(Signature.Cbase[i]);
    for(i=0; i<(1<<(mu + 1 + ErrorBasisDimension))+2; i++) Kinit(allcandidates[i]);
    Kinit(tempel);

    
    memcpy(&PublicKey, pk, sizeof(struct PublicKeyStruct));
    memcpy(m, sm, smlen - sizeof(struct SignatureStruct));
    (*mlen) = smlen - sizeof(struct SignatureStruct);
    
    memcpy(&Signature, &sm[(*mlen)], sizeof(struct SignatureStruct));
    
    /* Check are Signature.Cbase[ ] elements in apsolute increasing order */
    for(i=0; i<mu + ErrorBasisDimension; i++) {
        for(j=i+1; j<mu + 1 + ErrorBasisDimension; j++) {
            if(Kcmp(Signature.Cbase[i], Signature.Cbase[j]) >= 0) return VerificationFailureError;
        }
    }

    /* Uncompress C from Signature and check if it was corrupted or is it in right order */
    NumberOfAllCandidates = 0;
    for(i=0; i<NrColumns; i++) {
        temp01 = Signature.index[i];
        Kset_ui(C[i], 0);
        for(jj=0; jj<mu + 1 + ErrorBasisDimension; jj++) {
            if((temp01>>jj) & 1)
                Kadd(C[i], C[i], Signature.Cbase[jj]);
        }
        addcandidate(allcandidates, C[i], &NumberOfAllCandidates);
        
        if(NumberOfAllCandidates>=(1<<(mu + 1 + ErrorBasisDimension))) {
            return CorruptedCError;
        }
    }
    if(NumberOfAllCandidates == (1<<(mu + 1 + ErrorBasisDimension)) - 1){
         Kset_ui(allcandidates[NumberOfAllCandidates], 0);
         NumberOfAllCandidates++;
    }
    
    /* Check if Signature.Cbase[ ] is the minimal base */
    
    /* Produce a minimal base Cbase[ ] */
    /* First sort elements */
    qsort(allcandidates, NumberOfAllCandidates, sizeof(Kelt), compare);
    
    /* The first element is always 0 */
    /* Put the next two elements from allcandidates[ ] in the basis Cbase[ ] */
    Kset(Cbase[0], allcandidates[1]);
    Kset(Cbase[1], allcandidates[2]);
    nextcandidate = 2;
    nextbase = 2;

    /* Continue with the filling of the base by selecting the smallest elements */
    /* from the list allcandidates[ ], and by checking that newly added elements are */
    /* not binary linear combination of the existing elements in the base. */
    while( (nextbase < mu + 1 + ErrorBasisDimension) && (nextcandidate < NumberOfAllCandidates)) {
        nextcandidate++;
        cmp = 1;
        for(j=1; j<(1<<(nextbase)); j++ ) {
            Kset_ui(tempel, 0);
            for(jj=0; jj<nextbase; jj++) {
                if((j>>jj) & 1)
                    Kadd(tempel, tempel, Signature.Cbase[jj]);
            }
            cmp = cmp * Kcmp(tempel, allcandidates[nextcandidate]);
            if(cmp == 0)
                break;
        }
        if(cmp) {
            Kset(Cbase[nextbase], allcandidates[nextcandidate]);
            nextbase++;
        }
    }
    
    /* Check is the Signature.Cbase[ ] equal to Cbase[ ] */
    for(i=0; i<mu + 1 + ErrorBasisDimension; i++) {
        if(Kcmp(Signature.Cbase[i], Cbase[i])) return VerificationFailureError;
    }
    

    /* Compute y = PubMat * C */
#ifdef KAT
    printf("\nIn procedure crypto_sign_open( ), Compute y = PubMat * C :\n");
#endif
    for(i=0; i<NrRows; i++) {
        Kset_ui(y[i],0);
        for(j=0; j<NrColumns; j++) {
            Kset_ui(tempel,0);
            temp01 = PublicKey.cosetLeader[i] ^ PublicKey.linearCombination[i][j];
            for(jj=0; jj<BinarySpan; jj++) {
                if((temp01>>jj) & 1)
                    Kadd(tempel, tempel, PublicKey.p[jj]);
            }
            temp01 = PublicKey.linearCombination[i][j];
            for(jj=0; jj<BinarySpan; jj++) {
                if((temp01>>jj) & 1)
                    Kadd(tempel, tempel, PublicKey.p[jj + BinarySpan]);
            }
            Kmul(tempel, tempel, C[j]);
            Kadd(y[i], y[i], tempel);
        }
#ifdef KAT
    printf("y[%2d] = ",i); Kprint(y[i]); printf("\n");    
#endif
    }
    
    
    /* Compute SHA256(y || m) */
    tempmsg = malloc(smlen - sizeof(struct SignatureStruct) + NrRows*sizeof(Kelt));
    if (!tempmsg) 
        abort();
    memcpy(tempmsg, &y, NrRows*sizeof(Kelt));
    memcpy(&tempmsg[NrRows*sizeof(Kelt)], sm, smlen - sizeof(struct SignatureStruct));

#ifdef KATqq
    printf("\nHashing the following message:\n"); 
    for(ii=0; ii<smlen - sizeof(struct SignatureStruct) + NrRows*sizeof(Kelt); ii++) 
        printf("%02x", tempmsg[ii]); 
    printf("\n");
#endif

    crypto_hash_sha256(SHA2temp, tempmsg, smlen - sizeof(struct SignatureStruct) + NrRows*sizeof(Kelt));

#ifdef KATqq
    printf("\nSignature.M:\n"); 
    for(ii=0; ii<crypto_hash_sha256_BYTES; ii++) printf("%02x", Signature.M[ii]); 
    printf("\n");

    printf("\nSHA2temp:\n"); 
    for(ii=0; ii<crypto_hash_sha256_BYTES; ii++) printf("%02x", SHA2temp[ii]); 
    printf("\n");
#endif
    
    if( memcmp(&Signature.M, SHA2temp, crypto_hash_sha256_BYTES) )
        return VerificationFailureError;
    
    free(tempmsg);
    
    return 0;
}
