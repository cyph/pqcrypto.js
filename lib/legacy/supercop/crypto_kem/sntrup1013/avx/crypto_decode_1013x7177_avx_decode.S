	.text
	.file	"decode.c"
	.section	.rodata,"a",@progbits
	.p2align	1               # -- Begin function CRYPTO_NAMESPACE(crypto_decode_1013x7177)
.LCPI0_0:
	.short	538                     # 0x21a
.LCPI0_1:
	.short	63418                   # 0xf7ba
.LCPI0_2:
	.short	7921                    # 0x1ef1
.LCPI0_4:
	.short	4625                    # 0x1211
.LCPI0_5:
	.short	32                      # 0x20
.LCPI0_6:
	.short	64800                   # 0xfd20
.LCPI0_7:
	.short	89                      # 0x59
.LCPI0_8:
	.short	18409                   # 0x47e9
.LCPI0_9:
	.short	65452                   # 0xffac
.LCPI0_10:
	.short	58586                   # 0xe4da
.LCPI0_11:
	.short	2414                    # 0x96e
.LCPI0_12:
	.short	6407                    # 0x1907
.LCPI0_13:
	.short	46                      # 0x2e
.LCPI0_14:
	.short	44191                   # 0xac9f
.LCPI0_15:
	.short	786                     # 0x312
.LCPI0_16:
	.short	50361                   # 0xc4b9
.LCPI0_17:
	.short	62926                   # 0xf5ce
.LCPI0_18:
	.short	63198                   # 0xf6de
.LCPI0_19:
	.short	7177                    # 0x1c09
.LCPI0_20:
	.short	12857                   # 0x3239
.LCPI0_21:
	.short	61948                   # 0xf1fc
	.section	.rodata.cst32,"aM",@progbits,32
	.p2align	5
.LCPI0_3:
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.text
	.globl	CRYPTO_NAMESPACE(crypto_decode_1013x7177)
	.type	CRYPTO_NAMESPACE(crypto_decode_1013x7177),@function
CRYPTO_NAMESPACE(crypto_decode_1013x7177): # @CRYPTO_NAMESPACE(crypto_decode_1013x7177)
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	pushq	%rbx
	.cfi_def_cfa_offset 24
	subq	$1912, %rsp             # imm = 0x778
	.cfi_def_cfa_offset 1936
	.cfi_offset %rbx, -24
	.cfi_offset %rbp, -16
	movzbl	1622(%rsi), %eax
	imull	$-78, %eax, %ecx
	shrl	$16, %ecx
	imull	$282132480, %eax, %eax  # imm = 0x10D10000
	sarl	$16, %eax
	imull	$274, %eax, %eax        # imm = 0x112
	shrl	$16, %eax
	movzbl	1621(%rsi), %edx
	addl	%ecx, %edx
	subl	%eax, %edx
	leal	-274(%rdx), %eax
	cwtl
	shrl	$15, %eax
	movl	$274, %ecx              # imm = 0x112
	andl	%ecx, %eax
	addl	%eax, %edx
	addl	$-274, %edx             # imm = 0xFEEE
	movswl	%dx, %eax
	shrl	$15, %eax
	andl	%ecx, %eax
	addl	%edx, %eax
	movswl	%ax, %ecx
	sarl	$15, %ecx
	imull	$12243, %eax, %edx      # imm = 0x2FD3
	movswl	%dx, %edx
	imull	$91, %edx, %edx
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1620(%rsi), %r9d
	addl	%r9d, %ecx
	movswl	%cx, %edx
	sarl	$12, %edx
	imull	$-720, %ecx, %ecx       # imm = 0xFD30
	movswl	%cx, %ecx
	imull	$91, %ecx, %ecx
	shrl	$16, %ecx
	subl	%ecx, %edx
	movswl	%dx, %ecx
	shrl	$15, %ecx
	movl	$91, %r8d
	andl	%ecx, %r8d
	addl	%edx, %r8d
	movswl	%r8w, %r10d
	shll	$8, %eax
	orl	%r9d, %eax
	subl	%r8d, %eax
	imull	$12243, %eax, %eax      # imm = 0x2FD3
	leal	-769(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$769, %ecx              # imm = 0x301
	leal	(%rcx,%rax), %r11d
	addl	$-769, %r11d            # imm = 0xFCFF
	movswl	%r11w, %eax
	sarl	$10, %eax
	imull	$20696, %r11d, %ecx     # imm = 0x50D8
	movswl	%cx, %ecx
	imull	$152, %ecx, %ecx
	shrl	$16, %ecx
	subl	%ecx, %eax
	movzbl	1619(%rsi), %ecx
	addl	%ecx, %eax
	movswl	%ax, %edx
	shll	$3, %edx
	leal	(%rdx,%rdx,2), %edx
	shrl	$16, %edx
	imull	$-431, %eax, %eax       # imm = 0xFE51
	cwtl
	imull	$152, %eax, %eax
	shrl	$16, %eax
	subl	%eax, %edx
	movswl	%dx, %ebp
	shrl	$15, %ebp
	movl	$152, %r9d
	andl	%r9d, %ebp
	addl	%edx, %ebp
	shll	$5, %r11d
	movswl	%bp, %ebx
	subl	%ebx, %ecx
	shrl	$3, %ecx
	addl	%r11d, %ecx
	imull	$-13797, %ecx, %ecx     # imm = 0xCA1B
	leal	-1294(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$1294, %edx             # imm = 0x50E
	leal	(%rdx,%rcx), %r11d
	addl	$-1294, %r11d           # imm = 0xFAF2
	movw	%bx, -116(%rsp)
	movw	%r11w, -114(%rsp)
	sarl	$10, %r10d
	imull	$20696, %r8d, %ecx      # imm = 0x50D8
	movswl	%cx, %ecx
	imull	$152, %ecx, %ecx
	shrl	$16, %ecx
	subl	%ecx, %r10d
	movzbl	1618(%rsi), %ecx
	addl	%ecx, %r10d
	movswl	%r10w, %ebx
	shll	$3, %ebx
	leal	(%rbx,%rbx,2), %ebx
	shrl	$16, %ebx
	imull	$-431, %r10d, %edx      # imm = 0xFE51
	movswl	%dx, %edx
	imull	$152, %edx, %edx
	shrl	$16, %edx
	subl	%edx, %ebx
	movswl	%bx, %edx
	shrl	$15, %edx
	andl	%r9d, %edx
	addl	%ebx, %edx
	shll	$5, %r8d
	movswl	%dx, %edx
	subl	%edx, %ecx
	shrl	$3, %ecx
	addl	%r8d, %ecx
	imull	$-13797, %ecx, %ecx     # imm = 0xCA1B
	leal	-152(%rcx), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	andl	%r9d, %ebx
	addl	%ebx, %ecx
	addl	$-152, %ecx
	movw	%dx, -120(%rsp)
	movw	%cx, -118(%rsp)
	movswl	%r11w, %ecx
	imull	$-92, %ecx, %edx
	shrl	$16, %edx
	imull	$-19628, %r11d, %ecx    # imm = 0xB354
	movswl	%cx, %ecx
	imull	$197, %ecx, %ecx
	shrl	$16, %ecx
	movzbl	1617(%rsi), %r8d
	addl	%r8d, %edx
	subl	%ecx, %edx
	leal	-197(%rdx), %ecx
	movswl	%cx, %ebx
	shrl	$15, %ebx
	movl	$197, %r9d
	andl	%r9d, %ebx
	addl	%ebx, %edx
	addl	$-197, %edx
	movswl	%dx, %ebx
	shrl	$15, %ebx
	andl	%r9d, %ebx
	addl	%edx, %ebx
	shll	$8, %r11d
	orl	%r8d, %r11d
	subl	%ebx, %r11d
	imull	$32269, %r11d, %r8d     # imm = 0x7E0D
	leal	-1681(%r8), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$1681, %edx             # imm = 0x691
	addl	%r8d, %edx
	addl	$-1681, %edx            # imm = 0xF96F
	movw	%bx, -100(%rsp)
	movw	%dx, -98(%rsp)
	xorl	%edx, %edx
	movl	$-197, %r8d
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movswl	%bp, %ebx
	imull	$-92, %ebx, %ebx
	shrl	$16, %ebx
	imull	$-19628, %ebp, %ecx     # imm = 0xB354
	movswl	%cx, %ecx
	imull	$197, %ecx, %ecx
	shrl	$16, %ecx
	movzbl	1616(%rsi,%rdx), %eax
	addl	%eax, %ebx
	subl	%ecx, %ebx
	leal	(%rbx,%r8), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	%r9d, %ecx
	addl	%ebx, %ecx
	addl	$-197, %ecx
	movswl	%cx, %ebx
	shrl	$15, %ebx
	andl	%r9d, %ebx
	addl	%ecx, %ebx
	shll	$8, %ebp
	orl	%eax, %ebp
	subl	%ebx, %ebp
	imull	$32269, %ebp, %eax      # imm = 0x7E0D
	leal	(%rax,%r8), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	%r9d, %ecx
	addl	%ecx, %eax
	addl	$-197, %eax
	movw	%bx, -104(%rsp,%rdx,4)
	movw	%ax, -102(%rsp,%rdx,4)
	cmpq	$-2, %rdx
	je	.LBB0_3
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	movzwl	-118(%rsp,%rdx,2), %ebp
	decq	%rdx
	jmp	.LBB0_1
.LBB0_3:
	movswl	-98(%rsp), %eax
	imull	$-1678, %eax, %ecx      # imm = 0xF972
	shrl	$16, %ecx
	imull	$-4679, %eax, %edx      # imm = 0xEDB9
	movswl	%dx, %edx
	imull	$3586, %edx, %edx       # imm = 0xE02
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1613(%rsi), %edx
	addl	%edx, %ecx
	movswl	%cx, %ebp
	shrl	$15, %ebp
	movl	$3586, %r8d             # imm = 0xE02
	andl	%r8d, %ebp
	addl	%ecx, %ebp
	shll	$7, %eax
	movswl	%bp, %ecx
	subl	%ecx, %edx
	shrl	%edx
	addl	%eax, %edx
	imull	$-1791, %edx, %eax      # imm = 0xF901
	leal	-120(%rax), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$120, %edx
	addl	%edx, %eax
	addl	$-120, %eax
	movw	%cx, -68(%rsp)
	movw	%ax, -66(%rsp)
	xorl	%ecx, %ecx
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	movswl	-100(%rsp,%rcx,2), %r9d
	imull	$-1678, %r9d, %ebp      # imm = 0xF972
	shrl	$16, %ebp
	imull	$-4679, %r9d, %eax      # imm = 0xEDB9
	cwtl
	imull	$3586, %eax, %eax       # imm = 0xE02
	shrl	$16, %eax
	subl	%eax, %ebp
	movzbl	1612(%rsi,%rcx,2), %eax
	addl	%eax, %ebp
	movswl	%bp, %ebx
	imull	$-1678, %ebx, %ebx      # imm = 0xF972
	shrl	$16, %ebx
	imull	$-4679, %ebp, %ebp      # imm = 0xEDB9
	movswl	%bp, %ebp
	imull	$3586, %ebp, %ebp       # imm = 0xE02
	shrl	$16, %ebp
	subl	%ebp, %ebx
	movzbl	1611(%rsi,%rcx,2), %ebp
	addl	%ebp, %ebx
	movswl	%bx, %edx
	shrl	$15, %edx
	andl	%r8d, %edx
	addl	%ebx, %edx
	shll	$15, %r9d
	shll	$7, %eax
	orl	%r9d, %eax
	movswl	%dx, %edx
	subl	%edx, %ebp
	shrl	%ebp
	addl	%eax, %ebp
	imull	$-1791, %ebp, %eax      # imm = 0xF901
	leal	-3586(%rax), %ebp
	movswl	%bp, %ebp
	shrl	$15, %ebp
	andl	%r8d, %ebp
	addl	%ebp, %eax
	addl	$-3586, %eax            # imm = 0xF1FE
	movw	%dx, -72(%rsp,%rcx,4)
	movw	%ax, -70(%rsp,%rcx,4)
	decq	%rcx
	cmpq	$-7, %rcx
	jne	.LBB0_4
# %bb.5:
	movswl	-66(%rsp), %r8d
	imull	$-238, %r8d, %eax
	shrl	$16, %eax
	imull	$-17513, %r8d, %edx     # imm = 0xBB97
	movswl	%dx, %edx
	imull	$958, %edx, %edx        # imm = 0x3BE
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	1598(%rsi), %edx
	addl	%edx, %eax
	movswl	%ax, %ebp
	imull	$-238, %ebp, %ebp
	shrl	$16, %ebp
	imull	$-17513, %eax, %eax     # imm = 0xBB97
	cwtl
	imull	$958, %eax, %eax        # imm = 0x3BE
	shrl	$16, %eax
	subl	%eax, %ebp
	movzbl	1597(%rsi), %ebx
	addl	%ebx, %ebp
	movswl	%bp, %ecx
	shrl	$15, %ecx
	movl	$958, %r9d              # imm = 0x3BE
	andl	%r9d, %ecx
	addl	%ebp, %ecx
	shll	$15, %r8d
	shll	$7, %edx
	orl	%r8d, %edx
	movswl	%cx, %ecx
	subl	%ecx, %ebx
	shrl	%ebx
	addl	%edx, %ebx
	imull	$-1505, %ebx, %edx      # imm = 0xFA1F
	leal	-8200(%rdx), %ebp
	movswl	%bp, %ebp
	shrl	$15, %ebp
	andl	$8200, %ebp             # imm = 0x2008
	addl	%ebp, %edx
	addl	$-8200, %edx            # imm = 0xDFF8
	movw	%cx, -4(%rsp)
	movw	%dx, -2(%rsp)
	xorl	%ecx, %ecx
.LBB0_6:                                # =>This Inner Loop Header: Depth=1
	movswl	-68(%rsp,%rcx,2), %edx
	imull	$-238, %edx, %ebp
	shrl	$16, %ebp
	imull	$-17513, %edx, %ebx     # imm = 0xBB97
	movswl	%bx, %ebx
	imull	$958, %ebx, %ebx        # imm = 0x3BE
	shrl	$16, %ebx
	subl	%ebx, %ebp
	movzbl	1596(%rsi,%rcx), %ebx
	addl	%ebx, %ebp
	movswl	%bp, %eax
	shrl	$15, %eax
	andl	%r9d, %eax
	addl	%ebp, %eax
	shll	$7, %edx
	cwtl
	subl	%eax, %ebx
	shrl	%ebx
	addl	%edx, %ebx
	imull	$-1505, %ebx, %edx      # imm = 0xFA1F
	leal	-958(%rdx), %ebp
	movswl	%bp, %ebp
	shrl	$15, %ebp
	andl	%r9d, %ebp
	addl	%ebp, %edx
	addl	$-958, %edx             # imm = 0xFC42
	movw	%ax, -8(%rsp,%rcx,4)
	movw	%dx, -6(%rsp,%rcx,4)
	decq	%rcx
	cmpq	$-15, %rcx
	jne	.LBB0_6
# %bb.7:
	movswl	-2(%rsp), %eax
	imull	$538, %eax, %ecx        # imm = 0x21A
	shrl	$16, %ecx
	imull	$-2118, %eax, %edx      # imm = 0xF7BA
	movswl	%dx, %edx
	imull	$7921, %edx, %edx       # imm = 0x1EF1
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1581(%rsi), %edx
	addl	%edx, %ecx
	movswl	%cx, %ebp
	shrl	$15, %ebp
	movl	$7921, %ebx             # imm = 0x1EF1
	andl	%ebp, %ebx
	addl	%ecx, %ebx
	shll	$8, %eax
	orl	%edx, %eax
	subl	%ebx, %eax
	imull	$4625, %eax, %eax       # imm = 0x1211
	leal	-265(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$265, %ecx              # imm = 0x109
	addl	%ecx, %eax
	addl	$-265, %eax             # imm = 0xFEF7
	movw	%bx, 124(%rsp)
	vpbroadcastw	.LCPI0_0(%rip), %ymm1 # ymm1 = [538,538,538,538,538,538,538,538,538,538,538,538,538,538,538,538]
	movw	%ax, 126(%rsp)
	vpbroadcastw	.LCPI0_1(%rip), %ymm2 # ymm2 = [63418,63418,63418,63418,63418,63418,63418,63418,63418,63418,63418,63418,63418,63418,63418,63418]
	movl	$15, %eax
	vpbroadcastw	.LCPI0_2(%rip), %ymm3 # ymm3 = [7921,7921,7921,7921,7921,7921,7921,7921,7921,7921,7921,7921,7921,7921,7921,7921]
	vmovdqa	.LCPI0_3(%rip), %ymm0   # ymm0 = [255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0]
	vpbroadcastw	.LCPI0_4(%rip), %ymm4 # ymm4 = [4625,4625,4625,4625,4625,4625,4625,4625,4625,4625,4625,4625,4625,4625,4625,4625]
.LBB0_8:                                # =>This Inner Loop Header: Depth=1
	vmovdqu	-64(%rsp,%rax,2), %ymm5
	vmovdqu	1519(%rsi,%rax,2), %ymm6
	vpsrlw	$8, %ymm6, %ymm7
	vpmulhw	%ymm1, %ymm5, %ymm8
	vpmullw	%ymm2, %ymm5, %ymm5
	vpmulhw	%ymm3, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm8, %ymm5
	vpaddw	%ymm7, %ymm5, %ymm5
	vpmulhw	%ymm1, %ymm5, %ymm7
	vpmullw	%ymm2, %ymm5, %ymm5
	vpmulhw	%ymm3, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm7, %ymm5
	vpand	%ymm0, %ymm6, %ymm7
	vpaddw	%ymm7, %ymm5, %ymm5
	vpsraw	$15, %ymm5, %ymm7
	vpand	%ymm3, %ymm7, %ymm7
	vpaddw	%ymm5, %ymm7, %ymm5
	vpsubw	%ymm5, %ymm6, %ymm6
	vpmullw	%ymm4, %ymm6, %ymm6
	vpcmpgtw	%ymm6, %ymm3, %ymm7
	vpandn	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpunpcklwd	%ymm6, %ymm5, %ymm7 # ymm7 = ymm5[0],ymm6[0],ymm5[1],ymm6[1],ymm5[2],ymm6[2],ymm5[3],ymm6[3],ymm5[8],ymm6[8],ymm5[9],ymm6[9],ymm5[10],ymm6[10],ymm5[11],ymm6[11]
	vpunpckhwd	%ymm6, %ymm5, %ymm5 # ymm5 = ymm5[4],ymm6[4],ymm5[5],ymm6[5],ymm5[6],ymm6[6],ymm5[7],ymm6[7],ymm5[12],ymm6[12],ymm5[13],ymm6[13],ymm5[14],ymm6[14],ymm5[15],ymm6[15]
	vinserti128	$1, %xmm5, %ymm7, %ymm6
	vmovdqu	%ymm6, (%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm7, %ymm5 # ymm5 = ymm7[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 32(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_10
# %bb.9:                                #   in Loop: Header=BB0_8 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_8
.LBB0_10:
	vpbroadcastw	.LCPI0_5(%rip), %ymm1 # ymm1 = [32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32]
	movzwl	126(%rsp), %eax
	vpbroadcastw	.LCPI0_6(%rip), %ymm2 # ymm2 = [64800,64800,64800,64800,64800,64800,64800,64800,64800,64800,64800,64800,64800,64800,64800,64800]
	movw	%ax, 380(%rsp)
	vpbroadcastw	.LCPI0_7(%rip), %ymm3 # ymm3 = [89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89]
	movl	$47, %eax
	vpbroadcastw	.LCPI0_8(%rip), %ymm4 # ymm4 = [18409,18409,18409,18409,18409,18409,18409,18409,18409,18409,18409,18409,18409,18409,18409,18409]
.LBB0_11:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	(%rsp,%rax,2), %ymm5
	vpmulhw	%ymm1, %ymm5, %ymm6
	vpmullw	%ymm2, %ymm5, %ymm7
	vpmulhw	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpsraw	$15, %ymm6, %ymm7
	vpand	%ymm3, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm3, %ymm7
	vpandn	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm6, %ymm7 # ymm7 = ymm6[0],ymm5[0],ymm6[1],ymm5[1],ymm6[2],ymm5[2],ymm6[3],ymm5[3],ymm6[8],ymm5[8],ymm6[9],ymm5[9],ymm6[10],ymm5[10],ymm6[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm6, %ymm5 # ymm5 = ymm6[4],ymm5[4],ymm6[5],ymm5[5],ymm6[6],ymm5[6],ymm6[7],ymm5[7],ymm6[12],ymm5[12],ymm6[13],ymm5[13],ymm6[14],ymm5[14],ymm6[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm7, %ymm6
	vmovdqu	%ymm6, 128(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm7, %ymm5 # ymm5 = ymm7[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 160(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_13
# %bb.12:                               #   in Loop: Header=BB0_11 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_11
.LBB0_13:
	movswl	380(%rsp), %r8d
	imull	$-84, %r8d, %edx
	shrl	$16, %edx
	imull	$-6950, %r8d, %ecx      # imm = 0xE4DA
	movswl	%cx, %ecx
	imull	$2414, %ecx, %ecx       # imm = 0x96E
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	1518(%rsi), %ecx
	addl	%ecx, %edx
	movswl	%dx, %ebp
	imull	$-84, %ebp, %ebp
	shrl	$16, %ebp
	imull	$-6950, %edx, %edx      # imm = 0xE4DA
	movswl	%dx, %edx
	imull	$2414, %edx, %edx       # imm = 0x96E
	shrl	$16, %edx
	subl	%edx, %ebp
	movzbl	1517(%rsi), %edx
	addl	%edx, %ebp
	movswl	%bp, %ebx
	shrl	$15, %ebx
	movl	$2414, %eax             # imm = 0x96E
	andl	%ebx, %eax
	addl	%ebp, %eax
	shll	$15, %r8d
	shll	$7, %ecx
	orl	%r8d, %ecx
	cwtl
	subl	%eax, %edx
	shrl	%edx
	addl	%ecx, %edx
	imull	$6407, %edx, %ecx       # imm = 0x1907
	leal	-7177(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$7177, %edx             # imm = 0x1C09
	addl	%edx, %ecx
	addl	$-7177, %ecx            # imm = 0xE3F7
	vpbroadcastw	.LCPI0_9(%rip), %ymm1 # ymm1 = [65452,65452,65452,65452,65452,65452,65452,65452,65452,65452,65452,65452,65452,65452,65452,65452]
	movw	%ax, 888(%rsp)
	vpbroadcastw	.LCPI0_10(%rip), %ymm2 # ymm2 = [58586,58586,58586,58586,58586,58586,58586,58586,58586,58586,58586,58586,58586,58586,58586,58586]
	movw	%cx, 890(%rsp)
	vpbroadcastw	.LCPI0_11(%rip), %ymm3 # ymm3 = [2414,2414,2414,2414,2414,2414,2414,2414,2414,2414,2414,2414,2414,2414,2414,2414]
	movl	$110, %eax
	vpbroadcastw	.LCPI0_12(%rip), %ymm4 # ymm4 = [6407,6407,6407,6407,6407,6407,6407,6407,6407,6407,6407,6407,6407,6407,6407,6407]
.LBB0_14:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	128(%rsp,%rax,2), %ymm5
	vmovdqu	1265(%rsi,%rax,2), %ymm6
	vpsrlw	$8, %ymm6, %ymm7
	vpmulhw	%ymm1, %ymm5, %ymm8
	vpmullw	%ymm2, %ymm5, %ymm9
	vpmulhw	%ymm3, %ymm9, %ymm9
	vpsubw	%ymm9, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm8
	vpmulhw	%ymm1, %ymm8, %ymm9
	vpmullw	%ymm2, %ymm8, %ymm8
	vpmulhw	%ymm3, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm9, %ymm8
	vpand	%ymm0, %ymm6, %ymm6
	vpaddw	%ymm6, %ymm8, %ymm8
	vpsraw	$15, %ymm8, %ymm9
	vpand	%ymm3, %ymm9, %ymm9
	vpaddw	%ymm8, %ymm9, %ymm8
	vpsllw	$15, %ymm5, %ymm5
	vpsllw	$7, %ymm7, %ymm7
	vpor	%ymm5, %ymm7, %ymm5
	vpsubw	%ymm8, %ymm6, %ymm6
	vpsraw	$1, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm3, %ymm6
	vpandn	%ymm3, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm8, %ymm6 # ymm6 = ymm8[0],ymm5[0],ymm8[1],ymm5[1],ymm8[2],ymm5[2],ymm8[3],ymm5[3],ymm8[8],ymm5[8],ymm8[9],ymm5[9],ymm8[10],ymm5[10],ymm8[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm8, %ymm5 # ymm5 = ymm8[4],ymm5[4],ymm8[5],ymm5[5],ymm8[6],ymm5[6],ymm8[7],ymm5[7],ymm8[12],ymm5[12],ymm8[13],ymm5[13],ymm8[14],ymm5[14],ymm8[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm6, %ymm7
	vmovdqu	%ymm7, 384(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 416(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_16
# %bb.15:                               #   in Loop: Header=BB0_14 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_14
.LBB0_16:
	vpbroadcastw	.LCPI0_13(%rip), %ymm1 # ymm1 = [46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46]
	movzwl	890(%rsp), %eax
	vpbroadcastw	.LCPI0_14(%rip), %ymm2 # ymm2 = [44191,44191,44191,44191,44191,44191,44191,44191,44191,44191,44191,44191,44191,44191,44191,44191]
	movw	%ax, 1908(%rsp)
	vpbroadcastw	.LCPI0_15(%rip), %ymm3 # ymm3 = [786,786,786,786,786,786,786,786,786,786,786,786,786,786,786,786]
	movl	$237, %eax
	vpbroadcastw	.LCPI0_16(%rip), %ymm4 # ymm4 = [50361,50361,50361,50361,50361,50361,50361,50361,50361,50361,50361,50361,50361,50361,50361,50361]
.LBB0_17:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	384(%rsp,%rax,2), %ymm5
	vpmovzxbw	1012(%rsi,%rax), %ymm6 # ymm6 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm1, %ymm5, %ymm7
	vpmullw	%ymm2, %ymm5, %ymm8
	vpmulhw	%ymm3, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm7
	vpsraw	$15, %ymm7, %ymm8
	vpand	%ymm3, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm7
	vpsllw	$7, %ymm5, %ymm5
	vpsubw	%ymm7, %ymm6, %ymm6
	vpsraw	$1, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm3, %ymm6
	vpandn	%ymm3, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm7, %ymm6 # ymm6 = ymm7[0],ymm5[0],ymm7[1],ymm5[1],ymm7[2],ymm5[2],ymm7[3],ymm5[3],ymm7[8],ymm5[8],ymm7[9],ymm5[9],ymm7[10],ymm5[10],ymm7[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm7, %ymm5 # ymm5 = ymm7[4],ymm5[4],ymm7[5],ymm5[5],ymm7[6],ymm5[6],ymm7[7],ymm5[7],ymm7[12],ymm5[12],ymm7[13],ymm5[13],ymm7[14],ymm5[14],ymm7[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm6, %ymm7
	vmovdqu	%ymm7, 896(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 928(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_19
# %bb.18:                               #   in Loop: Header=BB0_17 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_17
.LBB0_19:
	movl	$-3588, %eax            # imm = 0xF1FC
	addl	1908(%rsp), %eax
	vpbroadcastw	.LCPI0_17(%rip), %ymm1 # ymm1 = [62926,62926,62926,62926,62926,62926,62926,62926,62926,62926,62926,62926,62926,62926,62926,62926]
	vpbroadcastw	.LCPI0_18(%rip), %ymm2 # ymm2 = [63198,63198,63198,63198,63198,63198,63198,63198,63198,63198,63198,63198,63198,63198,63198,63198]
	vpbroadcastw	.LCPI0_19(%rip), %ymm3 # ymm3 = [7177,7177,7177,7177,7177,7177,7177,7177,7177,7177,7177,7177,7177,7177,7177,7177]
	movw	%ax, 2024(%rdi)
	vpbroadcastw	.LCPI0_20(%rip), %ymm4 # ymm4 = [12857,12857,12857,12857,12857,12857,12857,12857,12857,12857,12857,12857,12857,12857,12857,12857]
	movl	$490, %eax              # imm = 0x1EA
	vpbroadcastw	.LCPI0_21(%rip), %ymm5 # ymm5 = [61948,61948,61948,61948,61948,61948,61948,61948,61948,61948,61948,61948,61948,61948,61948,61948]
.LBB0_20:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	896(%rsp,%rax,2), %ymm6
	vmovdqu	(%rsi,%rax,2), %ymm7
	vpsrlw	$8, %ymm7, %ymm8
	vpmulhw	%ymm1, %ymm6, %ymm9
	vpmullw	%ymm2, %ymm6, %ymm6
	vpmulhw	%ymm3, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm9, %ymm6
	vpaddw	%ymm6, %ymm8, %ymm6
	vpmulhw	%ymm1, %ymm6, %ymm8
	vpmullw	%ymm2, %ymm6, %ymm6
	vpmulhw	%ymm3, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm8, %ymm6
	vpand	%ymm0, %ymm7, %ymm8
	vpaddw	%ymm6, %ymm8, %ymm6
	vpsraw	$15, %ymm6, %ymm8
	vpand	%ymm3, %ymm8, %ymm8
	vpaddw	%ymm6, %ymm8, %ymm6
	vpsubw	%ymm6, %ymm7, %ymm7
	vpmullw	%ymm4, %ymm7, %ymm7
	vpcmpgtw	%ymm7, %ymm3, %ymm8
	vpandn	%ymm3, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm7, %ymm7
	vpaddw	%ymm5, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm7, %ymm7
	vpunpcklwd	%ymm7, %ymm6, %ymm8 # ymm8 = ymm6[0],ymm7[0],ymm6[1],ymm7[1],ymm6[2],ymm7[2],ymm6[3],ymm7[3],ymm6[8],ymm7[8],ymm6[9],ymm7[9],ymm6[10],ymm7[10],ymm6[11],ymm7[11]
	vpunpckhwd	%ymm7, %ymm6, %ymm6 # ymm6 = ymm6[4],ymm7[4],ymm6[5],ymm7[5],ymm6[6],ymm7[6],ymm6[7],ymm7[7],ymm6[12],ymm7[12],ymm6[13],ymm7[13],ymm6[14],ymm7[14],ymm6[15],ymm7[15]
	vinserti128	$1, %xmm6, %ymm8, %ymm7
	vmovdqu	%ymm7, (%rdi,%rax,4)
	vperm2i128	$49, %ymm6, %ymm8, %ymm6 # ymm6 = ymm8[2,3],ymm6[2,3]
	vmovdqu	%ymm6, 32(%rdi,%rax,4)
	testq	%rax, %rax
	je	.LBB0_22
# %bb.21:                               #   in Loop: Header=BB0_20 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_20
.LBB0_22:
	addq	$1912, %rsp             # imm = 0x778
	.cfi_def_cfa_offset 24
	popq	%rbx
	.cfi_def_cfa_offset 16
	popq	%rbp
	.cfi_def_cfa_offset 8
	vzeroupper
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_decode_1013x7177), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_decode_1013x7177)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
