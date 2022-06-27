/*
Implementation by the Keccak Team, namely, Guido Bertoni, Joan Daemen,
Michaël Peeters, Gilles Van Assche and Ronny Van Keer, 
hereby denoted as "the implementer".

For more information, feedback or questions, please refer to our website:
https://keccak.team/

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include <string.h>
#include "crypto_core.h"
#include "KeccakP-1600-SnP.h"

int crypto_core(unsigned char *out, const unsigned char *in, const unsigned char *k, const unsigned char *c)
{
    if (out != in)
        memcpy(out, in, 200);
    KeccakP1600_Permute_24rounds(out);
    return 0;
}
