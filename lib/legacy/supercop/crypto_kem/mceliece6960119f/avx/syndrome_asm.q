int64 b64
int64 synd
int64 addr
int64 c
int64 c_all
int64 row
reg256 pp
reg256 ee
reg256 ss
int64 b0
int64 b1
int64 i
int64 p
int64 e
int64 s
int64 tmp
stack64 back
int64 buf_ptr
stack256 buf
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
ss = mem256[ input_1 + 0 ]
ee = mem256[ input_2 + 0 ]
ss &= ee
pp = mem256[ input_1 + 32 ]
ee = mem256[ input_2 + 32 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 64 ]
ee = mem256[ input_2 + 64 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 96 ]
ee = mem256[ input_2 + 96 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 128 ]
ee = mem256[ input_2 + 128 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 160 ]
ee = mem256[ input_2 + 160 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 192 ]
ee = mem256[ input_2 + 192 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 224 ]
ee = mem256[ input_2 + 224 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 256 ]
ee = mem256[ input_2 + 256 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 288 ]
ee = mem256[ input_2 + 288 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 320 ]
ee = mem256[ input_2 + 320 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 352 ]
ee = mem256[ input_2 + 352 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 384 ]
ee = mem256[ input_2 + 384 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 416 ]
ee = mem256[ input_2 + 416 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 448 ]
ee = mem256[ input_2 + 448 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 480 ]
ee = mem256[ input_2 + 480 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 512 ]
ee = mem256[ input_2 + 512 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 544 ]
ee = mem256[ input_2 + 544 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 576 ]
ee = mem256[ input_2 + 576 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 608 ]
ee = mem256[ input_2 + 608 ]
pp &= ee
ss ^= pp
pp = mem256[ input_1 + 640 ]
ee = mem256[ input_2 + 640 ]
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
