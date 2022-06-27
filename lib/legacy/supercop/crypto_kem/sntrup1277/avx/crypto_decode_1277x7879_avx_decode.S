	.text
	.file	"decode.c"
	.section	.rodata.cst4,"aM",@progbits,4
	.p2align	2               # -- Begin function CRYPTO_NAMESPACE(crypto_decode_1277x7879)
.LCPI0_0:
	.long	262                     # 0x106
.LCPI0_3:
	.long	537                     # 0x219
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4
.LCPI0_1:
	.byte	2                       # 0x2
	.byte	3                       # 0x3
	.byte	6                       # 0x6
	.byte	7                       # 0x7
	.byte	10                      # 0xa
	.byte	11                      # 0xb
	.byte	14                      # 0xe
	.byte	15                      # 0xf
	.zero	1
	.zero	1
	.zero	1
	.zero	1
	.zero	1
	.zero	1
	.zero	1
	.zero	1
.LCPI0_4:
	.byte	3                       # 0x3
	.byte	255                     # 0xff
	.byte	2                       # 0x2
	.byte	255                     # 0xff
	.byte	1                       # 0x1
	.byte	255                     # 0xff
	.byte	0                       # 0x0
	.byte	255                     # 0xff
	.zero	1
	.byte	255                     # 0xff
	.zero	1
	.byte	255                     # 0xff
	.zero	1
	.byte	255                     # 0xff
	.zero	1
	.byte	255                     # 0xff
	.section	.rodata,"a",@progbits
	.p2align	1
.LCPI0_2:
	.short	34294                   # 0x85f6
.LCPI0_5:
	.short	64999                   # 0xfde7
.LCPI0_6:
	.short	537                     # 0x219
.LCPI0_7:
	.short	14889                   # 0x3a29
.LCPI0_8:
	.short	9                       # 0x9
.LCPI0_9:
	.short	64685                   # 0xfcad
.LCPI0_10:
	.short	77                      # 0x4d
.LCPI0_11:
	.short	14469                   # 0x3885
.LCPI0_12:
	.short	36                      # 0x24
.LCPI0_13:
	.short	11235                   # 0x2be3
.LCPI0_14:
	.short	140                     # 0x8c
.LCPI0_15:
	.short	16                      # 0x10
.LCPI0_16:
	.short	65068                   # 0xfe2c
.LCPI0_17:
	.short	44939                   # 0xaf8b
.LCPI0_18:
	.short	64                      # 0x40
.LCPI0_19:
	.short	42304                   # 0xa540
.LCPI0_20:
	.short	189                     # 0xbd
.LCPI0_21:
	.short	65347                   # 0xff43
.LCPI0_22:
	.short	64149                   # 0xfa95
.LCPI0_23:
	.short	1658                    # 0x67a
.LCPI0_24:
	.short	60758                   # 0xed56
.LCPI0_25:
	.short	3511                    # 0xdb7
.LCPI0_27:
	.short	24583                   # 0x6007
.LCPI0_28:
	.short	460                     # 0x1cc
.LCPI0_29:
	.short	47839                   # 0xbadf
.LCPI0_30:
	.short	948                     # 0x3b4
.LCPI0_31:
	.short	23781                   # 0x5ce5
.LCPI0_32:
	.short	2825                    # 0xb09
.LCPI0_33:
	.short	63407                   # 0xf7af
.LCPI0_34:
	.short	7879                    # 0x1ec7
.LCPI0_35:
	.short	17143                   # 0x42f7
.LCPI0_36:
	.short	61597                   # 0xf09d
	.section	.rodata.cst32,"aM",@progbits,32
	.p2align	5
.LCPI0_26:
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
	.globl	CRYPTO_NAMESPACE(crypto_decode_1277x7879)
	.type	CRYPTO_NAMESPACE(crypto_decode_1277x7879),@function
CRYPTO_NAMESPACE(crypto_decode_1277x7879): # @CRYPTO_NAMESPACE(crypto_decode_1277x7879)
	.cfi_startproc
# %bb.0:
	subq	$2472, %rsp             # imm = 0x9A8
	.cfi_def_cfa_offset 2480
	movzbl	2066(%rsi), %eax
	imull	$1072, %eax, %ecx       # imm = 0x430
	shrl	$16, %ecx
	imull	$-297467904, %eax, %eax # imm = 0xEE450000
	sarl	$16, %eax
	imull	$3696, %eax, %eax       # imm = 0xE70
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	2065(%rsi), %eax
	addl	%ecx, %eax
	movswl	%ax, %ecx
	shrl	$15, %ecx
	movl	$3696, %edx             # imm = 0xE70
	andl	%ecx, %edx
	addl	%eax, %edx
	movswl	%dx, %r8d
	shll	$5, %r8d
	leal	(%r8,%r8,2), %ecx
	shrl	$16, %ecx
	imull	$20916, %edx, %edx      # imm = 0x51B4
	movswl	%dx, %edx
	imull	$376, %edx, %eax        # imm = 0x178
	shrl	$16, %eax
	movzbl	2064(%rsi), %edx
	addl	%edx, %ecx
	subl	%eax, %ecx
	leal	-376(%rcx), %eax
	cwtl
	shrl	$15, %eax
	movl	$376, %r9d              # imm = 0x178
	andl	%r9d, %eax
	addl	%ecx, %eax
	addl	$-376, %eax             # imm = 0xFE88
	movswl	%ax, %ecx
	shrl	$15, %ecx
	andl	%r9d, %ecx
	addl	%eax, %ecx
	movswl	%cx, %r9d
	subl	%r9d, %edx
	shrl	$3, %edx
	addl	%r8d, %edx
	imull	$18127, %edx, %edx      # imm = 0x46CF
	leal	-2516(%rdx), %eax
	cwtl
	shrl	$15, %eax
	andl	$2516, %eax             # imm = 0x9D4
	leal	(%rax,%rdx), %r8d
	addl	$-2516, %r8d            # imm = 0xF62C
	movw	%r8w, -114(%rsp)
	imull	$694, %r9d, %eax        # imm = 0x2B6
	shrl	$16, %eax
	imull	$-3381, %ecx, %ecx      # imm = 0xF2CB
	movswl	%cx, %ecx
	imull	$4962, %ecx, %ecx       # imm = 0x1362
	shrl	$16, %ecx
	subl	%ecx, %eax
	movzbl	2063(%rsi), %r10d
	addl	%r10d, %eax
	movswl	%ax, %ecx
	imull	$694, %ecx, %ecx        # imm = 0x2B6
	shrl	$16, %ecx
	imull	$-3381, %eax, %eax      # imm = 0xF2CB
	cwtl
	imull	$4962, %eax, %eax       # imm = 0x1362
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	2062(%rsi), %eax
	addl	%eax, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	movl	$4962, %r11d            # imm = 0x1362
	andl	%r11d, %edx
	addl	%ecx, %edx
	shll	$15, %r9d
	shll	$7, %r10d
	orl	%r9d, %r10d
	movswl	%dx, %ecx
	subl	%ecx, %eax
	shrl	%eax
	addl	%r10d, %eax
	imull	$-24751, %eax, %eax     # imm = 0x9F51
	leal	-4962(%rax), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r11d, %edx
	addl	%eax, %edx
	addl	$-4962, %edx            # imm = 0xEC9E
	movw	%cx, -118(%rsp)
	movw	%dx, -116(%rsp)
	movw	%r8w, -104(%rsp)
	xorl	%r9d, %r9d
	movl	$1127, %r8d             # imm = 0x467
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movswl	%dx, %ecx
	imull	$-433, %ecx, %ecx       # imm = 0xFE4F
	shrl	$16, %ecx
	imull	$-14887, %edx, %eax     # imm = 0xC5D9
	cwtl
	imull	$1127, %eax, %eax       # imm = 0x467
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	2061(%rsi,%r9), %r10d
	addl	%r10d, %ecx
	movswl	%cx, %eax
	shrl	$15, %eax
	andl	%r8d, %eax
	addl	%ecx, %eax
	shll	$8, %edx
	orl	%r10d, %edx
	subl	%eax, %edx
	imull	$-10409, %edx, %ecx     # imm = 0xD757
	leal	-1127(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r8d, %edx
	addl	%edx, %ecx
	addl	$-1127, %ecx            # imm = 0xFB99
	movw	%ax, -108(%rsp,%r9,4)
	movw	%cx, -106(%rsp,%r9,4)
	cmpq	$-1, %r9
	je	.LBB0_3
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	movzwl	-118(%rsp,%r9,2), %edx
	decq	%r9
	jmp	.LBB0_1
.LBB0_3:
	movswl	-104(%rsp), %eax
	imull	$262, %eax, %ecx        # imm = 0x106
	shrl	$16, %ecx
	imull	$-31242, %eax, %edx     # imm = 0x85F6
	movswl	%dx, %edx
	imull	$537, %edx, %edx        # imm = 0x219
	shrl	$16, %edx
	movzbl	2059(%rsi), %r8d
	addl	%r8d, %ecx
	subl	%edx, %ecx
	leal	-537(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	movl	$537, %r9d              # imm = 0x219
	andl	%r9d, %edx
	addl	%edx, %ecx
	addl	$-537, %ecx             # imm = 0xFDE7
	movswl	%cx, %edx
	shrl	$15, %edx
	andl	%r9d, %edx
	addl	%ecx, %edx
	shll	$8, %eax
	orl	%r8d, %eax
	subl	%edx, %eax
	imull	$14889, %eax, %eax      # imm = 0x3A29
	leal	-1199(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$1199, %ecx             # imm = 0x4AF
	addl	%ecx, %eax
	addl	$-1199, %eax            # imm = 0xFB51
	movw	%dx, -80(%rsp)
	movw	%ax, -78(%rsp)
	vmovq	-112(%rsp), %xmm0       # xmm0 = mem[0],zero
	vpshuflw	$27, %xmm0, %xmm0 # xmm0 = xmm0[3,2,1,0,4,5,6,7]
	vpmovsxwd	%xmm0, %xmm1
	vpbroadcastd	.LCPI0_0(%rip), %xmm2 # xmm2 = [262,262,262,262]
	vpmulld	%xmm2, %xmm1, %xmm1
	vmovdqa	.LCPI0_1(%rip), %xmm2   # xmm2 = <2,3,6,7,10,11,14,15,u,u,u,u,u,u,u,u>
	vpbroadcastw	.LCPI0_2(%rip), %xmm3 # xmm3 = [34294,34294,34294,34294,34294,34294,34294,34294]
	vpmullw	%xmm3, %xmm0, %xmm3
	vpmovsxwd	%xmm3, %xmm3
	vpbroadcastd	.LCPI0_3(%rip), %xmm4 # xmm4 = [537,537,537,537]
	vpmulld	%xmm4, %xmm3, %xmm3
	vpshufb	%xmm2, %xmm1, %xmm1
	vmovd	2055(%rsi), %xmm4       # xmm4 = mem[0],zero,zero,zero
	vpshufb	.LCPI0_4(%rip), %xmm4, %xmm4 # xmm4 = xmm4[3],zero,xmm4[2],zero,xmm4[1],zero,xmm4[0],zero,xmm4[u],zero,xmm4[u],zero,xmm4[u],zero,xmm4[u],zero
	vpshufb	%xmm2, %xmm3, %xmm2
	vpbroadcastw	.LCPI0_5(%rip), %xmm3 # xmm3 = [64999,64999,64999,64999,64999,64999,64999,64999]
	vpaddw	%xmm1, %xmm4, %xmm1
	vpsubw	%xmm2, %xmm1, %xmm1
	vpaddw	%xmm3, %xmm1, %xmm1
	vpsraw	$15, %xmm1, %xmm2
	vpbroadcastw	.LCPI0_6(%rip), %xmm5 # xmm5 = [537,537,537,537,537,537,537,537]
	vpand	%xmm5, %xmm2, %xmm2
	vpaddw	%xmm1, %xmm2, %xmm1
	vpsraw	$15, %xmm1, %xmm2
	vpand	%xmm5, %xmm2, %xmm2
	vpaddw	%xmm1, %xmm2, %xmm1
	vpsllw	$8, %xmm0, %xmm0
	vpbroadcastw	.LCPI0_7(%rip), %xmm2 # xmm2 = [14889,14889,14889,14889,14889,14889,14889,14889]
	vpor	%xmm4, %xmm0, %xmm0
	vpsubw	%xmm1, %xmm0, %xmm0
	vpmullw	%xmm2, %xmm0, %xmm0
	vpaddw	%xmm3, %xmm0, %xmm0
	vpsraw	$15, %xmm0, %xmm2
	vpand	%xmm5, %xmm2, %xmm2
	vpaddw	%xmm0, %xmm2, %xmm0
	vpunpcklwd	%xmm0, %xmm1, %xmm0 # xmm0 = xmm1[0],xmm0[0],xmm1[1],xmm0[1],xmm1[2],xmm0[2],xmm1[3],xmm0[3]
	vpshufd	$27, %xmm0, %xmm0       # xmm0 = xmm0[3,2,1,0]
	vmovdqa	%xmm0, -96(%rsp)
	movswl	%ax, %ecx
	imull	$-1854, %ecx, %edx      # imm = 0xF8C2
	shrl	$16, %edx
	imull	$-2830, %eax, %eax      # imm = 0xF4F2
	cwtl
	imull	$5929, %eax, %eax       # imm = 0x1729
	shrl	$16, %eax
	subl	%eax, %edx
	movzbl	2054(%rsi), %r8d
	addl	%r8d, %edx
	movswl	%dx, %eax
	imull	$-1854, %eax, %ecx      # imm = 0xF8C2
	shrl	$16, %ecx
	imull	$-2830, %edx, %eax      # imm = 0xF4F2
	cwtl
	imull	$5929, %eax, %eax       # imm = 0x1729
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	2053(%rsi), %r9d
	addl	%r9d, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	movl	$5929, %r10d            # imm = 0x1729
	andl	%r10d, %edx
	addl	%ecx, %edx
	shll	$8, %r8d
	orl	%r9d, %r8d
	subl	%edx, %r8d
	imull	$29977, %r8d, %r8d      # imm = 0x7519
	leal	-13244(%r8), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$13244, %ecx            # imm = 0x33BC
	addl	%r8d, %ecx
	addl	$-13244, %ecx           # imm = 0xCC44
	movw	%dx, -28(%rsp)
	movw	%cx, -26(%rsp)
	xorl	%r8d, %r8d
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	movswl	-80(%rsp,%r8,2), %edx
	imull	$-1854, %edx, %eax      # imm = 0xF8C2
	shrl	$16, %eax
	imull	$-2830, %edx, %edx      # imm = 0xF4F2
	movswl	%dx, %edx
	imull	$5929, %edx, %edx       # imm = 0x1729
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	2052(%rsi,%r8,2), %edx
	addl	%edx, %eax
	movswl	%ax, %ecx
	imull	$-1854, %ecx, %ecx      # imm = 0xF8C2
	shrl	$16, %ecx
	imull	$-2830, %eax, %eax      # imm = 0xF4F2
	cwtl
	imull	$5929, %eax, %eax       # imm = 0x1729
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	2051(%rsi,%r8,2), %r9d
	addl	%r9d, %ecx
	movswl	%cx, %eax
	shrl	$15, %eax
	andl	%r10d, %eax
	addl	%ecx, %eax
	shll	$8, %edx
	orl	%r9d, %edx
	subl	%eax, %edx
	imull	$29977, %edx, %ecx      # imm = 0x7519
	leal	-5929(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r10d, %edx
	addl	%edx, %ecx
	addl	$-5929, %ecx            # imm = 0xE8D7
	movw	%ax, -32(%rsp,%r8,4)
	movw	%cx, -30(%rsp,%r8,4)
	decq	%r8
	cmpq	$-9, %r8
	jne	.LBB0_4
# %bb.5:
	movswl	-26(%rsp), %r8d
	leal	(%r8,%r8,8), %ecx
	shrl	$16, %ecx
	imull	$-851, %r8d, %edx       # imm = 0xFCAD
	movswl	%dx, %edx
	imull	$77, %edx, %edx
	shrl	$16, %edx
	subl	%edx, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	movl	$77, %eax
	andl	%edx, %eax
	addl	%ecx, %eax
	subl	%eax, %r8d
	imull	$14469, %r8d, %ecx      # imm = 0x3885
	leal	-172(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$172, %edx
	addl	%edx, %ecx
	addl	$-172, %ecx
	vpbroadcastw	.LCPI0_8(%rip), %ymm0 # ymm0 = [9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9]
	movw	%ax, 60(%rsp)
	vpbroadcastw	.LCPI0_9(%rip), %ymm1 # ymm1 = [64685,64685,64685,64685,64685,64685,64685,64685,64685,64685,64685,64685,64685,64685,64685,64685]
	movw	%cx, 62(%rsp)
	vpbroadcastw	.LCPI0_10(%rip), %ymm2 # ymm2 = [77,77,77,77,77,77,77,77,77,77,77,77,77,77,77,77]
	movl	$3, %eax
	vpbroadcastw	.LCPI0_11(%rip), %ymm3 # ymm3 = [14469,14469,14469,14469,14469,14469,14469,14469,14469,14469,14469,14469,14469,14469,14469,14469]
.LBB0_6:                                # =>This Inner Loop Header: Depth=1
	vmovdqu	-64(%rsp,%rax,2), %ymm4
	vpmulhw	%ymm0, %ymm4, %ymm5
	vpmullw	%ymm1, %ymm4, %ymm6
	vpmulhw	%ymm2, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpsraw	$15, %ymm5, %ymm6
	vpand	%ymm2, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm5
	vpsubw	%ymm5, %ymm4, %ymm4
	vpmullw	%ymm3, %ymm4, %ymm4
	vpcmpgtw	%ymm4, %ymm2, %ymm6
	vpandn	%ymm2, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm4, %ymm4
	vpunpcklwd	%ymm4, %ymm5, %ymm6 # ymm6 = ymm5[0],ymm4[0],ymm5[1],ymm4[1],ymm5[2],ymm4[2],ymm5[3],ymm4[3],ymm5[8],ymm4[8],ymm5[9],ymm4[9],ymm5[10],ymm4[10],ymm5[11],ymm4[11]
	vpunpckhwd	%ymm4, %ymm5, %ymm4 # ymm4 = ymm5[4],ymm4[4],ymm5[5],ymm4[5],ymm5[6],ymm4[6],ymm5[7],ymm4[7],ymm5[12],ymm4[12],ymm5[13],ymm4[13],ymm5[14],ymm4[14],ymm5[15],ymm4[15]
	vinserti128	$1, %xmm4, %ymm6, %ymm5
	vmovdqu	%ymm5, -16(%rsp,%rax,4)
	vperm2i128	$49, %ymm4, %ymm6, %ymm4 # ymm4 = ymm6[2,3],ymm4[2,3]
	vmovdqu	%ymm4, 16(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_8
# %bb.7:                                #   in Loop: Header=BB0_6 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_6
.LBB0_8:
	movswl	62(%rsp), %r8d
	leal	(,%r8,4), %ecx
	leal	(%rcx,%rcx,8), %edx
	shrl	$16, %edx
	imull	$11235, %r8d, %ecx      # imm = 0x2BE3
	movswl	%cx, %ecx
	imull	$140, %ecx, %ecx
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	2034(%rsi), %ecx
	addl	%ecx, %edx
	movswl	%dx, %eax
	sarl	$12, %eax
	imull	$-468, %edx, %edx       # imm = 0xFE2C
	movswl	%dx, %edx
	imull	$140, %edx, %edx
	shrl	$16, %edx
	subl	%edx, %eax
	movswl	%ax, %r9d
	shrl	$15, %r9d
	movl	$140, %edx
	andl	%r9d, %edx
	addl	%eax, %edx
	shll	$6, %r8d
	movswl	%dx, %eax
	subl	%eax, %ecx
	shrl	$2, %ecx
	addl	%r8d, %ecx
	imull	$-20597, %ecx, %ecx     # imm = 0xAF8B
	leal	-313(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	vpbroadcastw	.LCPI0_12(%rip), %ymm0 # ymm0 = [36,36,36,36,36,36,36,36,36,36,36,36,36,36,36,36]
	andl	$313, %edx              # imm = 0x139
	vpbroadcastw	.LCPI0_13(%rip), %ymm1 # ymm1 = [11235,11235,11235,11235,11235,11235,11235,11235,11235,11235,11235,11235,11235,11235,11235,11235]
	addl	%edx, %ecx
	addl	$-313, %ecx             # imm = 0xFEC7
	vpbroadcastw	.LCPI0_14(%rip), %ymm2 # ymm2 = [140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140]
	movw	%ax, 220(%rsp)
	vpbroadcastw	.LCPI0_15(%rip), %ymm3 # ymm3 = [16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16]
	movw	%cx, 222(%rsp)
	vpbroadcastw	.LCPI0_16(%rip), %ymm4 # ymm4 = [65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068]
	movl	$23, %eax
	vpbroadcastw	.LCPI0_17(%rip), %ymm5 # ymm5 = [44939,44939,44939,44939,44939,44939,44939,44939,44939,44939,44939,44939,44939,44939,44939,44939]
.LBB0_9:                                # =>This Inner Loop Header: Depth=1
	vmovdqu	-16(%rsp,%rax,2), %ymm6
	vpmovzxbw	1995(%rsi,%rax), %ymm7 # ymm7 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm0, %ymm6, %ymm8
	vpmullw	%ymm1, %ymm6, %ymm9
	vpmulhw	%ymm2, %ymm9, %ymm9
	vpsubw	%ymm9, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm8
	vpmulhw	%ymm3, %ymm8, %ymm9
	vpmullw	%ymm4, %ymm8, %ymm8
	vpmulhw	%ymm2, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm9, %ymm8
	vpsraw	$15, %ymm8, %ymm9
	vpand	%ymm2, %ymm9, %ymm9
	vpaddw	%ymm8, %ymm9, %ymm8
	vpsllw	$6, %ymm6, %ymm6
	vpsubw	%ymm8, %ymm7, %ymm7
	vpsraw	$2, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm6
	vpmullw	%ymm5, %ymm6, %ymm6
	vpcmpgtw	%ymm6, %ymm2, %ymm7
	vpandn	%ymm2, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpunpcklwd	%ymm6, %ymm8, %ymm7 # ymm7 = ymm8[0],ymm6[0],ymm8[1],ymm6[1],ymm8[2],ymm6[2],ymm8[3],ymm6[3],ymm8[8],ymm6[8],ymm8[9],ymm6[9],ymm8[10],ymm6[10],ymm8[11],ymm6[11]
	vpunpckhwd	%ymm6, %ymm8, %ymm6 # ymm6 = ymm8[4],ymm6[4],ymm8[5],ymm6[5],ymm8[6],ymm6[6],ymm8[7],ymm6[7],ymm8[12],ymm6[12],ymm8[13],ymm6[13],ymm8[14],ymm6[14],ymm8[15],ymm6[15]
	vinserti128	$1, %xmm6, %ymm7, %ymm8
	vmovdqu	%ymm8, 64(%rsp,%rax,4)
	vperm2i128	$49, %ymm6, %ymm7, %ymm6 # ymm6 = ymm7[2,3],ymm6[2,3]
	vmovdqu	%ymm6, 96(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_11
# %bb.10:                               #   in Loop: Header=BB0_9 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_9
.LBB0_11:
	movswl	222(%rsp), %eax
	movl	%eax, %ecx
	sarl	$10, %ecx
	imull	$-23232, %eax, %edx     # imm = 0xA540
	movswl	%dx, %edx
	imull	$189, %edx, %edx
	shrl	$16, %edx
	movzbl	1994(%rsi), %r8d
	addl	%r8d, %ecx
	subl	%edx, %ecx
	leal	-189(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	movl	$189, %r9d
	andl	%r9d, %edx
	addl	%edx, %ecx
	addl	$-189, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	andl	%r9d, %edx
	addl	%ecx, %edx
	shll	$8, %eax
	orl	%r8d, %eax
	subl	%edx, %eax
	imull	$-1387, %eax, %eax      # imm = 0xFA95
	leal	-423(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$423, %ecx              # imm = 0x1A7
	vpbroadcastw	.LCPI0_18(%rip), %ymm0 # ymm0 = [64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64]
	addl	%ecx, %eax
	addl	$-423, %eax             # imm = 0xFE59
	vpbroadcastw	.LCPI0_19(%rip), %ymm1 # ymm1 = [42304,42304,42304,42304,42304,42304,42304,42304,42304,42304,42304,42304,42304,42304,42304,42304]
	movw	%dx, 540(%rsp)
	vpbroadcastw	.LCPI0_20(%rip), %ymm2 # ymm2 = [189,189,189,189,189,189,189,189,189,189,189,189,189,189,189,189]
	movw	%ax, 542(%rsp)
	vpbroadcastw	.LCPI0_21(%rip), %ymm3 # ymm3 = [65347,65347,65347,65347,65347,65347,65347,65347,65347,65347,65347,65347,65347,65347,65347,65347]
	movl	$63, %eax
	vpbroadcastw	.LCPI0_22(%rip), %ymm4 # ymm4 = [64149,64149,64149,64149,64149,64149,64149,64149,64149,64149,64149,64149,64149,64149,64149,64149]
.LBB0_12:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	64(%rsp,%rax,2), %ymm5
	vpmovzxbw	1915(%rsi,%rax), %ymm6 # ymm6 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm0, %ymm5, %ymm7
	vpmullw	%ymm1, %ymm5, %ymm8
	vpmulhw	%ymm2, %ymm8, %ymm8
	vpaddw	%ymm6, %ymm7, %ymm7
	vpsubw	%ymm8, %ymm7, %ymm7
	vpaddw	%ymm3, %ymm7, %ymm7
	vpsraw	$15, %ymm7, %ymm8
	vpand	%ymm2, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm7
	vpsraw	$15, %ymm7, %ymm8
	vpand	%ymm2, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm7
	vpsllw	$8, %ymm5, %ymm5
	vpor	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm7, %ymm5, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm2, %ymm6
	vpandn	%ymm2, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm7, %ymm6 # ymm6 = ymm7[0],ymm5[0],ymm7[1],ymm5[1],ymm7[2],ymm5[2],ymm7[3],ymm5[3],ymm7[8],ymm5[8],ymm7[9],ymm5[9],ymm7[10],ymm5[10],ymm7[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm7, %ymm5 # ymm5 = ymm7[4],ymm5[4],ymm7[5],ymm5[5],ymm7[6],ymm5[6],ymm7[7],ymm5[7],ymm7[12],ymm5[12],ymm7[13],ymm5[13],ymm7[14],ymm5[14],ymm7[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm6, %ymm7
	vmovdqu	%ymm7, 224(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 256(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_14
# %bb.13:                               #   in Loop: Header=BB0_12 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_12
.LBB0_14:
	movswl	542(%rsp), %eax
	imull	$1658, %eax, %ecx       # imm = 0x67A
	shrl	$16, %ecx
	imull	$-4778, %eax, %eax      # imm = 0xED56
	cwtl
	imull	$3511, %eax, %eax       # imm = 0xDB7
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	1914(%rsi), %eax
	addl	%eax, %ecx
	movswl	%cx, %edx
	imull	$1658, %edx, %edx       # imm = 0x67A
	shrl	$16, %edx
	imull	$-4778, %ecx, %ecx      # imm = 0xED56
	movswl	%cx, %ecx
	imull	$3511, %ecx, %ecx       # imm = 0xDB7
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	1913(%rsi), %r8d
	addl	%r8d, %edx
	movswl	%dx, %r9d
	shrl	$15, %r9d
	movl	$3511, %ecx             # imm = 0xDB7
	andl	%r9d, %ecx
	addl	%edx, %ecx
	shll	$8, %eax
	orl	%r8d, %eax
	subl	%ecx, %eax
	imull	$24583, %eax, %eax      # imm = 0x6007
	leal	-7879(%rax), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$7879, %edx             # imm = 0x1EC7
	addl	%edx, %eax
	addl	$-7879, %eax            # imm = 0xE139
	movw	%cx, 1180(%rsp)
	vpbroadcastw	.LCPI0_23(%rip), %ymm1 # ymm1 = [1658,1658,1658,1658,1658,1658,1658,1658,1658,1658,1658,1658,1658,1658,1658,1658]
	movw	%ax, 1182(%rsp)
	vpbroadcastw	.LCPI0_24(%rip), %ymm2 # ymm2 = [60758,60758,60758,60758,60758,60758,60758,60758,60758,60758,60758,60758,60758,60758,60758,60758]
	movl	$143, %eax
	vpbroadcastw	.LCPI0_25(%rip), %ymm3 # ymm3 = [3511,3511,3511,3511,3511,3511,3511,3511,3511,3511,3511,3511,3511,3511,3511,3511]
	vmovdqa	.LCPI0_26(%rip), %ymm0  # ymm0 = [255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0]
	vpbroadcastw	.LCPI0_27(%rip), %ymm4 # ymm4 = [24583,24583,24583,24583,24583,24583,24583,24583,24583,24583,24583,24583,24583,24583,24583,24583]
.LBB0_15:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	224(%rsp,%rax,2), %ymm5
	vmovdqu	1595(%rsi,%rax,2), %ymm6
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
	vmovdqu	%ymm6, 544(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm7, %ymm5 # ymm5 = ymm7[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 576(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_17
# %bb.16:                               #   in Loop: Header=BB0_15 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_15
.LBB0_17:
	vpbroadcastw	.LCPI0_28(%rip), %ymm1 # ymm1 = [460,460,460,460,460,460,460,460,460,460,460,460,460,460,460,460]
	movzwl	1182(%rsp), %eax
	vpbroadcastw	.LCPI0_29(%rip), %ymm2 # ymm2 = [47839,47839,47839,47839,47839,47839,47839,47839,47839,47839,47839,47839,47839,47839,47839,47839]
	movw	%ax, 2460(%rsp)
	vpbroadcastw	.LCPI0_30(%rip), %ymm3 # ymm3 = [948,948,948,948,948,948,948,948,948,948,948,948,948,948,948,948]
	movl	$303, %eax              # imm = 0x12F
	vpbroadcastw	.LCPI0_31(%rip), %ymm4 # ymm4 = [23781,23781,23781,23781,23781,23781,23781,23781,23781,23781,23781,23781,23781,23781,23781,23781]
.LBB0_18:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	544(%rsp,%rax,2), %ymm5
	vpmovzxbw	1276(%rsi,%rax), %ymm6 # ymm6 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm1, %ymm5, %ymm7
	vpmullw	%ymm2, %ymm5, %ymm8
	vpmulhw	%ymm3, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm7
	vpsraw	$15, %ymm7, %ymm8
	vpand	%ymm3, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm7
	vpsllw	$6, %ymm5, %ymm5
	vpsubw	%ymm7, %ymm6, %ymm6
	vpsraw	$2, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm3, %ymm6
	vpandn	%ymm3, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm7, %ymm6 # ymm6 = ymm7[0],ymm5[0],ymm7[1],ymm5[1],ymm7[2],ymm5[2],ymm7[3],ymm5[3],ymm7[8],ymm5[8],ymm7[9],ymm5[9],ymm7[10],ymm5[10],ymm7[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm7, %ymm5 # ymm5 = ymm7[4],ymm5[4],ymm7[5],ymm5[5],ymm7[6],ymm5[6],ymm7[7],ymm5[7],ymm7[12],ymm5[12],ymm7[13],ymm5[13],ymm7[14],ymm5[14],ymm7[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm6, %ymm7
	vmovdqu	%ymm7, 1184(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 1216(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_20
# %bb.19:                               #   in Loop: Header=BB0_18 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_18
.LBB0_20:
	movl	$-3939, %eax            # imm = 0xF09D
	addl	2460(%rsp), %eax
	vpbroadcastw	.LCPI0_32(%rip), %ymm1 # ymm1 = [2825,2825,2825,2825,2825,2825,2825,2825,2825,2825,2825,2825,2825,2825,2825,2825]
	vpbroadcastw	.LCPI0_33(%rip), %ymm2 # ymm2 = [63407,63407,63407,63407,63407,63407,63407,63407,63407,63407,63407,63407,63407,63407,63407,63407]
	vpbroadcastw	.LCPI0_34(%rip), %ymm3 # ymm3 = [7879,7879,7879,7879,7879,7879,7879,7879,7879,7879,7879,7879,7879,7879,7879,7879]
	movw	%ax, 2552(%rdi)
	vpbroadcastw	.LCPI0_35(%rip), %ymm4 # ymm4 = [17143,17143,17143,17143,17143,17143,17143,17143,17143,17143,17143,17143,17143,17143,17143,17143]
	movl	$622, %eax              # imm = 0x26E
	vpbroadcastw	.LCPI0_36(%rip), %ymm5 # ymm5 = [61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597]
.LBB0_21:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	1184(%rsp,%rax,2), %ymm6
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
	je	.LBB0_23
# %bb.22:                               #   in Loop: Header=BB0_21 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_21
.LBB0_23:
	addq	$2472, %rsp             # imm = 0x9A8
	.cfi_def_cfa_offset 8
	vzeroupper
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_decode_1277x7879), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_decode_1277x7879)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
