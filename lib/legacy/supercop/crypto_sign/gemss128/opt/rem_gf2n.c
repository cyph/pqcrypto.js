#include "rem_gf2n.h"
#include "rem_gf2x.h"


/***********************************************************************/
/***********************************************************************/
/************************ Without SIMD version *************************/
/***********************************************************************/
/***********************************************************************/



#if (NB_WORD_GFqn==1)
    #if (HFEn<33)
        uint64_t PREFIX_NAME(rem_noSIMD_gf2n)(uint64_t Pol)
        {
            uint64_t P,Q;
            #if(K3!=1)
                uint64_t R;
            #endif
            REM_GF2N(P,Pol,Q,R);
            return P;
        }
    #else
        #if HFEnr
            void PREFIX_NAME(rem_noSIMD_gf2n)(uint64_t P[NB_WORD_GFqn], const uint64_t Pol[NB_WORD_MUL])
            {
                uint64_t Q,R;
                REM_GF2N(*P,Pol,Q,R);
            }
        #else
            /* HFEn == 64 */
            void PREFIX_NAME(rem_noSIMD_gf2n)(uint64_t P[NB_WORD_GFqn], const uint64_t Pol[NB_WORD_MUL])
            {
                uint64_t R;
                REM_GF2N(*P,Pol,,R);
            }
        #endif
    #endif
#else
    #if HFEnr
        void PREFIX_NAME(rem_noSIMD_gf2n)(uint64_t P[NB_WORD_GFqn], const uint64_t Pol[NB_WORD_MUL])
        {
            uint64_t Q[NB_WORD_GFqn] ALIGNED;
            #if ((HFEn!=312)&&(HFEn!=402)&&(HFEn!=544))
                uint64_t R;
            #endif
            REM_GF2N(P,Pol,Q,R);
        }
    #else
        void PREFIX_NAME(rem_noSIMD_gf2n)(uint64_t P[NB_WORD_GFqn], const uint64_t Pol[NB_WORD_MUL])
        {
            uint64_t R;
            REM_GF2N(P,Pol,,R);
        }
    #endif
#endif


