/********************************************************************************************
* FrodoKEM: Learning with Errors Key Encapsulation
*
* Abstract: configuration file
*********************************************************************************************/

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define _USE_OPENSSL_

// Selecting use of OpenSSL's AES functions
#if defined(_USE_OPENSSL_)
    #define USE_OPENSSL
#endif

// Macro to avoid compiler warnings when detecting unreferenced parameters
#define UNREFERENCED_PARAMETER(PAR) ((void)(PAR))


#endif
