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

"""Lilliput-I Authenticated Encryption mode.

This module provides the functions for authenticated encryption and decryption
using Lilliput-AE's nonce-respecting mode based on ΘCB3.
"""

from enum import Enum

from .constants import BLOCK_BYTES, NONCE_BITS
from .ae_common import (
    bytes_to_block_matrix,
    block_matrix_to_bytes,
    build_auth,
    integer_to_byte_array,
    pad10,
    TagValidationError,
    xor
)
from . import tbc


TWEAK_BITS = 192
TWEAK_BYTES = TWEAK_BITS//8


class _MessageTweak(Enum):
    BLOCK = 0b0000
    NO_PADDING = 0b0001
    PAD = 0b0100
    FINAL = 0b0101


def _upper_nibble(i):
    return i >> 4


def _lower_nibble(i):
    return i & 0b00001111


def _byte(high, low):
    return high<<4 ^ low


def _tweak_message(N, j, prefix):
    tweak = [_byte(prefix.value, _upper_nibble(N[0]))]

    tweak.extend(
        _byte(_lower_nibble(N[i-1]), _upper_nibble(N[i]))
        for i in range(1, NONCE_BITS//8)
    )

    # j is encoded on 68 bits; get 72 then set the upper 4 to the
    # nonce's lower 4.
    j_len = (TWEAK_BITS-NONCE_BITS-4)//8 + 1
    j_array = integer_to_byte_array(j, j_len)
    j_array[0] &= 0b00001111
    j_array[0] |= _lower_nibble(N[-1]) << 4

    tweak.extend(j_array)

    return tweak


def _treat_message_enc(M, N, key):
    checksum = [0]*BLOCK_BYTES

    l = len(M)//BLOCK_BYTES
    padding_bytes = len(M)%BLOCK_BYTES

    M = bytes_to_block_matrix(M)
    C = []

    for j in range(0, l):
        checksum = xor(checksum, M[j])
        tweak = _tweak_message(N, j, _MessageTweak.BLOCK)
        C.append(tbc.encrypt(tweak, key, M[j]))

    if padding_bytes == 0:
        tweak = _tweak_message(N, l, _MessageTweak.NO_PADDING)
        Final = tbc.encrypt(tweak, key, checksum)

    else:
        m_padded = pad10(M[l])
        checksum = xor(checksum, m_padded)
        tweak = _tweak_message(N, l, _MessageTweak.PAD)
        pad = tbc.encrypt(tweak, key, [0]*BLOCK_BYTES)

        C.append(xor(M[l], pad[:padding_bytes]))
        tweak_final = _tweak_message(N, l+1, _MessageTweak.FINAL)
        Final = tbc.encrypt(tweak_final, key, checksum)

    return Final, C


def _treat_message_dec(C, N, key):
    checksum = [0]*BLOCK_BYTES

    l = len(C)//BLOCK_BYTES
    padding_bytes = len(C)%BLOCK_BYTES

    C = bytes_to_block_matrix(C)
    M = []

    for j in range(0, l):
        tweak = _tweak_message(N, j, _MessageTweak.BLOCK)
        M.append(tbc.decrypt(tweak, key, C[j]))
        checksum = xor(checksum, M[j])

    if padding_bytes == 0:
        tweak = _tweak_message(N, l, _MessageTweak.NO_PADDING)
        Final = tbc.encrypt(tweak, key, checksum)

    else:
        tweak = _tweak_message(N, l, _MessageTweak.PAD)
        pad = tbc.encrypt(tweak, key, [0]*BLOCK_BYTES)
        M.append(xor(C[l], pad[:padding_bytes]))

        m_padded = pad10(M[l])
        checksum = xor(checksum, m_padded)
        tweak_final = _tweak_message(N, l+1, _MessageTweak.FINAL)
        Final = tbc.encrypt(tweak_final, key, checksum)

    return Final, M


def encrypt(A, M, N, key):
    K = list(key)
    N = list(N)

    Auth = build_auth(TWEAK_BITS, A, K)
    Final, C = _treat_message_enc(M, N, K)
    tag = xor(Auth, Final)

    return block_matrix_to_bytes(C), bytes(tag)


def decrypt(A, C, N, tag, key):
    K = list(key)
    N = list(N)
    tag = list(tag)

    Auth = build_auth(TWEAK_BITS, A, K)
    Final, M = _treat_message_dec(C, N, K)
    tag2 = xor(Auth, Final)

    if tag != tag2:
        raise TagValidationError(tag, tag2)

    return block_matrix_to_bytes(M)
