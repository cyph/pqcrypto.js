#include "findRoots_gf2nx.h"
#include "tools_gf2n.h"
#include "tools_gf2nv.h"
#include "frobeniusMap_gf2nx.h"
#include "conv_gf2nx.h"

#include "arith_gf2n.h"
#include "inv_gf2n.h"
#include "add_gf2nx.h"
#include "gcd_gf2nx.h"
#include "findRootsSplit_gf2nx.h"




/*  Input:
        F a polynomial in GF(2^n)[X]
        df the degree of F

    Output:
        the number of roots of F
        roots the list of roots of F : this function initializes the memory \
        of *roots, you will need to use free() to desallocate

    Requirement: F is monic
*/
int PREFIX_NAME(findRoots_gf2nx)(vec_gf2n* roots,const gf2nx F,unsigned int df)
{
    if(df==1)
    {
        *roots=(UINT*)calloc(NB_WORD_GFqn,sizeof(UINT));
        VERIFY_ALLOC_RET(roots);
        copy_gf2n(*roots,F);
        return 1U;
    }

    gf2nx tmp_p, poly, poly2;
    unsigned int i,l;


    ALIGNED_CALLOC(poly,UINT*,((df<<1)-1)*NB_WORD_GFqn,sizeof(UINT));
    if(!poly) 
    {
        return ERROR_ALLOC;
    }

    ALIGNED_CALLOC(poly2,UINT*,(df+1)*NB_WORD_GFqn,sizeof(UINT));
    if(!poly2) 
    {
        free(poly);
        return ERROR_ALLOC;
    }

    copy_gf2nx(poly2,F,df+1,i);


    /* X^(2^n) - X mod F */
    l=frobeniusMap_gf2nx(poly,F,df);

    /* GCD(F, X^(2^n)-X mod F) */
    l=gcd_gf2nx(&i,poly2,df,poly,l);


    if(i)
    {
        tmp_p=poly;
        poly=poly2;
        poly2=tmp_p;
    }

    if(!is0_gf2n(poly))
    {
        /* The gcd is a constant (!=0) */
        /* Irreducible: 0 root */
        /* l=0; */
        free(poly2);
        free(poly);
        return 0;
    } else
    {
        /* poly2 is the gcd */
        /* Here, it becomes monic */
        convMonic_gf2nx(poly2,l);
    }
    free(poly);


    *roots=(UINT*)calloc(l*NB_WORD_GFqn,sizeof(UINT));
    if(!roots) 
    {
        return ERROR_ALLOC;
    }
    findRootsSplit_gf2nx(*roots,poly2,l);
    free(poly2);

    return (int)l;
}


/*  Input:
        F a HFE polynomial in GF(2^n)[X]
        U a element of GF(2^n)

    Output:
        the number of roots of F-U
        roots the list of roots of F-U : this function initializes the memory \
        of *roots, you will need to use free() to desallocate

    Requirement: F is monic
*/
int PREFIX_NAME(findRootsHFE_gf2nx)(vec_gf2n* roots, const complete_sparse_monic_gf2nx F, cst_gf2n U)
{
    #if (HFEDeg==1)
        *roots=(UINT*)calloc(NB_WORD_GFqn,sizeof(UINT));
        VERIFY_ALLOC_RET(roots);
        add_gf2n(*roots,F.poly,U);
        return 1U;
    #else

    gf2nx tmp_p, poly, poly2;
    unsigned int i,l,d2=HFEDeg;


    ALIGNED_CALLOC(poly,UINT*,((HFEDeg<<1)-1)*NB_WORD_GFqn,sizeof(UINT));
    if(!poly) 
    {
        return ERROR_ALLOC;
    }

    ALIGNED_CALLOC(poly2,UINT*,(HFEDeg+1)*NB_WORD_GFqn,sizeof(UINT));
    if(!poly2) 
    {
        free(poly);
        return ERROR_ALLOC;
    }

    /* GCD(F-U, X^(2^n)-X mod (F-U)) */
    firstStepHFE_gf2nx(l,i,poly,poly2,F,U);

    if(i)
    {
        tmp_p=poly;
        poly=poly2;
        poly2=tmp_p;
    }

    if(!is0_gf2n(poly))
    {
        /* The gcd is a constant (!=0) */
        /* Irreducible: 0 root */
        /* l=0; */
        free(poly);
        free(poly2);
        return 0;
    } else
    {
        /* poly2 is the gcd */
        /* Here, it becomes monic */
        convMonic_gf2nx(poly2,l);
    }
    free(poly);



    *roots=(UINT*)calloc(l*NB_WORD_GFqn,sizeof(UINT));
    if(!roots) 
    {
        return ERROR_ALLOC;
    }
    findRootsSplit_gf2nx(*roots,poly2,l);
    free(poly2);

    return (int)l;
    #endif
}


/*  Input:
        F a HFE polynomial in GF(2^n)[X]
        U a element of GF(2^n)

    Output:
        1 is the root of F-U exists and is uniq, else 0
        root the root of F-U

    Requirement: F is monic
*/
int PREFIX_NAME(findUniqRootHFE_gf2nx)(gf2n root, const complete_sparse_monic_gf2nx F, cst_gf2n U)
{
    #if (HFEDeg==1)
        add_gf2n(root,F.poly,U);
        return 1U;
    #else

    static_gf2n inv[NB_WORD_GFqn] ALIGNED;
    gf2nx tmp_p, poly, poly2;
    unsigned int i,l,d2=HFEDeg;


    ALIGNED_CALLOC(poly,UINT*,((HFEDeg<<1)-1)*NB_WORD_GFqn,sizeof(UINT));
    if(!poly) 
    {
        return ERROR_ALLOC;
    }

    ALIGNED_CALLOC(poly2,UINT*,(HFEDeg+1)*NB_WORD_GFqn,sizeof(UINT));
    if(!poly2) 
    {
        free(poly);
        return ERROR_ALLOC;
    }

    /* GCD(F-U, X^(2^n)-X mod (F-U)) */
    firstStepHFE_gf2nx(l,i,poly,poly2,F,U);

    if(i)
    {
        tmp_p=poly;
        poly=poly2;
        poly2=tmp_p;
    }

    if((!is0_gf2n(poly))||(l!=1))
    {
        /* if (!is0_gf2n(poly)): the gcd is a constant (!=0) */
        /* Irreducible: 0 root */
        /* l=0; */

        /* else, l roots */

        free(poly);
        free(poly2);
        return 0;
    } else
    {
        /* poly2 is the gcd such that poly2 = a*x+b. */
        /* The root is b*a^(-1). */
        inv_gf2n(inv,poly2+NB_WORD_GFqn);
        mul_gf2n(root,inv,poly2);

        free(poly);
        free(poly2);
        return 1;
    }
    #endif
}

