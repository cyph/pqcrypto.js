	.text
	.file	"decode.c"
	.section	.rodata,"a",@progbits
	.p2align	1               # -- Begin function CRYPTO_NAMESPACE(crypto_decode_761x4591)
.LCPI0_0:
	.short	65402                   # 0xff7a
.LCPI0_1:
	.short	55186                   # 0xd792
.LCPI0_2:
	.short	1621                    # 0x655
.LCPI0_3:
	.short	50941                   # 0xc6fd
.LCPI0_4:
	.short	65264                   # 0xfef0
.LCPI0_5:
	.short	39484                   # 0x9a3c
.LCPI0_6:
	.short	644                     # 0x284
.LCPI0_7:
	.short	58209                   # 0xe361
.LCPI0_8:
	.short	78                      # 0x4e
.LCPI0_9:
	.short	24213                   # 0x5e95
.LCPI0_10:
	.short	406                     # 0x196
.LCPI0_11:
	.short	65130                   # 0xfe6a
.LCPI0_12:
	.short	25827                   # 0x64e3
.LCPI0_13:
	.short	50                      # 0x32
.LCPI0_14:
	.short	13433                   # 0x3479
.LCPI0_15:
	.short	322                     # 0x142
.LCPI0_16:
	.short	65214                   # 0xfebe
.LCPI0_17:
	.short	1702                    # 0x6a6
.LCPI0_18:
	.short	61882                   # 0xf1ba
.LCPI0_19:
	.short	4591                    # 0x11ef
.LCPI0_21:
	.short	15631                   # 0x3d0f
.LCPI0_22:
	.short	63241                   # 0xf709
	.section	.rodata.cst32,"aM",@progbits,32
	.p2align	5
.LCPI0_20:
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
	.globl	CRYPTO_NAMESPACE(crypto_decode_761x4591)
	.type	CRYPTO_NAMESPACE(crypto_decode_761x4591),@function
CRYPTO_NAMESPACE(crypto_decode_761x4591): # @CRYPTO_NAMESPACE(crypto_decode_761x4591)
	.cfi_startproc
# %bb.0:
	subq	$1432, %rsp             # imm = 0x598
	.cfi_def_cfa_offset 1440
	movzbl	1157(%rsi), %eax
	imull	$-656, %eax, %ecx       # imm = 0xFD70
	shrl	$16, %ecx
	imull	$-683802624, %eax, %eax # imm = 0xD73E0000
	sarl	$16, %eax
	imull	$1608, %eax, %eax       # imm = 0x648
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	1156(%rsi), %eax
	addl	%ecx, %eax
	movswl	%ax, %ecx
	shrl	$15, %ecx
	movl	$1608, %edx             # imm = 0x648
	andl	%ecx, %edx
	addl	%eax, %edx
	movswl	%dx, %r8d
	imull	$-3624, %r8d, %eax      # imm = 0xF1D8
	shrl	$16, %eax
	imull	$-1772, %edx, %edx      # imm = 0xF914
	movswl	%dx, %edx
	imull	$9470, %edx, %edx       # imm = 0x24FE
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	1155(%rsi), %r9d
	addl	%r9d, %eax
	movswl	%ax, %ecx
	imull	$-3624, %ecx, %ecx      # imm = 0xF1D8
	shrl	$16, %ecx
	imull	$-1772, %eax, %eax      # imm = 0xF914
	cwtl
	imull	$9470, %eax, %eax       # imm = 0x24FE
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	1154(%rsi), %edx
	addl	%edx, %ecx
	movswl	%cx, %r10d
	shrl	$15, %r10d
	movl	$9470, %eax             # imm = 0x24FE
	andl	%r10d, %eax
	addl	%ecx, %eax
	shll	$15, %r8d
	shll	$7, %r9d
	orl	%r8d, %r9d
	movswl	%ax, %r8d
	subl	%r8d, %edx
	shrl	%edx
	addl	%r9d, %edx
	imull	$-21121, %edx, %ecx     # imm = 0xAD7F
	leal	-11127(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$11127, %edx            # imm = 0x2B77
	leal	(%rdx,%rcx), %r9d
	addl	$-11127, %r9d           # imm = 0xD489
	movw	%r9w, -110(%rsp)
	imull	$541, %r8d, %edx        # imm = 0x21D
	shrl	$16, %edx
	imull	$-10775, %eax, %ecx     # imm = 0xD5E9
	movswl	%cx, %ecx
	imull	$1557, %ecx, %ecx       # imm = 0x615
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	1153(%rsi), %r8d
	addl	%r8d, %edx
	movswl	%dx, %ecx
	shrl	$15, %ecx
	movl	$1557, %r10d            # imm = 0x615
	andl	%r10d, %ecx
	addl	%edx, %ecx
	shll	$8, %eax
	orl	%r8d, %eax
	subl	%ecx, %eax
	imull	$-26307, %eax, %eax     # imm = 0x993D
	leal	-1557(%rax), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r10d, %edx
	addl	%eax, %edx
	addl	$-1557, %edx            # imm = 0xF9EB
	movw	%cx, -114(%rsp)
	movw	%dx, -112(%rsp)
	movswl	%r9w, %eax
	imull	$-545, %eax, %ecx       # imm = 0xFDDF
	shrl	$16, %ecx
	imull	$-1661, %r9d, %eax      # imm = 0xF983
	cwtl
	imull	$10101, %eax, %eax      # imm = 0x2775
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	1152(%rsi), %r8d
	addl	%r8d, %ecx
	movswl	%cx, %r10d
	shrl	$15, %r10d
	movl	$10101, %r11d           # imm = 0x2775
	andl	%r11d, %r10d
	addl	%ecx, %r10d
	shll	$8, %r9d
	orl	%r8d, %r9d
	subl	%r10d, %r9d
	imull	$12509, %r9d, %r8d      # imm = 0x30DD
	leal	-282(%r8), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$282, %ecx              # imm = 0x11A
	addl	%r8d, %ecx
	addl	$-282, %ecx             # imm = 0xFEE6
	movw	%r10w, -100(%rsp)
	movw	%cx, -98(%rsp)
	xorl	%r8d, %r8d
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movswl	%dx, %eax
	imull	$-545, %eax, %eax       # imm = 0xFDDF
	shrl	$16, %eax
	imull	$-1661, %edx, %edx      # imm = 0xF983
	movswl	%dx, %edx
	imull	$10101, %edx, %edx      # imm = 0x2775
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	1151(%rsi,%r8,2), %edx
	addl	%edx, %eax
	movswl	%ax, %ecx
	imull	$-545, %ecx, %ecx       # imm = 0xFDDF
	shrl	$16, %ecx
	imull	$-1661, %eax, %eax      # imm = 0xF983
	cwtl
	imull	$10101, %eax, %eax      # imm = 0x2775
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	1150(%rsi,%r8,2), %r9d
	addl	%r9d, %ecx
	movswl	%cx, %eax
	shrl	$15, %eax
	andl	%r11d, %eax
	addl	%ecx, %eax
	shll	$8, %edx
	orl	%r9d, %edx
	subl	%eax, %edx
	imull	$12509, %edx, %ecx      # imm = 0x30DD
	leal	-10101(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r11d, %edx
	addl	%edx, %ecx
	addl	$-10101, %ecx           # imm = 0xD88B
	movw	%ax, -104(%rsp,%r8,4)
	movw	%cx, -102(%rsp,%r8,4)
	cmpq	$-1, %r8
	je	.LBB0_3
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	movzwl	-114(%rsp,%r8,2), %edx
	decq	%r8
	jmp	.LBB0_1
.LBB0_3:
	movswl	-98(%rsp), %r8d
	imull	$-656, %r8d, %eax       # imm = 0xFD70
	shrl	$16, %eax
	imull	$-10434, %r8d, %edx     # imm = 0xD73E
	movswl	%dx, %edx
	imull	$1608, %edx, %edx       # imm = 0x648
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	1147(%rsi), %r9d
	addl	%r9d, %eax
	movswl	%ax, %ecx
	imull	$-656, %ecx, %ecx       # imm = 0xFD70
	shrl	$16, %ecx
	imull	$-10434, %eax, %eax     # imm = 0xD73E
	cwtl
	imull	$1608, %eax, %eax       # imm = 0x648
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	1146(%rsi), %r10d
	addl	%r10d, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	movl	$1608, %r11d            # imm = 0x648
	andl	%r11d, %edx
	addl	%ecx, %edx
	shll	$13, %r8d
	shll	$5, %r9d
	orl	%r8d, %r9d
	movswl	%dx, %r8d
	subl	%r8d, %r10d
	shrl	$3, %r10d
	addl	%r9d, %r10d
	imull	$6521, %r10d, %edx      # imm = 0x1979
	leal	-11468(%rdx), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$11468, %ecx            # imm = 0x2CCC
	addl	%edx, %ecx
	addl	$-11468, %ecx           # imm = 0xD334
	movw	%r8w, -76(%rsp)
	movw	%cx, -74(%rsp)
	xorl	%r9d, %r9d
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	movswl	-100(%rsp,%r9,2), %r8d
	imull	$-656, %r8d, %eax       # imm = 0xFD70
	shrl	$16, %eax
	imull	$-10434, %r8d, %edx     # imm = 0xD73E
	movswl	%dx, %edx
	imull	$1608, %edx, %edx       # imm = 0x648
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	1145(%rsi,%r9), %edx
	addl	%edx, %eax
	movswl	%ax, %ecx
	shrl	$15, %ecx
	andl	%r11d, %ecx
	addl	%eax, %ecx
	shll	$5, %r8d
	movswl	%cx, %eax
	subl	%eax, %edx
	shrl	$3, %edx
	addl	%r8d, %edx
	imull	$6521, %edx, %ecx       # imm = 0x1979
	leal	-1608(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r11d, %edx
	addl	%edx, %ecx
	addl	$-1608, %ecx            # imm = 0xF9B8
	movw	%ax, -80(%rsp,%r9,4)
	movw	%cx, -78(%rsp,%r9,4)
	decq	%r9
	cmpq	$-5, %r9
	jne	.LBB0_4
# %bb.5:
	movswl	-74(%rsp), %ecx
	imull	$4206, %ecx, %edx       # imm = 0x106E
	shrl	$16, %edx
	imull	$-1634, %ecx, %eax      # imm = 0xF99E
	cwtl
	imull	$10265, %eax, %eax      # imm = 0x2819
	shrl	$16, %eax
	subl	%eax, %edx
	movzbl	1140(%rsi), %r8d
	addl	%r8d, %edx
	movswl	%dx, %r9d
	shrl	$15, %r9d
	movl	$10265, %r10d           # imm = 0x2819
	andl	%r10d, %r9d
	addl	%edx, %r9d
	shll	$8, %ecx
	orl	%r8d, %ecx
	subl	%r9d, %ecx
	imull	$-19415, %ecx, %ecx     # imm = 0xB429
	leal	-286(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$286, %edx              # imm = 0x11E
	addl	%edx, %ecx
	addl	$-286, %ecx             # imm = 0xFEE2
	movw	%r9w, -20(%rsp)
	movw	%cx, -18(%rsp)
	xorl	%r8d, %r8d
.LBB0_6:                                # =>This Inner Loop Header: Depth=1
	movswl	-76(%rsp,%r8,2), %edx
	imull	$4206, %edx, %eax       # imm = 0x106E
	shrl	$16, %eax
	imull	$-1634, %edx, %edx      # imm = 0xF99E
	movswl	%dx, %edx
	imull	$10265, %edx, %edx      # imm = 0x2819
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	1139(%rsi,%r8,2), %edx
	addl	%edx, %eax
	movswl	%ax, %ecx
	imull	$4206, %ecx, %ecx       # imm = 0x106E
	shrl	$16, %ecx
	imull	$-1634, %eax, %eax      # imm = 0xF99E
	cwtl
	imull	$10265, %eax, %eax      # imm = 0x2819
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	1138(%rsi,%r8,2), %r9d
	addl	%r9d, %ecx
	movswl	%cx, %eax
	shrl	$15, %eax
	andl	%r10d, %eax
	addl	%ecx, %eax
	shll	$8, %edx
	orl	%r9d, %edx
	subl	%eax, %edx
	imull	$-19415, %edx, %ecx     # imm = 0xB429
	leal	-10265(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r10d, %edx
	addl	%edx, %ecx
	addl	$-10265, %ecx           # imm = 0xD7E7
	movw	%ax, -24(%rsp,%r8,4)
	movw	%cx, -22(%rsp,%r8,4)
	decq	%r8
	cmpq	$-11, %r8
	jne	.LBB0_6
# %bb.7:
	movswl	-18(%rsp), %eax
	imull	$-134, %eax, %ecx
	shrl	$16, %ecx
	imull	$-10350, %eax, %eax     # imm = 0xD792
	cwtl
	imull	$1621, %eax, %eax       # imm = 0x655
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	1117(%rsi), %eax
	addl	%eax, %ecx
	movswl	%cx, %edx
	imull	$-134, %edx, %edx
	shrl	$16, %edx
	imull	$-10350, %ecx, %ecx     # imm = 0xD792
	movswl	%cx, %ecx
	imull	$1621, %ecx, %ecx       # imm = 0x655
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	1116(%rsi), %r8d
	addl	%r8d, %edx
	movswl	%dx, %ecx
	shrl	$15, %ecx
	movl	$1621, %r9d             # imm = 0x655
	andl	%ecx, %r9d
	addl	%edx, %r9d
	shll	$8, %eax
	orl	%r8d, %eax
	subl	%r9d, %eax
	imull	$-14595, %eax, %eax     # imm = 0xC6FD
	leal	-11550(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$11550, %ecx            # imm = 0x2D1E
	addl	%ecx, %eax
	addl	$-11550, %eax           # imm = 0xD2E2
	vpbroadcastw	.LCPI0_0(%rip), %ymm0 # ymm0 = [65402,65402,65402,65402,65402,65402,65402,65402,65402,65402,65402,65402,65402,65402,65402,65402]
	movw	%r9w, 76(%rsp)
	vpbroadcastw	.LCPI0_1(%rip), %ymm1 # ymm1 = [55186,55186,55186,55186,55186,55186,55186,55186,55186,55186,55186,55186,55186,55186,55186,55186]
	movw	%ax, 78(%rsp)
	vpbroadcastw	.LCPI0_2(%rip), %ymm2 # ymm2 = [1621,1621,1621,1621,1621,1621,1621,1621,1621,1621,1621,1621,1621,1621,1621,1621]
	movl	$7, %eax
	vpbroadcastw	.LCPI0_3(%rip), %ymm3 # ymm3 = [50941,50941,50941,50941,50941,50941,50941,50941,50941,50941,50941,50941,50941,50941,50941,50941]
.LBB0_8:                                # =>This Inner Loop Header: Depth=1
	vmovdqu	-64(%rsp,%rax,2), %ymm4
	vpmovzxbw	1093(%rsi,%rax), %ymm5 # ymm5 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm0, %ymm4, %ymm6
	vpmullw	%ymm1, %ymm4, %ymm7
	vpmulhw	%ymm2, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm6
	vpsraw	$15, %ymm6, %ymm7
	vpand	%ymm2, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm6
	vpsllw	$8, %ymm4, %ymm4
	vpor	%ymm5, %ymm4, %ymm4
	vpsubw	%ymm6, %ymm4, %ymm4
	vpmullw	%ymm3, %ymm4, %ymm4
	vpcmpgtw	%ymm4, %ymm2, %ymm5
	vpandn	%ymm2, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm4, %ymm4
	vpunpcklwd	%ymm4, %ymm6, %ymm5 # ymm5 = ymm6[0],ymm4[0],ymm6[1],ymm4[1],ymm6[2],ymm4[2],ymm6[3],ymm4[3],ymm6[8],ymm4[8],ymm6[9],ymm4[9],ymm6[10],ymm4[10],ymm6[11],ymm4[11]
	vpunpckhwd	%ymm4, %ymm6, %ymm4 # ymm4 = ymm6[4],ymm4[4],ymm6[5],ymm4[5],ymm6[6],ymm4[6],ymm6[7],ymm4[7],ymm6[12],ymm4[12],ymm6[13],ymm4[13],ymm6[14],ymm4[14],ymm6[15],ymm4[15]
	vinserti128	$1, %xmm4, %ymm5, %ymm6
	vmovdqu	%ymm6, -16(%rsp,%rax,4)
	vperm2i128	$49, %ymm4, %ymm5, %ymm4 # ymm4 = ymm5[2,3],ymm4[2,3]
	vmovdqu	%ymm4, 16(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_10
# %bb.9:                                #   in Loop: Header=BB0_8 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_8
.LBB0_10:
	movswl	78(%rsp), %r8d
	imull	$-272, %r8d, %ecx       # imm = 0xFEF0
	shrl	$16, %ecx
	imull	$-26052, %r8d, %edx     # imm = 0x9A3C
	movswl	%dx, %edx
	imull	$644, %edx, %edx        # imm = 0x284
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1092(%rsi), %edx
	addl	%edx, %ecx
	movswl	%cx, %r9d
	shrl	$15, %r9d
	movl	$644, %eax              # imm = 0x284
	andl	%r9d, %eax
	addl	%ecx, %eax
	shll	$6, %r8d
	cwtl
	subl	%eax, %edx
	shrl	$2, %edx
	addl	%r8d, %edx
	imull	$-7327, %edx, %ecx      # imm = 0xE361
	leal	-4591(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$4591, %edx             # imm = 0x11EF
	addl	%edx, %ecx
	addl	$-4591, %ecx            # imm = 0xEE11
	vpbroadcastw	.LCPI0_4(%rip), %ymm1 # ymm1 = [65264,65264,65264,65264,65264,65264,65264,65264,65264,65264,65264,65264,65264,65264,65264,65264]
	movw	%ax, 268(%rsp)
	vpbroadcastw	.LCPI0_5(%rip), %ymm2 # ymm2 = [39484,39484,39484,39484,39484,39484,39484,39484,39484,39484,39484,39484,39484,39484,39484,39484]
	movw	%cx, 270(%rsp)
	vpbroadcastw	.LCPI0_6(%rip), %ymm3 # ymm3 = [644,644,644,644,644,644,644,644,644,644,644,644,644,644,644,644]
	movl	$31, %eax
	vpbroadcastw	.LCPI0_7(%rip), %ymm0 # ymm0 = [58209,58209,58209,58209,58209,58209,58209,58209,58209,58209,58209,58209,58209,58209,58209,58209]
.LBB0_11:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	-16(%rsp,%rax,2), %ymm4
	vpmovzxbw	1045(%rsi,%rax), %ymm5 # ymm5 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm1, %ymm4, %ymm6
	vpmullw	%ymm2, %ymm4, %ymm7
	vpmulhw	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm6
	vpsraw	$15, %ymm6, %ymm7
	vpand	%ymm3, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm6
	vpsllw	$6, %ymm4, %ymm4
	vpsubw	%ymm6, %ymm5, %ymm5
	vpsraw	$2, %ymm5, %ymm5
	vpaddw	%ymm4, %ymm5, %ymm4
	vpmullw	%ymm0, %ymm4, %ymm4
	vpcmpgtw	%ymm4, %ymm3, %ymm5
	vpandn	%ymm3, %ymm5, %ymm5
	vpsubw	%ymm5, %ymm4, %ymm4
	vpunpcklwd	%ymm4, %ymm6, %ymm5 # ymm5 = ymm6[0],ymm4[0],ymm6[1],ymm4[1],ymm6[2],ymm4[2],ymm6[3],ymm4[3],ymm6[8],ymm4[8],ymm6[9],ymm4[9],ymm6[10],ymm4[10],ymm6[11],ymm4[11]
	vpunpckhwd	%ymm4, %ymm6, %ymm4 # ymm4 = ymm6[4],ymm4[4],ymm6[5],ymm4[5],ymm6[6],ymm4[6],ymm6[7],ymm4[7],ymm6[12],ymm4[12],ymm6[13],ymm4[13],ymm6[14],ymm4[14],ymm6[15],ymm4[15]
	vinserti128	$1, %xmm4, %ymm5, %ymm6
	vmovdqu	%ymm6, 80(%rsp,%rax,4)
	vperm2i128	$49, %ymm4, %ymm5, %ymm4 # ymm4 = ymm5[2,3],ymm4[2,3]
	vmovdqu	%ymm4, 112(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_13
# %bb.12:                               #   in Loop: Header=BB0_11 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_11
.LBB0_13:
	vpbroadcastw	.LCPI0_8(%rip), %ymm1 # ymm1 = [78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78]
	vpbroadcastw	.LCPI0_9(%rip), %ymm2 # ymm2 = [24213,24213,24213,24213,24213,24213,24213,24213,24213,24213,24213,24213,24213,24213,24213,24213]
	movzwl	270(%rsp), %eax
	vpbroadcastw	.LCPI0_10(%rip), %ymm3 # ymm3 = [406,406,406,406,406,406,406,406,406,406,406,406,406,406,406,406]
	movw	%ax, 652(%rsp)
	vpbroadcastw	.LCPI0_11(%rip), %ymm4 # ymm4 = [65130,65130,65130,65130,65130,65130,65130,65130,65130,65130,65130,65130,65130,65130,65130,65130]
	movl	$79, %eax
	vpbroadcastw	.LCPI0_12(%rip), %ymm5 # ymm5 = [25827,25827,25827,25827,25827,25827,25827,25827,25827,25827,25827,25827,25827,25827,25827,25827]
.LBB0_14:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	80(%rsp,%rax,2), %ymm6
	vpmovzxbw	950(%rsi,%rax), %ymm7 # ymm7 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm1, %ymm6, %ymm8
	vpmullw	%ymm2, %ymm6, %ymm9
	vpmulhw	%ymm3, %ymm9, %ymm9
	vpaddw	%ymm7, %ymm8, %ymm8
	vpsubw	%ymm9, %ymm8, %ymm8
	vpaddw	%ymm4, %ymm8, %ymm8
	vpsraw	$15, %ymm8, %ymm9
	vpand	%ymm3, %ymm9, %ymm9
	vpaddw	%ymm8, %ymm9, %ymm8
	vpsraw	$15, %ymm8, %ymm9
	vpand	%ymm3, %ymm9, %ymm9
	vpaddw	%ymm8, %ymm9, %ymm8
	vpsllw	$7, %ymm6, %ymm6
	vpsubw	%ymm8, %ymm7, %ymm7
	vpsraw	$1, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm6
	vpmullw	%ymm5, %ymm6, %ymm6
	vpcmpgtw	%ymm6, %ymm3, %ymm7
	vpandn	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpunpcklwd	%ymm6, %ymm8, %ymm7 # ymm7 = ymm8[0],ymm6[0],ymm8[1],ymm6[1],ymm8[2],ymm6[2],ymm8[3],ymm6[3],ymm8[8],ymm6[8],ymm8[9],ymm6[9],ymm8[10],ymm6[10],ymm8[11],ymm6[11]
	vpunpckhwd	%ymm6, %ymm8, %ymm6 # ymm6 = ymm8[4],ymm6[4],ymm8[5],ymm6[5],ymm8[6],ymm6[6],ymm8[7],ymm6[7],ymm8[12],ymm6[12],ymm8[13],ymm6[13],ymm8[14],ymm6[14],ymm8[15],ymm6[15]
	vinserti128	$1, %xmm6, %ymm7, %ymm8
	vmovdqu	%ymm8, 272(%rsp,%rax,4)
	vperm2i128	$49, %ymm6, %ymm7, %ymm6 # ymm6 = ymm7[2,3],ymm6[2,3]
	vmovdqu	%ymm6, 304(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_16
# %bb.15:                               #   in Loop: Header=BB0_14 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_14
.LBB0_16:
	vpbroadcastw	.LCPI0_13(%rip), %ymm1 # ymm1 = [50,50,50,50,50,50,50,50,50,50,50,50,50,50,50,50]
	movzwl	652(%rsp), %eax
	vpbroadcastw	.LCPI0_14(%rip), %ymm2 # ymm2 = [13433,13433,13433,13433,13433,13433,13433,13433,13433,13433,13433,13433,13433,13433,13433,13433]
	movw	%ax, 1416(%rsp)
	vpbroadcastw	.LCPI0_15(%rip), %ymm3 # ymm3 = [322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322]
	movl	$174, %eax
	vpbroadcastw	.LCPI0_16(%rip), %ymm4 # ymm4 = [65214,65214,65214,65214,65214,65214,65214,65214,65214,65214,65214,65214,65214,65214,65214,65214]
.LBB0_17:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	272(%rsp,%rax,2), %ymm5
	vpmovzxbw	760(%rsi,%rax), %ymm6 # ymm6 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm1, %ymm5, %ymm7
	vpmullw	%ymm2, %ymm5, %ymm8
	vpmulhw	%ymm3, %ymm8, %ymm8
	vpaddw	%ymm6, %ymm7, %ymm7
	vpsubw	%ymm8, %ymm7, %ymm7
	vpaddw	%ymm4, %ymm7, %ymm7
	vpsraw	$15, %ymm7, %ymm8
	vpand	%ymm3, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm7
	vpsraw	$15, %ymm7, %ymm8
	vpand	%ymm3, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm7
	vpsllw	$7, %ymm5, %ymm5
	vpsubw	%ymm7, %ymm6, %ymm6
	vpsraw	$1, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm5
	vpmullw	%ymm0, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm3, %ymm6
	vpandn	%ymm3, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm7, %ymm6 # ymm6 = ymm7[0],ymm5[0],ymm7[1],ymm5[1],ymm7[2],ymm5[2],ymm7[3],ymm5[3],ymm7[8],ymm5[8],ymm7[9],ymm5[9],ymm7[10],ymm5[10],ymm7[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm7, %ymm5 # ymm5 = ymm7[4],ymm5[4],ymm7[5],ymm5[5],ymm7[6],ymm5[6],ymm7[7],ymm5[7],ymm7[12],ymm5[12],ymm7[13],ymm5[13],ymm7[14],ymm5[14],ymm7[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm6, %ymm7
	vmovdqu	%ymm7, 656(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 688(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_19
# %bb.18:                               #   in Loop: Header=BB0_17 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_17
.LBB0_19:
	movl	$-2295, %eax            # imm = 0xF709
	addl	1416(%rsp), %eax
	vpbroadcastw	.LCPI0_17(%rip), %ymm0 # ymm0 = [1702,1702,1702,1702,1702,1702,1702,1702,1702,1702,1702,1702,1702,1702,1702,1702]
	vpbroadcastw	.LCPI0_18(%rip), %ymm1 # ymm1 = [61882,61882,61882,61882,61882,61882,61882,61882,61882,61882,61882,61882,61882,61882,61882,61882]
	movw	%ax, 1520(%rdi)
	vpbroadcastw	.LCPI0_19(%rip), %ymm2 # ymm2 = [4591,4591,4591,4591,4591,4591,4591,4591,4591,4591,4591,4591,4591,4591,4591,4591]
	movl	$364, %eax              # imm = 0x16C
	vpbroadcastw	.LCPI0_21(%rip), %ymm3 # ymm3 = [15631,15631,15631,15631,15631,15631,15631,15631,15631,15631,15631,15631,15631,15631,15631,15631]
	vmovdqa	.LCPI0_20(%rip), %ymm4  # ymm4 = [255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0]
	vpbroadcastw	.LCPI0_22(%rip), %ymm5 # ymm5 = [63241,63241,63241,63241,63241,63241,63241,63241,63241,63241,63241,63241,63241,63241,63241,63241]
.LBB0_20:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	656(%rsp,%rax,2), %ymm6
	vmovdqu	(%rsi,%rax,2), %ymm7
	vpsrlw	$8, %ymm7, %ymm8
	vpmulhw	%ymm0, %ymm6, %ymm9
	vpmullw	%ymm1, %ymm6, %ymm6
	vpmulhw	%ymm2, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm9, %ymm6
	vpaddw	%ymm6, %ymm8, %ymm6
	vpmulhw	%ymm0, %ymm6, %ymm8
	vpmullw	%ymm1, %ymm6, %ymm6
	vpmulhw	%ymm2, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm8, %ymm6
	vpand	%ymm4, %ymm7, %ymm8
	vpaddw	%ymm6, %ymm8, %ymm6
	vpsraw	$15, %ymm6, %ymm8
	vpand	%ymm2, %ymm8, %ymm8
	vpaddw	%ymm6, %ymm8, %ymm6
	vpsubw	%ymm6, %ymm7, %ymm7
	vpmullw	%ymm3, %ymm7, %ymm7
	vpcmpgtw	%ymm7, %ymm2, %ymm8
	vpandn	%ymm2, %ymm8, %ymm8
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
	addq	$1432, %rsp             # imm = 0x598
	.cfi_def_cfa_offset 8
	vzeroupper
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_decode_761x4591), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_decode_761x4591)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
