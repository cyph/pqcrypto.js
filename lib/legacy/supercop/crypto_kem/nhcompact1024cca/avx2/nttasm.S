#include "params.h"

.macro red16 r,x
  vpmulhw         %ymm14,\r,\x
  vpsraw          $10,\x,\x
  vpmullw         %ymm15,\x,\x
  vpsubw          \x,\r,\r
.endm
.macro shuffle8 a,b,c,d
/*
   c={a0,b0}, d={a1,b1} <= a={a0,a1}, b={b0,b1}
   c= {a0,a1,a2,a3,a4,a5,a6,a7,b0,b1,b2,b3,b4,b5,b6,b7},
   d= {a8,a9,a10,a11,a12,a13,a14,a15,b8,b9,b10,b11,b12,b13,b14,b15} <= 
   a={a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15},b={b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14,b15}
*/
vperm2i128	$0x20,\b,\a,\c
vperm2i128	$0x31,\b,\a,\d
.endm

.macro shuffle4 a,b,c,d
/*
  c={a0,b0,a2,b2}, d={a1,b1,a3,b3} <= a={a0,a1,a2,a3}, b={b0,b1,b2,b3}
   c= {a0,a1,a2,a3,a8,a9,a10,a11,b0,b1,b2,b3,b8,b9,b10,b11},
   d= {a4,a5,a6,a7,a12,a13,a14,a15,b4,b5,b6,b7,b12,b13,b14,b15} <= 
   a={a0,a1,a2,a3,a4,a5,a6,a7,b0,b1,b2,b3,b4,b5,b6,b7},b={a8,a9,a10,a11,a12,a13,a14,a15,b8,b9,b10,b11,b12,b13,b14,b15}
*/
vpunpcklqdq	\b,\a,\c
vpunpckhqdq	\b,\a,\d
.endm

.macro shuffle2 a,b,c,d
/*
  c={a0,b0,a2,b2,a4,b4,a6,b6}, c={a1,b1,a3,b3,a5,b5,a7,b7} <= a={a0,a1,a2,a3,a4,a5,a6,a7}, b={b0,b1,b2,b3,b4,b5,b6,b7}
   c= {a0,a1,a4,a5,a8,b9,a12,a13,b0,b1,b4,b5,b8,b9,b12,b13},
   d= {a2,a3,a6,a7,a10,a11,a14,a15,b2,b3,b6,b7,b10,b11,b14,b15} <= 
   a={a0,a1,a2,a3,a8,a9,a10,a11,b0,b1,b2,b3,b8,b9,b10,b11},b={a4,a5,a6,a7,a12,a13,a14,a15,b4,b5,b6,b7,b12,b13,b14,b15}
*/
vpsllq		$32,\b,%ymm10
vpsrlq		$32,\a,%ymm11
vpblendd	$0xAA,%ymm10,\a,\c
vpblendd	$0xAA,\b,%ymm11,\d
.endm

.macro shuffle1 a,b,c,d
/*
   c= {a0,a2,a4,a6,a8,a10,a12,a14,b0,b2,b4,b6,b8,b10,b12,b14},
   d= {a1,a3,a5,a7,a9,a11,a13,a15,b1,b2,b5,b7,b9,b11,b13,b15} <= 
   a={a0,a1,a4,a5,a8,a9,a12,a13,b0,b1,b4,b5,b8,b9,b12,b13},b={a2,a3,a6,a7,a10,a11,a14,a15,b2,b3,b6,b7,b10,b11,b14,b15}
*/

vpslld		$16,\b,%ymm10
vpsrld		$16,\a,%ymm11
vpblendw	$0xAA,%ymm10,\a,\c
vpblendw	$0xAA,\b,%ymm11,\d
.endm

.macro CTbutterfly2 r0, r1, r2, r3, r4, r5, r6, r7, zr, z, q, t0, t1, t2, t3

vpmullw \zr, \r4, \t0
vpmullw \zr, \r5, \t1
vpmullw \zr, \r6, \t2
vpmullw \zr, \r7, \t3
vpmulhw \z, \r4, \r4
vpmulhw \z, \r5, \r5
vpmulhw \z, \r6, \r6
vpmulhw \z, \r7, \r7

vpmulhw \q, \t0, \t0
vpmulhw \q, \t1, \t1
vpmulhw \q, \t2, \t2
vpmulhw \q, \t3, \t3
vpsubw \t0, \r4, \t0
vpsubw \t1, \r5, \t1
vpsubw \t2, \r6, \t2
vpsubw \t3, \r7, \t3

vpsubw \t0, \r0, \r4
vpaddw \t0, \r0, \r0
vpsubw \t1, \r1, \r5
vpaddw \t1, \r1, \r1
vpsubw \t2, \r2, \r6
vpaddw \t2, \r2, \r2
vpsubw \t3, \r3, \r7
vpaddw \t3, \r3, \r3
.endm

.macro CTbutterfly r0, r1, r4, r5, zr, z, q, t0, t1 

vpmullw \zr, \r4, \t0
vpmullw \zr, \r5, \t1
vpmulhw \z, \r4, \r4
vpmulhw \z, \r5, \r5

vpmulhw \q, \t0, \t0
vpmulhw \q, \t1, \t1
vpsubw \t0, \r4, \t0
vpsubw \t1, \r5, \t1

vpsubw \t0, \r0, \r4
vpaddw \t0, \r0, \r0
vpsubw \t1, \r1, \r5
vpaddw \t1, \r1, \r1
.endm

.macro GSbutterfly2 r0, r1, r2, r3, r4, r5, r6, r7, zr, z, q, t0, t1, t2, t3

vpsubw \r4, \r0, \t0
vpaddw \r4, \r0, \r0
vpsubw \r5, \r1, \t1
vpaddw \r5, \r1, \r1
vpsubw \r6, \r2, \t2
vpaddw \r6, \r2, \r2
vpsubw \r7, \r3, \t3
vpaddw \r7, \r3, \r3


vpmullw \zr, \t0, \r4
vpmullw \zr, \t1, \r5
vpmullw \zr, \t2, \r6
vpmullw \zr, \t3, \r7
vpmulhw \z, \t0, \t0
vpmulhw \z, \t1, \t1
vpmulhw \z, \t2, \t2
vpmulhw \z, \t3, \t3

vpmulhw \q, \r4, \r4
vpmulhw \q, \r5, \r5
vpmulhw \q, \r6, \r6
vpmulhw \q, \r7, \r7
vpsubw \r4, \t0, \r4
vpsubw \r5, \t1, \r5
vpsubw \r6, \t2, \r6
vpsubw \r7, \t3, \r7
.endm

.macro GSbutterfly r0, r1, r4, r5, zr, z, q, t0, t1 

vpsubw \r4, \r0, \t0
vpaddw \r4, \r0, \r0
vpsubw \r5, \r1, \t1
vpaddw \r5, \r1, \r1


vpmullw \zr, \t0, \r4
vpmullw \zr, \t1, \r5
vpmulhw \z, \t0, \t0
vpmulhw \z, \t1, \t1

vpmulhw \q, \r4, \r4
vpmulhw \q, \r5, \r5
vpsubw \r4, \t0, \r4
vpsubw \r5, \t1, \r5
.endm


.global ntt_avx
ntt_avx:
vmovdqa		_16xq(%rip),%ymm15

vmovdqa		(%rdi),%ymm0
vmovdqa		32(%rdi),%ymm1
vmovdqa		64(%rdi),%ymm2
vmovdqa		96(%rdi),%ymm3
vmovdqa		128(%rdi),%ymm4
vmovdqa		160(%rdi),%ymm5
vmovdqa		192(%rdi),%ymm6
vmovdqa		224(%rdi),%ymm7

level0:

vpbroadcastd	(%rsi),%ymm14
vpbroadcastd	4(%rsi),%ymm13

#if (NEWHOPECMPCT_N == 768)
vpmullw %ymm14, %ymm4, %ymm8
vpmullw %ymm14, %ymm5, %ymm9
vpmullw %ymm14, %ymm6, %ymm10
vpmullw %ymm14, %ymm7, %ymm11
vpmulhw %ymm13, %ymm4, %ymm4
vpmulhw %ymm13, %ymm5, %ymm5
vpmulhw %ymm13, %ymm6, %ymm6
vpmulhw %ymm13, %ymm7, %ymm7


vpmulhw %ymm15, %ymm8, %ymm8
vpmulhw %ymm15, %ymm9, %ymm9
vpmulhw %ymm15, %ymm10, %ymm10
vpmulhw %ymm15, %ymm11, %ymm11
vpsubw %ymm8, %ymm4, %ymm8
vpsubw %ymm9, %ymm5, %ymm9
vpsubw %ymm10, %ymm6, %ymm10
vpsubw %ymm11, %ymm7, %ymm11


vpsubw %ymm8, %ymm0, %ymm4
vpaddw %ymm8, %ymm0, %ymm0
vpsubw %ymm9, %ymm1, %ymm5
vpaddw %ymm9, %ymm1, %ymm1
vpsubw %ymm10, %ymm2, %ymm6
vpaddw %ymm10, %ymm2, %ymm2
vpsubw %ymm11, %ymm3, %ymm7
vpaddw %ymm11, %ymm3, %ymm3

vmovdqa		128(%rdi),%ymm8
vmovdqa		160(%rdi),%ymm9
vmovdqa		192(%rdi),%ymm10
vmovdqa		224(%rdi),%ymm11

vpaddw %ymm8, %ymm4, %ymm4
vpaddw %ymm9, %ymm5, %ymm5
vpaddw %ymm10, %ymm6, %ymm6
vpaddw %ymm11, %ymm7, %ymm7

#else
CTbutterfly2	%ymm0,%ymm1,%ymm2,%ymm3,%ymm4,%ymm5,%ymm6,%ymm7,%ymm14,%ymm13,%ymm15,%ymm8,%ymm9,%ymm10,%ymm11
#endif

level1:
vpbroadcastd	8(%rsi),%ymm14
vpbroadcastd	12(%rsi),%ymm13
vpbroadcastd	400(%rsi),%ymm9
vpbroadcastd	404(%rsi),%ymm8

CTbutterfly	%ymm0,%ymm1,%ymm2,%ymm3,%ymm14,%ymm13,%ymm15,%ymm10,%ymm11
CTbutterfly	%ymm4,%ymm5,%ymm6,%ymm7,%ymm9,%ymm8,%ymm15,%ymm10,%ymm11
level2:

vmovdqu		16(%rsi),%ymm14
vmovdqu		48(%rsi),%ymm13
vmovdqu		408(%rsi),%ymm9
vmovdqu		440(%rsi),%ymm8

shuffle8	%ymm0, %ymm2, %ymm12, %ymm2 
shuffle8	%ymm4, %ymm6, %ymm0, %ymm6 
shuffle8	%ymm1, %ymm3, %ymm4, %ymm3 
shuffle8	%ymm5, %ymm7, %ymm1, %ymm7 

CTbutterfly	%ymm12,%ymm2,%ymm4,%ymm3,%ymm14,%ymm13,%ymm15,%ymm10,%ymm11
CTbutterfly	%ymm0,%ymm6,%ymm1,%ymm7,%ymm9,%ymm8,%ymm15,%ymm10,%ymm11

level3:

vmovdqu		80(%rsi),%ymm14
vmovdqu		112(%rsi),%ymm13
vmovdqu		472(%rsi),%ymm9
vmovdqu		504(%rsi),%ymm8

shuffle4	%ymm12, %ymm4, %ymm5, %ymm4 
shuffle4	%ymm2, %ymm3, %ymm12, %ymm3 
shuffle4	%ymm0, %ymm1, %ymm2, %ymm1 
shuffle4	%ymm6, %ymm7, %ymm0, %ymm7 

CTbutterfly	%ymm5, %ymm4, %ymm12, %ymm3, %ymm14, %ymm13, %ymm15, %ymm10, %ymm11
CTbutterfly	%ymm2, %ymm1, %ymm0, %ymm7, %ymm9, %ymm8, %ymm15, %ymm10, %ymm11

level4:

vmovdqu		144(%rsi),%ymm14
vmovdqu		176(%rsi),%ymm13
vmovdqu		536(%rsi),%ymm9
vmovdqu		568(%rsi),%ymm8

shuffle2	%ymm5, %ymm12, %ymm6, %ymm12 
shuffle2	%ymm4, %ymm3, %ymm5, %ymm3 
shuffle2	%ymm2, %ymm0, %ymm4, %ymm0 
shuffle2	%ymm1, %ymm7, %ymm2, %ymm7 

CTbutterfly	%ymm6, %ymm12, %ymm5, %ymm3, %ymm14, %ymm13, %ymm15, %ymm10, %ymm11 
CTbutterfly	%ymm4, %ymm0, %ymm2, %ymm7, %ymm9, %ymm8, %ymm15, %ymm10, %ymm11 

level5:

vmovdqu		208(%rsi),%ymm14
vmovdqu		240(%rsi),%ymm13
vmovdqu		600(%rsi),%ymm9
vmovdqu		632(%rsi),%ymm8

shuffle1  %ymm6, %ymm5, %ymm1, %ymm5 
shuffle1  %ymm12, %ymm3, %ymm6, %ymm3 
shuffle1  %ymm4, %ymm2, %ymm12, %ymm2 
shuffle1  %ymm0, %ymm7, %ymm4, %ymm7 

CTbutterfly	%ymm1, %ymm5, %ymm6, %ymm3, %ymm14, %ymm13, %ymm15, %ymm10, %ymm11 
CTbutterfly	%ymm12, %ymm2, %ymm4, %ymm7, %ymm9, %ymm8, %ymm15, %ymm10, %ymm11 

level6:

vmovdqu		272(%rsi),%ymm8
vmovdqu		336(%rsi),%ymm9
vmovdqu		304(%rsi),%ymm0
vmovdqu		368(%rsi),%ymm13
vmovdqu		664(%rsi),%ymm10
vmovdqu		728(%rsi),%ymm11
vmovdqu		696(%rsi),%ymm14
vmovdqu		760(%rsi),%ymm15

vpmullw %ymm8, %ymm5, %ymm8
vpmullw %ymm9, %ymm3, %ymm9
vpmullw %ymm10, %ymm2, %ymm10
vpmullw %ymm11, %ymm7, %ymm11
vpmulhw %ymm0, %ymm5, %ymm5
vpmulhw %ymm13, %ymm3, %ymm3
vpmulhw %ymm14, %ymm2, %ymm2
vpmulhw %ymm15, %ymm7, %ymm7

vmovdqa		_16xq(%rip),%ymm15

vpmulhw %ymm15, %ymm8, %ymm8
vpmulhw %ymm15, %ymm9, %ymm9
vpmulhw %ymm15, %ymm10, %ymm10
vpmulhw %ymm15, %ymm11, %ymm11
vpsubw %ymm8, %ymm5, %ymm8
vpsubw %ymm9, %ymm3, %ymm9
vpsubw %ymm10, %ymm2, %ymm10
vpsubw %ymm11, %ymm7, %ymm11

vpsubw %ymm8, %ymm1, %ymm5
vpaddw %ymm8, %ymm1, %ymm1
vpsubw %ymm9, %ymm6, %ymm3
vpaddw %ymm9, %ymm6, %ymm6
vpsubw %ymm10, %ymm12, %ymm2
vpaddw %ymm10, %ymm12, %ymm12
vpsubw %ymm11, %ymm4, %ymm7
vpaddw %ymm11, %ymm4, %ymm4

shuffle1  %ymm4, %ymm7, %ymm0, %ymm7 
shuffle1  %ymm12, %ymm2, %ymm4, %ymm2 
shuffle1  %ymm6, %ymm3, %ymm12, %ymm3 
shuffle1  %ymm1, %ymm5, %ymm6, %ymm5 

shuffle2  %ymm2, %ymm7, %ymm1, %ymm7 
shuffle2  %ymm4, %ymm0, %ymm2, %ymm0 
shuffle2  %ymm5, %ymm3, %ymm4, %ymm3 
shuffle2  %ymm6, %ymm12, %ymm5, %ymm12

shuffle4  %ymm0, %ymm7, %ymm6, %ymm7 
shuffle4  %ymm2, %ymm1, %ymm0, %ymm1 
shuffle4  %ymm12, %ymm3, %ymm2, %ymm3 
shuffle4  %ymm5, %ymm4, %ymm12, %ymm4 

shuffle8  %ymm1, %ymm7, %ymm5, %ymm7 
shuffle8  %ymm4, %ymm3, %ymm1, %ymm3 
shuffle8  %ymm0, %ymm6, %ymm4, %ymm6 
shuffle8  %ymm12, %ymm2, %ymm0, %ymm2 

vmovdqa		_16xv(%rip),%ymm14


vmovdqa		%ymm0,(%rdi)
vmovdqa		%ymm1,32(%rdi)
vmovdqa		%ymm2,64(%rdi)
vmovdqa		%ymm3,96(%rdi)
vmovdqa		%ymm4,128(%rdi)
vmovdqa		%ymm5,160(%rdi)
vmovdqa		%ymm6,192(%rdi)
vmovdqa		%ymm7,224(%rdi)

ret


.global invntt_avx
invntt_avx:

ilevel0:
vmovdqa		(%rdi),%ymm0
vmovdqa		32(%rdi),%ymm1
vmovdqa		64(%rdi),%ymm2
vmovdqa		96(%rdi),%ymm3
vmovdqa		128(%rdi),%ymm4
vmovdqa		160(%rdi),%ymm5
vmovdqa		192(%rdi),%ymm6
vmovdqa		224(%rdi),%ymm7

shuffle8	%ymm0, %ymm2, %ymm12, %ymm2 
shuffle8	%ymm4, %ymm6, %ymm0, %ymm6 
shuffle8	%ymm1, %ymm3, %ymm4, %ymm3 
shuffle8	%ymm5, %ymm7, %ymm1, %ymm7 

shuffle4	%ymm12, %ymm4, %ymm5, %ymm4 
shuffle4	%ymm2, %ymm3, %ymm12, %ymm3 
shuffle4	%ymm0, %ymm1, %ymm2, %ymm1 
shuffle4	%ymm6, %ymm7, %ymm0, %ymm7 

shuffle2	%ymm5, %ymm12, %ymm6, %ymm12 
shuffle2	%ymm4, %ymm3, %ymm5, %ymm3 
shuffle2	%ymm2, %ymm0, %ymm4, %ymm0 
shuffle2	%ymm1, %ymm7, %ymm2, %ymm7 

shuffle1  %ymm6, %ymm5, %ymm1, %ymm5 
shuffle1  %ymm12, %ymm3, %ymm6, %ymm3 
shuffle1  %ymm4, %ymm2, %ymm12, %ymm2 
shuffle1  %ymm0, %ymm7, %ymm4, %ymm7 


vpsubw %ymm5, %ymm1, %ymm8
vpaddw %ymm5, %ymm1, %ymm1
vpsubw %ymm3, %ymm6, %ymm9
vpaddw %ymm3, %ymm6, %ymm6
vpsubw %ymm2, %ymm12, %ymm10
vpaddw %ymm2, %ymm12, %ymm12
vpsubw %ymm7, %ymm4, %ymm11
vpaddw %ymm7, %ymm4, %ymm4


vmovdqu		(%rsi),%ymm5
vmovdqu		32(%rsi),%ymm0
vmovdqu		64(%rsi),%ymm2
vmovdqu		96(%rsi),%ymm14
vmovdqu		128(%rsi),%ymm3
vmovdqu		160(%rsi),%ymm13
vmovdqu		192(%rsi),%ymm7
vmovdqu		224(%rsi),%ymm15

vpmullw %ymm5, %ymm8, %ymm5
vpmullw %ymm3, %ymm9, %ymm3
vpmullw %ymm2, %ymm10, %ymm2
vpmullw %ymm7, %ymm11, %ymm7
vpmulhw %ymm0, %ymm8, %ymm8
vpmulhw %ymm13, %ymm9, %ymm9
vpmulhw %ymm14, %ymm10, %ymm10
vpmulhw %ymm15, %ymm11, %ymm11

vmovdqa		_16xq(%rip),%ymm15

vpmulhw %ymm15, %ymm5, %ymm5
vpmulhw %ymm15, %ymm3, %ymm3
vpmulhw %ymm15, %ymm2, %ymm2
vpmulhw %ymm15, %ymm7, %ymm7
vpsubw %ymm5, %ymm8, %ymm5
vpsubw %ymm3, %ymm9, %ymm3
vpsubw %ymm2, %ymm10, %ymm2
vpsubw %ymm7, %ymm11, %ymm7

ilevel1:

vmovdqu	256(%rsi),%ymm14
vmovdqu	288(%rsi),%ymm13
vmovdqu	320(%rsi),%ymm9
vmovdqu	352(%rsi),%ymm8

GSbutterfly	%ymm1, %ymm5, %ymm6, %ymm3, %ymm14, %ymm13, %ymm15, %ymm10, %ymm11 
GSbutterfly	%ymm12, %ymm2, %ymm4, %ymm7, %ymm9, %ymm8, %ymm15, %ymm10, %ymm11 

shuffle1  %ymm4, %ymm7, %ymm0, %ymm7 
shuffle1  %ymm12, %ymm2, %ymm4, %ymm2 
shuffle1  %ymm6, %ymm3, %ymm12, %ymm3 
shuffle1  %ymm1, %ymm5, %ymm6, %ymm5 

ilevel2:

vmovdqu		384(%rsi),%ymm14
vmovdqu		416(%rsi),%ymm13
vmovdqu		448(%rsi),%ymm9
vmovdqu		480(%rsi),%ymm8

GSbutterfly	%ymm6, %ymm12, %ymm5, %ymm3, %ymm14, %ymm13, %ymm15, %ymm10, %ymm11 
GSbutterfly	%ymm4, %ymm0, %ymm2, %ymm7, %ymm9, %ymm8, %ymm15, %ymm10, %ymm11 

vmovdqa		_16xv(%rip),%ymm14
red16 %ymm6, %ymm8
red16 %ymm4, %ymm8

shuffle2  %ymm2, %ymm7, %ymm1, %ymm7 
shuffle2  %ymm4, %ymm0, %ymm2, %ymm0 
shuffle2  %ymm5, %ymm3, %ymm4, %ymm3 
shuffle2  %ymm6, %ymm12, %ymm5, %ymm12

ilevel3:

vmovdqu		512(%rsi),%ymm14
vmovdqu		544(%rsi),%ymm13
vmovdqu		576(%rsi),%ymm9
vmovdqu		608(%rsi),%ymm8

GSbutterfly	%ymm5, %ymm4, %ymm12, %ymm3, %ymm14, %ymm13, %ymm15, %ymm10, %ymm11
GSbutterfly	%ymm2, %ymm1, %ymm0, %ymm7, %ymm9, %ymm8, %ymm15, %ymm10, %ymm11

vmovdqa		_16xv(%rip),%ymm14
red16 %ymm2, %ymm8
red16 %ymm5, %ymm8

shuffle4  %ymm0, %ymm7, %ymm6, %ymm7 
shuffle4  %ymm2, %ymm1, %ymm0, %ymm1 
shuffle4  %ymm12, %ymm3, %ymm2, %ymm3 
shuffle4  %ymm5, %ymm4, %ymm12, %ymm4 

ilevel4:

vmovdqu		640(%rsi),%ymm14
vmovdqu		672(%rsi),%ymm13
vmovdqu		704(%rsi),%ymm9
vmovdqu		736(%rsi),%ymm8

GSbutterfly	%ymm12,%ymm2,%ymm4,%ymm3,%ymm14,%ymm13,%ymm15,%ymm10,%ymm11
GSbutterfly	%ymm0,%ymm6,%ymm1,%ymm7,%ymm9,%ymm8,%ymm15,%ymm10,%ymm11

vmovdqa		_16xv(%rip),%ymm14
red16 %ymm12, %ymm8
red16 %ymm0, %ymm8

shuffle8  %ymm1, %ymm7, %ymm5, %ymm7 
shuffle8  %ymm4, %ymm3, %ymm1, %ymm3 
shuffle8  %ymm0, %ymm6, %ymm4, %ymm6 
shuffle8  %ymm12, %ymm2, %ymm0, %ymm2 


ilevel5:

vpbroadcastd	768(%rsi),%ymm14
vpbroadcastd	772(%rsi),%ymm13
vpbroadcastd	776(%rsi),%ymm9
vpbroadcastd	780(%rsi),%ymm8

GSbutterfly	%ymm0,%ymm1,%ymm2,%ymm3,%ymm14,%ymm13,%ymm15,%ymm10,%ymm11
GSbutterfly	%ymm4,%ymm5,%ymm6,%ymm7,%ymm9,%ymm8,%ymm15,%ymm10,%ymm11

vmovdqa		_16xv(%rip),%ymm14
red16 %ymm4, %ymm8

ilevel6:

vpbroadcastd	784(%rsi),%ymm14
vpbroadcastd	788(%rsi),%ymm13

GSbutterfly2	%ymm0,%ymm1,%ymm2,%ymm3,%ymm4,%ymm5,%ymm6,%ymm7,%ymm14,%ymm13,%ymm15,%ymm8,%ymm9,%ymm10,%ymm11

#if (NEWHOPECMPCT_N==768)
vpsubw %ymm4, %ymm0, %ymm0
vpsubw %ymm5, %ymm1, %ymm1
vpsubw %ymm6, %ymm2, %ymm2
vpsubw %ymm7, %ymm3, %ymm3

vpbroadcastd	800(%rsi),%ymm14
vpbroadcastd	804(%rsi),%ymm13

vpmullw %ymm14, %ymm4, %ymm8
vpmullw %ymm14, %ymm5, %ymm9
vpmullw %ymm14, %ymm6, %ymm10
vpmullw %ymm14, %ymm7, %ymm11
vpmulhw %ymm13, %ymm4, %ymm4
vpmulhw %ymm13, %ymm5, %ymm5
vpmulhw %ymm13, %ymm6, %ymm6
vpmulhw %ymm13, %ymm7, %ymm7


vpmulhw %ymm15, %ymm8, %ymm8
vpmulhw %ymm15, %ymm9, %ymm9
vpmulhw %ymm15, %ymm10, %ymm10
vpmulhw %ymm15, %ymm11, %ymm11
vpsubw %ymm8, %ymm4, %ymm4
vpsubw %ymm9, %ymm5, %ymm5
vpsubw %ymm10, %ymm6, %ymm6
vpsubw %ymm11, %ymm7, %ymm7

#endif

vpbroadcastd	792(%rsi),%ymm14
vpbroadcastd	796(%rsi),%ymm13

vpmullw %ymm14, %ymm0, %ymm8
vpmullw %ymm14, %ymm1, %ymm9
vpmullw %ymm14, %ymm2, %ymm10
vpmullw %ymm14, %ymm3, %ymm11
vpmulhw %ymm13, %ymm0, %ymm0
vpmulhw %ymm13, %ymm1, %ymm1
vpmulhw %ymm13, %ymm2, %ymm2
vpmulhw %ymm13, %ymm3, %ymm3


vpmulhw %ymm15, %ymm8, %ymm8
vpmulhw %ymm15, %ymm9, %ymm9
vpmulhw %ymm15, %ymm10, %ymm10
vpmulhw %ymm15, %ymm11, %ymm11
vpsubw %ymm8, %ymm0, %ymm8
vpsubw %ymm9, %ymm1, %ymm9
vpsubw %ymm10, %ymm2, %ymm10
vpsubw %ymm11, %ymm3, %ymm11

vmovdqa		%ymm8,(%rdi)
vmovdqa		%ymm9,32(%rdi)
vmovdqa		%ymm10,64(%rdi)
vmovdqa		%ymm11,96(%rdi)
vmovdqa		%ymm4,128(%rdi)
vmovdqa		%ymm5,160(%rdi)
vmovdqa		%ymm6,192(%rdi)
vmovdqa		%ymm7,224(%rdi)

ret


