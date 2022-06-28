int64 b64
int64 synd
int64 addr
int64 c
int64 c_all
int64 row
reg128 pp
reg128 ee
reg128 ss
int64 b0
int64 b1
int64 i
int64 p
int64 e
int64 s
int64 tmp
stack64 back
int64 buf_ptr
stack128 buf
enter syndrome_asm
input_2 += 193
*(uint8  *) (input_0 + 193) = 0
tmp = *(uint8  *) (input_2 + 0)
back = tmp
i = 0
inner1:
addr = input_2 + i

b0 = *(uint8  *) (addr + 0)
b1 = *(uint8  *) (addr + 1)
(uint64) b0 >>= 3
b1 <<= 5
b0 |= b1
*(uint8  *) (addr + 0) = b0

i += 1
=? i-676
goto inner1 if !=
b0 = *(uint8  *) (addr + 1)
(uint64) b0 >>= 3
*(uint8  *) (addr + 1) = b0

input_1 += 1047319
buf_ptr = &buf
row = 1547

loop:

row -= 1
input_1 -= 677
ss = mem128[ input_1 + 0 ]
ee = mem128[ input_2 + 0 ]
ss &= ee
pp = mem128[ input_1 + 16 ]
ee = mem128[ input_2 + 16 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 32 ]
ee = mem128[ input_2 + 32 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 48 ]
ee = mem128[ input_2 + 48 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 64 ]
ee = mem128[ input_2 + 64 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 80 ]
ee = mem128[ input_2 + 80 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 96 ]
ee = mem128[ input_2 + 96 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 112 ]
ee = mem128[ input_2 + 112 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 128 ]
ee = mem128[ input_2 + 128 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 144 ]
ee = mem128[ input_2 + 144 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 160 ]
ee = mem128[ input_2 + 160 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 176 ]
ee = mem128[ input_2 + 176 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 192 ]
ee = mem128[ input_2 + 192 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 208 ]
ee = mem128[ input_2 + 208 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 224 ]
ee = mem128[ input_2 + 224 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 240 ]
ee = mem128[ input_2 + 240 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 256 ]
ee = mem128[ input_2 + 256 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 272 ]
ee = mem128[ input_2 + 272 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 288 ]
ee = mem128[ input_2 + 288 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 304 ]
ee = mem128[ input_2 + 304 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 320 ]
ee = mem128[ input_2 + 320 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 336 ]
ee = mem128[ input_2 + 336 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 352 ]
ee = mem128[ input_2 + 352 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 368 ]
ee = mem128[ input_2 + 368 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 384 ]
ee = mem128[ input_2 + 384 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 400 ]
ee = mem128[ input_2 + 400 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 416 ]
ee = mem128[ input_2 + 416 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 432 ]
ee = mem128[ input_2 + 432 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 448 ]
ee = mem128[ input_2 + 448 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 464 ]
ee = mem128[ input_2 + 464 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 480 ]
ee = mem128[ input_2 + 480 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 496 ]
ee = mem128[ input_2 + 496 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 512 ]
ee = mem128[ input_2 + 512 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 528 ]
ee = mem128[ input_2 + 528 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 544 ]
ee = mem128[ input_2 + 544 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 560 ]
ee = mem128[ input_2 + 560 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 576 ]
ee = mem128[ input_2 + 576 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 592 ]
ee = mem128[ input_2 + 592 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 608 ]
ee = mem128[ input_2 + 608 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 624 ]
ee = mem128[ input_2 + 624 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 640 ]
ee = mem128[ input_2 + 640 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 656 ]
ee = mem128[ input_2 + 656 ]
pp &= ee
ss ^= pp
buf = ss
s = *(uint32  *) (input_1 + 672)
e = *(uint32  *) (input_2 + 672)
s &= e
p = *(uint8  *) (input_1 + 676)
e = *(uint8  *) (input_2 + 676)
p &= e
s ^= p

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
=? row-0

goto loop if !=

i = 676
inner2:
i -= 1
addr = input_2 + i

b0 = *(uint8  *) (addr + 0)
b1 = *(uint8  *) (addr + 1)
(uint64) b0 >>= 5
b1 <<= 3
b1 |= b0
*(uint8  *) (addr + 1) = b1

=? i-0
goto inner2 if !=

tmp = back
*(uint8  *) (input_2 + 0) = tmp

input_2 -= 193
i = 0
inner3:
s = *(uint8  *) (input_0 + 0)
e = *(uint8  *) (input_2 + 0)
s ^= e
*(uint8  *) (input_0 + 0) = s
i += 1
input_0 += 1
input_2 += 1
=? i-193
goto inner3 if !=
s = *(uint8  *) (input_0 + 0)
e = *(uint8  *) (input_2 + 0)
(uint32) e &= 7
s ^= e
*(uint8  *) (input_0 + 0) = s
return
