/*
 * param.h
 *
 *  Created on: May 3, 2018
 *      Author: vader
 */

#ifndef SRC_PARAMETERS_PARAM_H_
#define SRC_PARAMETERS_PARAM_H_

#define field 8 //2^8

#define extension 2 //m
#define field_extension (field*extension) //

#define F_q_m_size (1 << field_extension) // #F_q_m

#define F_q_m_order ((F_q_m_size)-1) // Field Group order

#define subfield (field_extension/extension)
#define F_q_size (1 << subfield)
#define F_q_order ((F_q_size)-1)

// irreducible polynomial for field: x^16 + x^5 + x^3 + x^2 + 1
#define irreducible_polynomial_field 65581
// Define Poly primitive subfield: x^8 + x^4+ x^3 + x + 1
#define primitive_polynomial_subfield 283
#define code_length 1216
#define code_dimension 512 // number of line of each block matrix
#define k_prime  32
#define signature_block_size (1<<5)
#define pol_deg 11 //block number 11
#define weight 176
#define ss_length 64
#define k_sec (code_dimension - k_prime)
#define n0 (code_length/signature_block_size) // 38


#endif /* SRC_PARAMETERS_PARAM_H_ */
