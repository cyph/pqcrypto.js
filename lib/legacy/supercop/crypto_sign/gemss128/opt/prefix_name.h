#ifndef _PREFIX_NAME_H
#define _PREFIX_NAME_H

#include "macro.h"
#include "parameters_HFE.h"

/** This macro permits to prefix the name of all functions with the name of the
 *  current cryptosystem. */
/* For example, if you use GeMSS with a level of security 128 bits, 
   PREFIX_NAME(function) will generate GeMSS_128U_function. */
#define PREFIX_NAME(name) CONCAT(CONCAT(CRYPTO_NAME,K),_##name)



#endif
