#include "sign_openHFE.h"
#include "choice_crypto.h"
#include "sizes_HFE.h"
#include "hash.h"
#include "add_gf2x.h"
#if !(HFEmr&&(NB_WORD_GF2nvm == 1))
    #include "init.h"
#endif

#if HYBRID_REPRESENTATION_PK
    #include "evalMQShybrid_gf2.h"
#else
    #include "evalMQSnocst_gf2.h"
#endif
#include "tools_gf2m.h"
#include "tools_gf2nv.h"



#if (NB_ITE!=1)

/* Number of bits to complete the byte of sm64, in [0,7] */
#define VAL_BITS_M (((HFEDELTA+HFEv)<(8-HFEmr8))?(HFEDELTA+HFEv):(8-HFEmr8))

/* Uncompress the signature */
static void uncompress_signHFE(UINT* sm, const unsigned char* sm8)
{
    unsigned char *sm64;
    unsigned int k2;

    sm64=(unsigned char*)sm;
    /* Take the (n+v) first bits */
    for(k2=0;k2<NB_BYTES_GFqnv;++k2)
    {
        sm64[k2]=sm8[k2];
    }

    /* Clean the last byte */
    #if ((NB_ITE>1)&&HFEnvr8)
        sm64[k2-1]&=HFE_MASKnv8;
    #endif

    /* Take the (Delta+v)*(nb_ite-1) bits */
    #if ((NB_ITE>1) || !(HFEDELTA+HFEv))
        unsigned int k1,nb_bits,nb_rem2,nb_rem_m,val_n;
        #if (HFEmr8)
            int nb_rem;
        #endif

        /* HFEnv bits are already extracted from sm8 */
        nb_bits=HFEnv;
        sm64+=(NB_WORD_GF2nv<<3)+(HFEmq8&7U);

        for(k1=1;k1<NB_ITE;++k1)
        {
            /* Number of bits to complete the byte of sm8, in [0,7] */
            val_n=((HFEDELTA+HFEv)<((8-(nb_bits&7U))&7U)?(HFEDELTA+HFEv):((8-(nb_bits&7U))&7U));

            /* First byte of sm8 */
            if(nb_bits&7U)
            {
                #if (HFEmr8)
                    *sm64^=(sm8[nb_bits>>3]>>(nb_bits&7U))<<HFEmr8;

                    /* Number of bits to complete the first byte of sm8 */
                    nb_rem=(int)((val_n-VAL_BITS_M));
                    if(nb_rem>=0)
                    {
                        /* We take the next byte since we used VAL_BITS_M bits */
                        ++sm64;
                    }
                    if(nb_rem > 0)
                    {
                        nb_bits+=VAL_BITS_M;
                        *sm64^=sm8[nb_bits>>3]>>(nb_bits&7U);
                        nb_bits+=nb_rem;
                    } else
                    {
                        nb_bits+=val_n;
                    }
                #else
                    /* We can take 8 bits, and we want at most 7 bits. */
                    (*sm64)=sm8[nb_bits>>3]>>(nb_bits&7U);
                    nb_bits+=val_n;
                #endif
            }

            /* Other bytes of sm8 */
            nb_rem2=(HFEDELTA+HFEv)-val_n;
            /*nb_rem2 can be zero only in this case */
            #if ((HFEDELTA+HFEv)<8)
            if(nb_rem2)
            {
            #endif
                /* Number of bits used of sm64, mod 8 */
                nb_rem_m=(HFEm+val_n)&7U;

                /* Other bytes */
                if(nb_rem_m)
                {
                    /* -1 to take the ceil of /8, -1 */
                    for(k2=0;k2<((nb_rem2-1)>>3);++k2)
                    {
                        *sm64^=sm8[nb_bits>>3]<<nb_rem_m;
                        ++sm64;
                        *sm64=sm8[nb_bits>>3]>>(8-nb_rem_m);

                        nb_bits+=8;
                    }
                    /* The last byte of sm8, between 1 and 8 bits to put */
                    *sm64^=sm8[nb_bits>>3]<<nb_rem_m;
                    ++sm64;

                    /* nb_rem2 between 1 and 8 bits */
                    nb_rem2=((nb_rem2+7U)&7U)+1U;
                    if(nb_rem2>(8-nb_rem_m))
                    {
                        *sm64=sm8[nb_bits>>3]>>(8-nb_rem_m);
                        ++sm64;
                    }

                    nb_bits+=nb_rem2;
                } else
                {
                    /* We are at the beginning of the bytes of sm8 and sm64 */

                    /* +7 to take the ceil of /8 */
                    for(k2=0;k2<((nb_rem2+7)>>3);++k2)
                    {
                        *sm64=sm8[nb_bits>>3];
                        nb_bits+=8;
                        ++sm64;
                    }
                    /* The last byte has AT MOST 8 bits. */
                    nb_bits-=(8-(nb_rem2&7U))&7U;
                }
            #if ((HFEDELTA+HFEv)<8)
            } else
            {
                ++sm64;
            }
            #endif

            /* Clean the last byte */
            #if HFEnvr8
                sm64[-1]&=HFE_MASKnv8;
            #endif

            /* We complete the word. Then we search the first byte. */
            sm64+=((8-(NB_BYTES_GFqnv&7U))&7U)+(HFEmq8&7U);
        }
    #endif
}
#endif


/* Input:
    m a document
    len the size of document (in byte)
    sm the signature
    pk the public key: MQ system with m equations in GF(2)[x1,...,x_(n+v)]

  Output:
    0 for a valid signature, !=0 else

   Requirement:
    for the vectorial evalMQSnocst_gf2, the public key must be aligned
*/
int PREFIX_NAME(sign_openHFE)(const unsigned char* m, size_t len, \
                              const unsigned char* sm8, const UINT* pk)
{
    UINT sm[SIZE_SIGN_UNCOMPRESSED-SIZE_SALT_WORD]={0};

    static_vecnv_gf2 Si_tab[NB_WORD_GF2nv] ALIGNED_GFqm,Si1_tab[NB_WORD_GF2nv] ALIGNED_GFqm;
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

    #if EUF_CMA_PROPERTY
        sm8+=SIZE_SALT;
    #endif

    #if (NB_ITE==1)
        /* Take the (n+v) first bits */
        for(i=0;i<NB_BYTES_GFqnv;++i)
        {
            ((unsigned char*)sm)[i]=sm8[i];
        }
    #else
        uncompress_signHFE(sm,sm8);
    #endif


    #if EUF_CMA_PROPERTY
        UINT Hd[SIZE_DIGEST_UINT+SIZE_SALT_WORD];

        /* Compute H(m) */
        HASH((unsigned char*)Hd,m,len);

        /* H(m)||r */
        sm8-=SIZE_SALT;
        for(i=0;i<SIZE_SALT_WORD;++i)
        {
            Hd[SIZE_DIGEST_UINT+i]=((UINT*)sm8)[i];
        }

        /* Compute H1 = H(H(m)||r) */
        HASH((unsigned char*)D,(unsigned char*)Hd,SIZE_DIGEST+SIZE_SALT);
    #else
        /* Compute H1 = H(m), the m first bits are D1 */
        HASH((unsigned char*)D,m,len);
    #endif

    for(i=1;i<NB_ITE;++i)
    {
        /* Compute Hi = H(H_(i-1)), the m first bits are Di */
        HASH((unsigned char*)(D+i*SIZE_DIGEST_UINT),(unsigned char*)(D+(i-1)*SIZE_DIGEST_UINT),SIZE_DIGEST);
        /* Clean the previous hash (= extract D_(i-1) from H_(i-1)) */
        #if HFEmr
            D[SIZE_DIGEST_UINT*(i-1)+NB_WORD_GF2m-1]&=HFE_MASKm;
        #endif
    }
    /* Clean the previous hash (= extract D_(i-1) from H_(i-1)) */
    #if HFEmr
        D[SIZE_DIGEST_UINT*(i-1)+NB_WORD_GF2m-1]&=HFE_MASKm;
    #endif

    /* Compute p(S_(NB_IT),X_(NB_IT)) */
    evalMQS_gf2_HFE(Si,sm,pk);
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
        add2_gf2m(Si,D+i*SIZE_DIGEST_UINT);

        #if (HFEnv!=HFEm)
            /* Compute Si||Xi */
            index=NB_WORD_GF2nv+(NB_ITE-1-i)*NB_WORD_GF2nvm;
            #if HFEmr
                Si[NB_WORD_GF2m-1]&=HFE_MASKm;

                /* Concatenation(Si,Xi): the intersection between S1 and X1 is 
                 * not null */
                Si[NB_WORD_GF2m-1]^=sm[index];
                #if (NB_WORD_GF2nvm != 1)
                    ++index;
                    copy_gf2nvm1(Si+NB_WORD_GF2m,sm+index);
                #endif
            #else
                /* Concatenation(Si,Xi) */
                copy_gf2nvm(Si+NB_WORD_GF2m,sm+index);
            #endif
        #endif

        #ifdef KAT_INT
            fprintBstr_KAT_INT(fp,"Si||Xi: ",(unsigned char*)Si,NB_BYTES_GFqnv);
        #endif

        /* Compute p(Si,Xi) */
        evalMQS_gf2_HFE(Si1,Si,pk);

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
    return !isEqual_gf2m(Si,D);
}
