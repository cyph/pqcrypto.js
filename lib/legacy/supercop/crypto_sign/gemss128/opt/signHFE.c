#include "signHFE.h"
#include "choice_crypto.h"
#include "gf2nx.h"
#include "config_HFE.h"
#include "sizes_HFE.h"
#include "hash.h"
#include "arith_gf2n.h"
#include "tools_gf2m.h"
#include "tools_gf2n.h"
#include "tools_gf2v.h"
#include "init.h"
#include "randombytes.h"
#if (!RIGHT_MULTIPLICATION_BY_T)
    #include "matVecProduct_gf2.h"
#endif
#include "vecMatProduct_gf2.h"
#if(HFEv)
    #include "evalMQSv_gf2.h"
#endif

#include "chooseRootHFE_gf2nx.h"
#if(HFEDeg!=1)
    #include "initListDifferences_gf2nx.h"
#endif


#if 0
    /* When the root finding fails, only the vinegars are regenerated */
    #define GEN_MINUS_VINEGARS 0
#else
    /* When the root finding fails, the minus and vinegars are regenerated */
    #define GEN_MINUS_VINEGARS 1
#endif



#if (NB_ITE!=1)

/* Number of bits to complete the byte of sm64, in [0,7] */
#define VAL_BITS_M (((HFEDELTA+HFEv)<(8-HFEmr8))?(HFEDELTA+HFEv):(8-HFEmr8))

static void compress_signHFE(unsigned char* sm8, const UINT* sm)
{
    unsigned char *sm64;
    unsigned int k2;

    sm64=(unsigned char*)sm;

    /* Take the (n+v) first bits */
    for(k2=0;k2<NB_BYTES_GFqnv;++k2)
    {
        sm8[k2]=sm64[k2];
    }

    /* Take the (Delta+v)*(nb_ite-1) bits */
    #if ((NB_ITE>1) || !(HFEDELTA+HFEv))
        unsigned int k1,nb_bits,nb_rem2,nb_rem_m,val_n;
        #if (HFEmr8)
            int nb_rem;
        #endif

        /* HFEnv bits are already stored in sm8 */
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
                    sm8[nb_bits>>3]^=((*sm64)>>HFEmr8)<<(nb_bits&7U);

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
                        sm8[nb_bits>>3]^=(*sm64)<<(nb_bits&7U);
                        nb_bits+=nb_rem;
                    } else
                    {
                        nb_bits+=val_n;
                    }
                #else
                    /* We can take 8 bits, and we want at most 7 bits. */
                    sm8[nb_bits>>3]^=(*sm64)<<(nb_bits&7U);
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
                        sm8[nb_bits>>3]=((*sm64)>>nb_rem_m)^((*(sm64+1))<<(8-nb_rem_m));
                        nb_bits+=8;
                        ++sm64;
                    }
                    /* The last byte of sm8, between 1 and 8 bits to put */
                    sm8[nb_bits>>3]=(*sm64)>>nb_rem_m;
                    ++sm64;

                    /* nb_rem2 between 1 and 8 bits */
                    nb_rem2=((nb_rem2+7U)&7U)+1U;
                    if(nb_rem2>(8-nb_rem_m))
                    {
                        sm8[nb_bits>>3]^=(*sm64)<<(8-nb_rem_m);
                        ++sm64;
                    }

                    nb_bits+=nb_rem2;
                } else
                {
                    /* We are at the beginning of the bytes of sm8 and sm64 */

                    /* +7 to take the ceil of /8 */
                    for(k2=0;k2<((nb_rem2+7)>>3);++k2)
                    {
                        sm8[nb_bits>>3]=*sm64;
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
            /* We complete the word. Then we search the first byte. */
            sm64+=((8-(NB_BYTES_GFqnv&7U))&7U)+(HFEmq8&7U);
        }
    #endif
}
#endif



/* Precomputation for one secret key */
int PREFIX_NAME(precSignHFE)(complete_sparse_monic_gf2nx* F_struct, \
                const UINT** linear_coefs, const UINT* sk)
{
    #if(HFEDeg!=1)
        initListDifferences_gf2nx(F_struct->L);
    #endif

    #if HFEv
        unsigned int i;
        UINT* F;
        ALIGNED_MALLOC(F,UINT*,NB_UINT_HFEPOLY,sizeof(UINT));
        VERIFY_ALLOC_RET(F);

        #if (HFEDeg>1)
        UINT *F_cp;
        unsigned int j;

        /* X^(2^0) */
        linear_coefs[0]=sk+MQv_GFqn_SIZE;
        /* X^(2^1) */
        sk=linear_coefs[0]+MLv_GFqn_SIZE;
        F_cp=F+2*NB_WORD_GFqn;
        for(i=0;i<HFEDegI;++i)
        {
            /* Copy i quadratic terms */

            #if ENABLED_REMOVE_ODD_DEGREE
            for(j=(((1U<<i)+1U)<=HFE_odd_degree)?0:1;j<i;++j)
            #else
            for(j=0;j<i;++j)
            #endif
            {
                /* X^(2^i + 2^j) */
                copy_gf2n(F_cp,sk);
                sk+=NB_WORD_GFqn;
                F_cp+=NB_WORD_GFqn;
            }
            /* Store the address of X^(2^(i+1)) */
            linear_coefs[i+1]=sk;
            /* Linear term is not copied */
            sk+=MLv_GFqn_SIZE;
            F_cp+=NB_WORD_GFqn;
        }
        #if HFEDegJ
            /* X^(2^HFEDegI + 2^j) */
            #if ENABLED_REMOVE_ODD_DEGREE
            for(j=(((1U<<i)+1U)<=HFE_odd_degree)?0:1;j<HFEDegJ;++j)
            #else
            for(j=0;j<HFEDegJ;++j)
            #endif
            {
                copy_gf2n(F_cp,sk);
                sk+=NB_WORD_GFqn;
                F_cp+=NB_WORD_GFqn;
            }
        #endif
        #endif
        F_struct->poly=F;
    #endif

    return 0;
}




/* Copy of signHFE1 without the precomputation */
/* This function can be used with precSignHFE */
int PREFIX_NAME(signHFE1withPrec)(unsigned char* sm8, const UINT* sk, \
                     complete_sparse_monic_gf2nx F_struct, \
                     const UINT** linear_coefs, const UINT* Hi)
{
    UINT sm[SIZE_SIGN_UNCOMPRESSED-SIZE_SALT_WORD]={0};

    static_vecnv_gf2 DR[NB_WORD_GF2nv];
    static_gf2n U[NB_WORD_GFqn] ALIGNED;
    #if(HFEn>HFEm)
        unsigned char* DR_cp=(unsigned char*)DR;
        #if(HFEm&7)
            unsigned char rem_char;
        #endif
    #endif

    unsigned int k2;
    int nb_root;

    #if(HFEv)
        UINT* F=(UINT*)F_struct.poly;
        unsigned int i;
        static_vecv_gf2 V[NB_WORD_GFqv];
        #if (HFEDeg>1)
            static_gf2n tmp_n[NB_WORD_GFqn] ALIGNED;
        #endif
    #endif


    #ifdef KAT_INT
        OPEN_KAT_INT_FILE;
        fputs("Sign:\n",fp);
        unsigned int nb_try_to_sign;
    #endif


    #ifdef KAT_INT
        nb_try_to_sign=0U;
        fputs("k=1\n",fp);
    #endif

    /* Compute xor(D_k,S_(k-1)) */
    copy_gf2m(DR,Hi);

    #if AFFINE_TRANSFORMATION_BY_t
        /* DR - t */
        add2_gf2m(DR,sk+ACCESS_VECTOR_t);
    #endif

    #if(HFEm&7)
        /* Clean the last char to compute rem_char (the last word is cleaned) */
        DR[NB_WORD_GF2m-1]&=HFE_MASKm;
        /* Save the last byte because we need to erase this value by 
         * randombytes */
        #if(HFEn>HFEm)
            rem_char=DR_cp[NB_BYTES_GFqm-1];
        #endif
    #endif

    #ifdef KAT_INT
        fprintBstr_KAT_INT(fp,"xor(Dk,S_(k-1)): ",(unsigned char*)DR,NB_BYTES_GFqm);
    #endif

    /* Since DR is not initialiazed to zero, we need to add this code */
    #if(HFEn>HFEm)
    /* Clean the last word only if it is useful: 
     * if (HFEn&7), it is already cleaned in the do while loop
     * if (!HFEnr), every bits of the word are used
     * if (HFEm&7)&&(NB_WORD_GFqn==NB_WORD_GF2m), the last word is already 
     * cleaned by the previous code */
    #if ((!(HFEn&7)) && HFEnr && ((!(HFEm&7))||(NB_WORD_GFqn!=NB_WORD_GF2m)))
        DR[NB_WORD_GFqn-1]=0UL;
    #endif
    #endif

    #if GEN_MINUS_VINEGARS
    /* When the root finding fails, the minus and vinegars are regenerated */
    do
    {
    #endif
        #if(HFEn>HFEm)
        /* Compute Dk||Rk: add random to have n bits, without erased the m 
         * bits */
        #if(HFEm&7)
            /* Generation of Rk */
            randombytes(DR_cp+NB_BYTES_GFqm-1,
                       (unsigned long long)(NB_BYTES_GFqn-NB_BYTES_GFqm+1));
            /* Put HFEm&7 first bits to 0 */
            DR_cp[NB_BYTES_GFqm-1]&=~((1U<<(HFEm&7))-1);
            /* Store rem_char */
            DR_cp[NB_BYTES_GFqm-1]^=rem_char;
        #else
            /* Generation of Rk */
            randombytes(DR_cp+NB_BYTES_GFqm,
                    (unsigned long long)(NB_BYTES_GFqn-NB_BYTES_GFqm));
        #endif

        /* To clean the last char (because of randombytes), the last word is 
         * cleaned */
        #if (HFEn&7)
            DR[NB_WORD_GFqn-1]&=HFE_MASKn;
        #endif
        #endif

        #ifdef KAT_INT
            ++nb_try_to_sign;
            fprintf(fp,"Try %u, ",nb_try_to_sign);
            fprintBstr_KAT_INT(fp,"Dk||Rk: ",(unsigned char*)DR,NB_BYTES_GFqn);
        #endif

        /* Compute Sk||Xk = Inv_p(Dk,Rk) */
        #if RIGHT_MULTIPLICATION_BY_T
            /* Firstly: compute c * T^(-1) */
            vecMatProductn_gf2(U,DR,sk+ACCESS_MATRIX_T);
        #else
            /* Firstly: compute T^(-1) * c */
            matVecProduct_gf2(U,sk+ACCESS_MATRIX_T,DR);
        #endif

    #if (!GEN_MINUS_VINEGARS)
    /* When the root finding fails, only the vinegars are regenerated */
    do
    {
    #endif
        /* Secondly: find v with F_HFE(v) = U */

        /* Generation of vinegar variables: v bits */
        #if(HFEv)
            randombytes((unsigned char*)V,(unsigned long long)(NB_BYTES_GFqv));
            #if HFEvr8
                /* Clean the last word */
                V[NB_WORD_GFqv-1]&=HFE_MASKv;
            #endif

            /* Evaluation of the constant, quadratic map with v vinegars */
            evalMQSv_gf2(F,V,sk);

            #if (HFEDeg>1)
                /* Evaluation of the linear terms, linear maps with v vinegars */
                #if ENABLED_REMOVE_ODD_DEGREE
                #if(HFEDegI==HFEDegJ)
                for(i=0;i<=LOG_odd_degree;++i)
                #elif(HFEDegI<=LOG_odd_degree)
                for(i=0;i<=HFEDegI;++i)
                #else
                for(i=0;i<=(LOG_odd_degree+1);++i)
                #endif
                {
                    vecMatProductv_gf2(tmp_n,V,linear_coefs[i]+NB_WORD_GFqn);
                    add_gf2n(F+NB_WORD_GFqn*(((i*(i+1))>>1)+1),linear_coefs[i],tmp_n);
                }
                for(;i<=HFEDegI;++i)
                {
                    vecMatProductv_gf2(tmp_n,V,linear_coefs[i]+NB_WORD_GFqn);
                    add_gf2n(F+NB_WORD_GFqn*(((i*(i-1))>>1)+2+LOG_odd_degree),linear_coefs[i],tmp_n);
                }
                #else
                for(i=0;i<=HFEDegI;++i)
                {
                    vecMatProductv_gf2(tmp_n,V,linear_coefs[i]+NB_WORD_GFqn);
                    add_gf2n(F+NB_WORD_GFqn*(((i*(i+1))>>1)+1),linear_coefs[i],tmp_n);
                }
                #endif
            #endif
        #endif

        nb_root=chooseRootHFE_gf2nx(DR,F_struct,U);
        if(!nb_root)
        {
            /* fail: retry with an other Rk */
            continue;
        }
        if(nb_root<0)
        {
            /* Error from chooseRootHFE */
            return nb_root;
        }

        break;
    } while(1);

    #if HFEv
        /* Add the v bits to DR */
        #if HFEnr
            DR[NB_WORD_GFqn-1]^=V[0]<<HFEnr;
            for(i=0;i<(NB_WORD_GFqv-1);++i)
            {
                DR[NB_WORD_GFqn+i]=(V[i]>>(64-HFEnr))^(V[i+1]<<HFEnr);
            }
            #if ((NB_WORD_GFqn+NB_WORD_GFqv) == NB_WORD_GF2nv)
                DR[NB_WORD_GFqn+i]=V[i]>>(64-HFEnr);
            #endif
        #else
            for(i=0;i<NB_WORD_GFqv;++i)
            {
                DR[NB_WORD_GFqn+i]=V[i];
            }
        #endif
    #endif

    /* Finally: compute Sk||Xk = v * S^(-1) */
    vecMatProductnv_gf2(sm,DR,sk+ACCESS_MATRIX_S);

    #ifdef KAT_INT
        CLOSE_KAT_INT_FILE;
    #endif

    /* Take the (n+v) first bits */
    for(k2=0;k2<NB_BYTES_GFqnv;++k2)
    {
        sm8[k2]=((unsigned char*)sm)[k2];
    }

    return 0;
}




/* Input:
    m a document to sign
    len the size of document (in byte)
    sk the concatenation of HFE polynomial in GF(2^n), \
    S matrix (n+v)*(n+v) and T matrix n*n, in GF(2)

  Output:
    0 if the result is correct, ERROR_ALLOC for error from calloc functions
    sm the signature

  Requirement: 
    for the vectorial version, sk must be aligned if HFEv==0
*/
/* Format of signature: S_1||X_1 on n+v bits */
/* Copy of signHFE without the number of iteration */
int PREFIX_NAME(signHFE1)(unsigned char* sm8, \
                          const unsigned char* m, size_t len, const UINT* sk)
{
    UINT sm[SIZE_SIGN_UNCOMPRESSED-SIZE_SALT_WORD]={0};

    static_vecnv_gf2 DR[NB_WORD_GF2nv];
    static_gf2n U[NB_WORD_GFqn] ALIGNED;
    UINT Hi_tab[SIZE_DIGEST_UINT];
    UINT *Hi=Hi_tab;
    #if(HFEn>HFEm)
        unsigned char* DR_cp=(unsigned char*)DR;
        #if(HFEm&7)
            unsigned char rem_char;
        #endif
    #endif

    unsigned int k2;
    int nb_root;
    complete_sparse_monic_gf2nx F_struct;

    #if(HFEv)
        UINT* F;
        unsigned int i;
        static_vecv_gf2 V[NB_WORD_GFqv];
        #if (HFEDeg>1)
            static_gf2n tmp_n[NB_WORD_GFqn] ALIGNED;
        #endif
    #endif

    #if(HFEv)
        #if (HFEDeg>1)
            const UINT* linear_coefs[HFEDegI+1];
            nb_root=precSignHFE(&F_struct,linear_coefs,sk);
        #else
            nb_root=precSignHFE(&F_struct,NULL,sk);
        #endif
        if(nb_root)
        {
            /* Error from malloc */
            return nb_root;
        }
        F=(UINT*)F_struct.poly;
    #else
        #if(HFEDeg!=1)
            initListDifferences_gf2nx(F_struct.L);
        #endif
        F_struct.poly=sk;
    #endif


    #ifdef KAT_INT
        OPEN_KAT_INT_FILE;
        fputs("Sign:\n",fp);
        unsigned int nb_try_to_sign;
    #endif


    /* Compute H1 = H(m) */
    HASH((unsigned char*)Hi,m,len);

    #ifdef KAT_INT
        nb_try_to_sign=0U;
        fputs("k=1\n",fp);
    #endif

    /* Compute xor(D_k,S_(k-1)) */
    copy_gf2m(DR,Hi);

    #if AFFINE_TRANSFORMATION_BY_t
        /* DR - t */
        add2_gf2m(DR,sk+ACCESS_VECTOR_t);
    #endif

    #if(HFEm&7)
        /* Clean the last char to compute rem_char (the last word is cleaned) */
        DR[NB_WORD_GF2m-1]&=HFE_MASKm;
        /* Save the last byte because we need to erase this value by 
         * randombytes */
        #if(HFEn>HFEm)
            rem_char=DR_cp[NB_BYTES_GFqm-1];
        #endif
    #endif

    #ifdef KAT_INT
        fprintBstr_KAT_INT(fp,"xor(Dk,S_(k-1)): ",(unsigned char*)DR,NB_BYTES_GFqm);
    #endif

    /* Since DR is not initialiazed to zero, we need to add this code */
    #if(HFEn>HFEm)
    /* Clean the last word only if it is useful: 
     * if (HFEn&7), it is already cleaned in the do while loop
     * if (!HFEnr), every bits of the word are used
     * if (HFEm&7)&&(NB_WORD_GFqn==NB_WORD_GF2m), the last word is already 
     * cleaned by the previous code */
    #if ((!(HFEn&7)) && HFEnr && ((!(HFEm&7))||(NB_WORD_GFqn!=NB_WORD_GF2m)))
        DR[NB_WORD_GFqn-1]=0UL;
    #endif
    #endif

    #if GEN_MINUS_VINEGARS
    /* When the root finding fails, the minus and vinegars are regenerated */
    do
    {
    #endif
        #if(HFEn>HFEm)
        /* Compute Dk||Rk: add random to have n bits, without erased the m 
         * bits */
        #if(HFEm&7)
            /* Generation of Rk */
            randombytes(DR_cp+NB_BYTES_GFqm-1,
                       (unsigned long long)(NB_BYTES_GFqn-NB_BYTES_GFqm+1));
            /* Put HFEm&7 first bits to 0 */
            DR_cp[NB_BYTES_GFqm-1]&=~((1U<<(HFEm&7))-1);
            /* Store rem_char */
            DR_cp[NB_BYTES_GFqm-1]^=rem_char;
        #else
            /* Generation of Rk */
            randombytes(DR_cp+NB_BYTES_GFqm,
                    (unsigned long long)(NB_BYTES_GFqn-NB_BYTES_GFqm));
        #endif

        /* To clean the last char (because of randombytes), the last word is 
         * cleaned */
        #if (HFEn&7)
            DR[NB_WORD_GFqn-1]&=HFE_MASKn;
        #endif
        #endif

        #ifdef KAT_INT
            ++nb_try_to_sign;
            fprintf(fp,"Try %u, ",nb_try_to_sign);
            fprintBstr_KAT_INT(fp,"Dk||Rk: ",(unsigned char*)DR,NB_BYTES_GFqn);
        #endif

        /* Compute Sk||Xk = Inv_p(Dk,Rk) */
        #if RIGHT_MULTIPLICATION_BY_T
            /* Firstly: compute c * T^(-1) */
            vecMatProductn_gf2(U,DR,sk+ACCESS_MATRIX_T);
        #else
            /* Firstly: compute T^(-1) * c */
            matVecProduct_gf2(U,sk+ACCESS_MATRIX_T,DR);
        #endif

    #if (!GEN_MINUS_VINEGARS)
    /* When the root finding fails, only the vinegars are regenerated */
    do
    {
    #endif
        /* Secondly: find v with F_HFE(v) = U */

        /* Generation of vinegar variables: v bits */
        #if(HFEv)
            randombytes((unsigned char*)V,(unsigned long long)(NB_BYTES_GFqv));
            #if HFEvr8
                /* Clean the last word */
                V[NB_WORD_GFqv-1]&=HFE_MASKv;
            #endif

            /* Evaluation of the constant, quadratic map with v vinegars */
            evalMQSv_gf2(F,V,sk);

            #if (HFEDeg>1)
                /* Evaluation of the linear terms, linear maps with v vinegars */
                #if ENABLED_REMOVE_ODD_DEGREE
                #if(HFEDegI==HFEDegJ)
                for(i=0;i<=LOG_odd_degree;++i)
                #elif(HFEDegI<=LOG_odd_degree)
                for(i=0;i<=HFEDegI;++i)
                #else
                for(i=0;i<=(LOG_odd_degree+1);++i)
                #endif
                {
                    vecMatProductv_gf2(tmp_n,V,linear_coefs[i]+NB_WORD_GFqn);
                    add_gf2n(F+NB_WORD_GFqn*(((i*(i+1))>>1)+1),linear_coefs[i],tmp_n);
                }
                for(;i<=HFEDegI;++i)
                {
                    vecMatProductv_gf2(tmp_n,V,linear_coefs[i]+NB_WORD_GFqn);
                    add_gf2n(F+NB_WORD_GFqn*(((i*(i-1))>>1)+2+LOG_odd_degree),linear_coefs[i],tmp_n);
                }
                #else
                for(i=0;i<=HFEDegI;++i)
                {
                    vecMatProductv_gf2(tmp_n,V,linear_coefs[i]+NB_WORD_GFqn);
                    add_gf2n(F+NB_WORD_GFqn*(((i*(i+1))>>1)+1),linear_coefs[i],tmp_n);
                }
                #endif
            #endif
        #endif

        nb_root=chooseRootHFE_gf2nx(DR,F_struct,U);
        if(!nb_root)
        {
            /* fail: retry with an other Rk */
            continue;
        }
        if(nb_root<0)
        {
            /* Error from chooseRootHFE */
            #if HFEv
                free(F);
            #endif
            return nb_root;
        }

        break;
    } while(1);

    #if HFEv
        /* Add the v bits to DR */
        #if HFEnr
            DR[NB_WORD_GFqn-1]^=V[0]<<HFEnr;
            for(i=0;i<(NB_WORD_GFqv-1);++i)
            {
                DR[NB_WORD_GFqn+i]=(V[i]>>(64-HFEnr))^(V[i+1]<<HFEnr);
            }
            #if ((NB_WORD_GFqn+NB_WORD_GFqv) == NB_WORD_GF2nv)
                DR[NB_WORD_GFqn+i]=V[i]>>(64-HFEnr);
            #endif
        #else
            for(i=0;i<NB_WORD_GFqv;++i)
            {
                DR[NB_WORD_GFqn+i]=V[i];
            }
        #endif
    #endif

    /* Finally: compute Sk||Xk = v * S^(-1) */
    vecMatProductnv_gf2(sm,DR,sk+ACCESS_MATRIX_S);

    #if HFEv
        free(F);
    #endif
    #ifdef KAT_INT
        CLOSE_KAT_INT_FILE;
    #endif

    /* Take the (n+v) first bits */
    for(k2=0;k2<NB_BYTES_GFqnv;++k2)
    {
        sm8[k2]=((unsigned char*)sm)[k2];
    }

    return 0;
}




/* Input:
    m a document to sign
    len the size of document (in byte)
    sk the concatenation of HFE polynomial in GF(2^n), \
    S matrix (n+v)*(n+v) and T matrix n*n, in GF(2)

  Output:
    0 if the result is correct, ERROR_ALLOC for error from calloc functions
    sm the signature

  Requirement: 
    for the vectorial version, sk must be aligned if HFEv==0
*/
/* Format of signature: S_NB_ITE||X_NB_ITE||...||X1
    _ S_NB_ITE||X_NB_ITE: n+v bits
    _ X_(NB_ITE-1): (n+v-m) bits, the first word is padding with m%64 zero bits
      It is to simply the concatenation of S_(NB_ITE-1) and X_(NB_ITE-1)
      during the verification.
    _ It is the same format for X_(NB_ITE-2),...,X1
*/

#if EUF_CMA_PROPERTY
int PREFIX_NAME(signHFE_FeistelPatarin)(unsigned char* sm8, \
                                        const unsigned char* m, size_t len, \
                                        const UINT* sk)
{
    UINT sm[SIZE_SIGN_UNCOMPRESSED-SIZE_SALT_WORD]={0};

    static_vecnv_gf2 DR[NB_WORD_GF2nv];
    static_gf2n U[NB_WORD_GFqn] ALIGNED;
    UINT Hd[SIZE_DIGEST_UINT+SIZE_SALT_WORD],Hi_tab[SIZE_DIGEST_UINT],Hi1_tab[SIZE_DIGEST_UINT];
    UINT *tmp,*Hi=Hi_tab,*Hi1=Hi1_tab;
    unsigned int k;
    unsigned char b;
    #if (HFEnv!=HFEm)
        unsigned int index;
    #endif
    #if(HFEn>HFEm)
        unsigned char* DR_cp=(unsigned char*)DR;
        #if(HFEm&7)
            unsigned char rem_char;
        #endif
    #endif

    int nb_root;
    complete_sparse_monic_gf2nx F_struct;
    UINT* F;

    #if(HFEv)
        unsigned int i;
        static_vecv_gf2 V_tab[NB_ITE*NB_WORD_GFqv];
        vecv_gf2 V;
        #if (HFEDeg>1)
            static_gf2n tmp_n[NB_WORD_GFqn] ALIGNED;
        #endif
    #endif


    #if(HFEv)
        #if (HFEDeg>1)
            const UINT* linear_coefs[HFEDegI+1];
            nb_root=precSignHFE(&F_struct,linear_coefs,sk);
        #else
            nb_root=precSignHFE(&F_struct,NULL,sk);
        #endif
        if(nb_root)
        {
            /* Error from malloc */
            return nb_root;
        }
        F=(UINT*)F_struct.poly;
    #else
        #if(HFEDeg!=1)
            initListDifferences_gf2nx(F_struct.L);
        #endif
        F_struct.poly=sk;
    #endif


    #ifdef KAT_INT
        OPEN_KAT_INT_FILE;
        fputs("Sign:\n",fp);
        unsigned int nb_try_to_sign;
    #endif


    /* Compute H(m) */
    HASH((unsigned char*)Hd,m,len);


    /* Generation of vinegar variables: v bits by iteration */
    #if HFEv
        randombytes((unsigned char*)V_tab,(unsigned long long)((NB_ITE*NB_WORD_GFqv)<<3));
        V=V_tab+NB_WORD_GFqv-1;
        #if HFEvr
            for(k=0;k<NB_ITE;++k)
            {
                /* Clean the last word */
                (*V)&=HFE_MASKv;
                V+=NB_WORD_GFqv;
            }
        #endif
    #endif


    #if(HFEn>HFEm)
        unsigned int j;
        #if(HFEm&7)
            unsigned char minus_vec[NB_ITE*(NB_BYTES_GFqn-NB_BYTES_GFqm+1)];
            randombytes(minus_vec, (unsigned long long)
                                   (NB_ITE*(NB_BYTES_GFqn-NB_BYTES_GFqm+1)));
        #else
            unsigned char minus_vec[NB_ITE*(NB_BYTES_GFqn-NB_BYTES_GFqm)];
            randombytes(minus_vec, (unsigned long long)
                                   (NB_ITE*(NB_BYTES_GFqn-NB_BYTES_GFqm)));
        #endif
    #endif


    do
    {
        b=0;
        /* V is the current vinegar variables */
        V=V_tab;

        /* It is to initialize S0 to 0, because Sk||Xk is stored in sm */
        for(k=0;k<NB_WORD_GF2nv;++k)
        {
            sm[k]=0UL;
            DR[k]=0UL;
        }

        /* Random salt r, Hd is H(m)||r */
        randombytes((unsigned char*)(Hd+SIZE_DIGEST_UINT),(unsigned long long)(SIZE_SALT));

        /* Compute H1 = H(H(m)||r) */
        HASH((unsigned char*)Hi,(unsigned char*)Hd,SIZE_DIGEST+SIZE_SALT);


        for(k=1;k<=NB_ITE;++k)
        {
            #ifdef KAT_INT
                nb_try_to_sign=0U;
                fprintf(fp,"k=%u\n",k);
            #endif

            /* Compute xor(D_k,S_(k-1)) */
            add_gf2m(DR,sm,Hi);

            #if AFFINE_TRANSFORMATION_BY_t
                /* DR - t */
                add2_gf2m(DR,sk+ACCESS_VECTOR_t);
            #endif

            #if(HFEm&7)
                /* Clean the last char to compute rem_char (the last word is cleaned) */
                DR[NB_WORD_GF2m-1]&=HFE_MASKm;
                /* Save the last byte because we need to erase this value by 
                 * randombytes */
                #if(HFEn>HFEm)
                    rem_char=DR_cp[NB_BYTES_GFqm-1];
                #endif
            #endif

            #ifdef KAT_INT
                fprintBstr_KAT_INT(fp,"xor(Dk,S_(k-1)): ",(unsigned char*)DR,NB_BYTES_GFqm);
            #endif



            #if(HFEn>HFEm)
            /* Compute Dk||Rk: add random to have n bits, without erased the m 
             * bits */
                #if(HFEm&7)
                /* Generation of Rk */
                for(j=0;j<(NB_BYTES_GFqn-NB_BYTES_GFqm+1);++j)
                {
                    DR_cp[NB_BYTES_GFqm-1+j]=minus_vec[(k-1)*(NB_BYTES_GFqn-NB_BYTES_GFqm+1)+j];
                }
                /* Put HFEm&7 first bits to 0 */
                DR_cp[NB_BYTES_GFqm-1]&=~((1U<<(HFEm&7))-1);
                /* Store rem_char */
                DR_cp[NB_BYTES_GFqm-1]^=rem_char;
                #else
                /* Generation of Rk */
                for(j=0;j<(NB_BYTES_GFqn-NB_BYTES_GFqm);++j)
                {
                    DR_cp[NB_BYTES_GFqm+j]=minus_vec[(k-1)*(NB_BYTES_GFqn-NB_BYTES_GFqm)+j];
                }
                #endif

                /* To clean the last char (because of randombytes), the last word is 
                 * cleaned */
                #if (HFEn&7)
                    DR[NB_WORD_GFqn-1]&=HFE_MASKn;
                #endif
            #endif

            #ifdef KAT_INT
                ++nb_try_to_sign;
                fprintf(fp,"Try %u, ",nb_try_to_sign);
                fprintBstr_KAT_INT(fp,"Dk||Rk: ",(unsigned char*)DR,NB_BYTES_GFqn);
            #endif

            /* Compute Sk||Xk = Inv_p(Dk,Rk) */
            #if RIGHT_MULTIPLICATION_BY_T
                /* Firstly: compute c * T^(-1) */
                vecMatProductn_gf2(U,DR,sk+ACCESS_MATRIX_T);
            #else
                /* Firstly: compute T^(-1) * c */
                matVecProduct_gf2(U,sk+ACCESS_MATRIX_T,DR);
            #endif



            /* Secondly: find v with F_HFE(v) = U */

           #if(HFEv)
                /* Evaluation of the constant, quadratic map with v vinegars */
                evalMQSv_gf2(F,V,sk);

                #if (HFEDeg>1)
                    /* Evaluation of the linear terms, linear maps with v vinegars */
                    #if ENABLED_REMOVE_ODD_DEGREE
                    #if(HFEDegI==HFEDegJ)
                    for(i=0;i<=LOG_odd_degree;++i)
                    #elif(HFEDegI<=LOG_odd_degree)
                    for(i=0;i<=HFEDegI;++i)
                    #else
                    for(i=0;i<=(LOG_odd_degree+1);++i)
                    #endif
                    {
                        vecMatProductv_gf2(tmp_n,V,linear_coefs[i]+NB_WORD_GFqn);
                        add_gf2n(F+NB_WORD_GFqn*(((i*(i+1))>>1)+1),linear_coefs[i],tmp_n);
                    }
                    for(;i<=HFEDegI;++i)
                    {
                        vecMatProductv_gf2(tmp_n,V,linear_coefs[i]+NB_WORD_GFqn);
                        add_gf2n(F+NB_WORD_GFqn*(((i*(i-1))>>1)+2+LOG_odd_degree),linear_coefs[i],tmp_n);
                    }
                    #else
                    for(i=0;i<=HFEDegI;++i)
                    {
                        vecMatProductv_gf2(tmp_n,V,linear_coefs[i]+NB_WORD_GFqn);
                        add_gf2n(F+NB_WORD_GFqn*(((i*(i+1))>>1)+1),linear_coefs[i],tmp_n);
                    }
                    #endif
                #endif
            #endif

            nb_root=chooseRootHFE_gf2nx(DR,F_struct,U);
            if(!nb_root)
            {
                /* fail: retry */
                b=1;
                break;
            }
            if(nb_root<0)
            {
                /* Error from chooseRootHFE */
                #if HFEv
                    free(F);
                #endif
                return nb_root;
            }


            #if HFEv
                /* Add the v bits to DR */
                #if HFEnr
                    DR[NB_WORD_GFqn-1]^=V[0]<<HFEnr;
                    for(i=0;i<(NB_WORD_GFqv-1);++i)
                    {
                        DR[NB_WORD_GFqn+i]=(V[i]>>(64-HFEnr))^(V[i+1]<<HFEnr);
                    }
                    #if ((NB_WORD_GFqn+NB_WORD_GFqv) == NB_WORD_GF2nv)
                        DR[NB_WORD_GFqn+i]=V[i]>>(64-HFEnr);
                    #endif
                #else
                    for(i=0;i<NB_WORD_GFqv;++i)
                    {
                        DR[NB_WORD_GFqn+i]=V[i];
                    }
                #endif
            #endif

            /* Finally: compute Sk||Xk = v * S^(-1) */
            vecMatProductnv_gf2(sm,DR,sk+ACCESS_MATRIX_S);

            if(k!=NB_ITE)
            {
                #ifdef KAT_INT
                    fprintBstr_KAT_INT(fp,"Sk||Xk: ",(unsigned char*)sm,NB_BYTES_GFqnv);
                #endif

                #if HFEv
                    /* Update of the vinegar variables */
                    V+=NB_WORD_GFqv;
                #endif

                #if (HFEnv!=HFEm)
                    /* Store X1 in the signature */
                    index=NB_WORD_GF2nv+(NB_ITE-1-k)*NB_WORD_GF2nvm;
                    copy_gf2nvm(sm+index,sm+NB_WORD_GF2nv-NB_WORD_GF2nvm);
                    /* To put zeros at the beginning of the first word of X1 */
                    #if HFEmr
                        sm[index]&=HFE_MASKnvm;
                    #endif
                #endif

                /* Compute H2 = H(H1) */
                HASH((unsigned char*)Hi1,(unsigned char*)Hi,SIZE_DIGEST);

                /* Permutation of pointers */
                tmp=Hi1;
                Hi1=Hi;
                Hi=tmp;
            }
        }
    } while(b);

    #if HFEv
        free(F);
    #endif


    /* Copy the salt in the signature */
    for(k=0;k<SIZE_SALT_WORD;++k)
    {
        ((UINT*)sm8)[k]=Hd[SIZE_DIGEST_UINT+k];
    }
    sm8+=SIZE_SALT;


    #ifdef KAT_INT
        CLOSE_KAT_INT_FILE;
    #endif

    #if (NB_ITE==1)
        /* Take the (n+v) first bits */
        for(k=0;k<NB_BYTES_GFqnv;++k)
        {
            sm8[k]=((unsigned char*)sm)[k];
        }
    #else
        compress_signHFE(sm8,sm);
    #endif

    return 0;
}


#else

int PREFIX_NAME(signHFE_FeistelPatarin)(unsigned char* sm8, \
                                        const unsigned char* m, size_t len, \
                                        const UINT* sk)
{
    UINT sm[SIZE_SIGN_UNCOMPRESSED-SIZE_SALT_WORD]={0};

    static_vecnv_gf2 DR[NB_WORD_GF2nv];
    static_gf2n U[NB_WORD_GFqn] ALIGNED;
    UINT Hi_tab[SIZE_DIGEST_UINT],Hi1_tab[SIZE_DIGEST_UINT];
    UINT *tmp, *Hi=Hi_tab,*Hi1=Hi1_tab;
    unsigned int k;
    #if (HFEnv!=HFEm)
        unsigned int index;
    #endif
    #if(HFEn>HFEm)
        unsigned char* DR_cp=(unsigned char*)DR;
        #if(HFEm&7)
            unsigned char rem_char;
        #endif
    #endif

    int nb_root;
    complete_sparse_monic_gf2nx F_struct;

    #if(HFEv)
        UINT* F;
        unsigned int i;
        static_vecv_gf2 V[NB_WORD_GFqv];
        #if (HFEDeg>1)
            static_gf2n tmp_n[NB_WORD_GFqn] ALIGNED;
        #endif
    #endif

    #if(HFEv)
        #if (HFEDeg>1)
            const UINT* linear_coefs[HFEDegI+1];
            nb_root=precSignHFE(&F_struct,linear_coefs,sk);
        #else
            nb_root=precSignHFE(&F_struct,NULL,sk);
        #endif
        if(nb_root)
        {
            /* Error from malloc */
            return nb_root;
        }
        F=(UINT*)F_struct.poly;
    #else
        #if(HFEDeg!=1)
            initListDifferences_gf2nx(F_struct.L);
        #endif
        F_struct.poly=sk;
    #endif


    #ifdef KAT_INT
        OPEN_KAT_INT_FILE;
        fputs("Sign:\n",fp);
        unsigned int nb_try_to_sign;
    #endif


    /* Compute H1 = H(m) */
    HASH((unsigned char*)Hi,m,len);

    /* It is to initialize S0 to 0, because Sk||Xk is stored in sm */
    for(k=0;k<NB_WORD_GF2nv;++k)
    {
        sm[k]=0UL;
        DR[k]=0UL;
    }

    for(k=1;k<=NB_ITE;++k)
    {
        #ifdef KAT_INT
            nb_try_to_sign=0U;
            fprintf(fp,"k=%u\n",k);
        #endif

        /* Compute xor(D_k,S_(k-1)) */
        add_gf2m(DR,sm,Hi);

        #if AFFINE_TRANSFORMATION_BY_t
            /* DR - t */
            add2_gf2m(DR,sk+ACCESS_VECTOR_t);
        #endif

        #if(HFEm&7)
            /* Clean the last char to compute rem_char (the last word is cleaned) */
            DR[NB_WORD_GF2m-1]&=HFE_MASKm;
            /* Save the last byte because we need to erase this value by 
             * randombytes */
            #if(HFEn>HFEm)
                rem_char=DR_cp[NB_BYTES_GFqm-1];
            #endif
        #endif

        #ifdef KAT_INT
            fprintBstr_KAT_INT(fp,"xor(Dk,S_(k-1)): ",(unsigned char*)DR,NB_BYTES_GFqm);
        #endif

        #if GEN_MINUS_VINEGARS
        /* When the root finding fails, the minus and vinegars are regenerated */
        do
        {
        #endif
            #if(HFEn>HFEm)
            /* Compute Dk||Rk: add random to have n bits, without erased the m 
             * bits */
            #if(HFEm&7)
                /* Generation of Rk */
                randombytes(DR_cp+NB_BYTES_GFqm-1,
                           (unsigned long long)(NB_BYTES_GFqn-NB_BYTES_GFqm+1));
                /* Put HFEm&7 first bits to 0 */
                DR_cp[NB_BYTES_GFqm-1]&=~((1U<<(HFEm&7))-1);
                /* Store rem_char */
                DR_cp[NB_BYTES_GFqm-1]^=rem_char;
            #else
                /* Generation of Rk */
                randombytes(DR_cp+NB_BYTES_GFqm,
                        (unsigned long long)(NB_BYTES_GFqn-NB_BYTES_GFqm));
            #endif

            /* To clean the last char (because of randombytes), the last word is 
             * cleaned */
            #if (HFEn&7)
                DR[NB_WORD_GFqn-1]&=HFE_MASKn;
            #endif
            #endif

            #ifdef KAT_INT
                ++nb_try_to_sign;
                fprintf(fp,"Try %u, ",nb_try_to_sign);
                fprintBstr_KAT_INT(fp,"Dk||Rk: ",(unsigned char*)DR,NB_BYTES_GFqn);
            #endif

            /* Compute Sk||Xk = Inv_p(Dk,Rk) */
            #if RIGHT_MULTIPLICATION_BY_T
                /* Firstly: compute c * T^(-1) */
                vecMatProductn_gf2(U,DR,sk+ACCESS_MATRIX_T);
            #else
                /* Firstly: compute T^(-1) * c */
                matVecProduct_gf2(U,sk+ACCESS_MATRIX_T,DR);
            #endif

        #if (!GEN_MINUS_VINEGARS)
        /* When the root finding fails, only the vinegars are regenerated */
        do
        {
        #endif
            /* Secondly: find v with F_HFE(v) = U */

            /* Generation of vinegar variables: v bits */
           #if(HFEv)
                randombytes((unsigned char*)V,(unsigned long long)(NB_BYTES_GFqv));
                #if HFEvr8
                    /* Clean the last word */
                    V[NB_WORD_GFqv-1]&=HFE_MASKv;
                #endif

                /* Evaluation of the constant, quadratic map with v vinegars */
                evalMQSv_gf2(F,V,sk);

                #if (HFEDeg>1)
                    /* Evaluation of the linear terms, linear maps with v vinegars */
                    #if ENABLED_REMOVE_ODD_DEGREE
                    #if(HFEDegI==HFEDegJ)
                    for(i=0;i<=LOG_odd_degree;++i)
                    #elif(HFEDegI<=LOG_odd_degree)
                    for(i=0;i<=HFEDegI;++i)
                    #else
                    for(i=0;i<=(LOG_odd_degree+1);++i)
                    #endif
                    {
                        vecMatProductv_gf2(tmp_n,V,linear_coefs[i]+NB_WORD_GFqn);
                        add_gf2n(F+NB_WORD_GFqn*(((i*(i+1))>>1)+1),linear_coefs[i],tmp_n);
                    }
                    for(;i<=HFEDegI;++i)
                    {
                        vecMatProductv_gf2(tmp_n,V,linear_coefs[i]+NB_WORD_GFqn);
                        add_gf2n(F+NB_WORD_GFqn*(((i*(i-1))>>1)+2+LOG_odd_degree),linear_coefs[i],tmp_n);
                    }
                    #else
                    for(i=0;i<=HFEDegI;++i)
                    {
                        vecMatProductv_gf2(tmp_n,V,linear_coefs[i]+NB_WORD_GFqn);
                        add_gf2n(F+NB_WORD_GFqn*(((i*(i+1))>>1)+1),linear_coefs[i],tmp_n);
                    }
                    #endif
                #endif
            #endif

            nb_root=chooseRootHFE_gf2nx(DR,F_struct,U);
            if(!nb_root)
            {
                /* fail: retry with an other Rk */
                continue;
            }
            if(nb_root<0)
            {
                /* Error from chooseRootHFE */
                #if HFEv
                    free(F);
                #endif
                return nb_root;
            }

            break;
        } while(1);

        #if HFEv
            /* Add the v bits to DR */
            #if HFEnr
                DR[NB_WORD_GFqn-1]^=V[0]<<HFEnr;
                for(i=0;i<(NB_WORD_GFqv-1);++i)
                {
                    DR[NB_WORD_GFqn+i]=(V[i]>>(64-HFEnr))^(V[i+1]<<HFEnr);
                }
                #if ((NB_WORD_GFqn+NB_WORD_GFqv) == NB_WORD_GF2nv)
                    DR[NB_WORD_GFqn+i]=V[i]>>(64-HFEnr);
                #endif
            #else
                for(i=0;i<NB_WORD_GFqv;++i)
                {
                    DR[NB_WORD_GFqn+i]=V[i];
                }
            #endif
        #endif

        /* Finally: compute Sk||Xk = v * S^(-1) */
        vecMatProductnv_gf2(sm,DR,sk+ACCESS_MATRIX_S);

        if(k!=NB_ITE)
        {
            #ifdef KAT_INT
                fprintBstr_KAT_INT(fp,"Sk||Xk: ",(unsigned char*)sm,NB_BYTES_GFqnv);
            #endif

            #if (HFEnv!=HFEm)
                /* Store X1 in the signature */
                index=NB_WORD_GF2nv+(NB_ITE-1-k)*NB_WORD_GF2nvm;
                copy_gf2nvm(sm+index,sm+NB_WORD_GF2nv-NB_WORD_GF2nvm);
                /* To put zeros at the beginning of the first word of X1 */
                #if HFEmr
                    sm[index]&=HFE_MASKnvm;
                #endif
            #endif

            /* Compute H2 = H(H1) */
            HASH((unsigned char*)Hi1,(unsigned char*)Hi,SIZE_DIGEST);

            /* Permutation of pointers */
            tmp=Hi1;
            Hi1=Hi;
            Hi=tmp;
        }
    }

    #if HFEv
        free(F);
    #endif
    #ifdef KAT_INT
        CLOSE_KAT_INT_FILE;
    #endif

    #if (NB_ITE==1)
        /* Take the (n+v) first bits */
        for(k=0;k<NB_BYTES_GFqnv;++k)
        {
            sm8[k]=((unsigned char*)sm)[k];
        }
    #else
        compress_signHFE(sm8,sm);
    #endif

    return 0;
}

#endif




