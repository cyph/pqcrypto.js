///  @file parallel_matrix_op_avx2.c
///  @brief the AVX2 implementations for functions in parallel_matrix_op_avx2.h
///
///

#include "blas_comm.h"
#include "blas.h"

#include "parallel_matrix_op.h"
#include "parallel_matrix_op_avx2.h"




////////////////////  matrix multiplications  ///////////////////////////////






void batch_trimat_madd_multab_gf16_avx2( unsigned char * bC , const unsigned char* btriA ,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch )
{
    (void) size_Bcolvec; // unused
    unsigned Awidth = Bheight;
    unsigned Aheight = Awidth;
    for(unsigned i=0;i<Aheight;i++) {
        for(unsigned j=0;j<Bwidth;j++) {
            for(unsigned k=0;k<Bheight;k++) {
                if(k<i) continue;
                gf16v_madd_multab_avx2( bC , & btriA[ (k-i)*size_batch ] , &B[(j*Bheight+k)*16] , size_batch );
            }
            bC += size_batch;
        }
        btriA += (Aheight-i)*size_batch;
    }
}

void batch_trimat_madd_multab_gf256_avx2( unsigned char * bC , const unsigned char* btriA ,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch )
{
    (void) size_Bcolvec; // unused
    unsigned Awidth = Bheight;
    unsigned Aheight = Awidth;
    for(unsigned i=0;i<Aheight;i++) {
        for(unsigned j=0;j<Bwidth;j++) {
            for(unsigned k=0;k<Bheight;k++) {
                if(k<i) continue;
                gf256v_madd_multab_avx2( bC , & btriA[ (k-i)*size_batch ] , &B[(j*Bheight+k)*32] , size_batch );
            }
            bC += size_batch;
        }
        btriA += (Aheight-i)*size_batch;
    }
}







void batch_trimatTr_madd_multab_gf16_avx2( unsigned char * bC , const unsigned char* btriA ,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch )
{
    (void) size_Bcolvec; // unused
    unsigned Aheight = Bheight;
    for(unsigned i=0;i<Aheight;i++) {
        for(unsigned j=0;j<Bwidth;j++) {
            for(unsigned k=0;k<Bheight;k++) {
                if(i<k) continue;
                gf16v_madd_multab_avx2( bC , & btriA[ size_batch*(idx_of_trimat(k,i,Aheight)) ] , &B[(j*Bheight+k)*16] , size_batch );
            }
            bC += size_batch;
        }
    }
}

void batch_trimatTr_madd_multab_gf256_avx2( unsigned char * bC , const unsigned char* btriA ,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch )
{
    (void) size_Bcolvec; // unused
    unsigned Aheight = Bheight;
    for(unsigned i=0;i<Aheight;i++) {
        for(unsigned j=0;j<Bwidth;j++) {
            for(unsigned k=0;k<Bheight;k++) {
                if(i<k) continue;
                gf256v_madd_multab_avx2( bC , & btriA[ size_batch*(idx_of_trimat(k,i,Aheight)) ] , &B[(j*Bheight+k)*32] , size_batch );
            }
            bC += size_batch;
        }
    }
}





void batch_2trimat_madd_multab_gf16_avx2( unsigned char * bC , const unsigned char* btriA ,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch )
{
    (void) size_Bcolvec; // unused
    unsigned Aheight = Bheight;
    for(unsigned i=0;i<Aheight;i++) {
        for(unsigned j=0;j<Bwidth;j++) {
            for(unsigned k=0;k<Bheight;k++) {
                if(i==k) continue;
                gf16v_madd_multab_avx2( bC , & btriA[ size_batch*(idx_of_2trimat(i,k,Aheight)) ] , &B[(j*Bheight+k)*16] , size_batch );
            }
            bC += size_batch;
        }
    }
}

void batch_2trimat_madd_multab_gf256_avx2( unsigned char * bC , const unsigned char* btriA ,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch )
{
    (void) size_Bcolvec; // unused
    unsigned Aheight = Bheight;
    for(unsigned i=0;i<Aheight;i++) {
        for(unsigned j=0;j<Bwidth;j++) {
            for(unsigned k=0;k<Bheight;k++) {
                if(i==k) continue;
                gf256v_madd_multab_avx2( bC , & btriA[ size_batch*(idx_of_2trimat(i,k,Aheight)) ] , &B[(j*Bheight+k)*32] , size_batch );
            }
            bC += size_batch;
        }
    }
}




void batch_matTr_madd_multab_gf16_avx2( unsigned char * bC , const unsigned char* A_to_tr , unsigned Aheight, unsigned size_Acolvec, unsigned Awidth,
        const unsigned char* bB, unsigned Bwidth, unsigned size_batch )
{
    (void) size_Acolvec; // unused
    unsigned Atr_height = Awidth;
    unsigned Atr_width  = Aheight;
    for(unsigned i=0;i<Atr_height;i++) {
        for(unsigned j=0;j<Atr_width;j++) {
            gf16v_madd_multab_avx2( bC , & bB[ j*Bwidth*size_batch ] , &A_to_tr[(i*Aheight+j)*16] , size_batch*Bwidth );
        }
        bC += size_batch*Bwidth;
    }
}

void batch_matTr_madd_multab_gf256_avx2( unsigned char * bC , const unsigned char* A_to_tr , unsigned Aheight, unsigned size_Acolvec, unsigned Awidth,
        const unsigned char* bB, unsigned Bwidth, unsigned size_batch )
{
    (void) size_Acolvec; // unused
    unsigned Atr_height = Awidth;
    unsigned Atr_width  = Aheight;
    for(unsigned i=0;i<Atr_height;i++) {
        for(unsigned j=0;j<Atr_width;j++) {
            gf256v_madd_multab_avx2( bC , & bB[ j*Bwidth*size_batch ] , &A_to_tr[(i*Aheight+j)*32] , size_batch*Bwidth );
        }
        bC += size_batch*Bwidth;
    }
}




void batch_bmatTr_madd_multab_gf16_avx2( unsigned char *bC , const unsigned char *bA_to_tr, unsigned Awidth_before_tr,
        const unsigned char *B, unsigned Bheight, unsigned size_Bcolvec, unsigned Bwidth, unsigned size_batch )
{
    (void) size_Bcolvec; // unused
    const unsigned char *bA = bA_to_tr;
    unsigned Aheight = Awidth_before_tr;
    for(unsigned i=0;i<Aheight;i++) {
        for(unsigned j=0;j<Bwidth;j++) {
            for(unsigned k=0;k<Bheight;k++) {
                gf16v_madd_multab_avx2( bC , & bA[ size_batch*(i+k*Aheight) ] , &B[(j*Bheight+k)*16] , size_batch );
            }
            bC += size_batch;
        }
    }
}

void batch_bmatTr_madd_multab_gf256_avx2( unsigned char *bC , const unsigned char *bA_to_tr, unsigned Awidth_before_tr,
        const unsigned char *B, unsigned Bheight, unsigned size_Bcolvec, unsigned Bwidth, unsigned size_batch )
{
    (void) size_Bcolvec; // unused
    const unsigned char *bA = bA_to_tr;
    unsigned Aheight = Awidth_before_tr;
    for(unsigned i=0;i<Aheight;i++) {
        for(unsigned j=0;j<Bwidth;j++) {
            for(unsigned k=0;k<Bheight;k++) {
                gf256v_madd_multab_avx2( bC , & bA[ size_batch*(i+k*Aheight) ] , &B[(j*Bheight+k)*32] , size_batch );
            }
            bC += size_batch;
        }
    }
}





void batch_mat_madd_multab_gf16_avx2( unsigned char * bC , const unsigned char* bA , unsigned Aheight,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch )
{
    (void) size_Bcolvec; // unused
    unsigned Awidth = Bheight;
    for(unsigned i=0;i<Aheight;i++) {
        for(unsigned j=0;j<Bwidth;j++) {
            for(unsigned k=0;k<Bheight;k++) {
                gf16v_madd_multab_avx2( bC , & bA[ k*size_batch ] , &B[(j*Bheight+k)*16] , size_batch );
            }
            bC += size_batch;
        }
        bA += (Awidth)*size_batch;
    }
}

void batch_mat_madd_multab_gf256_avx2( unsigned char * bC , const unsigned char* bA , unsigned Aheight,
        const unsigned char* B , unsigned Bheight, unsigned size_Bcolvec , unsigned Bwidth, unsigned size_batch )
{
    (void) size_Bcolvec; // unused
    unsigned Awidth = Bheight;
    for(unsigned i=0;i<Aheight;i++) {
        for(unsigned j=0;j<Bwidth;j++) {
            for(unsigned k=0;k<Bheight;k++) {
                gf256v_madd_multab_avx2( bC , & bA[ k*size_batch ] , &B[(j*Bheight+k)*32] , size_batch );
            }
            bC += size_batch;
        }
        bA += (Awidth)*size_batch;
    }
}









////////////////////  "quadratric" matrix evaluation  ///////////////////////////////


#include "stdint.h"



void batch_quad_trimat_eval_multab_gf16_avx2( unsigned char * y, const unsigned char * trimat, const unsigned char * x_multab, unsigned dim , unsigned size_batch )
{
///    assert( dim <= 128 );
///    assert( size_batch <= 128 );
    unsigned char tmp[256] __attribute__((aligned(32)));

    gf256v_set_zero( y , size_batch );
    for(unsigned i=0;i<dim;i++) {
        gf16mat_prod_multab_avx2( tmp , trimat , size_batch , dim-i , x_multab+16*i );
        trimat += size_batch*(dim-i);
        gf16v_madd_multab_avx2( y , tmp , x_multab+16*i , size_batch );
    }
}





void batch_quad_trimat_eval_multab_gf256_avx2( unsigned char * y, const unsigned char * trimat, const unsigned char * x_multab, unsigned dim , unsigned size_batch )
{
///    assert( dim <= 128 );
///    assert( size_batch <= 128 );
    unsigned char tmp[256] __attribute__((aligned(32)));

    gf256v_set_zero( y , size_batch );
    for(unsigned i=0;i<dim;i++) {
        gf256v_set_zero( tmp , size_batch );
        for(unsigned j=i;j<dim;j++) {
           gf256v_madd_multab_avx2( tmp , trimat , x_multab+32*j , size_batch );
           trimat += size_batch;
        }
        gf256v_madd_multab_avx2( y , tmp , x_multab+32*i , size_batch );
    }
}





