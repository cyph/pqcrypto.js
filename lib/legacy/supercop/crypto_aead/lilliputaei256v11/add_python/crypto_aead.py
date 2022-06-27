# Implementation of the Lilliput-AE tweakable block cipher.
#
# Authors, hereby denoted as "the implementer":
#     Kévin Le Gouguec,
#     2019.
#
# For more information, feedback or questions, refer to our website:
# https://paclido.fr/lilliput-ae
#
# To the extent possible under law, the implementer has waived all copyright
# and related or neighboring rights to the source code in this file.
# http://creativecommons.org/publicdomain/zero/1.0/

"""Python port of the crypto_aead API for Lilliput-AE."""

import lilliput

from lilliput.constants import (
    NONCE_BITS,
    TAG_BYTES
)

from parameters import (
    KEYBYTES,                   # Expose to genkat_aead.
    MODE
)


NPUBBYTES = NONCE_BITS//8


def encrypt(m, ad, npub, k):
    c, tag = lilliput.encrypt(m, ad, k, npub, MODE)
    return c+tag


def decrypt(c, ad, npub, k):
    ctext = c[:-TAG_BYTES]
    tag = c[-TAG_BYTES:]
    return lilliput.decrypt(ctext, tag, ad, k, npub, MODE)
