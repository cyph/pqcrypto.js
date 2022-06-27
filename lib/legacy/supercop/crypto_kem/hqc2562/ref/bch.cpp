/**
 * \file bch.cpp
 * \brief Implementation of BCH codes
 */

/* Portions of this code are somewhat inspired from the implementation of BCH code found here https://github.com/torvalds/linux/blob/master/lib/bch.c */

#include "bch.h"

/**
 * \fn int16_t gf_get_antilog(gf_tables* tables, int16_t i)
 * \brief Gets the Anti-Log value of the input i
 *
 * \param[in] tables Pointer to a Galois Field Log and Anti-Log tables
 * \param[in] i Integer
 * \return integer corresponding to the Anti-Log value of i
 */
int16_t gf_get_antilog(gf_tables* tables, int16_t i) {
  return tables->antilog_tab[i];
}

/**
 * \fn int16_t gf_get_log(gf_tables* tables, int16_t i)
 * \brief Gets the Log value of the input i
 *
 * \param[in] tables Pointer to a Galois Field Log and Anti-Log tables
 * \param[in] i Integer
 * \return integer corresponding to the Log value of i
 */
int16_t gf_get_log(gf_tables* tables, int16_t i) {
  return tables->log_tab[i];
}

/**
 * \fn int16_t  gf_mod(int16_t i)
 * \brief Computes the reminder modulo \f$ 2^{10} - 1\f$
 *
 * \param[in] i Integer
 * \return integer corresponding to the reminder modulo \f$ 2^{10} - 1\f$
 */
int16_t gf_mod(int16_t i) {
  return (i < PARAM_GF_MUL_ORDER) ? i : i - PARAM_GF_MUL_ORDER; 
}

/**
 * \fn void bch_code_encode(uint8_t* em, uint8_t* m)
 * \brief Encodes a message using the BCH code 
 *
 * This function encode a message of PARAM_K bits to a code word of size PARAM_N1 bits. Following \cite lin1983error (Chapter 4 - Cyclic Codes), we perform a systematic encoding using a linear (PARAM_N1 - PARAM_K)-stage shift register with feedback
 * connections based on the generator polynomial of the BCH code.
 *
 * For reasons of clarity and comprehensibility, we achieve the encoding process in four steps (the vectors are considered as polynomials):
 *    <ol>
 *    <li> First, we convert the input m which contains the message to be encoded to an array where each coordinate of the input is stored in an unsigned char.
 *    <li> Second, we parse the precomputed generator polynomial \f$ g \f$(x) of the BCH code (defined in <a href="../doc_bch.pdf" target="_blank"><b>BCH code</b></a>) from its hexadecimal representation.
 *    <li> Third, we use an LFSR as described in \cite lin1983error to encode the message and build the code word.
 *    <li> Fourth, we convert the code word to an array of unsigned chars.
 *    </ol>
 *
 * \param[out] em Pointer to an array that is the encoded message
 * \param[in] m Pointer to an array that is the message
 */
void bch_code_encode(uint8_t* em, uint8_t* m) {
  uint8_t tmp1[PARAM_K];
  uint8_t tmp2[PARAM_N1];
  uint8_t g [PARAM_G];
  memset(tmp2, 0, PARAM_N1);
  
  message_to_array(tmp1, m);
  get_generator_poly(g);
  lfsr_encoder(tmp2, g, tmp1);
  array_to_codeword(em, tmp2);
}

/**
 * \fn void message_to_array(uint8_t* o, uint8_t* v)
 * \brief Converts an array v that contains the message to be encoded to an array of unsigned char. Each coordinate of the input is stored in an unsigned char.
 *
 * \param[out] o Pointer to an array of unsigned char
 * \param[in] v Pointer to an array of unsigned char
 */
void message_to_array(uint8_t* o, uint8_t* v) { 
  for (uint8_t i = 0 ; i < VEC_K_SIZE_BYTES ; ++i)  {
    for (uint8_t j = 0 ; j < 8 ; ++j) {
      o[j + i * 8] = (v[i] >> j) & 0x01;
    }
  }
}

/**
 * \fn void get_generator_poly(uint8_t* g)
 * \brief Parses the generator polynomial of the BCH code from its hexadecimal representation
 *
 * This functions stores each coordinate of the generator polynomial in an unsigned char
 *
 * \param[out] g Pointer to an array of unsigned chars
 */
void get_generator_poly(uint8_t* g) {
  // The generator polynomial g(x) of the BCH code in hexadecimal representation
  char GENERATOR_POLY [] = "DA15FB7C969CE2ACA47F3D5A5C78BD3D13E8B148D5B3949A185B4759B16204323389E0CBF733E1750C22AC589329E66B00B035075BDDBE4C0A0BE70BC9F002B4B07A4208"; 

  char* g_poly_string = GENERATOR_POLY, *pos = g_poly_string;
  uint8_t g_bytes_size = (PARAM_G / 8) + 1;
  unsigned char tmp[g_bytes_size];

  for (int i = 0; i < g_bytes_size; ++i)  {
    sscanf(pos, "%2hhx", &tmp[i]);
    pos += 2;
  }

  for (int i = 0; i < (g_bytes_size - 1) ; ++i) {
    for (int j = 0; j < 8; ++j) {
      g[j + i*8] = (tmp[i] & (1 << (7 - j))) >> (7 - j);
    }
  }

  for (int j = 0; j < PARAM_G % 8 ; ++j)  {
      g[j + (g_bytes_size - 1) * 8] = (tmp[g_bytes_size - 1] & (1 << (7 - j))) >> (7 - j);
  }
}

/**
 * \fn void lfsr_encoder(uint8_t* em, uint8_t* g, uint8_t* m)
 * \brief Encode the message stored in the array m to a code word em using the generator polynomial \f$ g \f$ of the BCH code
 *
 * \param[out] em Pointer to an array that is the code word
 * \param[in] g Pointer to an array that is the generator polynomial
 * \param[in] m Pointer to an array that is the message to encode
 */
void lfsr_encoder(uint8_t* em, uint8_t* g, uint8_t* m) {
  int gate_value = 0;
  // Compute the Parity-check digits
  for (int i = PARAM_K-1; i >= 0; --i) {
    gate_value = m[i] ^ em[PARAM_N1 - PARAM_K - 1];

    if (gate_value) {
      for (int j = PARAM_N1 - PARAM_K - 1; j > 0; --j)  {
        em[j] = em[j-1] ^ g[j];       
      }

    } else {
      for (int j = PARAM_N1 - PARAM_K - 1; j > 0; --j)  {
        em[j] = em[j-1];      
      } 
    }

    em[0] = gate_value; 
  }
  // Add the message 
  int index = 0;
  for (int i = PARAM_N1 - PARAM_K ;  i < PARAM_N1 ; ++i) {
    em[i] = m[index];
    index++;
  }
}

/**
 * \fn void array_to_codeword(uint8_t* v, uint8_t* c)
 * \brief Converts an array that contains the bits a code word to an compact array
 *
 * \param[out] v Pointer to an array of unsigned char elements
 * \param[in] c Pointer to an array of unsigned char elements
 */
void array_to_codeword(uint8_t* v, uint8_t* c) {
  for (uint16_t i = 0 ; i < (VEC_N1_SIZE_BYTES - 1) ; ++i) {
    for (uint8_t j = 0 ; j < 8 ; ++j) {
      v[i] |= c[j + i * 8] << j;
    }
  }

  for (uint8_t j = 0 ; j < PARAM_N1 % 8 ; ++j) {
    v[VEC_N1_SIZE_BYTES - 1] |= ((uint8_t) c[j + 8 * (VEC_N1_SIZE_BYTES - 1)]) << j;
  }
}

/**
 * \fn void bch_code_decode(uint8_t* m, uint8_t* em)
 * \brief Decodes a supposed BCH code word
 *
 * The decoding procedure is described in <a href="../doc_bch.pdf" target="_blank"><b>BCH code</b></a>.
 *
 * This function takes as input a pointer to a vector that is the received BCH code word
 * and return a pointer to vector that is the original message being encoded (the information bits). The decoding procedure follows the steps described by
 * Shu. Lin and Daniel J. Costello in Error Control Coding: Fundamentals and Applications \cite lin1983error. In the following the vectors are considered as polynomials.
 *
 * - The decoding procedure consists of four major steps:
 *    <ol>
 *    <li> The first step is the computation of 2 * PARAM_DELTA (with PARAM_DELTA being the error correcting capacity of the BCH code) syndromes.
 *    <li> The second step is the computation of the error-location polynomial \f$ \sigma\f$(x) from (defined in <a href="../doc_bch.pdf" target="_blank"><b>BCH code</b></a>) the syndromes computed in the first step.
 *    <li> The third step is to find the error-location numbers (defined in <a href="../doc_bch.pdf" target="_blank"><b>BCH code</b></a>) by calculating the roots of the polynomial \f$ \sigma\f$(x) and returning their inverses.
 *    <li> The fourth step is the correction of errors in the received polynomial
 *    </ol>
 *
 *
 * \param[out] m Pointer to an array that is the decoded code word
 * \param[in] em Pointer to an array that is supposed to be a code word
 */
void bch_code_decode(uint8_t* m, uint8_t* em) {
  // Generate Galois Field GF(2^10) using the primitive polynomial defined in PARAM_POLY
  // GF(2^10) is represented by the lookup tables (Log Anti-Log tables)
  gf_tables* tables = gf_tables_init();
  gf_generation(tables);      

  // Calculate the 2 * PARAM_DELTA syndromes
  syndrome_set* synd_set = syndrome_init(); 
  syndrome_gen(synd_set, tables, em); 

  // Using the simplified Berlekamp's algorithm we compute the error location polynomial sigma(x)
  sigma_poly* sigma = sigma_poly_init(2 * PARAM_DELTA);
  get_error_location_poly(sigma, tables, synd_set);

  #ifdef VERBOSE
    printf("\n\nThe syndromes: "); for(uint16_t i = 0 ; i < synd_set->size ; ++i) printf("%d ", synd_set->tab[i]);
    printf("\n\nThe error location polynomial sigma(x) = : "); 
    for(uint16_t i = 0 ; i < sigma->deg ; ++i) printf("%d x^%d + ", sigma->value[i], i);
    printf("%d x^%d", sigma->value[sigma->deg], sigma->deg);
  #endif

  // Compute the error location numbers using the Chien Search algorithm
  uint16_t error_pos [PARAM_DELTA];
  memset(error_pos, 0, PARAM_DELTA * 2);
  uint16_t size = 0;
  chien_search(error_pos, &size, tables, sigma); 
  #ifdef VERBOSE
    printf("\n\nThe error location numbers: "); for(uint16_t i = 0 ; i < size ; ++i) printf("%d ", error_pos[i]);
  #endif

  // Compute the error polynomial 
  uint8_t e[VEC_N1_SIZE_BYTES] = {0};
  error_poly_gen(e, error_pos, size);

  #ifdef VERBOSE
    printf("\n\nThe error polynomial e(x) in binary representation: "); vect_print(e, VEC_N1_SIZE_BYTES);
  #endif

  // Add the error polynomial and the received polynomial 
  uint8_t tmp[VEC_N1_SIZE_BYTES] = {0};
  vect_add(tmp, e, em, VEC_N1_SIZE_BYTES);
  
  // Find the message from the decoded code word
  message_from_codeword(m, tmp);
  
  gf_tables_clear(tables);  
  syndrome_clear(synd_set);
  sigma_poly_clear(sigma);
}

/**
 * \fn gf_tables* gf_tables_init()
 * \brief Initializes a Galois Field Log and Anti-Log tables
 *
 * This function is used to initialize gf_tables structure by allocating the necessary
 * memory for the Galois Field Log and Anti-Log tables.
 *
 * \return a pointer to a gf_tables
 */
gf_tables* gf_tables_init() {
  gf_tables* tables = (gf_tables*) malloc(sizeof(gf_tables));

  tables->size = PARAM_GF_MUL_ORDER + 1;
  tables->log_tab = (int16_t*) malloc((tables->size) * sizeof(int16_t));
  tables->antilog_tab = (int16_t*) malloc((tables->size) * sizeof(int16_t));
  
  return tables;
}

/**
 * \fn void gf_generation(gf_tables* gf_tables)
 * \brief Generates the Log and Anti-Log tables for a \f$ GF(2^{10}) \f$
 *
 * We will be working in the Galois Field \f$ GF(2^{m}) \f$ (with \f$ m = 10 \f$), this field can be constructed using the primitive polynomial \f$ p(X) = 1 + X^3 + X^{10}\f$ of degree \f$ 10 \f$ over \f$ GF(2) \f$. 
 * Let \f$p(\alpha) = 1 + \alpha^3 + \alpha^{10} = 0\f$, then using the identity \f$\alpha^{10} = \alpha^3 + 1\f$, we can construct a polynomial representation of the elements of \f$ GF(2^{10}) \f$.
 * We can construct a binary representation using the polynomial representation. In fact, let  \f$ a_0 + a_1 \alpha +a_2 \alpha^2 + ... + a_9 \alpha^{9}\f$ be the polynomial representation of \f$ \beta\f$,
 * then \f$ \beta \f$ can be represented by the binary 10-tuple \f$ (a_0, a_1, ... , a_9) \f$. In the other hand, we have a power representation, in fact \f$ GF(2^{10}) = \{0, 1, \alpha, \alpha^2, ..., \alpha^{2^{10} - 2} \}\f$.
 * Thus, we have three representations of \f$ GF(2^{10}) \f$; a polynomial representation, a binary representation and a power representation. 
 * To perform computations (additions and multiplications) in a Galois Field, we use look-up tables. As in \cite morelos2006art, we will use Log and Anti-Log tables:
 *    <ol>
 *    <li> Anti-Log table \f$ a(i)\f$: is used when performing additions. At index \f$ i \f$, it gives the value of the binary vector that corresponds to the element \f$ \alpha^i \f$.
 *    <li> Log table \f$ log(i) \f$: is used when performing multiplications. At index \f$ i \f$, it gives \f$ log(i) \f$ which is the power of \f$ \alpha^{log(i)} \f$ that corresponds to the binary vector represented by the integer \f$ i \f$.
 *    </ol>
 * Such that the following relation holds: \f$ \alpha^{log(i)} = a(i)\f$
 *
 * \param[in] gf_tables Pointer to a Log and Anti-Log tables
 */
void gf_generation(gf_tables* gf_tables) {
  const uint16_t k  = 1 << PARAM_M; // k = 2^m = 2^10
  const uint16_t poly = PARAM_POLY; // get the primitive polynomial
  uint16_t val = 1;
  uint16_t alpha = 2; // alpha the root of the primitive polynomial is the primitive element of GF(2^10)

  for(int i = 0 ; i < PARAM_GF_MUL_ORDER ; ++i){
    gf_tables->antilog_tab[i] = val;
    gf_tables->log_tab[val] = i;
    val = val * alpha; // by multiplying by alpha and reducing later if needed we generate all the elements of GF(2^10)
    if(val >= k){ // if val is greater than 2^10
      val ^= poly; // replace alpha^10 by alpha^3 + 1
    }
  }

  gf_tables->antilog_tab[PARAM_GF_MUL_ORDER] = 1; 
  gf_tables->log_tab[0] = -1; // by convention 
} 

/**
 * \fn syndrome_set* syndrome_init()
 * \brief Initializes a syndrome_set structure
 *
 * This function is used to initialize syndrome_set structure by allocating the necessary
 * memory to store a set of syndromes of a BCH code.
 *
 * \return a pointer to a syndrome_set
 */
syndrome_set* syndrome_init() {
  syndrome_set* synd_set = (syndrome_set*) malloc(sizeof(syndrome_set));
  synd_set->size = 2 * PARAM_DELTA;
  synd_set->tab = (int16_t*) calloc((synd_set->size), sizeof(int16_t));
  
  return synd_set;
}

/**
 * \fn void syndrome_gen(syndrome_set* synd_set, gf_tables* tables, uint8_t* v)
 * \brief Generates 2 * PARAM_DELTA syndromes from the received vector
 *
 * \param[out] synd_set Pointer to the structure syndrome_set that contains the computed syndromes
 * \param[in] tables Pointer to a gf_tables
 * \param[in] v Pointer to an array of unsigned char elements
 */
void syndrome_gen(syndrome_set* synd_set, gf_tables* tables, uint8_t* v) {
  uint8_t tmp_array[PARAM_N1];
  // For clarity of computation we separate the coordinates of the vector v by putting each coordinate in an unsigned char.
  for (uint8_t i = 0; i < (VEC_N1_SIZE_BYTES - 1) ; ++i) {
    for (uint8_t j = 0; j < 8; ++j) {
      tmp_array[j + i * 8] = (v[i] >> j) & 0X01;  
    }
  }

  for (uint8_t i = 0; i < PARAM_N1 % 8 ; ++i) {
    tmp_array [i + (VEC_N1_SIZE_BYTES - 1) * 8] = (v[VEC_N1_SIZE_BYTES -1] >> i) & 0x01;
  }

  // Evaluation of the polynomial corresponding to the vector v in alpha^i for i in {1, ..., 2 * PARAM_DELTA}
  for(uint16_t i = 0; i < PARAM_N1 ; ++i) {
    int tmp_value = 0;
    if(tmp_array[i]) {
      for(uint16_t j = 1 ; j < synd_set->size + 1 ; ++j) {      
        tmp_value = gf_mod(tmp_value + i);
        synd_set->tab[j - 1] ^= gf_get_antilog(tables, tmp_value);
      }
    }
  }
}

/**
 * \fn sigma_poly* sigma_poly_init(int16_t dim)
 * \brief Initializes a structure that can store a polynomial having coordinates in \f$ GF(2^{10}) \f$
 *
 * This function is used to initialize sigma_poly structure by allocating the necessary
 * memory.
 * 
 * \param[in] dim Integer that is the size of polynomial
 * \return a pointer to a sigma_poly
 */
sigma_poly* sigma_poly_init(int16_t dim) {
  sigma_poly* poly = (sigma_poly*) malloc(sizeof(sigma_poly));
  poly->dim = dim;
  poly->value = (int16_t*) calloc(dim,sizeof(int16_t));
  
  return poly;
}

/**
* \fn void get_error_location_poly(sigma_poly* sigma, gf_tables* tables, syndrome_set* synd_set)
* \brief Computes the error location polynomial \f$ \sigma\f$(x) (see the document <a href="../doc_bch.pdf" target="_blank"><b>BCH code</b></a>)
*         
* This function implements the simplified Berlekamp's algorithm for finding the error location polynomial for
* binary BCH codes given by Joiner and Komo in \cite joiner1995decoding.
* 
* \param[out] sigma Pointer to the structure sigma_poly that contains the computed polynomial
* \param[in] tables Pointer to a gf_tables
* \param[in] synd_set Pointer to the structure syndrome_set that contains the set of syndromes computed by the function syndrome_gen
*/
void get_error_location_poly(sigma_poly* sigma, gf_tables* tables, syndrome_set* synd_set) {
  // Find the error location polynomial via Berlekamp's simplified algorithm as described by
  // Laurie L. Joiner and John J. Komo, the comments are following their terminology

  uint32_t mu, tmp, l, d_rho = 1, d = synd_set->tab[0];
  sigma_poly* sigma_rho = sigma_poly_init(2 * PARAM_DELTA);
  sigma_poly* sigma_copy = sigma_poly_init(2 * PARAM_DELTA);
  int k, pp = -1;
  // initializations
  sigma_rho->deg = 0;
  sigma_rho->value[0] = 1;
  sigma->deg = 0;
  sigma->value[0] = 1;

  for (mu = 0; (mu < PARAM_DELTA) && (sigma->deg <= PARAM_DELTA); mu++) {
    // Step (2) in Joinder and Komo algorithm
    if (d) {
      k = 2*mu-pp;
      sigma_poly_copy(sigma_copy, sigma);
      // Compute d_mu * d__rho^(-1)                                
      tmp = gf_get_log(tables, d) + PARAM_GF_MUL_ORDER - gf_get_log(tables, d_rho);
      // Compute sigma(mu+1)[x]
      for (int i = 0; i <= sigma_rho->deg; i++) {
       if (sigma_rho->value[i]) {
         l = gf_get_log(tables, sigma_rho->value[i]);
         sigma->value[i+k] ^= gf_get_antilog(tables, (tmp + l) % PARAM_GF_MUL_ORDER);
       }
      }
      // Compute l_mu + 1 the degree of sigma(mu+1)[x]
      // and update the polynomial sigma_rho
      tmp = sigma_rho->deg + k;
      if (tmp > sigma->deg) {
       sigma->deg = tmp;
       sigma_poly_copy(sigma_rho, sigma_copy);
       d_rho = d;
       pp = 2 * mu;
      }
    }
    // Step (3) in Joinder and Komo algorithm
    // compute discrepancy d_mu+1
    if (mu < PARAM_DELTA - 1) {
      d = synd_set->tab[2*mu + 2];
      for (int i = 1; i <= sigma->deg; i++) {
        int tmp_val = gf_get_log(tables, synd_set->tab[2 * mu + 2 - i]);
        if((sigma->value[i])  && (tmp_val != -1))
        d ^= gf_get_antilog(tables, gf_mod(gf_get_log(tables, sigma->value[i]) + tmp_val));
      }           
    }

  }
  sigma_poly_clear(sigma_rho);
  sigma_poly_clear(sigma_copy);
}

/**
 * \fn void sigma_poly_copy(sigma_poly* p1, sigma_poly* p2)
 * \brief Copy a Galois Field polynomial p2 the Galois Field polynomial p1
 *
 * \param[out] p1 Pointer to a sigma_poly
 * \param[in] p2 Pointer to a sigma_poly
 */
void sigma_poly_copy(sigma_poly* p1, sigma_poly* p2) {
  for (int i = 0; i <= p2->deg ; ++i) {
    p1->value[i] = p2->value[i];
  }
  p1->deg = p2->deg;
}

/**
 * \fn void chien_search(uint16_t* error_pos, uint16_t* size, gf_tables* tables, sigma_poly* sigma)
 * \brief Computes the error location numbers
 *  
 * We use Chien procedure for searching error-location numbers \cite lin1983error \cite chien1964cyclic. The Chien search algorithm computes the roots of the 
 * error location polynomial and then returns their inverses. See the document <a href="../doc_bch.pdf" target="_blank"><b>BCH code</b></a> for more details about this algorithm.
 *  
 * \param[out] error_pos Pointer to an array that contains the error location numbers
 * \param[out] size Pointer to the size of the array that contains the error location numbers
 * \param[in] tables Pointer to gf_tables
 * \param[in] sigma Pointer to sigma_poly a structure that contains the error location polynomial
 */
void chien_search(uint16_t* error_pos, uint16_t* size, gf_tables* tables, sigma_poly* sigma) {
  int i = sigma->deg + 1;
  // Put the coordinates of the error location polynomial in the log format. Its better for multiplication.
  while(i--) {
    sigma->value[i] = gf_get_log(tables, sigma->value[i]);
  }
  
  int k = PARAM_GF_MUL_ORDER - PARAM_N1;

  int tmp = 0;

  // Compute sigma(alpha^k)
  for(uint16_t j = 1 ; j < sigma->deg + 1 ; ++j) {
    tmp = gf_mod(tmp + k);
    if (sigma->value[j] != -1) {
      sigma->value[j] = gf_mod(sigma->value[j] + tmp);
    }
  }
  // Evaluate sigma in a field element and check if it's a root of sigma
  *size = 0;
  for(int i = k + 1 ; i <= PARAM_GF_MUL_ORDER ; ++i) {
    int sum = 0;
    int j = sigma->deg + 1;
    while(--j) {
      if (sigma->value[j] != -1) {
        sigma->value[j] = gf_mod(sigma->value[j] + j);
        sum ^= gf_get_antilog(tables, sigma->value[j]);
      }
    }
    // Compute the inverse and update the list of error location numbers
    if (sum == 1) {
      error_pos[*size] = PARAM_GF_MUL_ORDER - i;
      ++(*size);
    }
  }
}

/**
 * \fn void error_poly_gen(uint8_t* e, uint16_t* error_pos, uint16_t size)
 * \brief Computes the error polynomial which correspond to \f$ e\f$(x) in the document <a href="../doc_bch.pdf" target="_blank"><b>BCH code</b></a>
 *
 * This function puts the error location numbers in a binary polynomial. For example if the error location numbers are \f$ \alpha^{130} \f$, \f$ \alpha^{80} \f$, and \f$ \alpha^{11} \f$,
 * then the error polynomial is \f$ X^{11} + X^{80} + X^{130} \f$ 
 *  
 * \param[out] e Pointer to an array that contains the error polynomial
 * \param[in] error_pos Pointer to an array that contains the error location numbers
 * \param[in] size Integer that is the size of the array error_pos
 */
void error_poly_gen(uint8_t* e, uint16_t* error_pos, uint16_t size) {
  for (int i = 0; i < size; ++i) {
    int index = error_pos[i] / 8;
    e[index] ^= 0x01 << (error_pos[i] % 8);
  }
}

/**
 * \fn void message_from_codeword(uint8_t* o, uint8_t* v)
 * \brief Finds the message from the decoded code word
 *  
 * This function recover the message (informations bits) from a code word. Since we have performed a systematic encoding, the message is the last PARAM_K bits of the vector v.
 *
 * \param[out] o Pointer to an array that contains the message
 * \param[in] v Pointer to an array that contains a code word
 */
void message_from_codeword(uint8_t* o, uint8_t* v) {
  int val = PARAM_N1 - PARAM_K;

  uint8_t mask_m1 = 0xF0;
  uint8_t mask_m2 = 0x0F;

  for (uint8_t i = 0 ; i < VEC_K_SIZE_BYTES ; ++i) {
    int index = (val / 8) + i ;
    uint8_t  m1 = (v[index] & mask_m1) >> (val % 8);
    uint8_t  m2 = (v[index + 1] & mask_m2) << (8 - (val % 8));
    o[i] = m1 | m2;
  }
}

/**
 * \fn void gf_tables_clear(gf_tables* gf_tables)
 * \brief Free the allocated memory for a gf_tables
 *
 * \param[in] gf_tables Pointer to a Galois Field Log and Anti-Log tables
 */
void gf_tables_clear(gf_tables* gf_tables) {
  free(gf_tables->log_tab);
  free(gf_tables->antilog_tab);
  free(gf_tables);
}

/**
 * \fn void syndrome_clear(syndrome_set* synd_set)
 * \brief Free the allocated memory for a syndrome_set
 *
 * \param[in] synd_set Pointer to a syndrome_set
 */
void syndrome_clear(syndrome_set* synd_set) {
  free(synd_set->tab);
  free(synd_set);
}

/**
 * \fn void sigma_poly_clear(sigma_poly* poly)
 * \brief Free the allocated memory for a sigma_poly
 *
 * \param[in] poly Pointer to a sigma_poly structure
 * \return void
 */
void sigma_poly_clear(sigma_poly* poly){
  free(poly->value);
  free(poly);
}

/**
 * \fn int16_t gf_mult(gf_tables* tables, int16_t a, int16_t b)
 * \brief Multiply two elements in GF(2^m)
 *
 * \param[in] tables Pointer to gf_tables
 * \param[in] a Integer that is in GF(2^m)
 * \param[in] b Integer that is in GF(2^m)
 * \return integer that is the result 
 */
int16_t gf_mult(gf_tables* tables, int16_t a, int16_t b) {
  return (a && b) ? tables->antilog_tab[ gf_mod(tables->log_tab[a] + tables->log_tab[b]) ] : 0;
} 

/**
 * \fn cyclotomic_sets* cyclotomic_init()
 * \brief Initializes a structure that can store the cyclotomic sets
 * 
 * \return a pointer to a cyclotomic_sets
 */
cyclotomic_sets* cyclotomic_init() {
  cyclotomic_sets* c_tab = (cyclotomic_sets*) malloc(sizeof(cyclotomic_sets));
  c_tab->nb_c = 0;
  c_tab->tab = (int8_t*) calloc(PARAM_GF_MUL_ORDER + 1, sizeof(int8_t));
  return c_tab;
}

/**
 * \fn void cyclotomic_clear(cyclotomic_sets* c_tab)
 * \brief Free the allocated memory for a cyclotomic_sets
 *
 * \param[in] c_tab Pointer to a cyclotomic_sets structure
 * \return void
 */
void cyclotomic_clear(cyclotomic_sets* c_tab) {
  free(c_tab->tab);
  free(c_tab);
}

/**
 * \fn void cyclotomic_gen(cyclotomic_sets* c_tab)
 * \brief Generate the cyclotomic sets
 *
 * \param[in] c_tab Pointer to a cyclotomic_sets structure
 * \return void
 */
void cyclotomic_gen(cyclotomic_sets* c_tab) {
  int i,j;
  int tmp;
  int t2 = 2 * PARAM_DELTA;
  c_tab->tab[0] = 0;
  
  for(i = 1 ; i < t2 ; i+=2){ // Compute the odd cyclotomic classes 
    if( !(c_tab->tab[i]) ){ // Check if an element is not in a cyclotomic classe and that i is the smallest index
      c_tab->nb_c++;
      c_tab->tab[i] = i;
      tmp = i;
      j = PARAM_M;
      while(--j){
        tmp = gf_mod(tmp << 1);
        c_tab->tab[tmp] = i;
      }
    }
  }
}

/**
 * \fn uint8_t* polynomial_hex(int16_t* g)
 * \brief Put the generator polynomial in hexadecimal form
 *
 * \param[in] g Array that is the binary representation of the generator polynomial
 * \return An array of unsigned chars that is the hexadecimal representation of the generator polynomial
 */
uint8_t* polynomial_hex(int16_t* g) {
  int size = PARAM_G / 8 + 1;
  uint8_t* hex = (uint8_t*) calloc(size, sizeof(uint8_t));

  for (int i = 0; i < size - 1 ; ++i) {
    for (int j = 0; j < 8; ++j) {
      hex[i] ^= ((uint8_t) g[j + (i<<3)]) << (7 - j); 
    }
  }
  
  for (int i = 0; i < PARAM_G % 8; ++i) {
    hex[size - 1] ^= ((uint8_t) g[i + ((size - 1) << 3)]) << (7 - i);
  }

  for (int i = 0; i < size; ++i) {
    printf("%02X", hex[i]);
  }
  printf("\n");
  return hex;
}

/**
 * \fn void compute_generator_poly(int16_t* g)
 * \brief Compute the generator polynomial
 *
 * \param[out] g Array that is the binary representation of the generator polynomial
 * \return void
 */
void compute_generator_poly(int16_t* g) {
  gf_tables* tables =  gf_tables_init();
  gf_generation(tables);
  
  cyclotomic_sets* set = cyclotomic_init();
  cyclotomic_gen(set);
  
  g[0] = 1; // g(x) = 1
  int deg_g = 0; // deg(g) = 0

  for (int i = 0 ; i < PARAM_GF_MUL_ORDER ; ++i) {
    if(set->tab[i]) {
      int tmp = gf_get_antilog(tables, i);
      g[deg_g + 1] = 1; // Set the greater degree to 1
      for(int j = deg_g ; j > 0 ; j--) {
        g[j] = gf_mult(tables, g[j], tmp) ^ (g[j-1]);
      }
      g[0] = gf_mult(tables, g[0], tmp); // Multiply the root by the last element of the polynomial 
      deg_g ++;
    }
  }
  gf_tables_clear(tables);
  cyclotomic_clear(set);
}