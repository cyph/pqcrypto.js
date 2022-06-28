#include "chooseRootHFE.hpp"
#include "init.h"
#include "bit.h"
#include "convNTL.hpp"
#include <NTL/GF2EXFactoring.h>


/* To take root only if it is uniq */
/* #define UNIQ_ROOT */

#ifndef UNIQ_ROOT
    /* Choose the method to choose the root */
    #define FIRST_ROOT 0
    #define DETERMINIST_ROOT 1

    #if ((FIRST_ROOT+DETERMINIST_ROOT)!=1)
        #error "It needs a uniq method to choose a root."
    #endif

    #if DETERMINIST_ROOT
        BEGIN_EXTERNC
            #include <libkeccak.a.headers/SimpleFIPS202.h>
        END_EXTERNC
    #endif
#endif


/* To sort roots */
static void quickSort(vec_gf2n tab,int d,int f)
{
    /* If the length is null */
    if(d>=f)
    {
        return;
    }

    int left=d;
    int right=f;

    while(1)
    {
        while(HFECMP_LT(tab+left,tab+d))
        {
            left+=NB_WORD_GFqn;
        }
        while(HFECMP_GT(tab+right,tab+d))
        {
            right-=NB_WORD_GFqn;
        }

        if(left<right)
        {
            HFEPERMUTATION(tab+left,tab+right);
        } else
        {
            break;
        }
    }

    quickSort(tab,d,right);
    quickSort(tab,right+NB_WORD_GFqn,f);
}





/*  Input:
        F a HFE polynomial in GF(2^n)[X]
        U a element of GF(2^n)

    Output:
        the number of roots
        root a root of F-U

    Requirement: F is monic
*/
unsigned int chooseRootHFE(gf2n root, cst_sparse_monic_gf2nx F, cst_gf2n U)
{
    #if (HFEDeg==1)
        /* X+constant = U so X = U-constant */
        HFEADD(root,F,U);
        return 1U;
    #else

    GF2EXModulus F_NTL_opt;
    GF2EX F_NTL,Xqn,Pgcd;
    vec_GF2E roots_NTL;
    GF2X monomX;
    GF2E monom;

    unsigned int l;
    #ifndef UNIQ_ROOT
        unsigned int i;
    #endif

    #if DETERMINIST_ROOT
        UINT hash[SIZE_DIGEST_UINT];
    #endif


    /*  F_HFE(x) */
    convHFEpolynomToNTLGF2EX(F_NTL,F);
    /* The previous function initialises the modulus of GF2E */

    /* U */
    GF2XFromBytes(monomX,(unsigned char*)U,(long)NB_BYTES_GFqn);
    conv(monom,monomX);

    /*  F_HFE(x) - U = 0 */
    add(F_NTL,F_NTL,monom);

    /* FindRoots needs monic HFE polynom */

    /* Optimize the modular reduction by F_NTL */
    build(F_NTL_opt,F_NTL);
    F_NTL.kill();
    /* X^(2^n) - X mod F */
    FrobeniusMap(Xqn,F_NTL_opt);
    add(Xqn,Xqn,GF2EX(1,1));
    GCD(Pgcd,F_NTL_opt,Xqn);

    Xqn.kill();
    l=(unsigned int)deg(Pgcd);
    #ifdef UNIQ_ROOT
        /* if the root is not uniq, fail to choose a root */
        if(l!=1U)
        {
            l=0U;
        }
    #endif
    if(!l)
    {
        monomX.kill();
        clear(monom);
        Pgcd.kill();
        return 0;
    }

    FindRoots(roots_NTL,Pgcd);
    Pgcd.kill();

    #ifdef UNIQ_ROOT
        /* Choose the uniq root */
        conv(monomX,roots_NTL[0]);
        BytesFromGF2X((unsigned char*)root,monomX,(long)NB_BYTES_GFqn);
        /* To clean the last word */
        #if HFEnr
            root[NB_WORD_GFqn-1]&=HFE_MASKn;
        #endif
    #else
        if(l==1)
        {
            /* Choose the uniq root */
            conv(monomX,roots_NTL[0]);
            BytesFromGF2X((unsigned char*)root,monomX,(long)NB_BYTES_GFqn);
            /* To clean the last word */
            #if HFEnr
                root[NB_WORD_GFqn-1]&=HFE_MASKn;
            #endif
        } else
        {
            /* Conversion of roots in UINT* */
            static_gf2n roots_UINT[NB_WORD_GFqn*l];
            for(i=0;i<l;++i)
            {
                BytesFromGF2X((unsigned char*)(roots_UINT+i*NB_WORD_GFqn),conv<GF2X>(roots_NTL[i]),NB_WORD_GFqn<<3);
                /* Clean the last word */
                #if HFEnr
                    roots_UINT[(i+1)*NB_WORD_GFqn-1]&=HFE_MASKn;
                #endif
            }

            /* Sort the roots */
            quickSort(roots_UINT,0,(l-1)*NB_WORD_GFqn);

            #if FIRST_ROOT
                /* Choose the first root */
                HFECOPY(root,roots_UINT);
            #elif DETERMINIST_ROOT
                /* Choose a root with a determinist hash */
                HASH((unsigned char*)hash,(unsigned char*)U,NB_BYTES_GFqn);
                HFECOPY(root,roots_UINT+(hash[0]%l)*NB_WORD_GFqn);
            #endif
        }
    #endif

    monomX.kill();
    clear(monom);
    return l;
    #endif
}
