int64 b64
int64 synd
int64 addr
int64 c
int64 c_all
int64 row
int64 p
int64 e
int64 s
reg128 pp
reg128 ee
reg128 ss
int64 buf_ptr
stack128 buf
enter syndrome_asm
input_1 += 260780
buf_ptr = &buf
row = 768

loop:

row -= 1
ss = mem128[ input_1 + 0 ]
ee = mem128[ input_2 + 96 ]
ss &= ee
pp = mem128[ input_1 + 16 ]
ee = mem128[ input_2 + 112 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 32 ]
ee = mem128[ input_2 + 128 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 48 ]
ee = mem128[ input_2 + 144 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 64 ]
ee = mem128[ input_2 + 160 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 80 ]
ee = mem128[ input_2 + 176 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 96 ]
ee = mem128[ input_2 + 192 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 112 ]
ee = mem128[ input_2 + 208 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 128 ]
ee = mem128[ input_2 + 224 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 144 ]
ee = mem128[ input_2 + 240 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 160 ]
ee = mem128[ input_2 + 256 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 176 ]
ee = mem128[ input_2 + 272 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 192 ]
ee = mem128[ input_2 + 288 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 208 ]
ee = mem128[ input_2 + 304 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 224 ]
ee = mem128[ input_2 + 320 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 240 ]
ee = mem128[ input_2 + 336 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 256 ]
ee = mem128[ input_2 + 352 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 272 ]
ee = mem128[ input_2 + 368 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 288 ]
ee = mem128[ input_2 + 384 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 304 ]
ee = mem128[ input_2 + 400 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 320 ]
ee = mem128[ input_2 + 416 ]
pp &= ee
ss ^= pp

buf = ss
s = *(uint32 *)(input_1 + 336)
e = *(uint32 *)(input_2 + 432)
s &= e
c_all = count(s)
b64 = mem64[ buf_ptr + 0 ]
c = count(b64)
c_all ^= c
b64 = mem64[ buf_ptr + 8 ]
c = count(b64)
c_all ^= c

addr = row
(uint64) addr >>= 3
addr += input_0
synd = *(uint8  *) (addr + 0)
synd <<= 1
(uint32) c_all &= 1
synd |= c_all
*(uint8  *) (addr + 0) = synd
input_1 -= 340
=? row-0

goto loop if !=

ss = mem128[ input_0 + 0 ]
ee = mem128[ input_2 + 0 ]
ss ^= ee
mem128[ input_0 + 0 ] = ss
ss = mem128[ input_0 + 16 ]
ee = mem128[ input_2 + 16 ]
ss ^= ee
mem128[ input_0 + 16 ] = ss
ss = mem128[ input_0 + 32 ]
ee = mem128[ input_2 + 32 ]
ss ^= ee
mem128[ input_0 + 32 ] = ss
ss = mem128[ input_0 + 48 ]
ee = mem128[ input_2 + 48 ]
ss ^= ee
mem128[ input_0 + 48 ] = ss
ss = mem128[ input_0 + 64 ]
ee = mem128[ input_2 + 64 ]
ss ^= ee
mem128[ input_0 + 64 ] = ss
ss = mem128[ input_0 + 80 ]
ee = mem128[ input_2 + 80 ]
ss ^= ee
mem128[ input_0 + 80 ] = ss
return
