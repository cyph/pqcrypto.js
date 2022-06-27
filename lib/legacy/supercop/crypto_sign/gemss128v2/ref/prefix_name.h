#ifndef _PREFIX_NAME_H
#define _PREFIX_NAME_H

#include "choice_crypto.h"
#include "parameters_HFE.h"


/** Auxiliar macro. */
#define CONCAT_NAME2(a,b) a ## b
/** This macro permits to concat the names. */
#define CONCAT_NAME(a,b) CONCAT_NAME2(a,b)


/** This macro permits to prefix the name of all functions with the name of the
 *  current cryptosystem. */
/* For example, if you use GeMSS with a level of security 128 bits, 
   PREFIX_NAME(function) will generate GeMSS_128U_function. */
#define PREFIX_NAME(name) CONCAT_NAME(CONCAT_NAME(CRYPTO_NAME,K),_##name)


#endif

