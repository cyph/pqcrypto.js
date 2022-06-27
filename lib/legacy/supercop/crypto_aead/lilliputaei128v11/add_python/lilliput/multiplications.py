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

"""Multiplications for Lilliput-TBC's tweakey schedule.

This module provides a list of functions implementing lane multiplications,
from ALPHAS[0] = α₀ = I to ALPHAS[6] = α₆ = M_R³.
"""


from functools import reduce
from operator import xor


def _shl(xi, n):
    return (xi << n) & 0xff

def _Sl(n):
    return lambda xi: _shl(xi, n)

def _Sr(n):
    return lambda xi: xi>>n

def _Id(xi):
    return xi

def _0(xi):
    return 0

def _M1(xi):
    return _shl(xi, 3) ^ xi>>3

def _M2(xi):
    return _shl(xi, 6) ^ xi&0b11111000 ^ xi>>6

def _M3(xi):
    return _shl(xi>>3, 6) ^ xi>>6<<3

def _M4(xi):
    return _shl(xi, 2) >> 3

def _M5(xi):
    return _shl(xi, 5) ^ xi>>3<<2

def _M6(xi):
    return xi & 0b00011111

def _M7(xi):
    return _shl(xi, 2) >> 3


M = (
    ( _0,    _Id,     _0,     _0,  _0,  _0,  _0,  _0),
    ( _0,     _0,    _Id,     _0,  _0,  _0,  _0,  _0),
    ( _0,     _0, _Sl(3),    _Id,  _0,  _0,  _0,  _0),
    ( _0,     _0,     _0, _Sr(3), _Id,  _0,  _0,  _0),
    ( _0,     _0,     _0,     _0,  _0, _Id,  _0,  _0),
    ( _0, _Sl(2),     _0,     _0,  _0,  _0, _Id,  _0),
    ( _0,     _0,     _0,     _0,  _0,  _0,  _0, _Id),
    (_Id,     _0,     _0,     _0,  _0,  _0,  _0,  _0),
)

M2 = (
    ( _0,     _0,    _Id,     _0,     _0,  _0,  _0,  _0),
    ( _0,     _0, _Sl(3),    _Id,     _0,  _0,  _0,  _0),
    ( _0,     _0, _Sl(6),    _M1,    _Id,  _0,  _0,  _0),
    ( _0,     _0,     _0, _Sr(6), _Sr(3), _Id,  _0,  _0),
    ( _0, _Sl(2),     _0,     _0,     _0,  _0, _Id,  _0),
    ( _0,     _0, _Sl(2),     _0,     _0,  _0,  _0, _Id),
    (_Id,     _0,     _0,     _0,     _0,  _0,  _0,  _0),
    ( _0,    _Id,     _0,     _0,     _0,  _0,  _0,  _0),
)

M3 = (
    ( _0,     _0, _Sl(3),    _Id,     _0,     _0,  _0,  _0),
    ( _0,     _0, _Sl(6),    _M1,    _Id,     _0,  _0,  _0),
    ( _0,     _0,     _0,    _M2,    _M1,    _Id,  _0,  _0),
    ( _0, _Sl(2),     _0,     _0, _Sr(6), _Sr(3), _Id,  _0),
    ( _0,     _0, _Sl(2),     _0,     _0,     _0,  _0, _Id),
    (_Id,     _0, _Sl(5), _Sl(2),     _0,     _0,  _0,  _0),
    ( _0,    _Id,     _0,     _0,     _0,     _0,  _0,  _0),
    ( _0,     _0,    _Id,     _0,     _0,     _0,  _0,  _0),
)

M4 = (
    ( _0,     _0, _Sl(6),    _M1,    _Id,     _0,     _0,  _0),
    ( _0,     _0,     _0,    _M2,    _M1,    _Id,     _0,  _0),
    ( _0, _Sl(2),     _0,    _M3,    _M2,    _M1,    _Id,  _0),
    ( _0,    _M4, _Sl(2),     _0,     _0, _Sr(6), _Sr(3), _Id),
    (_Id,     _0, _Sl(5), _Sl(2),     _0,     _0,     _0,  _0),
    ( _0,    _Id,     _0,    _M5, _Sl(2),     _0,     _0,  _0),
    ( _0,     _0,    _Id,     _0,     _0,     _0,     _0,  _0),
    ( _0,     _0, _Sl(3),    _Id,     _0,     _0,     _0,  _0),
)

# NB: shift directions are reversed with respect to the specification
# for powers of M_R, since the specification reverses the byte order
# for those matrices.

MR = (
    ( _0, _Id,  _0,     _0,     _0,  _0,     _0,  _0),
    ( _0,  _0, _Id,     _0,     _0,  _0,     _0,  _0),
    ( _0,  _0,  _0,    _Id, _Sr(3),  _0,     _0,  _0),
    ( _0,  _0,  _0,     _0,    _Id,  _0,     _0,  _0),
    ( _0,  _0,  _0,     _0,     _0, _Id, _Sl(3),  _0),
    ( _0,  _0,  _0, _Sl(2),     _0,  _0,    _Id,  _0),
    ( _0,  _0,  _0,     _0,     _0,  _0,     _0, _Id),
    (_Id,  _0,  _0,     _0,     _0,  _0,     _0,  _0),
)

MR2 = (
    ( _0,  _0, _Id,     _0,     _0,     _0,     _0,     _0),
    ( _0,  _0,  _0,    _Id, _Sr(3),     _0,     _0,     _0),
    ( _0,  _0,  _0,     _0,    _Id, _Sr(3),    _M6,     _0),
    ( _0,  _0,  _0,     _0,     _0,    _Id, _Sl(3),     _0),
    ( _0,  _0,  _0, _Sl(2),     _0,     _0,    _Id, _Sl(3)),
    ( _0,  _0,  _0,     _0, _Sl(2),     _0,     _0,    _Id),
    (_Id,  _0,  _0,     _0,     _0,     _0,     _0,     _0),
    ( _0, _Id,  _0,     _0,     _0,     _0,     _0,     _0),
)

MR3 = (
    (    _0,  _0,  _0,    _Id, _Sr(3),     _0,     _0,     _0),
    (    _0,  _0,  _0,     _0,    _Id, _Sr(3),    _M6,     _0),
    (    _0,  _0,  _0,    _M7,     _0,    _Id,    _M1,    _M6),
    (    _0,  _0,  _0, _Sl(2),     _0,     _0,    _Id, _Sl(3)),
    (_Sl(3),  _0,  _0,     _0, _Sl(2),     _0,     _0,    _Id),
    (   _Id,  _0,  _0,     _0,     _0, _Sl(2), _Sl(5),     _0),
    (    _0, _Id,  _0,     _0,     _0,     _0,     _0,     _0),
    (    _0,  _0, _Id,     _0,     _0,     _0,     _0,     _0),
)


def _multiplication(m, reverse=True):
    def ordered(l):
        if reverse:
            return list(reversed(list(l)))
        return l

    def _multiply(x):
        return ordered(
            reduce(xor, (mj[i](xi) for i, xi in enumerate(ordered(x))))
            for mj in m
        )

    return _multiply


ALPHAS = (
    _multiplication(M),
    _multiplication(M2),
    _multiplication(M3),
    _multiplication(M4),
    _multiplication(MR, reverse=False),
    _multiplication(MR2, reverse=False),
    _multiplication(MR3, reverse=False)
)
