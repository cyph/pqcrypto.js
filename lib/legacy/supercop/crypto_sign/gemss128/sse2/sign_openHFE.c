#include "sign_openHFE.h"
BEGIN_EXTERNC
    #include <libkeccak.a.headers/SimpleFIPS202.h>
END_EXTERNC
#include "add.h"
#if !(HFEmr&&(NB_WORD_GFqnvm == 1))
    #include "init.h"
#endif

#include "evaluateMQSnoconst.h"


/* Input:
    m a document
    len the size of document (in byte)
    sm the signature
    pk the public key: MQ system with m equations in GF(2)[x1,...,x_(n+v)]

  Output:
    0 for a valid signature, !=0 else
*/
int sign_openHFE(const unsigned char* m, size_t len, const UINT* sm, 
                 const UINT* pk)
{
    static_vecnv_gf2 Si_tab[NB_WORD_GFqnv],Si1_tab[NB_WORD_GFqnv];
    /* Copy of pointer */
    vecnv_gf2 tmp,Si=Si_tab,Si1=Si1_tab;
    /* Vector of D_1, ..., D_(NB_ITE) */
    UINT D[NB_ITE*SIZE_DIGEST_UINT];
    unsigned int i;
    #if (HFEnv!=HFEm)
        unsigned int index;
    #endif

    #ifdef KAT_INT
        OPEN_KAT_INT_FILE;
        fputs("Sign_open:\n",fp);
    #endif

    /* Compute H1 = H(m), the m first bits are D1 */
    HASH((unsigned char*)D,m,len);
    for(i=1;i<NB_ITE;++i)
    {
        /* Compute Hi = H(H_(i-1)), the m first bits are Di */
        HASH((unsigned char*)(D+i*SIZE_DIGEST_UINT),(unsigned char*)(D+(i-1)*SIZE_DIGEST_UINT),SIZE_DIGEST);
        /* Clean the previous hash (= extract D_(i-1) from H_(i-1)) */
        #if HFEmr
            D[SIZE_DIGEST_UINT*(i-1)+NB_WORD_GFqm-1]&=HFE_MASKm;
        #endif
    }
    /* Clean the previous hash (= extract D_(i-1) from H_(i-1)) */
    #if HFEmr
        D[SIZE_DIGEST_UINT*(i-1)+NB_WORD_GFqm-1]&=HFE_MASKm;
    #endif

    /* Compute p(S_(NB_IT),X_(NB_IT)) */
    verifHFE(Si,sm,pk);
    #ifdef KAT_INT
        fprintf(fp,"i=%u\n",NB_ITE);
        fprintBstr_KAT_INT(fp,"xor(Di,S_(i-1)): ",(unsigned char*)Si,NB_BYTES_GFqm);
    #endif

    for(i=NB_ITE-1;i>0;--i)
    {
        #ifdef KAT_INT
            fprintf(fp,"i=%u\n",i);
        #endif
        /* Compute Si = xor(p(S_i+1,X_i+1),D_i+1) */
        HFEADD2m(Si,D+i*SIZE_DIGEST_UINT);

        #if (HFEnv!=HFEm)
            /* Compute Si||Xi */
            index=NB_WORD_GFqnv+(NB_ITE-1-i)*NB_WORD_GFqnvm;
            #if HFEmr
                Si[NB_WORD_GFqm-1]&=HFE_MASKm;

                /* Concatenation(Si,Xi): the intersection between S1 and X1 is 
                 * not null */
                Si[NB_WORD_GFqm-1]^=sm[index];
                #if (NB_WORD_GFqnvm != 1)
                    ++index;
                    HFECOPYnvm1(Si+NB_WORD_GFqm,sm+index);
                #endif
            #else
                /* Concatenation(Si,Xi) */
                HFECOPYnvm(Si+NB_WORD_GFqm,sm+index);
            #endif
        #endif

        #ifdef KAT_INT
            fprintBstr_KAT_INT(fp,"Si||Xi: ",(unsigned char*)Si,NB_BYTES_GFqnv);
        #endif

        /* Compute p(Si,Xi) */
        verifHFE(Si1,Si,pk);

        /* Permutation of pointers */
        tmp=Si1;
        Si1=Si;
        Si=tmp;

        #ifdef KAT_INT
            fprintBstr_KAT_INT(fp,"xor(Di,S_(i-1)): ",(unsigned char*)Si,NB_BYTES_GFqm);
        #endif
    }

    #ifdef KAT_INT
        fprintBstr_KAT_INT(fp,"true D1: ",(unsigned char*)D,NB_BYTES_GFqm);
        CLOSE_KAT_INT_FILE;
    #endif

    /* D1'' == D1 */
    return !HFEISEQUALm(Si,D);
}
