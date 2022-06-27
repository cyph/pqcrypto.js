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

This file provides a system-specific function to generate random bytes.
*/

/* glibc < 2.25 does not provide getrandom(2): use the system call. */

#define _GNU_SOURCE

#include <stddef.h>
#include <stdint.h>

#include <unistd.h>
#include <sys/syscall.h>

#include "random.h"


void randombytes(size_t nb, uint8_t out[nb])
{
    syscall(SYS_getrandom, out, nb, 0);
}
