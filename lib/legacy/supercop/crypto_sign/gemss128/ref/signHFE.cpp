#include "signHFE.hpp"

BEGIN_EXTERNC
    #include <libkeccak.a.headers/SimpleFIPS202.h>
END_EXTERNC
#include "add.h"
#include "init.h"
#include "randombytes.h"
#include "chooseRootHFE.hpp"

#if(HFEv)
    #include "evaluateMQS.hpp"
#endif

#include "convNTL.hpp"

#include <NTL/mat_GF2.h>
#include <NTL/mat_GF2E.h>

using namespace NTL;


/* Input:
    m a document to sign
    len the size of document (in byte)
    sk the concatenation of HFE polynom in GF(2^n), \
    S matrix (n+v)*(n+v) and T matrix n*n, in GF(2)

  Output:
    0 if the result is correct, ERROR_ALLOC for error from calloc functions
    sm the signature
*/
/* Format of signature: S_NB_ITE||X_NB_ITE||...||X1
    _ S_NB_ITE||X_NB_ITE: n+v bits
    _ X_(NB_ITE-1): (n+v-m) bits, the first word is padding with m%64 zero bits
      It is to simply the concatenation of S_(NB_ITE-1) and X_(NB_ITE-1)
      during the verification.
    _ It is the same format for X_(NB_ITE-2),...,X1
*/

int signHFE(UINT* sm, const unsigned char* m, size_t len, const UINT* sk)
{
    static_vecnv_gf2 DR[NB_WORD_GFqnv];
    static_gf2n U[NB_WORD_GFqn];
    UINT Hi_tab[SIZE_DIGEST_UINT],Hi1_tab[SIZE_DIGEST_UINT];
    UINT *tmp, *Hi=Hi_tab,*Hi1=Hi1_tab;
    unsigned int k,nb_root;
    #if (HFEnv!=HFEm)
        unsigned int index;
    #endif

    #if(HFEn>HFEm)
        unsigned char* DR_cp=(unsigned char*)DR;
        #if(HFEm&7)
            unsigned char rem_char;
        #endif
    #endif

    vec_GF2 c_vec,U_vec;
    mat_GF2 T_inv,S_inv;


    #if(HFEv)
        vec_GF2 v_vec;
        mat_GF2 V_lin;

        unsigned int i;
        static_vecv_gf2 V[NB_WORD_GFqv];
        UINT* F=MALLOC_HFEPOLY_EVAL;
        VERIFY_ALLOC_RET(F);

        #if (HFEDeg>1)
        static_gf2n tmp_n[NB_WORD_GFqn];
        const UINT* linear_coefs[HFEDegI+1];
        UINT *F_cp;
        const UINT *sk_cp;
        unsigned int j;

        /* X^(2^0) */
        linear_coefs[0]=sk+MQv_GFqn_SIZE;
        /* X^(2^1) */
        sk_cp=linear_coefs[0]+MLv_GFqn_SIZE;
        F_cp=F+2*NB_WORD_GFqn;
        for(i=0;i<HFEDegI;++i)
        {
            /* Copy i quadratic terms */
            for(j=0;j<i;++j)
            {
                /* X^(2^i + 2^j) */
                HFECOPY(F_cp,sk_cp);
                sk_cp+=NB_WORD_GFqn;
                F_cp+=NB_WORD_GFqn;
            }
            /* Store the address of X^(2^(i+1)) */
            linear_coefs[i+1]=sk_cp;
            /* Linear term is not copied */
            sk_cp+=MLv_GFqn_SIZE;
            F_cp+=NB_WORD_GFqn;
        }
        #if HFEDegJ
            /* X^(2^HFEDegI + 2^j) */
            for(j=0;j<HFEDegJ;++j)
            {
                HFECOPY(F_cp,sk_cp);
                sk_cp+=NB_WORD_GFqn;
                F_cp+=NB_WORD_GFqn;
            }
        #endif
        #endif
    #endif

    convUINTToNTLmatT_GF2(T_inv,sk+NB_UINT_HFEPOLY+MATRIXnv_SIZE);
    convUINTToNTLmatS_GF2(S_inv,sk+NB_UINT_HFEPOLY);

    #ifdef KAT_INT
        OPEN_KAT_INT_FILE;
        fputs("Sign:\n",fp);
        unsigned int nb_try_to_sign;
    #endif


    /* Compute H1 = H(m) */
    HASH((unsigned char*)Hi,m,len);

    /* It is to initialize S0 to 0, because Sk||Xk is stored in sm */
    for(k=0;k<NB_WORD_GFqnv;++k)
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
        HFEADDm(DR,sm,Hi);

        #if(HFEm&7)
            /* Clean the last char to compute rem_char (the last word is cleaned) */
            DR[NB_WORD_GFqm-1]&=HFE_MASKm;
            /* Save the last byte because we need to erase this value by 
             * randombytes */
            #if(HFEn>HFEm)
                rem_char=DR_cp[NB_BYTES_GFqm-1];
            #endif
        #endif

        #ifdef KAT_INT
            fprintBstr_KAT_INT(fp,"xor(Dk,S_(k-1)): ",(unsigned char*)DR,NB_BYTES_GFqm);
        #endif
        do 
        {
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

            /* Firstly: compute T^(-1) * c */
            convUINTToNTLvecn_GF2(c_vec,DR);
            mul(U_vec,T_inv,c_vec);

            /* Secondly: find v with F_HFE(v) = U */
            convNTLvecn_GF2ToUINT(U,U_vec);

            /* Generation of vinegar variables: v bits */
            #if(HFEv)
                randombytes((unsigned char*)V,(unsigned long long)(NB_BYTES_GFqv));
                #if HFEvr8
                    /* Clean the last word */
                    V[NB_WORD_GFqv-1]&=HFE_MASKv;
                #endif

                /* Evaluation of the constant, quadratic map with v vinegars */
                evaluateMQS(F,V,sk);

                #if (HFEDeg>1)
                    /* Evaluation of the linear terms, linear maps with v vinegars */
                    convUINTToNTLvecv_GF2(v_vec,V);

                    for(i=0;i<=HFEDegI;++i)
                    {
                        convUINTToNTLmatV_GF2(V_lin,linear_coefs[i]+NB_WORD_GFqn);
                        c_vec.SetLength(HFEn);
                        mul(c_vec,v_vec,V_lin);
                        c_vec.SetLength(HFEn);
                        convNTLvecn_GF2ToUINT(tmp_n,c_vec);
                        HFEADD(F+NB_WORD_GFqn*(((i*(i+1))>>1)+1),linear_coefs[i],tmp_n);
                    }
                #endif

                nb_root=chooseRootHFE(DR,F,U);
            #else
                nb_root=chooseRootHFE(DR,sk,U);
            #endif

            if(!nb_root)
            {
                /* fail: retry with an other Rk */
                continue;
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
                #if ((NB_WORD_GFqn+NB_WORD_GFqv) == NB_WORD_GFqnv)
                    DR[NB_WORD_GFqn+i]=V[i]>>(64-HFEnr);
                #endif
            #else
                for(i=0;i<NB_WORD_GFqv;++i)
                {
                    DR[NB_WORD_GFqn+i]=V[i];
                }
            #endif
        #endif
        convUINTToNTLvecnv_GF2(U_vec,DR);

        /* Finally: compute S1||X1 = v * S^(-1) */
        mul(c_vec,U_vec,S_inv);
        convNTLvecnv_GF2ToUINT(sm,c_vec);

        if(k!=NB_ITE)
        {
            #ifdef KAT_INT
                fprintBstr_KAT_INT(fp,"Sk||Xk: ",(unsigned char*)sm,NB_BYTES_GFqnv);
            #endif

            #if (HFEnv!=HFEm)
                /* Store X1 in the signature */
                index=NB_WORD_GFqnv+(NB_ITE-1-k)*NB_WORD_GFqnvm;
                HFECOPYnvm(sm+index,sm+NB_WORD_GFqnv-NB_WORD_GFqnvm);
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

    T_inv.kill();
    S_inv.kill();
    U_vec.kill();
    c_vec.kill();

    #if HFEv
        v_vec.kill();
        V_lin.kill();
        free(F);
    #endif
    #ifdef KAT_INT
        CLOSE_KAT_INT_FILE;
    #endif

    return 0;
}

