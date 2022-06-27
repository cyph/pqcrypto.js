# Implementation of the Lilliput-AE tweakable block cipher.
#
# Authors, hereby denoted as "the implementer":
#     Kévin Le Gouguec,
#     Léo Reynaud
#     2019.
#
# For more information, feedback or questions, refer to our website:
# https://paclido.fr/lilliput-ae
#
# To the extent possible under law, the implementer has waived all copyright
# and related or neighboring rights to the source code in this file.
# http://creativecommons.org/publicdomain/zero/1.0/

"""Lilliput-AE tweakable block cipher.

This module provides the high-level functions for authenticated encryption and
decryption.  Both functions take and return bytestring values.

The "mode" argument can be either of the following integers:

- 1, for the ΘCB3 nonce-respecting mode,
- 2, for the SCT-2 nonce-misuse-resistant mode.
"""


from . import ae_mode_1
from . import ae_mode_2
from .constants import NONCE_BITS


_AE_MODES = {
    1: ae_mode_1,
    2: ae_mode_2
}


def _check_inputs(key, mode, nonce):
    valid_key_lengths = (128, 192, 256)
    if len(key)*8 not in valid_key_lengths:
        raise ValueError('invalid key size: {} not in {}'.format(len(key)*8, valid_key_lengths))

    if mode not in _AE_MODES:
        raise ValueError('invalid mode: {} not in {}'.format(mode, tuple(_AE_MODES)))

    if len(nonce)*8 != NONCE_BITS:
        raise ValueError('invalid nonce size: expecting {}, have {}'.format(NONCE_BITS, len(nonce)*8))


def encrypt(plaintext, adata, key, nonce, mode):
    _check_inputs(key, mode, nonce)
    return _AE_MODES[mode].encrypt(adata, plaintext, nonce, key)


def decrypt(ciphertext, tag, adata, key, nonce, mode):
    _check_inputs(key, mode, nonce)
    return _AE_MODES[mode].decrypt(adata, ciphertext, nonce, tag, key)
