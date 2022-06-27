#include "genSecretMQS.hpp"
#if(HFEv)
    #include <NTL/GF2X.h>
#endif


/* Compute a secret MQ System from a HFE polynom */


#define QUADRATIC_CASE(NB_IT) \
    /* 2^i */\
    exp_i=1U<<i;\
    /* Quadratic cases : j!=i */\
    for(;j<NB_IT;++j)\
    {\
        /* 2^j */\
        exp_j=1U<<j;\
        /* for each monom x_i2 * x_j2 */\
        for(i2=0;i2<HFEn;++i2)\
        {\
            /* (theta^i2)^(2^i) */\
            power(tmp_theta_i2,theta_vec[i2],exp_i);\
            /* ((theta^i2)^(2^i)) * coef of X^(2^i + 2^j) */\
            mul(tmp_coef_i2,tmp_theta_i2,coeff(F,exp_i+exp_j));\
            for(j2=0;j2<HFEn;++j2)\
            {\
                /* (theta^j2)^(2^j) */\
                power(tmp_theta_j2,theta_vec[j2],exp_j);\
                /* (theta^j2)^(2^j) * (((theta^i2)^(2^i)) * coef of X^(2^i + 2^j)) */\
                mul(tmp_coef_j2,tmp_theta_j2,tmp_coef_i2);\
                /* coefficient of x_i2*x_j2 */\
                add(Q[i2][j2],Q[i2][j2],tmp_coef_j2);\
            }\
        }\
    }


/* Just replace coeff(F,exp_i+exp_j) from QUADRATIC_CASE by 1 for j=HFEnr*/
#define QUADRATIC_MONIC_CASE \
    /* 2^j */\
    exp_j=1U<<j;\
    /* for each monom x_i2 * x_j2 */\
    for(i2=0;i2<HFEn;++i2)\
    {\
        /* (theta^i2)^(2^i) */\
        power(tmp_theta_i2,theta_vec[i2],exp_i);\
        /* ((theta^i2)^(2^i)) * 1 */\
        for(j2=0;j2<HFEn;++j2)\
        {\
            /* (theta^j2)^(2^j) */\
            power(tmp_theta_j2,theta_vec[j2],exp_j);\
            /* (theta^j2)^(2^j) * (((theta^i2)^(2^i)) * 1 */\
            mul(tmp_coef_j2,tmp_theta_j2,tmp_theta_i2);\
            /* coefficient of x_i2*x_j2 */\
            add(Q[i2][j2],Q[i2][j2],tmp_coef_j2);\
        }\
    }


#if HFEv
    #define LINEAR_VCASE \
        for(j=0;j<HFEv;++j)\
        {\
            GF2XFromBytes(monomX,(unsigned char*)(V_lin+j*NB_WORD_GFqn),(long)NB_BYTES_GFqn);\
            conv(monom,monomX);\
            /* x_i2 x_(n+j) with x_(n+j) the jth vinegar variables */\
            mul(tmp_coef_i2,tmp_theta_i2,monom);\
            add(Q[i2][HFEn+j],Q[i2][HFEn+j],tmp_coef_i2);\
        }
#else
    #define LINEAR_VCASE
#endif


#define LINEAR_CASE \
    /* 2^(i+1) */\
    exp_i=2U<<i;\
    /* Linear case : j==i */\
    /* for each monom x_i2*x_i2 = x_i2 */\
    for(i2=0;i2<HFEn;++i2)\
    {\
        /* (theta^i2)^(2^(i+1)) */\
        power(tmp_theta_i2,theta_vec[i2],exp_i);\
        /* (theta^i2)^(2^(i+1)) * coef of X^(2^(i+1)) */\
        mul(tmp_coef_i2,tmp_theta_i2,coeff(F,exp_i));\
        /* coefficient of x_i2^2 = x_i2 */\
        add(Q[i2][i2],Q[i2][i2],tmp_coef_i2);\
        LINEAR_VCASE;\
    }


/* Just replace coeff(F,exp_i) from LINEAR_CASE by 1 */
#define LINEAR_MONIC_CASE \
    /* 2^(i+1) */\
    exp_i=2U<<i;\
    /* Linear case : j==i */\
    /* for each monom x_i2*x_i2 = x_i2 */\
    for(i2=0;i2<HFEn;++i2)\
    {\
        /* (theta^i2)^(2^(i+1)) */\
        power(tmp_theta_i2,theta_vec[i2],exp_i);\
        /* (theta^i2)^(2^(i+1)) * 1 */\
        /* coefficient of x_i2^2 = x_i2 */\
        add(Q[i2][i2],Q[i2][i2],tmp_theta_i2);\
    }


/* Input:
    F a HFE polynomial in GF(2^n)[X,x_(n+1),...,x_(n+v)]

  Output:
    MQS a MQ system with n equations in GF(2)[x1,...,x_(n+v)]
    MQS = Q, the quadratic matrix of a MQ system in GF(2^n)[x1,...,x_(n+v)]
    the constant is the constant coefficient of F
    return ERROR_ALLOC if the allocation fails, 0 else
*/
void genSecretMQS(mat_GF2E& Q, const GF2EX& F, const UINT* F2)
{
    /* Vector of theta^i: theta_vec[i] = theta_(i+1) = alpha^i */
    vec_GF2E theta_vec;
    GF2E tmp_theta_i2,tmp_theta_j2,tmp_coef_i2,tmp_coef_j2;
    unsigned int i,i2;
    #if(HFEDeg!=1)
        unsigned int j,j2,exp_i,exp_j;
    #endif
    #if(HFEv)
        const UINT *F2_cp=F2;
        GF2X monomX;
        #if(HFEDeg!=1)
            const UINT *V_lin=F2+MQv_GFqn_SIZE+NB_WORD_GFqn;
            GF2E monom;
            /* Number of quadratic term +1 between two linear terms */
            unsigned int nb_quad=1;
        #endif
    #endif

    /* Generation of theta_vec the vector of canonical basis of GF(2^n) */
    theta_vec.SetLength(HFEn);
    /* Initialise theta_1 to 1 */
    set(theta_vec[0]);

    #if (HFEn!=1)
        /* theta_vec[i] = alpha^i */
        for(i=1;i<HFEn;++i)
        {
            conv(theta_vec[i],GF2X(i,1));
        }
    #endif

    Q.SetDims(HFEnv,HFEnv);

    /* The case X^0 is just the coef of X^0 of F, so it is useless to return the constant of MQ System */
    #if (HFEv)
        /* Copy the linear and quadratic terms of the constant in GF(2^n)[y1,...,yv] */
        for(i=HFEn;i<HFEnv;++i)
        {
            for(i2=i;i2<HFEnv;++i2)
            {
                F2_cp+=NB_WORD_GFqn;
                GF2XFromBytes(monomX,(unsigned char*)F2_cp,(long)NB_BYTES_GFqn);
                conv(Q[i][i2],monomX);
            }
        }
    #endif

    /* X^1 */
    /* Linear case */
    /* for each monom x_i2*x_i2 = x_i2 */
    for(i2=0;i2<HFEn;++i2)
    {
        #if(HFEDeg==1)
            /* monic case */
            /* (theta^i2)^(2^0) * 1 */
            /* coefficient of x_i2^2 = x_i2 */
            add(Q[i2][i2],Q[i2][i2],theta_vec[i2]);
        #else
            /* (theta^i2)^(2^0) * coef of X^1 */
            mul(tmp_coef_i2,theta_vec[i2],coeff(F,1));
            /* coefficient of x_i2^2 = x_i2 */
            add(Q[i2][i2],Q[i2][i2],tmp_coef_i2);
            #if HFEv
                for(i=0;i<HFEv;++i)
                {
                    GF2XFromBytes(monomX,(unsigned char*)(V_lin+i*NB_WORD_GFqn),(long)NB_BYTES_GFqn);
                    conv(monom,monomX);
                    /* x_i2 x_(n+i) with x_(n+i) the ith vinegar variables */
                    mul(Q[i2][HFEn+i],theta_vec[i2],monom);
                }
            #endif
        #endif
    }
    #if(HFEDeg!=1)
        #if HFEv
            V_lin+=MLv_GFqn_SIZE;
        #endif

        /* for each monom X^(2^i + 2^j) of univariate HFE polynom */
        for(i=0;i<HFEDegI;++i)
        {
            #if ENABLED_REMOVE_ODD_DEGREE
                j=(((1U<<i)+1U)<=HFE_odd_degree)?0:1;
            #else
                j=0;
            #endif
            QUADRATIC_CASE(i);
            LINEAR_CASE;
            #if HFEv
                V_lin+=MLv_GFqn_SIZE+nb_quad*NB_WORD_GFqn;
                #if ENABLED_REMOVE_ODD_DEGREE
                    /* If i == (LOG_odd_degree-1), the gap does not change */
                    if(i!=(LOG_odd_degree-1))
                    {
                        ++nb_quad;
                    }
                #else
                    ++nb_quad;
                #endif
            #endif
        }

        /* X^(2^HFEDegI + 2^j), j<HFEDegJ */
        #if ENABLED_REMOVE_ODD_DEGREE
            j=(((1U<<i)+1U)<=HFE_odd_degree)?0:1;
        #else
            j=0;
        #endif
        QUADRATIC_CASE(HFEDegJ);

        /* j = HFEDegJ */
        /* It is the leading term and F is monic, so the coefficient is 1 */
        #if(HFEDegI!=HFEDegJ)
            QUADRATIC_MONIC_CASE;
        #else
            LINEAR_MONIC_CASE;
        #endif

    #endif

    #if(HFEv)
        monomX.kill();
    #endif
    theta_vec.kill();
    clear(tmp_theta_i2);
    clear(tmp_theta_j2);
    clear(tmp_coef_i2);
    clear(tmp_coef_j2);
}

