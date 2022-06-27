#include "genSecretMQS_gf2.h"
#include <stdlib.h>
#include "tools_gf2n.h"
#include "arith_gf2n.h"
#include "gf2nx.h"

#if (PRECOMPUTE)
    /* Disabled for the submission */
    #include "../precompute_cbasis.h"
#else
    #include "genCanonicalBasis_gf2n.h"
#endif

/* Compute a secret MQ System from a HFE polynomial */
/* The complexity of the used algorithm is the best:
   O(n Log(D) * (n + v + Log(D))) multiplications in GF(2)[x]
   and O(n * (n + v + Log(D))) modular reductions.
*/


#if(HFEDegI!=HFEDegJ)
    #define SIZE_ROW (HFEDegI+1)
#else
    #define SIZE_ROW (HFEDegI+2)
#endif



#include "dotProduct_gf2n.h"

#if (HFEDeg>2)
static void special_buffer(UINT* buf, cst_sparse_monic_gf2nx F, cst_vec_gf2n alpha_vec)
{
    #if(HFEDeg!=3)
        cst_sparse_monic_gf2nx F_cp;
        unsigned int i,j;
    #endif
    unsigned int k;


    /* Special case: alpha^0 */
    #if(HFEDeg==3)
        /* Monic case */
        set1_gf2n(buf);
        buf+=NB_WORD_GFqn;
    #else
        /* F begins to X^3, the first "quadratic" term */
        F+=(NB_WORD_GFqn*(HFEv+1))<<1;

        F_cp=F;

        /* X^3 */
        #if ((!ENABLED_REMOVE_ODD_DEGREE)||(1<=LOG_odd_degree))
            copy_gf2n(buf,F);
            buf+=NB_WORD_GFqn;
        #endif

        /* X^5: we jump X^4 because it is linear */
        F_cp=F+NB_WORD_GFqn*(HFEv+2);
        /* A_i,j X^(2^i + 2^j) */
        #if ENABLED_REMOVE_ODD_DEGREE
        /* min(L,SIZE_ROW-1) */
        for(i=2;i<((LOG_odd_degree<(SIZE_ROW-1))?(LOG_odd_degree+1):(SIZE_ROW-1));++i)
        #else
        for(i=2;i<(SIZE_ROW-1);++i)
        #endif
        {
            /* j=0: A_i,0 */
            copy_gf2n(buf,F_cp);
            for(j=1;j<i;++j)
            {
                F_cp+=NB_WORD_GFqn;
                add2_gf2n(buf,F_cp);
            }
            buf+=NB_WORD_GFqn;
            /* To jump a linear term X^(2^i) */
            F_cp+=NB_WORD_GFqn*(HFEv+2);
        }

        #if ENABLED_REMOVE_ODD_DEGREE
        for(;i<(SIZE_ROW-1);++i)
        {
            /* j=0 is removed because the term is odd */
            /* j=1: A_i,1 */
            copy_gf2n(buf,F_cp);
            for(j=2;j<i;++j)
            {
                F_cp+=NB_WORD_GFqn;
                add2_gf2n(buf,F_cp);
            }
            buf+=NB_WORD_GFqn;
            /* To jump a linear term X^(2^i) */
            F_cp+=NB_WORD_GFqn*(HFEv+2);
        }
        #endif


        #if(HFEDegI!=HFEDegJ)
            /* Monic case */
            set1_gf2n(buf);
            #if ((!ENABLED_REMOVE_ODD_DEGREE)||(LOG_odd_degree>=HFEDegI))
            for(j=0;j<HFEDegJ;++j)
            #else
            for(j=1;j<HFEDegJ;++j)
            #endif
            {
                add2_gf2n(buf,F_cp);
                F_cp+=NB_WORD_GFqn;
            }
            buf+=NB_WORD_GFqn;
        #endif
    #endif


    /* Squares of (alpha^(k+1)) */
    for(k=0;k<(HFEn-1);++k)
    {
        #if(HFEDeg!=3)
        #if ((!ENABLED_REMOVE_ODD_DEGREE)||(1<=LOG_odd_degree))
        /* X^3: i=1,j=0 */
        mul_gf2n(buf,alpha_vec,F);
        buf+=NB_WORD_GFqn;
        #endif

        /* X^5: we jump X^4 because it is linear */
        F_cp=F+NB_WORD_GFqn*(HFEv+2);
        #if ENABLED_REMOVE_ODD_DEGREE
        /* min(L,SIZE_ROW-1) */
        for(i=2;i<((LOG_odd_degree<(SIZE_ROW-1))?(LOG_odd_degree+1):(SIZE_ROW-1));++i)
        #else
        /* A_i,j X^(2^i + 2^j) */
        for(i=2;i<(SIZE_ROW-1);++i)
        #endif
        {
            dotProduct_gf2n(buf,alpha_vec,F_cp,i);
            buf+=NB_WORD_GFqn;

            /* To jump quadratic terms + a linear term X^(2^i) */
            F_cp+=(i+HFEv+1)*NB_WORD_GFqn;
        }
        #endif

        #if ENABLED_REMOVE_ODD_DEGREE
        for(;i<(SIZE_ROW-1);++i)
        {
            dotProduct_gf2n(buf,alpha_vec+NB_WORD_GFqn,F_cp,i-1);
            buf+=NB_WORD_GFqn;

            /* To jump quadratic terms + a linear term X^(2^i) */
            F_cp+=(i+HFEv)*NB_WORD_GFqn;
        }
        #endif

        /* j=0: A_i,0 */
        #if (!HFEDegJ)
            /* Monic case */
            copy_gf2n(buf,alpha_vec);
            buf+=NB_WORD_GFqn;

            /* To change the row of alpha_vec */
            alpha_vec+=SIZE_ROW*NB_WORD_GFqn;

        #elif(HFEDegI!=HFEDegJ)
            #if ((!ENABLED_REMOVE_ODD_DEGREE)||(LOG_odd_degree>=HFEDegI))
                #if(HFEDegJ==1)
                    mul_gf2n(buf,alpha_vec,F_cp);
                #else
                    dotProduct_gf2n(buf,alpha_vec,F_cp,HFEDegJ);
                #endif
            #else
                #if(HFEDegJ>1)
                    dotProduct_gf2n(buf,alpha_vec+NB_WORD_GFqn,F_cp,HFEDegJ-1);
                #endif
            #endif
            /* j=HFEDegJ: monic case */
            alpha_vec+=HFEDegJ*NB_WORD_GFqn;
            add2_gf2n(buf,alpha_vec);
            /* To change the row of alpha_vec */
            alpha_vec+=(SIZE_ROW-HFEDegJ)*NB_WORD_GFqn;
            buf+=NB_WORD_GFqn;
        #else
            /* To change the row of alpha_vec */
            alpha_vec+=SIZE_ROW*NB_WORD_GFqn;
        #endif
    }
}
#endif



int PREFIX_NAME(genSecretMQS_gf2)(mqsnv_gf2n MQS, cst_sparse_monic_gf2nx F)
{
    cst_vec_gf2n a_vec_k;
    #if(HFEDeg<3)
    unsigned int k;
        #if ((HFEDeg==2)&&HFEv)
            unsigned int kp;
        #endif
    #else
    cst_vec_gf2n a_vec_kp,buf_k,buf_kp;
    cst_sparse_monic_gf2nx F_cp;
    static_gf2n tmp1[NB_WORD_MUL] ALIGNED, tmp2[NB_WORD_MUL] ALIGNED, tmp3[NB_WORD_GFqn] ALIGNED;
    unsigned int i,k,kp;
    #endif

    #if(HFEDeg>2)
        /* Vector with linear terms of F */
        UINT* F_lin;

        ALIGNED_CALLOC(F_lin,UINT*,(HFEDegI+1)*(HFEv+1)*NB_WORD_GFqn,sizeof(UINT));
        VERIFY_ALLOC_RET(F_lin);

        F_cp=F+MQv_GFqn_SIZE;

        #if ((!ENABLED_REMOVE_ODD_DEGREE)||(LOG_odd_degree>=HFEDegI))
        for(i=0;i<=HFEDegI;++i)
        #else
        for(i=0;i<=LOG_odd_degree;++i)
        #endif
        {
            for(k=0;k<=HFEv;++k)
            {
                copy_gf2n(F_lin+(k*(HFEDegI+1)+i)*NB_WORD_GFqn,F_cp);
                F_cp+=NB_WORD_GFqn;
            }
            F_cp+=i*NB_WORD_GFqn;
        }

        #if !((!ENABLED_REMOVE_ODD_DEGREE)||(LOG_odd_degree>=HFEDegI))
        for(;i<=HFEDegI;++i)
        {
            for(k=0;k<=HFEv;++k)
            {
                copy_gf2n(F_lin+(k*(HFEDegI+1)+i)*NB_WORD_GFqn,F_cp);
                F_cp+=NB_WORD_GFqn;
            }
            F_cp+=(i-1)*NB_WORD_GFqn;
        }
        #endif


    #endif

    /* Precompute alpha_vec is disabled in the submission */
    #if (!PRECOMPUTE)
        vec_gf2n alpha_vec;

        /* Matrix in GF(2^n) with HFEn-1 rows and (HFEDegI+1) columns */
        /* calloc is useful when it initialises a multiple precision element to 1UL */
        ALIGNED_CALLOC(alpha_vec,UINT*,SIZE_ROW*(HFEn-1)*NB_WORD_GFqn,sizeof(UINT));
        VERIFY_ALLOC_RET(alpha_vec);

        genCanonicalBasisVertical_gf2n(alpha_vec);
    #endif


    /* Constant: copy the first coefficient of F in MQS */
    copy_gf2n(MQS,F);
    F+=MQv_GFqn_SIZE;
    MQS+=NB_WORD_GFqn;


    /* Precompute an other table */
    #if(HFEDeg>2)
        UINT* buf;
        ALIGNED_CALLOC(buf,UINT*,HFEDegI*HFEn*NB_WORD_GFqn,sizeof(UINT));
        VERIFY_ALLOC_RET(buf);

        special_buffer(buf,F,alpha_vec);
    #endif



    /* Monic case */
    #if (HFEDeg==1)
        a_vec_k=alpha_vec;
        *MQS^=1;
        for(k=HFEnv;k>(HFEv+1);--k)
        {
            MQS+=k*NB_WORD_GFqn;
            copy_gf2n(MQS,a_vec_k);
            a_vec_k+=NB_WORD_GFqn;
        }
    #elif (HFEDeg==2)
        /* x_0: sum of coefficients of F excepted the constant */
        copy_gf2n(MQS,F);
        /* Monic case */
        *MQS^=1;
        MQS+=HFEn*NB_WORD_GFqn;

        /* Vinegar case */
        #if HFEv
            for(kp=1;kp<=HFEv;++kp)
            {
                copy_gf2n(MQS,F+kp*NB_WORD_GFqn);
                MQS+=NB_WORD_GFqn;
            }
        #endif

        a_vec_k=alpha_vec;
        /* Compute the term x_k */
        for(k=1;k<HFEn;++k)
        {
            /* dot_product(a_vec_k,buf_k) */
            mul_gf2n(MQS,F,a_vec_k);
            a_vec_k+=NB_WORD_GFqn;
            add2_gf2n(MQS,a_vec_k);

            MQS+=(HFEn-k)*NB_WORD_GFqn;

            /* Vinegar case */
            #if HFEv
                a_vec_k-=NB_WORD_GFqn;
                for(kp=1;kp<=HFEv;++kp)
                {
                    mul_gf2n(MQS,F+kp*NB_WORD_GFqn,a_vec_k);
                    MQS+=NB_WORD_GFqn;
                }
                a_vec_k+=(NB_WORD_GFqn<<1);
            #else
                a_vec_k+=NB_WORD_GFqn;
            #endif
        }
    #else
        /* k=0 */
        buf_k=buf;
        /* kp=0 */
        buf_kp=buf;

        /* x_0*x_0: quadratic terms of F */
        /* i=0 */
        copy_gf2n(MQS,buf_kp);
        buf_kp+=NB_WORD_GFqn;
        for(i=1;i<HFEDegI;++i)
        {
            add2_gf2n(MQS,buf_kp);
            buf_kp+=NB_WORD_GFqn;
        }
        /* At this step, buf_kp corresponds to kp=1 */

        /* x_0: linear terms of F */
        #if(HFEDegI==HFEDegJ)
            /* Monic case */
            *MQS^=1UL;
        #endif
        F_cp=F_lin;
        /* X^(2^i) */
        for(i=0;i<=HFEDegI;++i)
        {
            /* Next linear term of F: X^(2^i) */            
            add2_gf2n(MQS,F_cp);
            F_cp+=NB_WORD_GFqn;
        }
        MQS+=NB_WORD_GFqn;

        /* kp=1 (because kp=0 is not stored, it is just (1,1,1,...,1) */
        /* +NB_WORD_GFqn to jump (alpha^kp)^(2^0) */
        a_vec_kp=alpha_vec+NB_WORD_GFqn;


        /* k=0: x_0 x_kp */
        for(kp=1;kp<HFEn;++kp)
        {
            /* dot_product(a_vec_kp, buf_k) */
            #if (HFEDegI==1)
                mul_gf2n(MQS,a_vec_kp,buf_k);
            #else
                dotProduct_gf2n(MQS,a_vec_kp,buf_k,HFEDegI);
            #endif
            a_vec_kp+=SIZE_ROW*NB_WORD_GFqn;

            /* dot_product(a_vec_k=(1,1,...,1) , buf_kp) */
            for(i=0;i<HFEDegI;++i)
            {
                add2_gf2n(MQS,buf_kp);
                buf_kp+=NB_WORD_GFqn;
            }

            MQS+=NB_WORD_GFqn;
        }

        /* Vinegar variables */
        for(;kp<HFEnv;++kp)
        {
            copy_gf2n(MQS,F_cp);
            F_cp+=NB_WORD_GFqn;
            for(i=1;i<=HFEDegI;++i)
            {
                add2_gf2n(MQS,F_cp);
                F_cp+=NB_WORD_GFqn;
            }
            MQS+=NB_WORD_GFqn;
        }


        /* k=0 becomes k=1 */
        /* +NB_WORD_GFqn to jump (alpha^k)^(2^0) */
        a_vec_k=alpha_vec+NB_WORD_GFqn;

        /* Compute the term x_k x_kp */
        for(k=1;k<HFEn;++k)
        {
            /* k=0 becomes k=1 */
            buf_k+=HFEDegI*NB_WORD_GFqn;

            /* kp=k: x_k + x_k*x_k */
            a_vec_kp=a_vec_k;
            buf_kp=buf_k;

            /* Term X^(2^0) of F */
            mul_gf2x(tmp1,F_lin,a_vec_kp-NB_WORD_GFqn);

            /* dot_product(a_vec_k,buf_k) */
            /* i=0 */
            for(i=1;i<=HFEDegI;++i)
            {
                /* Next linear term of F: X^(2^i) */
                add_gf2n(tmp3,buf_kp,F_lin+i*NB_WORD_GFqn);
                mul_gf2x(tmp2,tmp3,a_vec_kp);
                add2mul_gf2n(tmp1,tmp2);
                buf_kp+=NB_WORD_GFqn;
                a_vec_kp+=NB_WORD_GFqn;
            }

            /* Monic case */
            #if(HFEDegI==HFEDegJ)
                add2_gf2n(tmp1,a_vec_kp);
                a_vec_kp+=NB_WORD_GFqn<<1;
            #else
                /* To jump (alpha^kp)^(2^0) */
                a_vec_kp+=NB_WORD_GFqn;
            #endif

            rem_gf2n(MQS,tmp1);
            MQS+=NB_WORD_GFqn;


            /* x_k*x_kp */
            for(kp=k+1;kp<HFEn;++kp)
            {
                doubleDotProduct_gf2n(MQS,a_vec_kp,buf_k,a_vec_k,buf_kp,HFEDegI);
                a_vec_kp+=SIZE_ROW*NB_WORD_GFqn;
                buf_kp+=HFEDegI*NB_WORD_GFqn;
                MQS+=NB_WORD_GFqn;
            }

            /* Vinegar variables */
            F_cp=F_lin;
            a_vec_k-=NB_WORD_GFqn;
            for(;kp<HFEnv;++kp)
            {
                F_cp+=(HFEDegI+1)*NB_WORD_GFqn;
                dotProduct_gf2n(MQS,a_vec_k,F_cp,HFEDegI+1);
                MQS+=NB_WORD_GFqn;
            }
            a_vec_k+=NB_WORD_GFqn;

            /* k becomes k+1 */
            a_vec_k+=SIZE_ROW*NB_WORD_GFqn;
        }
        free(buf);
        free(F_lin);
    #endif

    #if (!PRECOMPUTE)
        free(alpha_vec);
    #endif

    /* MQS with v vinegar variables */
    #if (HFEv)
        F-=MQv_GFqn_SIZE;
        #if(HFEDeg==1)
            MQS+=(HFEv+1)*NB_WORD_GFqn;
        #endif

        /* Copy the linear and quadratic terms of the constant in GF(2^n)[y1,...,yv] */
        for(k=1;k<NB_MONOM_VINEGAR;++k)
        {
            F+=NB_WORD_GFqn;
            copy_gf2n(MQS,F);
            MQS+=NB_WORD_GFqn;
        }
    #endif

    return 0;
}


