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

"""Lilliput-TBC tweakable block cipher.

This module provides functions to encrypt and decrypt blocks of 128 bits.
"""

from .constants import BLOCK_BYTES, SBOX
from .helpers import xor
from .multiplications import ALPHAS


_PERMUTATION = [14, 11, 12, 10, 8, 9, 13, 15, 3, 1, 4, 5, 6, 0, 2, 7]
_PERMUTATION_INV = [13, 9, 14, 8, 10, 11, 12, 15, 4, 5, 3, 1, 2, 6 ,0 ,7]


_ROUNDS = {
    128: 32,
    192: 36,
    256: 42
}


def _build_tweakey(tweak, key):
    return tweak+key


def _lane(TK, j):
    return TK[j*8:(j+1)*8]


def _round_tweakey_schedule(tweakey):
    p = len(tweakey)//8

    multiplied_lanes = (
        ALPHAS[j](_lane(tweakey, j)) for j in range(p)
    )

    return [byte for lane in multiplied_lanes for byte in lane]


def _subtweakey_extract(tweakey, Ci):
    RTKi = [0]*8

    for j, byte in enumerate(tweakey):
        RTKi[j%8] ^= byte

    RTKi[0] ^= Ci

    return RTKi


def _tweakey_schedule_whole(tweakey, r):
    TKs = [tweakey]
    RTKs = [_subtweakey_extract(TKs[0], 0)]

    for i in range(1, r):
        TKs.append(_round_tweakey_schedule(TKs[i-1]))
        RTKs.append(_subtweakey_extract(TKs[i], i))

    return RTKs


def _non_linear_layer(state, subtweakey):
    variables_xored = xor(state, subtweakey)

    variables_sboxed = [
        SBOX[variables_xored[i]] for i in range(8)
    ]

    state_output = state
    for i in range(8):
        state_output[15-i] ^= variables_sboxed[i]

    return state_output


def _linear_layer(state):
    state_output = state

    for byte in range(1, 8):
        state_output[15] ^= state[byte]

    for byte in range(9, 15):
        state_output[byte] ^= state[7]

    return state_output


def _permutation_layer(state, p):
    return [
        state[p[i]] for i in range(BLOCK_BYTES)
    ]


def _one_round_egfn_enc(state, subtweakey):
    state_non_linear = _non_linear_layer(state, subtweakey)
    state_linear = _linear_layer(state_non_linear)
    state_permutation = _permutation_layer(state_linear, _PERMUTATION)

    return state_permutation


def _last_round_egfn(state, subtweakey):
    state_non_linear = _non_linear_layer(state, subtweakey)
    state_linear = _linear_layer(state_non_linear)

    return state_linear


def _one_round_egfn_dec(state, subtweakey):
    state_non_linear = _non_linear_layer(state, subtweakey)
    state_linear = _linear_layer(state_non_linear)
    state_permutation = _permutation_layer(state_linear, _PERMUTATION_INV)

    return state_permutation


def encrypt(tweak, key, message):
    r = _ROUNDS[8*len(key)]

    tweakey = _build_tweakey(tweak, key)
    RTKs = _tweakey_schedule_whole(tweakey, r)

    state = message

    for i in range(r-1):
        state = _one_round_egfn_enc(state, RTKs[i])

    return _last_round_egfn(state, RTKs[r-1])


def decrypt(tweak, key, cipher):
    r = _ROUNDS[8*len(key)]

    tweakey = _build_tweakey(tweak, key)
    RTKs = _tweakey_schedule_whole(tweakey, r)

    state = cipher

    for i in range(r-1):
        state = _one_round_egfn_dec(state, RTKs[r-i-1])

    return _last_round_egfn(state, RTKs[0])
