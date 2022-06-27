#include "inv_gf2n.h"
#include "tools_gf2n.h"
#include "mul_gf2x.h"
#include "rem_gf2x.h"
#include "rem_gf2n.h"

#include "arith_gf2n.h"
#include "ITMIA_addchains.h"



/* Extended Euclidean Algorithm */


/* It is MSB_MP is assuming U!=0 */
/* Requirement: U!=0 */
#define HFE_MSB(res,U,i,j) \
    i=NB_WORD_GFqn-1;\
    /* Search the first word different from zero */\
    while(!U[i]) \
    {\
        --i;\
    }\
    /* Search the MSB of one word */\
    MSB_SP(res,U[i],j);\
    res^=i<<6;


#define DIV_GF2N(a,msb_a,b,msb_b,q) \
    set0_gf2n(q);\
    while(msb_a>=msb_b)\
    {\
        x=msb_a-msb_b;\
        rem_x=x&63;\
        q[x>>6]^=1UL<<rem_x;\
\
        if(!(x&63))\
        {\
            x>>=6;\
            for(j=0;j<=(msb_b>>6);++j)\
            {\
                a[j+x]^=b[j];\
            }\
        } else\
        {\
            x>>=6;\
            rem_x64=(64-rem_x);\
            /* j=0 */\
            a[x]^=b[0]<<rem_x;\
            for(j=1;j<=(msb_b>>6);++j)\
            {\
                a[j+x]^=(b[j]<<rem_x)^(b[j-1]>>rem_x64);\
            }\
            if((j+x)==(msb_a>>6))\
            {\
                a[msb_a>>6]^=b[j-1]>>rem_x64;\
            }\
        }\
\
        --msb_a;\
        i=msb_a>>6;\
        /* Search the first word different from zero */\
        while(!a[i]) \
        {\
            --i;\
        }\
        j=(i==(msb_a>>6))?msb_a&63:63;\
        while(!((a[i]>>j)&1))\
        {\
            --j;\
        }\
        msb_a=(i<<6)^j;\
    }



#define DIV_BEZOUT_GF2N(a,msb_a,b,msb_b,u2,v2) \
    while(msb_a>=msb_b)\
    {\
        i=msb_a-msb_b;\
        x=i>>6;\
\
        if(!(i&63))\
        {\
            for(j=0;j<=(msb_b>>6);++j)\
            {\
                a[j+x]^=b[j];\
            }\
\
            for(j=x;j<NB_WORD_GFqn;++j)\
            {\
                u2[j]^=v2[j-x];\
            }\
        } else\
        {\
            rem_x=i&63;\
            rem_x64=(64-rem_x);\
            /* j=0 */\
            a[x]^=b[0]<<rem_x;\
            for(j=1;j<=(msb_b>>6);++j)\
            {\
                a[j+x]^=(b[j]<<rem_x)^(b[j-1]>>rem_x64);\
            }\
            if((j+x)==(msb_a>>6))\
            {\
                a[msb_a>>6]^=b[j-1]>>rem_x64;\
            }\
\
            u2[x]^=v2[0]<<rem_x;\
            for(j=x+1;j<NB_WORD_GFqn;++j)\
            {\
                u2[j]^=(v2[j-x]<<rem_x)^(v2[j-x-1]>>rem_x64);\
            }\
        }\
\
        --msb_a;\
        i=msb_a>>6;\
        /* Search the first word different from zero */\
        while(!a[i]) \
        {\
            --i;\
        }\
        j=(i==(msb_a>>6))?msb_a&63:63;\
        while(!((a[i]>>j)&1))\
        {\
            --j;\
        }\
        msb_a=(i<<6)^j;\
    }




#if (HFEn&63)
void PREFIX_NAME(inv_EEA_gf2n)(uint64_t v2[NB_WORD_GFqn], const uint64_t A[NB_WORD_GFqn])
{
    static_gf2n a[NB_WORD_GFqn],b[HFEnq+1]={0},u2[NB_WORD_GFqn];
    unsigned int i,j,msb_a,msb_b,x,rem_x,rem_x64;

    HFE_MSB(msb_a,A,i,j);
    if(!msb_a)
    {
        /* A == 1 so A^(-1) == 1 */
        set1_gf2n(v2);
        return;
    }

    copy_gf2n(a,A);
    set1_gf2n(u2);
    msb_b=HFEn;

    #if (K3>63)
        b[0]=1UL;
        b[K3>>6]=(1UL<<(K3&63U));
        #ifdef __PENTANOMHFE__
            #if ((K2<64)&&(K1<64))
                b[0]^=(1UL<<K2)^(1UL<<K1);
            #else
                b[K2>>6]^=(1UL<<(K2&63U));
                b[K1>>6]^=(1UL<<(K1&63U));
            #endif
        #endif
    #else
        #ifdef __TRINOMHFE__
            b[0]=(1UL<<K3)^1UL;
        #else
            b[0]=(1UL<<K3)^(1UL<<K2)^(1UL<<K1)^1UL;
        #endif
    #endif

    #if (((K3>63)&&((K3>>6)==HFEnq))||(!HFEnq))
        b[HFEnq]^=(1UL<<HFEnr);
    #else
        b[HFEnq]=(1UL<<HFEnr);
    #endif

    DIV_GF2N(b,msb_b,a,msb_a,v2);

    /* while b!=1 */
    while(msb_b)
    {
        DIV_BEZOUT_GF2N(a,msb_a,b,msb_b,u2,v2);

        if(msb_a)
        {
            DIV_BEZOUT_GF2N(b,msb_b,a,msb_a,v2,u2);
        } else
        {
            copy_gf2n(v2,u2);
            break;
        }
    }
}
#endif




/* Fermat */



/* Itoh-Tsujii is used to compute modular inverse */
#ifdef ITOH


#define HFEn1 (HFEn-1)



#if (defined(ENABLED_PCLMUL)&&(NB_WORD_GFqn<10))


#if (HFEn<33)
    #define rem_function REM_GF2N(*r,*r,Q,R)
#elif(NB_WORD_GFqn==1)
    #define rem_function REM_GF2N(*r,r,Q,R)
#else
    #define rem_function REM_GF2N(r,r,Q,R)
#endif


#if (HFEn<33)

    #if (HFEn<5)
        #define INIT_REGISTERS \
            __m128i r_128,a_128;
    #else
        #define INIT_REGISTERS \
            __m128i r_128,r_128_copy,a_128;
    #endif
    #define HFEINIT_(r_128,r2_128,r3_128,r4_128,r5_128,r) HFEINIT(r_128,r)
    #define SQR_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,ra) \
             (*(r))=SQR_GF2X(r_128)
    #define MUL_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,a_128,a2_128,a3_128,a4_128,a5_128,ra,ra2,sum,res1,res2) \
             (*(r))=MUL_GF2X(r_128,a_128)

#elif (NB_WORD_GFqn==1)
    #define INIT_REGISTERS \
                __m128i r_128,r_128_copy,a_128,ra;
    #define HFEINIT_(r_128,r2_128,r3_128,r4_128,r5_128,r) HFEINIT(r_128,r)
    #define SQR_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,ra) SQR_GF2X(r,r_128,ra,0)
    #define MUL_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,a_128,a2_128,a3_128,a4_128,a5_128,ra,ra2,sum,res1,res2) \
             MUL_GF2X(r,r_128,a_128,ra,0)

#elif (NB_WORD_GFqn==2)
    #define INIT_REGISTERS \
                __m128i r_128,r_128_copy,a_128,ra,ra2,sum,res1,res2;
    #define HFEINIT_(r_128,r2_128,r3_128,r4_128,r5_128,r) HFEINIT(r_128,r)
    #define SQR_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,ra) SQR_GF2X(r,r_128,ra)
    #define MUL_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,a_128,a2_128,a3_128,a4_128,a5_128,ra,ra2,sum,res1,res2) \
             MUL_GF2X(r,r_128,a_128,ra,ra2,sum,res1,res2)

#elif ((NB_WORD_GFqn==3)||(NB_WORD_GFqn==4))
    #define INIT_REGISTERS \
                __m128i r_128,r2_128,r_128_copy,r2_128_copy,a_128,a2_128,ra,ra2,sum,res1,res2;
    #define HFEINIT_(r_128,r2_128,r3_128,r4_128,r5_128,r) HFEINIT(r_128,r2_128,r)
    #define SQR_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,ra) SQR_GF2X(r,r_128,r2_128,ra)
    #define MUL_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,a_128,a2_128,a3_128,a4_128,a5_128,ra,ra2,sum,res1,res2) \
             MUL_GF2X(r,r_128,r2_128,a_128,a2_128,ra,ra2,sum,res1,res2)

#elif ((NB_WORD_GFqn==5)||(NB_WORD_GFqn==6))
    #define INIT_REGISTERS \
            __m128i r_128,r2_128,r3_128,r_128_copy,r2_128_copy,r3_128_copy,a_128,a2_128,a3_128,ra,ra2,sum,res1,res2;
    #define HFEINIT_(r_128,r2_128,r3_128,r4_128,r5_128,r) HFEINIT(r_128,r2_128,r3_128,r)
    #define SQR_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,ra) SQR_GF2X(r,r_128,r2_128,r3_128,ra)
    #define MUL_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,a_128,a2_128,a3_128,a4_128,a5_128,ra,ra2,sum,res1,res2) \
             MUL_GF2X(r,r_128,r2_128,r3_128,a_128,a2_128,a3_128,ra,ra2,sum,res1,res2)

#elif ((NB_WORD_GFqn==7)||(NB_WORD_GFqn==8))
    #define INIT_REGISTERS \
            __m128i r_128,r2_128,r3_128,r4_128,r_128_copy,r2_128_copy,r3_128_copy,r4_128_copy,a_128,a2_128,a3_128,a4_128,ra,ra2,sum,res1,res2;
    #define HFEINIT_(r_128,r2_128,r3_128,r4_128,r5_128,r) HFEINIT(r_128,r2_128,r3_128,r4_128,r)
    #define SQR_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,ra) SQR_GF2X(r,r_128,r2_128,r3_128,r4_128,ra)
    #define MUL_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,a_128,a2_128,a3_128,a4_128,a5_128,ra,ra2,sum,res1,res2) \
             MUL_GF2X(r,r_128,r2_128,r3_128,r4_128,a_128,a2_128,a3_128,a4_128,ra,ra2,sum,res1,res2)

#elif (NB_WORD_GFqn==9)
    #define INIT_REGISTERS \
            __m128i r_128,r2_128,r3_128,r4_128,r5_128,r_128_copy,r2_128_copy,r3_128_copy,r4_128_copy,r5_128_copy,a_128,a2_128,a3_128,a4_128,a5_128,ra,ra2,sum,res1,res2;
    #define HFEINIT_(r_128,r2_128,r3_128,r4_128,r5_128,r) HFEINIT(r_128,r2_128,r3_128,r4_128,r5_128,r)
    #define SQR_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,ra) SQR_GF2X(r,r_128,r2_128,r3_128,r4_128,r5_128,ra)
    #define MUL_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,a_128,a2_128,a3_128,a4_128,a5_128,ra,ra2,sum,res1,res2) \
             MUL_GF2X(r,r_128,r2_128,r3_128,r4_128,r5_128,a_128,a2_128,a3_128,a4_128,a5_128,ra,ra2,sum,res1,res2)


#endif

#define SQR_GF2N_(r,r_128,r2_128,r3_128,r4_128,r5_128,ra,rem_function) \
    HFEINIT_(r_128,r2_128,r3_128,r4_128,r5_128,r);\
    SQR_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,ra); \
    rem_function;

#define MUL_GF2N_(r,r_128,r2_128,r3_128,r4_128,r5_128,a_128,a2_128,a3_128,a4_128,a5_128,ra,ra2,sum,res1,res2,rem_function) \
    HFEINIT_(r_128,r2_128,r3_128,r4_128,r5_128,r);\
    MUL_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,a_128,a2_128,a3_128,a4_128,a5_128,ra,ra2,sum,res1,res2); \
    rem_function;

/* Take in input HFEn1>>i (i the current bit of HFEn1) */
#define EVEN_CASE(val) \
    /* res=res * res^(2^(HFEn1>>i)) */\
    /* res^(2^(HFEn1>>i)) */\
    HFEINIT_(r_128_copy,r2_128_copy,r3_128_copy,r4_128_copy,r5_128_copy,r);\
    for(j=0;j<val;++j)\
    {\
        SQR_GF2N_(r,r_128,r2_128,r3_128,r4_128,r5_128,ra,rem_function);\
    }\
    /* multiply by the old res (res=res * res^(2^(HFEn1>>i))) */\
    MUL_GF2N_(r,r_128,r2_128,r3_128,r4_128,r5_128,r_128_copy,r2_128_copy,r3_128_copy,r4_128_copy,r5_128_copy,ra,ra2,sum,res1,res2,rem_function);

/* Take in input HFEn1>>i (i the current bit of HFEn1) */
#define ODD_CASE(val) \
    EVEN_CASE(val);\
    /* res = res^2 *A */\
    SQR_GF2N_(r,r_128,r2_128,r3_128,r4_128,r5_128,ra,rem_function);\
    MUL_GF2N_(r,r_128,r2_128,r3_128,r4_128,r5_128,a_128,a2_128,a3_128,a4_128,a5_128,ra,ra2,sum,res1,res2,rem_function);


/* Part of odd case */
/* Compute A^((2^3)-1 from A^((2^2)-1) */
#define ODD_INIT \
    SQR_GF2N_(r,r_128,r2_128,r3_128,r4_128,r5_128,ra,rem_function);\
    MUL_GF2N_(r,r_128,r2_128,r3_128,r4_128,r5_128,a_128,a2_128,a3_128,a4_128,a5_128,ra,ra2,sum,res1,res2,rem_function);\


#endif



#if (defined(ENABLED_PCLMUL)&&(NB_WORD_GFqn<10))
void PREFIX_NAME(inv_ITMIA_gf2n)(uint64_t res[NB_WORD_GFqn], const uint64_t A[NB_WORD_GFqn])
{
    INIT_REGISTERS;
    uint64_t r[NB_WORD_MUL];
    #if (((K3!=1)||(NB_WORD_GFqn!=1))&&(HFEn!=312)&&(HFEn!=402)&&(HFEn!=544))
        uint64_t R;
    #endif

    #if (NB_WORD_GFqn==1)
        uint64_t Q;
    #elif (HFEn&63)
        uint64_t Q[NB_WORD_GFqn];
    #endif

    #if (HFEn>4)
        unsigned int j;
    #endif

    HFEINIT_(a_128,a2_128,a3_128,a4_128,a5_128,A);

    /* The MSB of HFEn1 is done here : A^(2^1 -1) = A */
    HFEINIT_(r_128,r2_128,r3_128,r4_128,r5_128,A);

    /* The second MSB of HFEn1 is done here */
    /* Compute A^(2^2 -1) = A^3 */
    SQR_GF2X_(r,r_128,r2_128,r3_128,r4_128,r5_128,ra);
    rem_function;
    MUL_GF2N_(r,r_128,r2_128,r3_128,r4_128,r5_128,a_128,a2_128,a3_128,a4_128,a5_128,ra,ra2,sum,res1,res2,rem_function);

    /* For the others bits of HFEn1: */
    /* bit 5 to bit 0 */
    ITOH;

    /* res=A^(2^HFEn1 -1) here */
    /* (2^HFEn1 -1)*2 = 2^k -2 */
    /* Square */
    SQR_GF2N_(r,r_128,r2_128,r3_128,r4_128,r5_128,ra,rem_function);
    copy_gf2n(res,r);
}
#endif





#endif


