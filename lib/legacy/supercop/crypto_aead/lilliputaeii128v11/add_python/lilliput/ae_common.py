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

"""Helper functions used in both Lilliput-I and Lilliput-II."""


from .constants import BLOCK_BYTES
from .helpers import xor
from . import tbc


def bytes_to_block_matrix(array):
    vector = list(array)

    blocks_nb = len(vector)//BLOCK_BYTES

    block_starts = (
        i*BLOCK_BYTES for i in range(blocks_nb)
    )

    matrix = [
        vector[start:start+BLOCK_BYTES] for start in block_starts
    ]

    padding_len = len(vector)%BLOCK_BYTES

    if padding_len > 0:
        padding = vector[-padding_len:]
        matrix.append(padding)

    return matrix


def block_matrix_to_bytes(matrix):
    return bytes(byte for block in matrix for byte in block)


def pad10(X):
    zeroes = [0] * (BLOCK_BYTES-len(X)-1)
    return X + [0b10000000] + zeroes


def integer_to_byte_array(i, n):
    return list(i.to_bytes(n, 'big'))


def _tweak_associated_data(t, i, padded):
    tweak = integer_to_byte_array(i, t//8)

    prefix = 0b0110 if padded else 0b0010

    # Clear upper 4 bits and set them to prefix.
    tweak[0] &= 0b00001111
    tweak[0] |= prefix << 4

    return tweak


def build_auth(t, A, key):
    Auth = [0]*BLOCK_BYTES

    l_a = len(A)//BLOCK_BYTES
    need_padding = len(A)%BLOCK_BYTES > 0

    A = bytes_to_block_matrix(A)

    for i in range(l_a):
        tweak = _tweak_associated_data(t, i, padded=False)
        enc = tbc.encrypt(tweak, key, A[i])
        Auth = xor(Auth, enc)

    if not need_padding:
        return Auth

    tweak = _tweak_associated_data(t, l_a, padded=True)
    ad_padded = pad10(A[l_a])
    enc = tbc.encrypt(tweak, key, ad_padded)
    Auth = xor(Auth, enc)

    return Auth


class TagValidationError(Exception):
    def __init__(self, announced, computed):
        msg = '\n'.join((
            'Invalid tag:',
            announced.hex().upper()+' (announced)',
            computed.hex().upper()+' (computed)'
        ))

        super().__init__(msg)
        self._announced = announced
        self._computed = computed
