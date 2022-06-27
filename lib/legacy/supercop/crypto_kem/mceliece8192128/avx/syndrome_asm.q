int64 b64
int64 synd
int64 addr
int64 c
int64 c_all
int64 row
int64 p
int64 e
int64 s
reg256 pp
reg256 ee
reg256 ss
int64 buf_ptr
stack256 buf
enter syndrome_asm
input_1 += 1357008
buf_ptr = &buf
row = 1664

loop:

row -= 1
ss = mem256[ input_1 + 0 ]
ee = mem256[ input_2 + 208 ]
ss &= ee
pp = mem256[ input_1 + 32 ]
ee = mem256[ input_2 + 240 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 64 ]
ee = mem256[ input_2 + 272 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 96 ]
ee = mem256[ input_2 + 304 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 128 ]
ee = mem256[ input_2 + 336 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 160 ]
ee = mem256[ input_2 + 368 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 192 ]
ee = mem256[ input_2 + 400 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 224 ]
ee = mem256[ input_2 + 432 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 256 ]
ee = mem256[ input_2 + 464 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 288 ]
ee = mem256[ input_2 + 496 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 320 ]
ee = mem256[ input_2 + 528 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 352 ]
ee = mem256[ input_2 + 560 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 384 ]
ee = mem256[ input_2 + 592 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 416 ]
ee = mem256[ input_2 + 624 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 448 ]
ee = mem256[ input_2 + 656 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 480 ]
ee = mem256[ input_2 + 688 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 512 ]
ee = mem256[ input_2 + 720 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 544 ]
ee = mem256[ input_2 + 752 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 576 ]
ee = mem256[ input_2 + 784 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 608 ]
ee = mem256[ input_2 + 816 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 640 ]
ee = mem256[ input_2 + 848 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 672 ]
ee = mem256[ input_2 + 880 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 704 ]
ee = mem256[ input_2 + 912 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 736 ]
ee = mem256[ input_2 + 944 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 768 ]
ee = mem256[ input_2 + 976 ]
pp &= ee
ss ^= pp

buf = ss
s = mem64[input_1 + 800]
e = mem64[input_2 + 1008]
s &= e
p = mem64[input_1 + 808]
e = mem64[input_2 + 1016]
p &= e
s ^= p
c_all = count(s)
b64 = mem64[ buf_ptr + 0 ]
c = count(b64)
c_all ^= c
b64 = mem64[ buf_ptr + 8 ]
c = count(b64)
c_all ^= c
b64 = mem64[ buf_ptr + 16 ]
c = count(b64)
c_all ^= c
b64 = mem64[ buf_ptr + 24 ]
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
input_1 -= 816
=? row-0

goto loop if !=

ss = mem256[ input_0 + 0 ]
ee = mem256[ input_2 + 0 ]
ss ^= ee
mem256[ input_0 + 0 ] = ss
ss = mem256[ input_0 + 32 ]
ee = mem256[ input_2 + 32 ]
ss ^= ee
mem256[ input_0 + 32 ] = ss
ss = mem256[ input_0 + 64 ]
ee = mem256[ input_2 + 64 ]
ss ^= ee
mem256[ input_0 + 64 ] = ss
ss = mem256[ input_0 + 96 ]
ee = mem256[ input_2 + 96 ]
ss ^= ee
mem256[ input_0 + 96 ] = ss
ss = mem256[ input_0 + 128 ]
ee = mem256[ input_2 + 128 ]
ss ^= ee
mem256[ input_0 + 128 ] = ss
ss = mem256[ input_0 + 160 ]
ee = mem256[ input_2 + 160 ]
ss ^= ee
mem256[ input_0 + 160 ] = ss
s = mem64[ input_0 + 192 ]
e = mem64[ input_2 + 192 ]
s ^= e
mem64[ input_0 + 192 ] = s
s = mem64[ input_0 + 200 ]
e = mem64[ input_2 + 200 ]
s ^= e
mem64[ input_0 + 200 ] = s
return
