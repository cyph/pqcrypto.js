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

"""Lilliput-II Authenticated Encryption mode.

This module provides the functions for authenticated encryption and decryption
using Lilliput-AE's nonce-misuse-resistant mode based on SCT-2.
"""

from enum import Enum

from .constants import BLOCK_BYTES
from .ae_common import (
    bytes_to_block_matrix,
    block_matrix_to_bytes,
    integer_to_byte_array,
    build_auth,
    pad10,
    TagValidationError,
    xor
)
from . import tbc


TWEAK_BITS = 128
TWEAK_BYTES = TWEAK_BITS//8


class _TagTweak(Enum):
    BLOCK = 0b0000
    PAD = 0b0100


def _tweak_tag(j, prefix):
    tweak = integer_to_byte_array(j, TWEAK_BYTES)

    # Clear upper 4 bits and set them to prefix.
    tweak[0] &= 0b00001111
    tweak[0] |= prefix.value << 4

    return tweak


def _add_tag_j(tag, j):
    tweak = xor(tag, integer_to_byte_array(j, TWEAK_BYTES))
    tweak[0] |= 0b10000000

    return tweak


def _message_auth_tag(M, N, Auth, key):
    l = len(M)//BLOCK_BYTES
    need_padding = len(M)%BLOCK_BYTES > 0

    tag = list(Auth)
    M = bytes_to_block_matrix(M)

    for j in range(0, l):
        tweak = _tweak_tag(j, _TagTweak.BLOCK)
        encryption = tbc.encrypt(tweak, key, M[j])
        tag = xor(tag, encryption)

    if need_padding:
        tweak = _tweak_tag(l, _TagTweak.PAD)
        encryption = tbc.encrypt(tweak, key, pad10(M[l]))
        tag = xor(tag, encryption)

    tag = tbc.encrypt([0b00010000]+N, key, tag)

    return tag


def _message_encryption(M, N, tag, key):
    l = len(M)//BLOCK_BYTES
    need_padding = len(M)%BLOCK_BYTES > 0

    M = bytes_to_block_matrix(M)
    C = []

    for j in range(0, l):
        tweak = _add_tag_j(tag, j)
        encryption = tbc.encrypt(tweak, key, [0b00000000]+N)
        C.append(xor(M[j], encryption))

    if need_padding:
        tweak = _add_tag_j(tag, l)
        encryption = tbc.encrypt(tweak, key, [0b00000000]+N)
        C.append(xor(M[l], encryption))

    return C


def encrypt(A, M, N, key):
    K = list(key)
    N = list(N)

    Auth = build_auth(TWEAK_BITS, A, K)
    tag = _message_auth_tag(M, N, Auth, K)
    C = _message_encryption(M, N, tag, K)

    return block_matrix_to_bytes(C), bytes(tag)


def decrypt(A, C, N, tag, key):
    K = list(key)
    N = list(N)
    tag = list(tag)

    M = block_matrix_to_bytes(
        _message_encryption(C, N, tag, K)
    )
    Auth = build_auth(TWEAK_BITS, A, K)
    tag2 = _message_auth_tag(M, N, Auth, K)

    if tag != tag2:
        raise TagValidationError(tag, tag2)

    return M
