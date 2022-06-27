#include "evalMQShybrid_gf2.h"
#include "tools_gf2m.h"
#include "tools_gf2n.h"
#include "dotProduct_gf2.h"
#include "MQ_gf2.h"

#if HFEmq
    #include "evalMQSnocst_quo_gf2.h"
#endif

#if HFEmr
    #include "evalMQnocst_gf2.h"
#endif


/* Input: x a vector of n+v elements of GF(2)
          mq = (c,Q',Q_(HFEm-HFEmr),...,Q_(m-1))
          Q' is upper triangular of size (n+v)*(n+v) in GF(2^(m-mr))
          the HFEmr Qi are lower triangular of size (n+v)*(n+v) in GF(2)
          for each Qi, the rows are stored separately
          (we take new words for each new row)

   Output:
    res a vector of m elements of GF(2), res is the evaluation of mq in x

   This version is not in constant time.
*/
void PREFIX_NAME(evalMQShybrid_nocst_gf2_m)(vecm_gf2 res, cst_vecnv_gf2 x, const UINT* mq)
{
    #if HFEmq
        evalMQSnocst_quo_gf2(res,x,mq);
    #endif
    #if HFEmr
        unsigned int i;
        for(i=0;i<HFEmr;++i)
        {
            res[HFEmq]^=evalMQnocst_gf2(x,\
                        mq+ACCESS_last_equations+NB_WORD_ONE_EQUATION*i)<<i;
        }
    #endif
}


