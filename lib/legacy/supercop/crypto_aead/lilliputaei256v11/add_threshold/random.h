/*
Implementation of the Lilliput-AE tweakable block cipher.

Authors, hereby denoted as "the implementer":
    Kévin Le Gouguec,
    2019.

For more information, feedback or questions, refer to our website:
https://paclido.fr/lilliput-ae

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/

---

This file provides an interface to generate random bytes.
*/

#ifndef RANDOM_H
#define RANDOM_H

#include <stddef.h>
#include <stdint.h>


void randombytes(size_t nb, uint8_t out[nb]);


#endif /* RANDOM_H */
