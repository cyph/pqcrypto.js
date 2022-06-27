/*=============================================================================
 * Copyright (c) 2020 by Cryptographic Engineering Research Group (CERG)
 * ECE Department, George Mason University
 * Fairfax, VA, U.S.A.
 * Author: Duc Tri Nguyen

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=============================================================================*/


#include "batch_64coefficient_multiplications.c"
/*
 * toom_cook_4way_neon
 * Main function 
 * a1_avx: In memory [256]
 * b1_avx: In memory  [256]
 * res_avx_output: In memory [512] -- improved, in memory [256]
 */

#define AVX_N (SABER_N / 16)
#define small_len_avx (AVX_N / 4)

void toom_cook_4way_neon(uint16_t  *a1_avx,
						uint16_t *b1_avx,
						uint16_t p_mod,
						uint16_t *res_avx_output)
{
	uint16_t i;

	//-----Memory data declaration-----------------
	// Zeroing res_avx
	uint16_t res_avx[16 * 2 * AVX_N] = {0};
	
    // All in memory
	uint16_t    w1_avx[2 * 16 * small_len_avx] = {0}, 
                w2_avx[2 * 16 * small_len_avx] = {0}, 
                w3_avx[2 * 16 * small_len_avx] = {0}, 
                w4_avx[2 * 16 * small_len_avx] = {0}, 
                w5_avx[2 * 16 * small_len_avx] = {0}, 
                w6_avx[2 * 16 * small_len_avx] = {0}, 
                w7_avx[2 * 16 * small_len_avx] = {0};


	//--------------------these data are created for place holding---------
	// This is memory or register ? For now, let's compiler optimize it
	uint16_t a1_ph_avx[small_len_avx*16], b1_ph_avx[small_len_avx*16];
	uint16_t a2_ph_avx[small_len_avx*16], b2_ph_avx[small_len_avx*16];
	uint16_t a3_ph_avx[small_len_avx*16], b3_ph_avx[small_len_avx*16];
	uint16_t a4_ph_avx[small_len_avx*16], b4_ph_avx[small_len_avx*16];
	uint16_t a5_ph_avx[small_len_avx*16], b5_ph_avx[small_len_avx*16];
	uint16_t a6_ph_avx[small_len_avx*16], b6_ph_avx[small_len_avx*16];
	uint16_t     a_avx[small_len_avx*16],     b_avx[small_len_avx*16];

    uint16x8x2_t a1_tmp, b1_tmp;
    uint16x8x2_t a2_tmp, b2_tmp;
    uint16x8x2_t th_a_avx, th_b_avx;
    uint16x8x2_t t_h_a_avx, t_h_b_avx;

    uint16x8x2_t w1, w2, w3, w4, w5, w6, w7;
	uint16x8x2_t temp1_neon;

	//--------------------these data are created for place holding ends---------

	// Add for NEON
	uint16x8x2_t int0_avx;
	vxor(int0_avx, w1, w1);

	//-----AVX data declaration ends------------

	(void) p_mod;

	//do the partial products


	//-------------------th and t_h. th <-a(1/2)*b(1/2). t_h <- a(-1/2)*b(-1/2) ---------------------
	//create partial sum for th and t_h

	for (i = 0; i < small_len_avx; i++)
	{ 
	    //create a(0)*b(0)
	    //-------------------t0--------------------
        vload(a1_tmp, &a1_avx[i*16]);
        vload(b1_tmp, &b1_avx[i*16]);

        vstore(&a1_ph_avx[i*16], a1_tmp);
        vstore(&b1_ph_avx[i*16], b1_tmp);    
	    //-------------------t0 ends------------------

        //th_x_avx contains 4*x[0]
		vsl(th_a_avx, a1_tmp, 2);
		vsl(th_b_avx, b1_tmp, 2);

        vload(a1_tmp, &a1_avx[(small_len_avx * 2 + i)*16 ] );
        vload(b1_tmp, &b1_avx[(small_len_avx * 2 + i)*16 ] );

		//th_x_avx contains 4*x[0]+x[2]
		vadd(th_a_avx, th_a_avx,  a1_tmp);
		vadd(th_b_avx, th_b_avx,  b1_tmp);

		//th_x_avx contains 8*x[0]+2*x[2]
		vsl(th_a_avx, th_a_avx, 1);
		vsl(th_b_avx, th_b_avx, 1);

		//t_h_x_avx contains x[1]
        vload(t_h_a_avx, &a1_avx[(small_len_avx * 1 + i)*16 ] );
        vload(t_h_b_avx, &b1_avx[(small_len_avx * 1 + i)*16 ] );

		//t_h_x_avx contains 4*x[1]
		vsl(t_h_a_avx, t_h_a_avx, 2);
		vsl(t_h_b_avx, t_h_b_avx, 2);

		//th_x_avx contains 4*x[1]+x[3]
        vload(a1_tmp, &a1_avx[ (small_len_avx * 3 + i)*16 ] );
        vload(b1_tmp, &b1_avx[ (small_len_avx * 3 + i)*16 ] );
		
        vadd(t_h_a_avx, t_h_a_avx, a1_tmp);
		vadd(t_h_b_avx, t_h_b_avx, b1_tmp);
	

    	//create th
        vadd(a1_tmp, th_a_avx, t_h_a_avx);
		vadd(b1_tmp, th_b_avx, t_h_b_avx);

        vstore(&a2_ph_avx[i*16], a1_tmp);
        vstore(&b2_ph_avx[i*16], b1_tmp);

	    //create t_h

        vsub(a2_tmp, th_a_avx, t_h_a_avx);
		vsub(b2_tmp, th_b_avx, t_h_b_avx);

        vstore(&a3_ph_avx[i*16], a2_tmp);
        vstore(&b3_ph_avx[i*16], b2_tmp);

	//-------------------t1 and t_1. t1 <-a(1)*b(1). t_1 <- a(-1)*b(-1) ---------------------

	    //create partial sum for t_1 and t1

	    //th_x_avx contains x[2]+x[0]
        vload(a1_tmp, &a1_avx[(small_len_avx * 2 + i)*16] );
        vload(b1_tmp, &b1_avx[(small_len_avx * 2 + i)*16] );
        vload(a2_tmp, &a1_avx[(small_len_avx * 0 + i)*16] );
        vload(b2_tmp, &b1_avx[(small_len_avx * 0 + i)*16] );

		vadd(th_a_avx, a1_tmp, a2_tmp);
		vadd(th_b_avx, b1_tmp, b2_tmp);

		//th_x_avx contains x[3]+x[1]
        vload(a1_tmp, &a1_avx[(small_len_avx * 3 + i)*16] );
        vload(b1_tmp, &b1_avx[(small_len_avx * 3 + i)*16] );
        vload(a2_tmp, &a1_avx[(small_len_avx * 1 + i)*16] );
        vload(b2_tmp, &b1_avx[(small_len_avx * 1 + i)*16] );

		vadd(t_h_a_avx, a1_tmp, a2_tmp);
		vadd(t_h_b_avx, b1_tmp, b2_tmp);
	

	    //create t1
	    // x[0]+x[1]+x[2]+x[3]
        vadd(a2_tmp, th_a_avx, t_h_a_avx);
		vadd(b2_tmp, th_b_avx, t_h_b_avx);

        vstore(&a4_ph_avx[i*16], a2_tmp);
        vstore(&b4_ph_avx[i*16], b2_tmp);
	

	    //create t_1
	    //-x[3]+x[2]-x[1]+x[0]
        vsub(a2_tmp, th_a_avx, t_h_a_avx);
		vsub(b2_tmp, th_b_avx, t_h_b_avx);

        vstore(&a5_ph_avx[i*16], a2_tmp);
        vstore(&b5_ph_avx[i*16], b2_tmp);
	

	//------------------t_inf------------------------------
	//create t_inf

        vstore(&a6_ph_avx[i*16], a1_tmp);
        vstore(&b6_ph_avx[i*16], b1_tmp);

	//-------------------t_inf ends----------------------

	//-------------------t2-------------------------
        // 2*x[3]
		vsl(b1_tmp, b1_tmp, 1);
		vsl(a1_tmp, a1_tmp, 1);

		// 2*x[3]+x[2]
        vload(a2_tmp, &a1_avx[(small_len_avx * 2 + i)*16] );
        vload(b2_tmp, &b1_avx[(small_len_avx * 2 + i)*16] );

		vadd(a1_tmp, a1_tmp, a2_tmp);
		vadd(b1_tmp, b1_tmp, b2_tmp);

		// 4*x[3]+2*x[2]
		vsl(a1_tmp, a1_tmp, 1);
		vsl(b1_tmp, b1_tmp, 1);

        vload(a2_tmp, &a1_avx[(small_len_avx * 1 + i)*16] );
        vload(b2_tmp, &b1_avx[(small_len_avx * 1 + i)*16] );

		// 4*x[3]+2*x[2]+x[1]
		vadd(a1_tmp, a1_tmp, a2_tmp);
		vadd(b1_tmp, b1_tmp, b2_tmp);

		// 8*x[3]+4*x[2]+2*x[1]
		vsl(a1_tmp, a1_tmp, 1);
		vsl(b1_tmp, b1_tmp, 1);

		// 8*x[3]+8*x[2]+2*x[1]+x[0]
        vload(a2_tmp, &a1_avx[(small_len_avx * 0 + i)*16] );
        vload(b2_tmp, &b1_avx[(small_len_avx * 0 + i)*16] );


		vadd(a1_tmp, a1_tmp, a2_tmp);
		vadd(b1_tmp, b1_tmp, b2_tmp);

        vstore(&a_avx[i*16], a1_tmp);
        vstore(&b_avx[i*16], b1_tmp);
	}

	batch_64coefficient_multiplications(
		a1_ph_avx, b1_ph_avx, w7_avx,
		a2_ph_avx, b2_ph_avx, w5_avx,
		a3_ph_avx, b3_ph_avx, w6_avx,
		a4_ph_avx, b4_ph_avx, w3_avx,
		a5_ph_avx, b5_ph_avx, w4_avx,
		a6_ph_avx, b6_ph_avx, w1_avx,
		a_avx, b_avx, w2_avx);

	/*	--------------------------------------------
		---------------Solution starts--------------
		--------------------------------------------

	*/

	for (i = 0; i < 2 * small_len_avx; i++)
	{
		vload(w1, &w1_avx[i*16]);
		vload(w2, &w2_avx[i*16]);
		vload(w3, &w3_avx[i*16]);
		vload(w4, &w4_avx[i*16]);
		vload(w5, &w5_avx[i*16]);
		vload(w6, &w6_avx[i*16]);
		vload(w7, &w7_avx[i*16]);

		vadd(w2, w2, w5);//w2 <- w2+w5
		vsub(w6, w6, w5); // w6 <- w6-w5
		vsub(w4, w4, w3); // w4 <- w4-w3

		vsub(w5, w5, w1);	// w5 <- w5-w1
		vsl(temp1_neon, w7, 6);			//temp <- 64*w7
		vsub(w5, w5, temp1_neon); // w5 <- w5-64*w7

		vsr(w4, w4, 1);	  //w4 <- w4/2
		vadd(w3, w3,w4);//w3 <- w3+w4

		vsl(temp1_neon, w5, 1);		 //temp <- 2*w5
		vadd(w5, w6,temp1_neon);//w5 <- 2*w5+w6

		vsl(temp1_neon, w3, 6);			   //temp <- 64*w3
		vadd(temp1_neon, w3, temp1_neon); //temp <- 65*w3
		vsub(w2, w2, temp1_neon);	   // w2 <- w2-65*w3

		vsub(w3, w3, w7); // w3 <- w3-w7
		vsub(w3, w3, w1); // w3 <- w3-w1

		vmuln(temp1_neon, w3, 45);		//temp <- 45*w3
		vadd(w2, w2, temp1_neon); //w2 <- w2+45*w3

		vsl(temp1_neon, w3, 3);			//temp <- 8*w3
		vsub(w5, w5, temp1_neon); //w5 <- w5-8*w3

		vmuln(w5, w5, 43691); //w5 <- w5*1/3
		vsr(w5, w5, 3);	 //w5 <- w5*1/8 --. w5=w5/24

		vadd(w6, w2, w6);	//w6 <- w6+w2
		vsl(temp1_neon, w4, 4);			//temp <- 16*w4
		vadd(w2, w2, temp1_neon); //w2 <- w2+16*w4

		vmuln(w2, w2, 36409); //w2 <- w2*1/9
		vsr(w2, w2, 1);	 //w2 <- w2*1/2 --. w2=w2/18

		vsub(w3, w3, w5); //w3 <- w3-w5

		vadd(w4, w4, w2); //w4 <- w4+w2

		vsub(w4, int0_avx, w4); //w4 <- -(w4+w2)

		vmuln(temp1_neon, w2, 30);		//temp <- w2*30
		vsub(w6, temp1_neon, w6); //w6 <- 30*w2-w6

		vmuln(w6, w6, 61167); //w6 <- w6*1/15
		vsr(w6, w6, 2);	 //w6 <- w6*1/4 --. w6=w6/60

		vsub(w2, w2, w6); //w2 <- w2-w6

		vstore(&w2_avx[i*16], w2);
		vstore(&w3_avx[i*16], w3);
		vstore(&w4_avx[i*16], w4);
		vstore(&w5_avx[i*16], w5);
		vstore(&w6_avx[i*16], w6);

		vload(a1_tmp, &res_avx[(0 * small_len_avx + i)*16]);
		vadd(a1_tmp, a1_tmp, w7);
		vstore(&res_avx[(0 * small_len_avx + i)*16], a1_tmp);

		// vload(w6, &w6_avx[i*16]);
		vload(b1_tmp, &res_avx[(1 * small_len_avx + i)*16]);
		vadd(b1_tmp, b1_tmp, w6);
		vstore(&res_avx[(1 * small_len_avx + i)*16], b1_tmp);

		// vload(w5, &w5_avx[i*16]);
		vload(a2_tmp, &res_avx[(2 * small_len_avx + i)*16]);
		vadd(a2_tmp, a2_tmp, w5);
		vstore(&res_avx[(2 * small_len_avx + i)*16], a2_tmp);

		// vload(w4, &w4_avx[i*16]);
		vload(b2_tmp, &res_avx[(3 * small_len_avx + i)*16]);
		vadd(b2_tmp, b2_tmp, w4);
		vstore(&res_avx[(3 * small_len_avx + i)*16], b2_tmp);

		// vload(w3, &w3_avx[i*16]);
		vload(th_a_avx, &res_avx[(4 * small_len_avx + i)*16]);
		vadd(th_a_avx, th_a_avx, w3);
		vstore(&res_avx[(4 * small_len_avx + i)*16], th_a_avx);

		// vload(w2, &w2_avx[i*16]);
		vload(th_b_avx, &res_avx[(5 * small_len_avx + i)*16]);
		vadd(th_b_avx, th_b_avx, w2);
		vstore(&res_avx[(5 * small_len_avx + i)*16], th_b_avx);

		// vload(w1, &w1_avx[i*16]);
		vload(t_h_a_avx, &res_avx[(6 * small_len_avx + i)*16]);
		vadd(t_h_a_avx, t_h_a_avx, w1);
		vstore(&res_avx[(6 * small_len_avx + i)*16], t_h_a_avx);
	}


	// Reduction by X^256 + 1
	for (i = 0; i < 16; i++)
	{
		vload(a1_tmp, &res_avx[i*16]);
		vload(b1_tmp, &res_avx[(i+16)*16]);
		vsub(a2_tmp, a1_tmp, b1_tmp);
		vstore(&res_avx_output[i*16], a2_tmp);
	}
}
