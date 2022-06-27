	.text
	.file	"decode.c"
	.section	.rodata,"a",@progbits
	.p2align	1               # -- Begin function CRYPTO_NAMESPACE(crypto_decode_1013x2393)
.LCPI0_0:
	.short	412                     # 0x19c
.LCPI0_1:
	.short	60418                   # 0xec02
.LCPI0_2:
	.short	3278                    # 0xcce
.LCPI0_4:
	.short	46423                   # 0xb557
.LCPI0_5:
	.short	65296                   # 0xff10
.LCPI0_6:
	.short	47220                   # 0xb874
.LCPI0_7:
	.short	916                     # 0x394
.LCPI0_8:
	.short	48365                   # 0xbced
.LCPI0_9:
	.short	3712                    # 0xe80
.LCPI0_10:
	.short	63370                   # 0xf78a
.LCPI0_11:
	.short	7744                    # 0x1e40
.LCPI0_12:
	.short	27081                   # 0x69c9
.LCPI0_13:
	.short	65512                   # 0xffe8
.LCPI0_14:
	.short	64791                   # 0xfd17
.LCPI0_15:
	.short	88                      # 0x58
.LCPI0_16:
	.short	35747                   # 0x8ba3
.LCPI0_17:
	.short	65429                   # 0xff95
.LCPI0_18:
	.short	58525                   # 0xe49d
.LCPI0_19:
	.short	2393                    # 0x959
.LCPI0_20:
	.short	44777                   # 0xaee9
.LCPI0_21:
	.short	3                       # 0x3
.LCPI0_22:
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
	.globl	CRYPTO_NAMESPACE(crypto_decode_1013x2393)
	.type	CRYPTO_NAMESPACE(crypto_decode_1013x2393),@function
CRYPTO_NAMESPACE(crypto_decode_1013x2393): # @CRYPTO_NAMESPACE(crypto_decode_1013x2393)
	.cfi_startproc
# %bb.0:
	subq	$1912, %rsp             # imm = 0x778
	.cfi_def_cfa_offset 1920
	movzbl	1422(%rsi), %eax
	imull	$311230464, %eax, %eax  # imm = 0x128D0000
	sarl	$16, %eax
	imull	$276, %eax, %eax        # imm = 0x114
	shrl	$16, %eax
	movzbl	1421(%rsi), %ecx
	movl	%ecx, %edx
	subl	%eax, %edx
	negl	%eax
	addl	%ecx, %eax
	addl	$-276, %eax             # imm = 0xFEEC
	cwtl
	shrl	$15, %eax
	movl	$276, %ecx              # imm = 0x114
	andl	%ecx, %eax
	addl	%edx, %eax
	addl	$-276, %eax             # imm = 0xFEEC
	movswl	%ax, %edx
	shrl	$15, %edx
	andl	%ecx, %edx
	addl	%eax, %edx
	movswl	%dx, %r8d
	imull	$-6226, %r8d, %ecx      # imm = 0xE7AE
	shrl	$16, %ecx
	imull	$-1157, %edx, %edx      # imm = 0xFB7B
	movswl	%dx, %edx
	imull	$14506, %edx, %edx      # imm = 0x38AA
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1420(%rsi), %r10d
	addl	%r10d, %ecx
	movswl	%cx, %eax
	imull	$-6226, %eax, %eax      # imm = 0xE7AE
	shrl	$16, %eax
	imull	$-1157, %ecx, %ecx      # imm = 0xFB7B
	movswl	%cx, %ecx
	imull	$14506, %ecx, %ecx      # imm = 0x38AA
	shrl	$16, %ecx
	subl	%ecx, %eax
	movzbl	1419(%rsi), %ecx
	addl	%ecx, %eax
	movswl	%ax, %edx
	shrl	$15, %edx
	movl	$14506, %r11d           # imm = 0x38AA
	andl	%edx, %r11d
	addl	%eax, %r11d
	shll	$15, %r8d
	shll	$7, %r10d
	orl	%r8d, %r10d
	movswl	%r11w, %r9d
	subl	%r9d, %ecx
	shrl	%ecx
	addl	%r10d, %ecx
	imull	$253, %ecx, %eax
	leal	-1243(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$1243, %ecx             # imm = 0x4DB
	leal	(%rcx,%rax), %edx
	addl	$-1243, %edx            # imm = 0xFB25
	movswl	%dx, %eax
	imull	$754, %eax, %ecx        # imm = 0x2F2
	shrl	$16, %ecx
	imull	$-8706, %edx, %eax      # imm = 0xDDFE
	cwtl
	imull	$1927, %eax, %eax       # imm = 0x787
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	1418(%rsi), %r10d
	addl	%r10d, %ecx
	movswl	%cx, %eax
	shrl	$15, %eax
	movl	$1927, %r8d             # imm = 0x787
	andl	%r8d, %eax
	addl	%ecx, %eax
	shll	$8, %edx
	orl	%r10d, %edx
	subl	%eax, %edx
	imull	$3639, %edx, %ecx       # imm = 0xE37
	leal	-165(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$165, %edx
	leal	(%rdx,%rcx), %r10d
	addl	$-165, %r10d
	movw	%ax, -116(%rsp)
	movw	%r10w, -114(%rsp)
	imull	$754, %r9d, %ecx        # imm = 0x2F2
	shrl	$16, %ecx
	imull	$-8706, %r11d, %edx     # imm = 0xDDFE
	movswl	%dx, %edx
	imull	$1927, %edx, %edx       # imm = 0x787
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1417(%rsi), %r9d
	addl	%r9d, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	andl	%r8d, %edx
	addl	%ecx, %edx
	shll	$8, %r11d
	orl	%r9d, %r11d
	subl	%edx, %r11d
	imull	$3639, %r11d, %r9d      # imm = 0xE37
	leal	-1927(%r9), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	%r8d, %ecx
	addl	%r9d, %ecx
	addl	$-1927, %ecx            # imm = 0xF879
	movw	%dx, -120(%rsp)
	movw	%cx, -118(%rsp)
	movswl	%r10w, %r9d
	imull	$1868, %r9d, %edx       # imm = 0x74C
	shrl	$16, %edx
	imull	$-1493, %r10d, %ecx     # imm = 0xFA2B
	movswl	%cx, %ecx
	imull	$11236, %ecx, %ecx      # imm = 0x2BE4
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	1416(%rsi), %r10d
	addl	%r10d, %edx
	movswl	%dx, %ecx
	imull	$1868, %ecx, %ecx       # imm = 0x74C
	shrl	$16, %ecx
	imull	$-1493, %edx, %edx      # imm = 0xFA2B
	movswl	%dx, %edx
	imull	$11236, %edx, %edx      # imm = 0x2BE4
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1415(%rsi), %r11d
	addl	%r11d, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	movl	$11236, %r8d            # imm = 0x2BE4
	andl	%r8d, %edx
	addl	%ecx, %edx
	shll	$14, %r9d
	shll	$6, %r10d
	orl	%r9d, %r10d
	movswl	%dx, %r9d
	subl	%r9d, %r11d
	shrl	$2, %r11d
	addl	%r10d, %r11d
	imull	$-26807, %r11d, %edx    # imm = 0x9749
	leal	-962(%rdx), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$962, %ecx              # imm = 0x3C2
	addl	%edx, %ecx
	addl	$-962, %ecx             # imm = 0xFC3E
	movw	%r9w, -100(%rsp)
	movw	%cx, -98(%rsp)
	xorl	%r9d, %r9d
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movswl	%ax, %r10d
	imull	$1868, %r10d, %edx      # imm = 0x74C
	shrl	$16, %edx
	imull	$-1493, %eax, %eax      # imm = 0xFA2B
	cwtl
	imull	$11236, %eax, %eax      # imm = 0x2BE4
	shrl	$16, %eax
	subl	%eax, %edx
	movzbl	1414(%rsi,%r9,2), %r11d
	addl	%r11d, %edx
	movswl	%dx, %ecx
	imull	$1868, %ecx, %ecx       # imm = 0x74C
	shrl	$16, %ecx
	imull	$-1493, %edx, %edx      # imm = 0xFA2B
	movswl	%dx, %edx
	imull	$11236, %edx, %edx      # imm = 0x2BE4
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1413(%rsi,%r9,2), %edx
	addl	%edx, %ecx
	movswl	%cx, %eax
	shrl	$15, %eax
	andl	%r8d, %eax
	addl	%ecx, %eax
	shll	$14, %r10d
	shll	$6, %r11d
	orl	%r10d, %r11d
	cwtl
	subl	%eax, %edx
	shrl	$2, %edx
	addl	%r11d, %edx
	imull	$-26807, %edx, %ecx     # imm = 0x9749
	leal	-11236(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r8d, %edx
	addl	%edx, %ecx
	addl	$-11236, %ecx           # imm = 0xD41C
	movw	%ax, -104(%rsp,%r9,4)
	movw	%cx, -102(%rsp,%r9,4)
	cmpq	$-2, %r9
	je	.LBB0_3
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	movzwl	-118(%rsp,%r9,2), %eax
	decq	%r9
	jmp	.LBB0_1
.LBB0_3:
	movswl	-98(%rsp), %r8d
	leal	(,%r8,8), %eax
	leal	(%rax,%rax,4), %eax
	negl	%eax
	shrl	$16, %eax
	imull	$-27204, %r8d, %edx     # imm = 0x95BC
	movswl	%dx, %edx
	imull	$106, %edx, %edx
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	1408(%rsi), %r10d
	addl	%r10d, %eax
	movswl	%ax, %r9d
	leal	(%r9,%r9,8), %ecx
	leal	(%rcx,%rcx,2), %ecx
	addl	%r9d, %ecx
	shrl	$16, %ecx
	imull	$-618, %eax, %eax       # imm = 0xFD96
	cwtl
	imull	$106, %eax, %eax
	shrl	$16, %eax
	subl	%eax, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	movl	$106, %r11d
	andl	%r11d, %edx
	addl	%ecx, %edx
	shll	$7, %r8d
	movswl	%dx, %r9d
	subl	%r9d, %r10d
	shrl	%r10d
	addl	%r8d, %r10d
	imull	$21021, %r10d, %edx     # imm = 0x521D
	leal	-2322(%rdx), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$2322, %ecx             # imm = 0x912
	addl	%edx, %ecx
	addl	$-2322, %ecx            # imm = 0xF6EE
	movw	%r9w, -68(%rsp)
	movw	%cx, -66(%rsp)
	movl	$6, %r8d
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	movswl	-112(%rsp,%r8,2), %edx
	leal	(%rdx,%rdx,8), %eax
	leal	(%rax,%rax,2), %eax
	addl	%edx, %eax
	shrl	$16, %eax
	imull	$-618, %edx, %ecx       # imm = 0xFD96
	movswl	%cx, %ecx
	imull	$106, %ecx, %ecx
	shrl	$16, %ecx
	subl	%ecx, %eax
	movswl	%ax, %ecx
	shrl	$15, %ecx
	andl	%r11d, %ecx
	addl	%eax, %ecx
	movswl	%cx, %eax
	subl	%eax, %edx
	shrl	%edx
	imull	$21021, %edx, %ecx      # imm = 0x521D
	leal	-106(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r11d, %edx
	addl	%edx, %ecx
	addl	$-106, %ecx
	movw	%ax, -96(%rsp,%r8,4)
	movw	%cx, -94(%rsp,%r8,4)
	addq	$-1, %r8
	jb	.LBB0_4
# %bb.5:
	movswl	-66(%rsp), %r9d
	movl	%r9d, %eax
	sarl	$12, %eax
	imull	$28772, %r9d, %edx      # imm = 0x7064
	movswl	%dx, %edx
	imull	$164, %edx, %edx
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	1407(%rsi), %edx
	addl	%edx, %eax
	movswl	%ax, %ecx
	shll	$6, %ecx
	negl	%ecx
	shrl	$16, %ecx
	imull	$-400, %eax, %eax       # imm = 0xFE70
	cwtl
	imull	$164, %eax, %eax
	shrl	$16, %eax
	subl	%eax, %ecx
	movswl	%cx, %eax
	shrl	$15, %eax
	movl	$164, %r8d
	andl	%r8d, %eax
	addl	%ecx, %eax
	shll	$6, %r9d
	cwtl
	subl	%eax, %edx
	shrl	$2, %edx
	addl	%r9d, %edx
	imull	$-25575, %edx, %ecx     # imm = 0x9C19
	leal	-3624(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$3624, %edx             # imm = 0xE28
	addl	%edx, %ecx
	addl	$-3624, %ecx            # imm = 0xF1D8
	movw	%ax, -4(%rsp)
	movw	%cx, -2(%rsp)
	xorl	%r10d, %r10d
.LBB0_6:                                # =>This Inner Loop Header: Depth=1
	movswl	-68(%rsp,%r10,2), %r9d
	movl	%r9d, %edx
	sarl	$12, %edx
	imull	$28772, %r9d, %eax      # imm = 0x7064
	cwtl
	imull	$164, %eax, %eax
	shrl	$16, %eax
	subl	%eax, %edx
	movzbl	1406(%rsi,%r10), %eax
	addl	%eax, %edx
	movswl	%dx, %ecx
	shll	$6, %ecx
	negl	%ecx
	shrl	$16, %ecx
	imull	$-400, %edx, %edx       # imm = 0xFE70
	movswl	%dx, %edx
	imull	$164, %edx, %edx
	shrl	$16, %edx
	subl	%edx, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	andl	%r8d, %edx
	addl	%ecx, %edx
	shll	$6, %r9d
	movswl	%dx, %ecx
	subl	%ecx, %eax
	shrl	$2, %eax
	addl	%r9d, %eax
	imull	$-25575, %eax, %eax     # imm = 0x9C19
	leal	-164(%rax), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r8d, %edx
	addl	%edx, %eax
	addl	$-164, %eax
	movw	%cx, -8(%rsp,%r10,4)
	movw	%ax, -6(%rsp,%r10,4)
	decq	%r10
	cmpq	$-15, %r10
	jne	.LBB0_6
# %bb.7:
	movswl	-2(%rsp), %r8d
	imull	$412, %r8d, %ecx        # imm = 0x19C
	shrl	$16, %ecx
	imull	$-5118, %r8d, %edx      # imm = 0xEC02
	movswl	%dx, %edx
	imull	$3278, %edx, %edx       # imm = 0xCCE
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1391(%rsi), %edx
	addl	%edx, %ecx
	movswl	%cx, %r9d
	shrl	$15, %r9d
	movl	$3278, %eax             # imm = 0xCCE
	andl	%r9d, %eax
	addl	%ecx, %eax
	shll	$7, %r8d
	cwtl
	subl	%eax, %edx
	shrl	%edx
	addl	%r8d, %edx
	imull	$-19113, %edx, %ecx     # imm = 0xB557
	leal	-283(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$283, %edx              # imm = 0x11B
	addl	%edx, %ecx
	addl	$-283, %ecx             # imm = 0xFEE5
	movw	%ax, 124(%rsp)
	vpbroadcastw	.LCPI0_0(%rip), %ymm1 # ymm1 = [412,412,412,412,412,412,412,412,412,412,412,412,412,412,412,412]
	movw	%cx, 126(%rsp)
	vpbroadcastw	.LCPI0_1(%rip), %ymm2 # ymm2 = [60418,60418,60418,60418,60418,60418,60418,60418,60418,60418,60418,60418,60418,60418,60418,60418]
	movl	$15, %eax
	vpbroadcastw	.LCPI0_2(%rip), %ymm3 # ymm3 = [3278,3278,3278,3278,3278,3278,3278,3278,3278,3278,3278,3278,3278,3278,3278,3278]
	vmovdqa	.LCPI0_3(%rip), %ymm0   # ymm0 = [255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0]
	vpbroadcastw	.LCPI0_4(%rip), %ymm4 # ymm4 = [46423,46423,46423,46423,46423,46423,46423,46423,46423,46423,46423,46423,46423,46423,46423,46423]
.LBB0_8:                                # =>This Inner Loop Header: Depth=1
	vmovdqu	-64(%rsp,%rax,2), %ymm5
	vmovdqu	1329(%rsi,%rax,2), %ymm6
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
	vmovdqu	%ymm7, (%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 32(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_10
# %bb.9:                                #   in Loop: Header=BB0_8 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_8
.LBB0_10:
	vpbroadcastw	.LCPI0_5(%rip), %ymm1 # ymm1 = [65296,65296,65296,65296,65296,65296,65296,65296,65296,65296,65296,65296,65296,65296,65296,65296]
	movzwl	126(%rsp), %eax
	vpbroadcastw	.LCPI0_6(%rip), %ymm2 # ymm2 = [47220,47220,47220,47220,47220,47220,47220,47220,47220,47220,47220,47220,47220,47220,47220,47220]
	movw	%ax, 380(%rsp)
	vpbroadcastw	.LCPI0_7(%rip), %ymm3 # ymm3 = [916,916,916,916,916,916,916,916,916,916,916,916,916,916,916,916]
	movl	$47, %eax
	vpbroadcastw	.LCPI0_8(%rip), %ymm4 # ymm4 = [48365,48365,48365,48365,48365,48365,48365,48365,48365,48365,48365,48365,48365,48365,48365,48365]
.LBB0_11:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	(%rsp,%rax,2), %ymm5
	vpmovzxbw	1266(%rsi,%rax), %ymm6 # ymm6 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
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
	vmovdqu	%ymm7, 128(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 160(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_13
# %bb.12:                               #   in Loop: Header=BB0_11 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_11
.LBB0_13:
	movswl	380(%rsp), %r9d
	imull	$3712, %r9d, %edx       # imm = 0xE80
	shrl	$16, %edx
	imull	$-2166, %r9d, %ecx      # imm = 0xF78A
	movswl	%cx, %ecx
	imull	$7744, %ecx, %ecx       # imm = 0x1E40
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	1265(%rsi), %r8d
	addl	%r8d, %edx
	movswl	%dx, %ecx
	imull	$3712, %ecx, %ecx       # imm = 0xE80
	shrl	$16, %ecx
	imull	$-2166, %edx, %edx      # imm = 0xF78A
	movswl	%dx, %edx
	imull	$7744, %edx, %edx       # imm = 0x1E40
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1264(%rsi), %edx
	addl	%edx, %ecx
	movswl	%cx, %r10d
	shrl	$15, %r10d
	movl	$7744, %eax             # imm = 0x1E40
	andl	%r10d, %eax
	addl	%ecx, %eax
	shll	$10, %r9d
	leal	(%r9,%r8,4), %ecx
	cwtl
	subl	%eax, %edx
	shrl	$6, %edx
	addl	%ecx, %edx
	imull	$27081, %edx, %ecx      # imm = 0x69C9
	leal	-2393(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$2393, %edx             # imm = 0x959
	addl	%edx, %ecx
	addl	$-2393, %ecx            # imm = 0xF6A7
	vpbroadcastw	.LCPI0_9(%rip), %ymm1 # ymm1 = [3712,3712,3712,3712,3712,3712,3712,3712,3712,3712,3712,3712,3712,3712,3712,3712]
	movw	%ax, 888(%rsp)
	vpbroadcastw	.LCPI0_10(%rip), %ymm2 # ymm2 = [63370,63370,63370,63370,63370,63370,63370,63370,63370,63370,63370,63370,63370,63370,63370,63370]
	movw	%cx, 890(%rsp)
	vpbroadcastw	.LCPI0_11(%rip), %ymm3 # ymm3 = [7744,7744,7744,7744,7744,7744,7744,7744,7744,7744,7744,7744,7744,7744,7744,7744]
	movl	$110, %eax
	vpbroadcastw	.LCPI0_12(%rip), %ymm4 # ymm4 = [27081,27081,27081,27081,27081,27081,27081,27081,27081,27081,27081,27081,27081,27081,27081,27081]
.LBB0_14:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	128(%rsp,%rax,2), %ymm5
	vmovdqu	1012(%rsi,%rax,2), %ymm6
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
	vpsllw	$10, %ymm5, %ymm5
	vpsllw	$2, %ymm7, %ymm7
	vpor	%ymm5, %ymm7, %ymm5
	vpsubw	%ymm8, %ymm6, %ymm6
	vpsraw	$6, %ymm6, %ymm6
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
	vpbroadcastw	.LCPI0_13(%rip), %ymm1 # ymm1 = [65512,65512,65512,65512,65512,65512,65512,65512,65512,65512,65512,65512,65512,65512,65512,65512]
	movzwl	890(%rsp), %eax
	vpbroadcastw	.LCPI0_14(%rip), %ymm2 # ymm2 = [64791,64791,64791,64791,64791,64791,64791,64791,64791,64791,64791,64791,64791,64791,64791,64791]
	movw	%ax, 1908(%rsp)
	vpbroadcastw	.LCPI0_15(%rip), %ymm3 # ymm3 = [88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88]
	movl	$237, %eax
	vpbroadcastw	.LCPI0_16(%rip), %ymm4 # ymm4 = [35747,35747,35747,35747,35747,35747,35747,35747,35747,35747,35747,35747,35747,35747,35747,35747]
.LBB0_17:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	384(%rsp,%rax,2), %ymm5
	vpmulhw	%ymm1, %ymm5, %ymm6
	vpmullw	%ymm2, %ymm5, %ymm7
	vpmulhw	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpsraw	$15, %ymm6, %ymm7
	vpand	%ymm3, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpsraw	$3, %ymm5, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm3, %ymm7
	vpandn	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm6, %ymm7 # ymm7 = ymm6[0],ymm5[0],ymm6[1],ymm5[1],ymm6[2],ymm5[2],ymm6[3],ymm5[3],ymm6[8],ymm5[8],ymm6[9],ymm5[9],ymm6[10],ymm5[10],ymm6[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm6, %ymm5 # ymm5 = ymm6[4],ymm5[4],ymm6[5],ymm5[5],ymm6[6],ymm5[6],ymm6[7],ymm5[7],ymm6[12],ymm5[12],ymm6[13],ymm5[13],ymm6[14],ymm5[14],ymm6[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm7, %ymm6
	vmovdqu	%ymm6, 896(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm7, %ymm5 # ymm5 = ymm7[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 928(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_19
# %bb.18:                               #   in Loop: Header=BB0_17 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_17
.LBB0_19:
	vpbroadcastw	.LCPI0_17(%rip), %ymm1 # ymm1 = [65429,65429,65429,65429,65429,65429,65429,65429,65429,65429,65429,65429,65429,65429,65429,65429]
	vpbroadcastw	.LCPI0_18(%rip), %ymm2 # ymm2 = [58525,58525,58525,58525,58525,58525,58525,58525,58525,58525,58525,58525,58525,58525,58525,58525]
	movl	1908(%rsp), %eax
	vpbroadcastw	.LCPI0_19(%rip), %ymm3 # ymm3 = [2393,2393,2393,2393,2393,2393,2393,2393,2393,2393,2393,2393,2393,2393,2393,2393]
	leal	(%rax,%rax,2), %eax
	addl	$-3588, %eax            # imm = 0xF1FC
	vpbroadcastw	.LCPI0_20(%rip), %ymm4 # ymm4 = [44777,44777,44777,44777,44777,44777,44777,44777,44777,44777,44777,44777,44777,44777,44777,44777]
	movw	%ax, 2024(%rdi)
	vpbroadcastw	.LCPI0_21(%rip), %ymm5 # ymm5 = [3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3]
	movl	$490, %eax              # imm = 0x1EA
	vpbroadcastw	.LCPI0_22(%rip), %ymm6 # ymm6 = [61948,61948,61948,61948,61948,61948,61948,61948,61948,61948,61948,61948,61948,61948,61948,61948]
.LBB0_20:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	896(%rsp,%rax,2), %ymm7
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
	je	.LBB0_22
# %bb.21:                               #   in Loop: Header=BB0_20 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_20
.LBB0_22:
	addq	$1912, %rsp             # imm = 0x778
	.cfi_def_cfa_offset 8
	vzeroupper
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_decode_1013x2393), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_decode_1013x2393)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
