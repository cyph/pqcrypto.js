	.text
	.file	"decode.c"
	.section	.rodata.cst4,"aM",@progbits,4
	.p2align	2               # -- Begin function CRYPTO_NAMESPACE(crypto_decode_1277x2627)
.LCPI0_0:
	.long	4294967270              # 0xffffffe6
.LCPI0_2:
	.long	98                      # 0x62
	.section	.rodata,"a",@progbits
	.p2align	1
.LCPI0_1:
	.short	64867                   # 0xfd63
.LCPI0_3:
	.short	98                      # 0x62
.LCPI0_5:
	.short	22737                   # 0x58d1
.LCPI0_6:
	.short	65438                   # 0xff9e
.LCPI0_7:
	.short	65277                   # 0xfefd
.LCPI0_8:
	.short	60311                   # 0xeb97
.LCPI0_9:
	.short	3211                    # 0xc8b
.LCPI0_11:
	.short	64291                   # 0xfb23
.LCPI0_12:
	.short	59310                   # 0xe7ae
.LCPI0_13:
	.short	64379                   # 0xfb7b
.LCPI0_14:
	.short	14506                   # 0x38aa
.LCPI0_15:
	.short	253                     # 0xfd
.LCPI0_16:
	.short	754                     # 0x2f2
.LCPI0_17:
	.short	56830                   # 0xddfe
.LCPI0_18:
	.short	1927                    # 0x787
.LCPI0_19:
	.short	3639                    # 0xe37
.LCPI0_20:
	.short	1868                    # 0x74c
.LCPI0_21:
	.short	64043                   # 0xfa2b
.LCPI0_22:
	.short	11236                   # 0x2be4
.LCPI0_23:
	.short	38729                   # 0x9749
.LCPI0_24:
	.short	28                      # 0x1c
.LCPI0_25:
	.short	64918                   # 0xfd96
.LCPI0_26:
	.short	106                     # 0x6a
.LCPI0_27:
	.short	21021                   # 0x521d
.LCPI0_28:
	.short	1194                    # 0x4aa
.LCPI0_29:
	.short	59150                   # 0xe70e
.LCPI0_30:
	.short	2627                    # 0xa43
.LCPI0_31:
	.short	4715                    # 0x126b
.LCPI0_32:
	.short	3                       # 0x3
.LCPI0_33:
	.short	61597                   # 0xf09d
	.section	.rodata.cst16,"aM",@progbits,16
	.p2align	4
.LCPI0_4:
	.byte	0                       # 0x0
	.byte	1                       # 0x1
	.byte	4                       # 0x4
	.byte	5                       # 0x5
	.byte	8                       # 0x8
	.byte	9                       # 0x9
	.byte	12                      # 0xc
	.byte	13                      # 0xd
	.byte	8                       # 0x8
	.byte	9                       # 0x9
	.byte	12                      # 0xc
	.byte	13                      # 0xd
	.byte	12                      # 0xc
	.byte	13                      # 0xd
	.byte	14                      # 0xe
	.byte	15                      # 0xf
	.section	.rodata.cst32,"aM",@progbits,32
	.p2align	5
.LCPI0_10:
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
	.globl	CRYPTO_NAMESPACE(crypto_decode_1277x2627)
	.type	CRYPTO_NAMESPACE(crypto_decode_1277x2627),@function
CRYPTO_NAMESPACE(crypto_decode_1277x2627): # @CRYPTO_NAMESPACE(crypto_decode_1277x2627)
	.cfi_startproc
# %bb.0:
	subq	$2472, %rsp             # imm = 0x9A8
	.cfi_def_cfa_offset 2480
	movzbl	1814(%rsi), %eax
	imull	$130154496, %eax, %eax  # imm = 0x7C20000
	movl	%eax, %ecx
	sarl	$13, %ecx
	sarl	$8, %eax
	addl	%ecx, %eax
	shrl	$16, %eax
	movzbl	1813(%rsi), %ecx
	movl	%ecx, %edx
	subl	%eax, %edx
	negl	%eax
	addl	%ecx, %eax
	addl	$-264, %eax             # imm = 0xFEF8
	cwtl
	shrl	$15, %eax
	movl	$264, %ecx              # imm = 0x108
	andl	%ecx, %eax
	addl	%edx, %eax
	addl	$-264, %eax             # imm = 0xFEF8
	movswl	%ax, %edx
	shrl	$15, %edx
	andl	%ecx, %edx
	addl	%eax, %edx
	movswl	%dx, %r9d
	imull	$3712, %r9d, %ecx       # imm = 0xE80
	shrl	$16, %ecx
	imull	$-2166, %edx, %edx      # imm = 0xF78A
	movswl	%dx, %edx
	imull	$7744, %edx, %edx       # imm = 0x1E40
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1812(%rsi), %r8d
	addl	%r8d, %ecx
	movswl	%cx, %edx
	imull	$3712, %edx, %eax       # imm = 0xE80
	shrl	$16, %eax
	imull	$-2166, %ecx, %ecx      # imm = 0xF78A
	movswl	%cx, %ecx
	imull	$7744, %ecx, %ecx       # imm = 0x1E40
	shrl	$16, %ecx
	subl	%ecx, %eax
	movzbl	1811(%rsi), %ecx
	addl	%ecx, %eax
	movswl	%ax, %edx
	shrl	$15, %edx
	movl	$7744, %r10d            # imm = 0x1E40
	andl	%edx, %r10d
	addl	%eax, %r10d
	shll	$10, %r9d
	leal	(%r9,%r8,4), %edx
	movswl	%r10w, %r9d
	subl	%r9d, %ecx
	shrl	$6, %ecx
	addl	%edx, %ecx
	imull	$27081, %ecx, %ecx      # imm = 0x69C9
	leal	-2229(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$2229, %edx             # imm = 0x8B5
	leal	(%rdx,%rcx), %r8d
	addl	$-2229, %r8d            # imm = 0xF74B
	movw	%r8w, -114(%rsp)
	movl	%r9d, %ecx
	shll	$9, %ecx
	negl	%ecx
	shrl	$16, %ecx
	imull	$-11916, %r10d, %edx    # imm = 0xD174
	movswl	%dx, %edx
	imull	$1408, %edx, %edx       # imm = 0x580
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1810(%rsi), %edx
	addl	%edx, %ecx
	movswl	%cx, %eax
	shrl	$15, %eax
	movl	$1408, %r10d            # imm = 0x580
	andl	%r10d, %eax
	addl	%ecx, %eax
	cwtl
	subl	%eax, %edx
	shrl	$7, %edx
	leal	(%rdx,%r9,2), %ecx
	imull	$-29789, %ecx, %ecx     # imm = 0x8BA3
	leal	-1408(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r10d, %edx
	addl	%edx, %ecx
	addl	$-1408, %ecx            # imm = 0xFA80
	movw	%ax, -118(%rsp)
	movw	%cx, -116(%rsp)
	movw	%r8w, -104(%rsp)
	xorl	%r9d, %r9d
	movl	$9604, %r8d             # imm = 0x2584
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movswl	%cx, %r10d
	imull	$-972, %r10d, %eax      # imm = 0xFC34
	shrl	$16, %eax
	imull	$-1747, %ecx, %ecx      # imm = 0xF92D
	movswl	%cx, %ecx
	imull	$9604, %ecx, %ecx       # imm = 0x2584
	shrl	$16, %ecx
	subl	%ecx, %eax
	movzbl	1809(%rsi,%r9,2), %r11d
	addl	%r11d, %eax
	movswl	%ax, %edx
	imull	$-972, %edx, %edx       # imm = 0xFC34
	shrl	$16, %edx
	imull	$-1747, %eax, %eax      # imm = 0xF92D
	cwtl
	imull	$9604, %eax, %eax       # imm = 0x2584
	shrl	$16, %eax
	subl	%eax, %edx
	movzbl	1808(%rsi,%r9,2), %eax
	addl	%eax, %edx
	movswl	%dx, %ecx
	shrl	$15, %ecx
	andl	%r8d, %ecx
	addl	%edx, %ecx
	shll	$14, %r10d
	shll	$6, %r11d
	orl	%r10d, %r11d
	movswl	%cx, %ecx
	subl	%ecx, %eax
	shrl	$2, %eax
	addl	%r11d, %eax
	imull	$23201, %eax, %eax      # imm = 0x5AA1
	leal	-9604(%rax), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r8d, %edx
	addl	%edx, %eax
	addl	$-9604, %eax            # imm = 0xDA7C
	movw	%cx, -108(%rsp,%r9,4)
	movw	%ax, -106(%rsp,%r9,4)
	cmpq	$-1, %r9
	je	.LBB0_3
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	movzwl	-118(%rsp,%r9,2), %ecx
	decq	%r9
	jmp	.LBB0_1
.LBB0_3:
	movswl	-104(%rsp), %r8d
	movl	%r8d, %edx
	sarl	$13, %edx
	imull	$25412, %r8d, %ecx      # imm = 0x6344
	movswl	%cx, %ecx
	imull	$98, %ecx, %ecx
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	1805(%rsi), %ecx
	addl	%ecx, %edx
	movswl	%dx, %eax
	imull	$-26, %eax, %eax
	shrl	$16, %eax
	imull	$-669, %edx, %edx       # imm = 0xFD63
	movswl	%dx, %edx
	imull	$98, %edx, %edx
	shrl	$16, %edx
	subl	%edx, %eax
	movswl	%ax, %r9d
	shrl	$15, %r9d
	movl	$98, %edx
	andl	%r9d, %edx
	addl	%eax, %edx
	shll	$7, %r8d
	movswl	%dx, %edx
	subl	%edx, %ecx
	shrl	%ecx
	addl	%r8d, %ecx
	imull	$22737, %ecx, %eax      # imm = 0x58D1
	leal	-5822(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$5822, %ecx             # imm = 0x16BE
	leal	(%rcx,%rax), %r8d
	addl	$-5822, %r8d            # imm = 0xE942
	movw	%dx, -80(%rsp)
	vmovq	-112(%rsp), %xmm0       # xmm0 = mem[0],zero
	vpshuflw	$27, %xmm0, %xmm0 # xmm0 = xmm0[3,2,1,0,4,5,6,7]
	vpmovsxwd	%xmm0, %xmm1
	vpbroadcastd	.LCPI0_0(%rip), %xmm2 # xmm2 = [4294967270,4294967270,4294967270,4294967270]
	vpmulld	%xmm2, %xmm1, %xmm2
	movw	%r8w, -78(%rsp)
	vpbroadcastw	.LCPI0_1(%rip), %xmm3 # xmm3 = [64867,64867,64867,64867,64867,64867,64867,64867]
	vpsrld	$16, %xmm2, %xmm2
	vpmullw	%xmm3, %xmm0, %xmm0
	vpmovsxwd	%xmm0, %xmm0
	vpbroadcastd	.LCPI0_2(%rip), %xmm3 # xmm3 = [98,98,98,98]
	vpmulld	%xmm3, %xmm0, %xmm0
	vpsrld	$16, %xmm0, %xmm0
	vpsubw	%xmm0, %xmm2, %xmm0
	vpackusdw	%xmm0, %xmm0, %xmm0
	vpbroadcastw	.LCPI0_3(%rip), %xmm2 # xmm2 = [98,98,98,98,98,98,98,98]
	vpsraw	$15, %xmm0, %xmm3
	vpand	%xmm2, %xmm3, %xmm3
	vpaddw	%xmm0, %xmm3, %xmm0
	vpmovsxwd	%xmm0, %xmm3
	vpsubd	%xmm3, %xmm1, %xmm1
	vpsrld	$1, %xmm1, %xmm1
	vpshufb	.LCPI0_4(%rip), %xmm1, %xmm1 # xmm1 = xmm1[0,1,4,5,8,9,12,13,8,9,12,13,12,13,14,15]
	vpbroadcastw	.LCPI0_5(%rip), %xmm3 # xmm3 = [22737,22737,22737,22737,22737,22737,22737,22737]
	vpmullw	%xmm3, %xmm1, %xmm1
	vpbroadcastw	.LCPI0_6(%rip), %xmm3 # xmm3 = [65438,65438,65438,65438,65438,65438,65438,65438]
	vpaddw	%xmm3, %xmm1, %xmm1
	vpsraw	$15, %xmm1, %xmm3
	vpand	%xmm2, %xmm3, %xmm2
	vpaddw	%xmm1, %xmm2, %xmm1
	vpunpcklwd	%xmm1, %xmm0, %xmm0 # xmm0 = xmm0[0],xmm1[0],xmm0[1],xmm1[1],xmm0[2],xmm1[2],xmm0[3],xmm1[3]
	vpshufd	$27, %xmm0, %xmm0       # xmm0 = xmm0[3,2,1,0]
	vmovdqa	%xmm0, -96(%rsp)
	movswl	%r8w, %r9d
	imull	$-14, %r9d, %eax
	shrl	$16, %eax
	imull	$24887, %r8d, %edx      # imm = 0x6137
	movswl	%dx, %edx
	imull	$158, %edx, %edx
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	1804(%rsi), %edx
	addl	%edx, %eax
	movswl	%ax, %ecx
	imull	$-34, %ecx, %ecx
	shrl	$16, %ecx
	imull	$-415, %eax, %eax       # imm = 0xFE61
	cwtl
	imull	$158, %eax, %eax
	shrl	$16, %eax
	subl	%eax, %ecx
	movswl	%cx, %eax
	shrl	$15, %eax
	movl	$158, %r8d
	andl	%r8d, %eax
	addl	%ecx, %eax
	shll	$7, %r9d
	cwtl
	subl	%eax, %edx
	shrl	%edx
	addl	%r9d, %edx
	imull	$5807, %edx, %ecx       # imm = 0x16AF
	leal	-9433(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$9433, %edx             # imm = 0x24D9
	addl	%edx, %ecx
	addl	$-9433, %ecx            # imm = 0xDB27
	movw	%ax, -28(%rsp)
	movw	%cx, -26(%rsp)
	xorl	%r10d, %r10d
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	movswl	-80(%rsp,%r10,2), %r9d
	imull	$-14, %r9d, %edx
	shrl	$16, %edx
	imull	$24887, %r9d, %eax      # imm = 0x6137
	cwtl
	imull	$158, %eax, %eax
	shrl	$16, %eax
	subl	%eax, %edx
	movzbl	1803(%rsi,%r10), %eax
	addl	%eax, %edx
	movswl	%dx, %ecx
	imull	$-34, %ecx, %ecx
	shrl	$16, %ecx
	imull	$-415, %edx, %edx       # imm = 0xFE61
	movswl	%dx, %edx
	imull	$158, %edx, %edx
	shrl	$16, %edx
	subl	%edx, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	andl	%r8d, %edx
	addl	%ecx, %edx
	shll	$7, %r9d
	movswl	%dx, %ecx
	subl	%ecx, %eax
	shrl	%eax
	addl	%r9d, %eax
	imull	$5807, %eax, %eax       # imm = 0x16AF
	leal	-158(%rax), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r8d, %edx
	addl	%edx, %eax
	addl	$-158, %eax
	movw	%cx, -32(%rsp,%r10,4)
	movw	%ax, -30(%rsp,%r10,4)
	decq	%r10
	cmpq	$-9, %r10
	jne	.LBB0_4
# %bb.5:
	movswl	-26(%rsp), %eax
	imull	$-259, %eax, %ecx       # imm = 0xFEFD
	shrl	$16, %ecx
	imull	$-5225, %eax, %edx      # imm = 0xEB97
	movswl	%dx, %edx
	imull	$3211, %edx, %edx       # imm = 0xC8B
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1794(%rsi), %r8d
	addl	%r8d, %ecx
	movswl	%cx, %r9d
	shrl	$15, %r9d
	movl	$3211, %edx             # imm = 0xC8B
	andl	%r9d, %edx
	addl	%ecx, %edx
	shll	$8, %eax
	orl	%r8d, %eax
	subl	%edx, %eax
	imull	$-1245, %eax, %eax      # imm = 0xFB23
	leal	-752(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$752, %ecx              # imm = 0x2F0
	addl	%ecx, %eax
	addl	$-752, %eax             # imm = 0xFD10
	movw	%dx, 60(%rsp)
	vpbroadcastw	.LCPI0_7(%rip), %ymm1 # ymm1 = [65277,65277,65277,65277,65277,65277,65277,65277,65277,65277,65277,65277,65277,65277,65277,65277]
	movw	%ax, 62(%rsp)
	vpbroadcastw	.LCPI0_8(%rip), %ymm2 # ymm2 = [60311,60311,60311,60311,60311,60311,60311,60311,60311,60311,60311,60311,60311,60311,60311,60311]
	movl	$3, %eax
	vpbroadcastw	.LCPI0_9(%rip), %ymm3 # ymm3 = [3211,3211,3211,3211,3211,3211,3211,3211,3211,3211,3211,3211,3211,3211,3211,3211]
	vmovdqa	.LCPI0_10(%rip), %ymm0  # ymm0 = [255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0]
	vpbroadcastw	.LCPI0_11(%rip), %ymm4 # ymm4 = [64291,64291,64291,64291,64291,64291,64291,64291,64291,64291,64291,64291,64291,64291,64291,64291]
.LBB0_6:                                # =>This Inner Loop Header: Depth=1
	vmovdqu	-64(%rsp,%rax,2), %ymm5
	vmovdqu	1756(%rsi,%rax,2), %ymm6
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
	vmovdqu	%ymm6, -16(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm7, %ymm5 # ymm5 = ymm7[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 16(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_8
# %bb.7:                                #   in Loop: Header=BB0_6 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_6
.LBB0_8:
	movswl	62(%rsp), %r8d
	imull	$-6226, %r8d, %edx      # imm = 0xE7AE
	shrl	$16, %edx
	imull	$-1157, %r8d, %ecx      # imm = 0xFB7B
	movswl	%cx, %ecx
	imull	$14506, %ecx, %ecx      # imm = 0x38AA
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	1755(%rsi), %r9d
	addl	%r9d, %edx
	movswl	%dx, %eax
	imull	$-6226, %eax, %eax      # imm = 0xE7AE
	shrl	$16, %eax
	imull	$-1157, %edx, %edx      # imm = 0xFB7B
	movswl	%dx, %edx
	imull	$14506, %edx, %edx      # imm = 0x38AA
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	1754(%rsi), %edx
	addl	%edx, %eax
	movswl	%ax, %r10d
	shrl	$15, %r10d
	movl	$14506, %ecx            # imm = 0x38AA
	andl	%r10d, %ecx
	addl	%eax, %ecx
	shll	$15, %r8d
	shll	$7, %r9d
	orl	%r8d, %r9d
	movswl	%cx, %eax
	subl	%eax, %edx
	shrl	%edx
	addl	%r9d, %edx
	imull	$253, %edx, %ecx
	leal	-3395(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$3395, %edx             # imm = 0xD43
	addl	%edx, %ecx
	addl	$-3395, %ecx            # imm = 0xF2BD
	vpbroadcastw	.LCPI0_12(%rip), %ymm1 # ymm1 = [59310,59310,59310,59310,59310,59310,59310,59310,59310,59310,59310,59310,59310,59310,59310,59310]
	movw	%ax, 220(%rsp)
	vpbroadcastw	.LCPI0_13(%rip), %ymm2 # ymm2 = [64379,64379,64379,64379,64379,64379,64379,64379,64379,64379,64379,64379,64379,64379,64379,64379]
	movw	%cx, 222(%rsp)
	vpbroadcastw	.LCPI0_14(%rip), %ymm3 # ymm3 = [14506,14506,14506,14506,14506,14506,14506,14506,14506,14506,14506,14506,14506,14506,14506,14506]
	movl	$23, %eax
	vpbroadcastw	.LCPI0_15(%rip), %ymm4 # ymm4 = [253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,253]
.LBB0_9:                                # =>This Inner Loop Header: Depth=1
	vmovdqu	-16(%rsp,%rax,2), %ymm5
	vmovdqu	1676(%rsi,%rax,2), %ymm6
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
	vmovdqu	%ymm7, 64(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 96(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_11
# %bb.10:                               #   in Loop: Header=BB0_9 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_9
.LBB0_11:
	movswl	222(%rsp), %eax
	imull	$754, %eax, %ecx        # imm = 0x2F2
	shrl	$16, %ecx
	imull	$-8706, %eax, %edx      # imm = 0xDDFE
	movswl	%dx, %edx
	imull	$1927, %edx, %edx       # imm = 0x787
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1675(%rsi), %r8d
	addl	%r8d, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	movl	$1927, %r9d             # imm = 0x787
	andl	%edx, %r9d
	addl	%ecx, %r9d
	shll	$8, %eax
	orl	%r8d, %eax
	subl	%r9d, %eax
	imull	$3639, %eax, %eax       # imm = 0xE37
	leal	-451(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$451, %ecx              # imm = 0x1C3
	addl	%ecx, %eax
	addl	$-451, %eax             # imm = 0xFE3D
	vpbroadcastw	.LCPI0_16(%rip), %ymm1 # ymm1 = [754,754,754,754,754,754,754,754,754,754,754,754,754,754,754,754]
	movw	%r9w, 540(%rsp)
	vpbroadcastw	.LCPI0_17(%rip), %ymm2 # ymm2 = [56830,56830,56830,56830,56830,56830,56830,56830,56830,56830,56830,56830,56830,56830,56830,56830]
	movw	%ax, 542(%rsp)
	vpbroadcastw	.LCPI0_18(%rip), %ymm3 # ymm3 = [1927,1927,1927,1927,1927,1927,1927,1927,1927,1927,1927,1927,1927,1927,1927,1927]
	movl	$63, %eax
	vpbroadcastw	.LCPI0_19(%rip), %ymm4 # ymm4 = [3639,3639,3639,3639,3639,3639,3639,3639,3639,3639,3639,3639,3639,3639,3639,3639]
.LBB0_12:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	64(%rsp,%rax,2), %ymm5
	vpmovzxbw	1596(%rsi,%rax), %ymm6 # ymm6 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm1, %ymm5, %ymm7
	vpmullw	%ymm2, %ymm5, %ymm8
	vpmulhw	%ymm3, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm7
	vpsraw	$15, %ymm7, %ymm8
	vpand	%ymm3, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm7
	vpsllw	$8, %ymm5, %ymm5
	vpor	%ymm6, %ymm5, %ymm5
	vpsubw	%ymm7, %ymm5, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm3, %ymm6
	vpandn	%ymm3, %ymm6, %ymm6
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
	movswl	542(%rsp), %r8d
	imull	$1868, %r8d, %edx       # imm = 0x74C
	shrl	$16, %edx
	imull	$-1493, %r8d, %ecx      # imm = 0xFA2B
	movswl	%cx, %ecx
	imull	$11236, %ecx, %ecx      # imm = 0x2BE4
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	1595(%rsi), %r9d
	addl	%r9d, %edx
	movswl	%dx, %eax
	imull	$1868, %eax, %eax       # imm = 0x74C
	shrl	$16, %eax
	imull	$-1493, %edx, %edx      # imm = 0xFA2B
	movswl	%dx, %edx
	imull	$11236, %edx, %edx      # imm = 0x2BE4
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	1594(%rsi), %edx
	addl	%edx, %eax
	movswl	%ax, %r10d
	shrl	$15, %r10d
	movl	$11236, %ecx            # imm = 0x2BE4
	andl	%r10d, %ecx
	addl	%eax, %ecx
	shll	$14, %r8d
	shll	$6, %r9d
	orl	%r8d, %r9d
	movswl	%cx, %eax
	subl	%eax, %edx
	shrl	$2, %edx
	addl	%r9d, %edx
	imull	$-26807, %edx, %ecx     # imm = 0x9749
	leal	-2627(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$2627, %edx             # imm = 0xA43
	addl	%edx, %ecx
	addl	$-2627, %ecx            # imm = 0xF5BD
	vpbroadcastw	.LCPI0_20(%rip), %ymm1 # ymm1 = [1868,1868,1868,1868,1868,1868,1868,1868,1868,1868,1868,1868,1868,1868,1868,1868]
	movw	%ax, 1180(%rsp)
	vpbroadcastw	.LCPI0_21(%rip), %ymm2 # ymm2 = [64043,64043,64043,64043,64043,64043,64043,64043,64043,64043,64043,64043,64043,64043,64043,64043]
	movw	%cx, 1182(%rsp)
	vpbroadcastw	.LCPI0_22(%rip), %ymm3 # ymm3 = [11236,11236,11236,11236,11236,11236,11236,11236,11236,11236,11236,11236,11236,11236,11236,11236]
	movl	$143, %eax
	vpbroadcastw	.LCPI0_23(%rip), %ymm4 # ymm4 = [38729,38729,38729,38729,38729,38729,38729,38729,38729,38729,38729,38729,38729,38729,38729,38729]
.LBB0_15:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	224(%rsp,%rax,2), %ymm5
	vmovdqu	1276(%rsi,%rax,2), %ymm6
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
	vpsllw	$14, %ymm5, %ymm5
	vpsllw	$6, %ymm7, %ymm7
	vpor	%ymm5, %ymm7, %ymm5
	vpsubw	%ymm8, %ymm6, %ymm6
	vpsraw	$2, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm3, %ymm6
	vpandn	%ymm3, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm8, %ymm6 # ymm6 = ymm8[0],ymm5[0],ymm8[1],ymm5[1],ymm8[2],ymm5[2],ymm8[3],ymm5[3],ymm8[8],ymm5[8],ymm8[9],ymm5[9],ymm8[10],ymm5[10],ymm8[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm8, %ymm5 # ymm5 = ymm8[4],ymm5[4],ymm8[5],ymm5[5],ymm8[6],ymm5[6],ymm8[7],ymm5[7],ymm8[12],ymm5[12],ymm8[13],ymm5[13],ymm8[14],ymm5[14],ymm8[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm6, %ymm7
	vmovdqu	%ymm7, 544(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 576(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_17
# %bb.16:                               #   in Loop: Header=BB0_15 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_15
.LBB0_17:
	vpbroadcastw	.LCPI0_24(%rip), %ymm1 # ymm1 = [28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28]
	movzwl	1182(%rsp), %eax
	vpbroadcastw	.LCPI0_25(%rip), %ymm2 # ymm2 = [64918,64918,64918,64918,64918,64918,64918,64918,64918,64918,64918,64918,64918,64918,64918,64918]
	movw	%ax, 2460(%rsp)
	vpbroadcastw	.LCPI0_26(%rip), %ymm3 # ymm3 = [106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106]
	movl	$303, %eax              # imm = 0x12F
	vpbroadcastw	.LCPI0_27(%rip), %ymm4 # ymm4 = [21021,21021,21021,21021,21021,21021,21021,21021,21021,21021,21021,21021,21021,21021,21021,21021]
.LBB0_18:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	544(%rsp,%rax,2), %ymm5
	vpmulhw	%ymm1, %ymm5, %ymm6
	vpmullw	%ymm2, %ymm5, %ymm7
	vpmulhw	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpsraw	$15, %ymm6, %ymm7
	vpand	%ymm3, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpsraw	$1, %ymm5, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm3, %ymm7
	vpandn	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm6, %ymm7 # ymm7 = ymm6[0],ymm5[0],ymm6[1],ymm5[1],ymm6[2],ymm5[2],ymm6[3],ymm5[3],ymm6[8],ymm5[8],ymm6[9],ymm5[9],ymm6[10],ymm5[10],ymm6[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm6, %ymm5 # ymm5 = ymm6[4],ymm5[4],ymm6[5],ymm5[5],ymm6[6],ymm5[6],ymm6[7],ymm5[7],ymm6[12],ymm5[12],ymm6[13],ymm5[13],ymm6[14],ymm5[14],ymm6[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm7, %ymm6
	vmovdqu	%ymm6, 1184(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm7, %ymm5 # ymm5 = ymm7[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 1216(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_20
# %bb.19:                               #   in Loop: Header=BB0_18 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_18
.LBB0_20:
	vpbroadcastw	.LCPI0_28(%rip), %ymm1 # ymm1 = [1194,1194,1194,1194,1194,1194,1194,1194,1194,1194,1194,1194,1194,1194,1194,1194]
	vpbroadcastw	.LCPI0_29(%rip), %ymm2 # ymm2 = [59150,59150,59150,59150,59150,59150,59150,59150,59150,59150,59150,59150,59150,59150,59150,59150]
	movl	2460(%rsp), %eax
	vpbroadcastw	.LCPI0_30(%rip), %ymm3 # ymm3 = [2627,2627,2627,2627,2627,2627,2627,2627,2627,2627,2627,2627,2627,2627,2627,2627]
	leal	(%rax,%rax,2), %eax
	addl	$-3939, %eax            # imm = 0xF09D
	vpbroadcastw	.LCPI0_31(%rip), %ymm4 # ymm4 = [4715,4715,4715,4715,4715,4715,4715,4715,4715,4715,4715,4715,4715,4715,4715,4715]
	movw	%ax, 2552(%rdi)
	vpbroadcastw	.LCPI0_32(%rip), %ymm5 # ymm5 = [3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3]
	movl	$622, %eax              # imm = 0x26E
	vpbroadcastw	.LCPI0_33(%rip), %ymm6 # ymm6 = [61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597,61597]
.LBB0_21:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	1184(%rsp,%rax,2), %ymm7
	vmovdqu	(%rsi,%rax,2), %ymm8
	vpsrlw	$8, %ymm8, %ymm9
	vpmulhw	%ymm1, %ymm7, %ymm10
	vpmullw	%ymm2, %ymm7, %ymm7
	vpmulhw	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm10, %ymm7
	vpaddw	%ymm7, %ymm9, %ymm7
	vpmulhw	%ymm1, %ymm7, %ymm9
	vpmullw	%ymm2, %ymm7, %ymm7
	vpmulhw	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm9, %ymm7
	vpand	%ymm0, %ymm8, %ymm9
	vpaddw	%ymm7, %ymm9, %ymm7
	vpsraw	$15, %ymm7, %ymm9
	vpand	%ymm3, %ymm9, %ymm9
	vpaddw	%ymm7, %ymm9, %ymm7
	vpsubw	%ymm7, %ymm8, %ymm8
	vpmullw	%ymm4, %ymm8, %ymm8
	vpcmpgtw	%ymm8, %ymm3, %ymm9
	vpandn	%ymm3, %ymm9, %ymm9
	vpsubw	%ymm9, %ymm8, %ymm8
	vpmullw	%ymm5, %ymm7, %ymm7
	vpmullw	%ymm5, %ymm8, %ymm8
	vpaddw	%ymm6, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm8, %ymm8
	vpunpcklwd	%ymm8, %ymm7, %ymm9 # ymm9 = ymm7[0],ymm8[0],ymm7[1],ymm8[1],ymm7[2],ymm8[2],ymm7[3],ymm8[3],ymm7[8],ymm8[8],ymm7[9],ymm8[9],ymm7[10],ymm8[10],ymm7[11],ymm8[11]
	vpunpckhwd	%ymm8, %ymm7, %ymm7 # ymm7 = ymm7[4],ymm8[4],ymm7[5],ymm8[5],ymm7[6],ymm8[6],ymm7[7],ymm8[7],ymm7[12],ymm8[12],ymm7[13],ymm8[13],ymm7[14],ymm8[14],ymm7[15],ymm8[15]
	vinserti128	$1, %xmm7, %ymm9, %ymm8
	vmovdqu	%ymm8, (%rdi,%rax,4)
	vperm2i128	$49, %ymm7, %ymm9, %ymm7 # ymm7 = ymm9[2,3],ymm7[2,3]
	vmovdqu	%ymm7, 32(%rdi,%rax,4)
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
	.size	CRYPTO_NAMESPACE(crypto_decode_1277x2627), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_decode_1277x2627)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
