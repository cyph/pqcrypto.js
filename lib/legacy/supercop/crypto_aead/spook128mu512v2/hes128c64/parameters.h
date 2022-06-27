/* Spook Reference Implementation v1
 *
 * Written in 2019 at UCLouvain (Belgium) by Olivier Bronchain, Gaetan Cassiers
 * and Charles Momin.
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with
 * this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */
#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

// Selects the "su" variant if set to 0 and the "mu" variant if set to "1"
#ifndef MULTI_USER
#define MULTI_USER 1
#endif // MULTI_USER

// Selects the "512" variant if set to 0 and the "384" variant if set to "1"
#ifndef SMALL_PERM
#define SMALL_PERM 0
#endif // SMALL_PERM

#if MULTI_USER
#define KEYBYTES 32
#else
#define KEYBYTES 16
#endif

#include "api.h"

#if (KEYBYTES != CRYPTO_KEYBYTES)
#error "Wrong parameters in api.h"
#endif

#define CLYTE_TYPE_64_BIT
#define SHADOW_TYPE_128_BIT
#endif //_PARAMETERS_H_
