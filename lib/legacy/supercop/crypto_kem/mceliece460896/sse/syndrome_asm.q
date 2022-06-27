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
input_1 += 523740
buf_ptr = &buf
row = 1248

loop:

row -= 1
ss = mem128[ input_1 + 0 ]
ee = mem128[ input_2 + 156 ]
ss &= ee
pp = mem128[ input_1 + 16 ]
ee = mem128[ input_2 + 172 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 32 ]
ee = mem128[ input_2 + 188 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 48 ]
ee = mem128[ input_2 + 204 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 64 ]
ee = mem128[ input_2 + 220 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 80 ]
ee = mem128[ input_2 + 236 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 96 ]
ee = mem128[ input_2 + 252 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 112 ]
ee = mem128[ input_2 + 268 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 128 ]
ee = mem128[ input_2 + 284 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 144 ]
ee = mem128[ input_2 + 300 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 160 ]
ee = mem128[ input_2 + 316 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 176 ]
ee = mem128[ input_2 + 332 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 192 ]
ee = mem128[ input_2 + 348 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 208 ]
ee = mem128[ input_2 + 364 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 224 ]
ee = mem128[ input_2 + 380 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 240 ]
ee = mem128[ input_2 + 396 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 256 ]
ee = mem128[ input_2 + 412 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 272 ]
ee = mem128[ input_2 + 428 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 288 ]
ee = mem128[ input_2 + 444 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 304 ]
ee = mem128[ input_2 + 460 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 320 ]
ee = mem128[ input_2 + 476 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 336 ]
ee = mem128[ input_2 + 492 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 352 ]
ee = mem128[ input_2 + 508 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 368 ]
ee = mem128[ input_2 + 524 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 384 ]
ee = mem128[ input_2 + 540 ]
pp &= ee
ss ^= pp
pp = mem128[ input_1 + 400 ]
ee = mem128[ input_2 + 556 ]
pp &= ee
ss ^= pp

buf = ss
s = *(uint32 *)(input_1 + 416)
e = *(uint32 *)(input_2 + 572)
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
input_1 -= 420
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
ss = mem128[ input_0 + 96 ]
ee = mem128[ input_2 + 96 ]
ss ^= ee
mem128[ input_0 + 96 ] = ss
ss = mem128[ input_0 + 112 ]
ee = mem128[ input_2 + 112 ]
ss ^= ee
mem128[ input_0 + 112 ] = ss
s = mem64[ input_0 + 128 ]
e = mem64[ input_2 + 128 ]
s ^= e
mem64[ input_0 + 128 ] = s
s = mem64[ input_0 + 136 ]
e = mem64[ input_2 + 136 ]
s ^= e
mem64[ input_0 + 136 ] = s
s = mem64[ input_0 + 144 ]
e = mem64[ input_2 + 144 ]
s ^= e
mem64[ input_0 + 144 ] = s
s = *(uint32 *)( input_0 + 152 )
e = *(uint32 *)( input_2 + 152 )
s ^= e
*(uint32 *)( input_0 + 152 ) = s
return
