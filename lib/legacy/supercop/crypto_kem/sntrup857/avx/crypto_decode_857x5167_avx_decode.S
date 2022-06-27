	.text
	.file	"decode.c"
	.section	.rodata,"a",@progbits
	.p2align	1               # -- Begin function CRYPTO_NAMESPACE(crypto_decode_857x5167)
.LCPI0_0:
	.short	658                     # 0x292
.LCPI0_1:
	.short	63970                   # 0xf9e2
.LCPI0_2:
	.short	10713                   # 0x29d9
.LCPI0_4:
	.short	50793                   # 0xc669
.LCPI0_5:
	.short	280                     # 0x118
.LCPI0_6:
	.short	55405                   # 0xd86d
.LCPI0_7:
	.short	1656                    # 0x678
.LCPI0_8:
	.short	1583                    # 0x62f
.LCPI0_9:
	.short	295                     # 0x127
.LCPI0_10:
	.short	39765                   # 0x9b55
.LCPI0_11:
	.short	651                     # 0x28b
.LCPI0_12:
	.short	64885                   # 0xfd75
.LCPI0_13:
	.short	54563                   # 0xd523
.LCPI0_14:
	.short	65384                   # 0xff68
.LCPI0_15:
	.short	24415                   # 0x5f5f
.LCPI0_16:
	.short	408                     # 0x198
.LCPI0_17:
	.short	65128                   # 0xfe68
.LCPI0_18:
	.short	64251                   # 0xfafb
.LCPI0_19:
	.short	65503                   # 0xffdf
.LCPI0_20:
	.short	62289                   # 0xf351
.LCPI0_21:
	.short	5167                    # 0x142f
.LCPI0_22:
	.short	45775                   # 0xb2cf
.LCPI0_23:
	.short	62953                   # 0xf5e9
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
	.globl	CRYPTO_NAMESPACE(crypto_decode_857x5167)
	.type	CRYPTO_NAMESPACE(crypto_decode_857x5167),@function
CRYPTO_NAMESPACE(crypto_decode_857x5167): # @CRYPTO_NAMESPACE(crypto_decode_857x5167)
	.cfi_startproc
# %bb.0:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$1632, %rsp             # imm = 0x660
	.cfi_def_cfa_offset 1648
	.cfi_offset %rbx, -16
	movzbl	1321(%rsi), %eax
	imull	$841, %eax, %ecx        # imm = 0x349
	shrl	$16, %ecx
	imull	$-176619520, %eax, %eax # imm = 0xF5790000
	sarl	$16, %eax
	imull	$6225, %eax, %eax       # imm = 0x1851
	shrl	$16, %eax
	subl	%eax, %ecx
	movzbl	1320(%rsi), %eax
	addl	%ecx, %eax
	movswl	%ax, %ecx
	shrl	$15, %ecx
	movl	$6225, %edx             # imm = 0x1851
	andl	%ecx, %edx
	addl	%eax, %edx
	movswl	%dx, %r8d
	imull	$-1248, %r8d, %ecx      # imm = 0xFB20
	shrl	$16, %ecx
	imull	$-3064, %edx, %edx      # imm = 0xF408
	movswl	%dx, %edx
	imull	$5476, %edx, %edx       # imm = 0x1564
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1319(%rsi), %edx
	addl	%edx, %ecx
	movswl	%cx, %eax
	shrl	$15, %eax
	movl	$5476, %r9d             # imm = 0x1564
	andl	%eax, %r9d
	addl	%ecx, %r9d
	shll	$6, %r8d
	movswl	%r9w, %r10d
	subl	%r10d, %edx
	shrl	$2, %edx
	addl	%r8d, %edx
	imull	$-3351, %edx, %eax      # imm = 0xF2E9
	leal	-291(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$291, %ecx              # imm = 0x123
	addl	%ecx, %eax
	addl	$-291, %eax             # imm = 0xFEDD
	movswl	%ax, %r11d
	leal	(%r11,%r11), %edx
	leal	(%rdx,%rdx,4), %edx
	shrl	$16, %edx
	imull	$-30111, %eax, %eax     # imm = 0x8A61
	cwtl
	imull	$74, %eax, %eax
	shrl	$16, %eax
	subl	%eax, %edx
	movzbl	1318(%rsi), %eax
	addl	%eax, %edx
	movswl	%dx, %ecx
	imull	$-28, %ecx, %ecx
	shrl	$16, %ecx
	imull	$-886, %edx, %edx       # imm = 0xFC8A
	movswl	%dx, %edx
	imull	$74, %edx, %edx
	shrl	$16, %edx
	subl	%edx, %ecx
	movswl	%cx, %edx
	shrl	$15, %edx
	movl	$74, %r8d
	andl	%r8d, %edx
	addl	%ecx, %edx
	shll	$7, %r11d
	movswl	%dx, %ebx
	subl	%ebx, %eax
	shrl	%eax
	addl	%r11d, %eax
	imull	$7085, %eax, %eax       # imm = 0x1BAD
	leal	-1004(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$1004, %ecx             # imm = 0x3EC
	leal	(%rcx,%rax), %r11d
	addl	$-1004, %r11d           # imm = 0xFC14
	movw	%bx, -114(%rsp)
	movw	%r11w, -112(%rsp)
	imull	$-28, %r10d, %eax
	shrl	$16, %eax
	imull	$-886, %r9d, %ebx       # imm = 0xFC8A
	movswl	%bx, %ebx
	imull	$74, %ebx, %ebx
	shrl	$16, %ebx
	subl	%ebx, %eax
	movswl	%ax, %ebx
	shrl	$15, %ebx
	andl	%r8d, %ebx
	addl	%eax, %ebx
	movswl	%bx, %eax
	subl	%eax, %r10d
	shrl	%r10d
	imull	$7085, %r10d, %ebx      # imm = 0x1BAD
	leal	-74(%rbx), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	%r8d, %ecx
	addl	%ebx, %ecx
	addl	$-74, %ecx
	movw	%ax, -118(%rsp)
	movw	%cx, -116(%rsp)
	movw	%r11w, -98(%rsp)
	xorl	%r9d, %r9d
	movl	$2194, %r8d             # imm = 0x892
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movswl	%dx, %r10d
	imull	$-302, %r10d, %ebx      # imm = 0xFED2
	shrl	$16, %ebx
	imull	$-7647, %edx, %edx      # imm = 0xE221
	movswl	%dx, %edx
	imull	$2194, %edx, %edx       # imm = 0x892
	shrl	$16, %edx
	subl	%edx, %ebx
	movzbl	1317(%rsi,%r9,2), %edx
	addl	%edx, %ebx
	movswl	%bx, %eax
	imull	$-302, %eax, %eax       # imm = 0xFED2
	shrl	$16, %eax
	imull	$-7647, %ebx, %ebx      # imm = 0xE221
	movswl	%bx, %ebx
	imull	$2194, %ebx, %ebx       # imm = 0x892
	shrl	$16, %ebx
	subl	%ebx, %eax
	movzbl	1316(%rsi,%r9,2), %ebx
	addl	%ebx, %eax
	movswl	%ax, %ecx
	shrl	$15, %ecx
	andl	%r8d, %ecx
	addl	%eax, %ecx
	shll	$15, %r10d
	shll	$7, %edx
	orl	%r10d, %edx
	movswl	%cx, %eax
	subl	%eax, %ebx
	shrl	%ebx
	addl	%edx, %ebx
	imull	$11769, %ebx, %ecx      # imm = 0x2DF9
	leal	-2194(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r8d, %edx
	addl	%edx, %ecx
	addl	$-2194, %ecx            # imm = 0xF76E
	movw	%ax, -102(%rsp,%r9,4)
	movw	%cx, -100(%rsp,%r9,4)
	cmpq	$-2, %r9
	je	.LBB0_3
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	movzwl	-116(%rsp,%r9,2), %edx
	decq	%r9
	jmp	.LBB0_1
.LBB0_3:
	movswl	-98(%rsp), %eax
	imull	$1807, %eax, %edx       # imm = 0x70F
	shrl	$16, %edx
	imull	$-1399, %eax, %eax      # imm = 0xFA89
	cwtl
	imull	$11991, %eax, %eax      # imm = 0x2ED7
	shrl	$16, %eax
	subl	%eax, %edx
	movzbl	1311(%rsi), %ecx
	addl	%ecx, %edx
	movswl	%dx, %eax
	imull	$1807, %eax, %ebx       # imm = 0x70F
	shrl	$16, %ebx
	imull	$-1399, %edx, %eax      # imm = 0xFA89
	cwtl
	imull	$11991, %eax, %eax      # imm = 0x2ED7
	shrl	$16, %eax
	subl	%eax, %ebx
	movzbl	1310(%rsi), %r8d
	addl	%r8d, %ebx
	movswl	%bx, %edx
	shrl	$15, %edx
	movl	$11991, %r9d            # imm = 0x2ED7
	andl	%r9d, %edx
	addl	%ebx, %edx
	shll	$8, %ecx
	orl	%r8d, %ecx
	subl	%edx, %ecx
	imull	$-23321, %ecx, %ecx     # imm = 0xA4E7
	leal	-5483(%rcx), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	andl	$5483, %ebx             # imm = 0x156B
	addl	%ebx, %ecx
	addl	$-5483, %ecx            # imm = 0xEA95
	movw	%dx, -72(%rsp)
	movw	%cx, -70(%rsp)
	xorl	%ecx, %ecx
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	movswl	-100(%rsp,%rcx,2), %edx
	imull	$1807, %edx, %ebx       # imm = 0x70F
	shrl	$16, %ebx
	imull	$-1399, %edx, %edx      # imm = 0xFA89
	movswl	%dx, %edx
	imull	$11991, %edx, %edx      # imm = 0x2ED7
	shrl	$16, %edx
	subl	%edx, %ebx
	movzbl	1309(%rsi,%rcx,2), %edx
	addl	%edx, %ebx
	movswl	%bx, %eax
	imull	$1807, %eax, %eax       # imm = 0x70F
	shrl	$16, %eax
	imull	$-1399, %ebx, %ebx      # imm = 0xFA89
	movswl	%bx, %ebx
	imull	$11991, %ebx, %ebx      # imm = 0x2ED7
	shrl	$16, %ebx
	subl	%ebx, %eax
	movzbl	1308(%rsi,%rcx,2), %r8d
	addl	%r8d, %eax
	movswl	%ax, %ebx
	shrl	$15, %ebx
	andl	%r9d, %ebx
	addl	%eax, %ebx
	shll	$8, %edx
	orl	%r8d, %edx
	subl	%ebx, %edx
	imull	$-23321, %edx, %eax     # imm = 0xA4E7
	leal	-11991(%rax), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r9d, %edx
	addl	%edx, %eax
	addl	$-11991, %eax           # imm = 0xD129
	movw	%bx, -76(%rsp,%rcx,4)
	movw	%ax, -74(%rsp,%rcx,4)
	decq	%rcx
	cmpq	$-6, %rcx
	jne	.LBB0_4
# %bb.5:
	movzwl	-70(%rsp), %eax
	movw	%ax, -12(%rsp)
	xorl	%eax, %eax
	movl	$1752, %r8d             # imm = 0x6D8
.LBB0_6:                                # =>This Inner Loop Header: Depth=1
	movswl	-72(%rsp,%rax,2), %r9d
	movl	%r9d, %edx
	sarl	$10, %edx
	imull	$-9576, %r9d, %ecx      # imm = 0xDA98
	movswl	%cx, %ecx
	imull	$1752, %ecx, %ecx       # imm = 0x6D8
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	1297(%rsi,%rax), %ecx
	addl	%ecx, %edx
	movswl	%dx, %ebx
	shrl	$15, %ebx
	andl	%r8d, %ebx
	addl	%edx, %ebx
	shll	$5, %r9d
	movswl	%bx, %edx
	subl	%edx, %ecx
	shrl	$3, %ecx
	addl	%r9d, %ecx
	imull	$-1197, %ecx, %ecx      # imm = 0xFB53
	leal	-1752(%rcx), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	andl	%r8d, %ebx
	addl	%ebx, %ecx
	addl	$-1752, %ecx            # imm = 0xF928
	movw	%dx, -16(%rsp,%rax,4)
	movw	%cx, -14(%rsp,%rax,4)
	decq	%rax
	cmpq	$-13, %rax
	jne	.LBB0_6
# %bb.7:
	movswl	-12(%rsp), %eax
	imull	$658, %eax, %ecx        # imm = 0x292
	shrl	$16, %ecx
	imull	$-1566, %eax, %edx      # imm = 0xF9E2
	movswl	%dx, %edx
	imull	$10713, %edx, %edx      # imm = 0x29D9
	shrl	$16, %edx
	subl	%edx, %ecx
	movzbl	1284(%rsi), %r8d
	addl	%r8d, %ecx
	movswl	%cx, %ebx
	shrl	$15, %ebx
	movl	$10713, %edx            # imm = 0x29D9
	andl	%ebx, %edx
	addl	%ecx, %edx
	shll	$8, %eax
	orl	%r8d, %eax
	subl	%edx, %eax
	imull	$-14743, %eax, %eax     # imm = 0xC669
	leal	-131(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$131, %ecx
	addl	%ecx, %eax
	addl	$-131, %eax
	movw	%dx, 104(%rsp)
	vpbroadcastw	.LCPI0_0(%rip), %ymm1 # ymm1 = [658,658,658,658,658,658,658,658,658,658,658,658,658,658,658,658]
	movw	%ax, 106(%rsp)
	vpbroadcastw	.LCPI0_1(%rip), %ymm2 # ymm2 = [63970,63970,63970,63970,63970,63970,63970,63970,63970,63970,63970,63970,63970,63970,63970,63970]
	movl	$10, %eax
	vpbroadcastw	.LCPI0_2(%rip), %ymm3 # ymm3 = [10713,10713,10713,10713,10713,10713,10713,10713,10713,10713,10713,10713,10713,10713,10713,10713]
	vmovdqa	.LCPI0_3(%rip), %ymm0   # ymm0 = [255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0]
	vpbroadcastw	.LCPI0_4(%rip), %ymm4 # ymm4 = [50793,50793,50793,50793,50793,50793,50793,50793,50793,50793,50793,50793,50793,50793,50793,50793]
.LBB0_8:                                # =>This Inner Loop Header: Depth=1
	vmovdqu	-64(%rsp,%rax,2), %ymm5
	vmovdqu	1232(%rsi,%rax,2), %ymm6
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
	movswl	106(%rsp), %r8d
	imull	$280, %r8d, %edx        # imm = 0x118
	shrl	$16, %edx
	imull	$-10131, %r8d, %ecx     # imm = 0xD86D
	movswl	%cx, %ecx
	imull	$1656, %ecx, %ecx       # imm = 0x678
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	1231(%rsi), %r9d
	addl	%r9d, %edx
	movswl	%dx, %ebx
	imull	$280, %ebx, %ebx        # imm = 0x118
	shrl	$16, %ebx
	imull	$-10131, %edx, %edx     # imm = 0xD86D
	movswl	%dx, %edx
	imull	$1656, %edx, %edx       # imm = 0x678
	shrl	$16, %edx
	subl	%edx, %ebx
	movzbl	1230(%rsi), %edx
	addl	%edx, %ebx
	movswl	%bx, %eax
	shrl	$15, %eax
	movl	$1656, %ecx             # imm = 0x678
	andl	%eax, %ecx
	addl	%ebx, %ecx
	shll	$13, %r8d
	shll	$5, %r9d
	orl	%r8d, %r9d
	movswl	%cx, %eax
	subl	%eax, %edx
	shrl	$3, %edx
	addl	%r9d, %edx
	imull	$1583, %edx, %ecx       # imm = 0x62F
	leal	-5167(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$5167, %edx             # imm = 0x142F
	addl	%edx, %ecx
	addl	$-5167, %ecx            # imm = 0xEBD1
	vpbroadcastw	.LCPI0_5(%rip), %ymm1 # ymm1 = [280,280,280,280,280,280,280,280,280,280,280,280,280,280,280,280]
	movw	%ax, 324(%rsp)
	vpbroadcastw	.LCPI0_6(%rip), %ymm2 # ymm2 = [55405,55405,55405,55405,55405,55405,55405,55405,55405,55405,55405,55405,55405,55405,55405,55405]
	movw	%cx, 326(%rsp)
	vpbroadcastw	.LCPI0_7(%rip), %ymm3 # ymm3 = [1656,1656,1656,1656,1656,1656,1656,1656,1656,1656,1656,1656,1656,1656,1656,1656]
	movl	$37, %eax
	vpbroadcastw	.LCPI0_8(%rip), %ymm4 # ymm4 = [1583,1583,1583,1583,1583,1583,1583,1583,1583,1583,1583,1583,1583,1583,1583,1583]
.LBB0_11:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	(%rsp,%rax,2), %ymm5
	vpmovzxbw	1177(%rsi,%rax), %ymm6 # ymm6 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm1, %ymm5, %ymm7
	vpmullw	%ymm2, %ymm5, %ymm8
	vpmulhw	%ymm3, %ymm8, %ymm8
	vpsubw	%ymm8, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm7
	vpsraw	$15, %ymm7, %ymm8
	vpand	%ymm3, %ymm8, %ymm8
	vpaddw	%ymm7, %ymm8, %ymm7
	vpsllw	$5, %ymm5, %ymm5
	vpsubw	%ymm7, %ymm6, %ymm6
	vpsraw	$3, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm3, %ymm6
	vpandn	%ymm3, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm7, %ymm6 # ymm6 = ymm7[0],ymm5[0],ymm7[1],ymm5[1],ymm7[2],ymm5[2],ymm7[3],ymm5[3],ymm7[8],ymm5[8],ymm7[9],ymm5[9],ymm7[10],ymm5[10],ymm7[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm7, %ymm5 # ymm5 = ymm7[4],ymm5[4],ymm7[5],ymm5[5],ymm7[6],ymm5[6],ymm7[7],ymm5[7],ymm7[12],ymm5[12],ymm7[13],ymm5[13],ymm7[14],ymm5[14],ymm7[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm6, %ymm7
	vmovdqu	%ymm7, 112(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 144(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_13
# %bb.12:                               #   in Loop: Header=BB0_11 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_11
.LBB0_13:
	vpbroadcastw	.LCPI0_9(%rip), %ymm1 # ymm1 = [295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295]
	vpbroadcastw	.LCPI0_10(%rip), %ymm2 # ymm2 = [39765,39765,39765,39765,39765,39765,39765,39765,39765,39765,39765,39765,39765,39765,39765,39765]
	movzwl	326(%rsp), %eax
	vpbroadcastw	.LCPI0_11(%rip), %ymm3 # ymm3 = [651,651,651,651,651,651,651,651,651,651,651,651,651,651,651,651]
	movw	%ax, 764(%rsp)
	vpbroadcastw	.LCPI0_12(%rip), %ymm4 # ymm4 = [64885,64885,64885,64885,64885,64885,64885,64885,64885,64885,64885,64885,64885,64885,64885,64885]
	movl	$91, %eax
	vpbroadcastw	.LCPI0_13(%rip), %ymm5 # ymm5 = [54563,54563,54563,54563,54563,54563,54563,54563,54563,54563,54563,54563,54563,54563,54563,54563]
.LBB0_14:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	112(%rsp,%rax,2), %ymm6
	vpmovzxbw	1070(%rsi,%rax), %ymm7 # ymm7 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
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
	vpsllw	$8, %ymm6, %ymm6
	vpor	%ymm7, %ymm6, %ymm6
	vpsubw	%ymm8, %ymm6, %ymm6
	vpmullw	%ymm5, %ymm6, %ymm6
	vpcmpgtw	%ymm6, %ymm3, %ymm7
	vpandn	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpunpcklwd	%ymm6, %ymm8, %ymm7 # ymm7 = ymm8[0],ymm6[0],ymm8[1],ymm6[1],ymm8[2],ymm6[2],ymm8[3],ymm6[3],ymm8[8],ymm6[8],ymm8[9],ymm6[9],ymm8[10],ymm6[10],ymm8[11],ymm6[11]
	vpunpckhwd	%ymm6, %ymm8, %ymm6 # ymm6 = ymm8[4],ymm6[4],ymm8[5],ymm6[5],ymm8[6],ymm6[6],ymm8[7],ymm6[7],ymm8[12],ymm6[12],ymm8[13],ymm6[13],ymm8[14],ymm6[14],ymm8[15],ymm6[15]
	vinserti128	$1, %xmm6, %ymm7, %ymm8
	vmovdqu	%ymm8, 336(%rsp,%rax,4)
	vperm2i128	$49, %ymm6, %ymm7, %ymm6 # ymm6 = ymm7[2,3],ymm6[2,3]
	vmovdqu	%ymm6, 368(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_16
# %bb.15:                               #   in Loop: Header=BB0_14 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_14
.LBB0_16:
	vpbroadcastw	.LCPI0_14(%rip), %ymm1 # ymm1 = [65384,65384,65384,65384,65384,65384,65384,65384,65384,65384,65384,65384,65384,65384,65384,65384]
	vpbroadcastw	.LCPI0_15(%rip), %ymm2 # ymm2 = [24415,24415,24415,24415,24415,24415,24415,24415,24415,24415,24415,24415,24415,24415,24415,24415]
	movzwl	764(%rsp), %eax
	vpbroadcastw	.LCPI0_16(%rip), %ymm3 # ymm3 = [408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408]
	movw	%ax, 1624(%rsp)
	vpbroadcastw	.LCPI0_17(%rip), %ymm4 # ymm4 = [65128,65128,65128,65128,65128,65128,65128,65128,65128,65128,65128,65128,65128,65128,65128,65128]
	movl	$198, %eax
	vpbroadcastw	.LCPI0_18(%rip), %ymm5 # ymm5 = [64251,64251,64251,64251,64251,64251,64251,64251,64251,64251,64251,64251,64251,64251,64251,64251]
.LBB0_17:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	336(%rsp,%rax,2), %ymm6
	vpmovzxbw	856(%rsi,%rax), %ymm7 # ymm7 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
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
	vpsllw	$5, %ymm6, %ymm6
	vpsubw	%ymm8, %ymm7, %ymm7
	vpsraw	$3, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm6
	vpmullw	%ymm5, %ymm6, %ymm6
	vpcmpgtw	%ymm6, %ymm3, %ymm7
	vpandn	%ymm3, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpunpcklwd	%ymm6, %ymm8, %ymm7 # ymm7 = ymm8[0],ymm6[0],ymm8[1],ymm6[1],ymm8[2],ymm6[2],ymm8[3],ymm6[3],ymm8[8],ymm6[8],ymm8[9],ymm6[9],ymm8[10],ymm6[10],ymm8[11],ymm6[11]
	vpunpckhwd	%ymm6, %ymm8, %ymm6 # ymm6 = ymm8[4],ymm6[4],ymm8[5],ymm6[5],ymm8[6],ymm6[6],ymm8[7],ymm6[7],ymm8[12],ymm6[12],ymm8[13],ymm6[13],ymm8[14],ymm6[14],ymm8[15],ymm6[15]
	vinserti128	$1, %xmm6, %ymm7, %ymm8
	vmovdqu	%ymm8, 768(%rsp,%rax,4)
	vperm2i128	$49, %ymm6, %ymm7, %ymm6 # ymm6 = ymm7[2,3],ymm6[2,3]
	vmovdqu	%ymm6, 800(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_19
# %bb.18:                               #   in Loop: Header=BB0_17 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_17
.LBB0_19:
	movl	$-2583, %eax            # imm = 0xF5E9
	addl	1624(%rsp), %eax
	vpbroadcastw	.LCPI0_19(%rip), %ymm1 # ymm1 = [65503,65503,65503,65503,65503,65503,65503,65503,65503,65503,65503,65503,65503,65503,65503,65503]
	vpbroadcastw	.LCPI0_20(%rip), %ymm2 # ymm2 = [62289,62289,62289,62289,62289,62289,62289,62289,62289,62289,62289,62289,62289,62289,62289,62289]
	vpbroadcastw	.LCPI0_21(%rip), %ymm3 # ymm3 = [5167,5167,5167,5167,5167,5167,5167,5167,5167,5167,5167,5167,5167,5167,5167,5167]
	movw	%ax, 1712(%rdi)
	vpbroadcastw	.LCPI0_22(%rip), %ymm4 # ymm4 = [45775,45775,45775,45775,45775,45775,45775,45775,45775,45775,45775,45775,45775,45775,45775,45775]
	movl	$412, %eax              # imm = 0x19C
	vpbroadcastw	.LCPI0_23(%rip), %ymm5 # ymm5 = [62953,62953,62953,62953,62953,62953,62953,62953,62953,62953,62953,62953,62953,62953,62953,62953]
.LBB0_20:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	768(%rsp,%rax,2), %ymm6
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
	addq	$1632, %rsp             # imm = 0x660
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	vzeroupper
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_decode_857x5167), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_decode_857x5167)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
