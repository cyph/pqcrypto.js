	.text
	.file	"decode.c"
	.section	.rodata,"a",@progbits
	.p2align	1               # -- Begin function CRYPTO_NAMESPACE(crypto_decode_653x4621)
.LCPI0_0:
	.short	65068                   # 0xfe2c
.LCPI0_1:
	.short	56142                   # 0xdb4e
.LCPI0_2:
	.short	1786                    # 0x6fa
.LCPI0_3:
	.short	52693                   # 0xcdd5
.LCPI0_4:
	.short	248                     # 0xf8
.LCPI0_5:
	.short	40718                   # 0x9f0e
.LCPI0_6:
	.short	676                     # 0x2a4
.LCPI0_7:
	.short	41881                   # 0xa399
.LCPI0_8:
	.short	65472                   # 0xffc0
.LCPI0_9:
	.short	25206                   # 0x6276
.LCPI0_10:
	.short	416                     # 0x1a0
.LCPI0_11:
	.short	65120                   # 0xfe60
.LCPI0_12:
	.short	20165                   # 0x4ec5
.LCPI0_13:
	.short	65488                   # 0xffd0
.LCPI0_14:
	.short	14072                   # 0x36f8
.LCPI0_15:
	.short	326                     # 0x146
.LCPI0_16:
	.short	65210                   # 0xfeba
.LCPI0_17:
	.short	45835                   # 0xb30b
.LCPI0_18:
	.short	63901                   # 0xf99d
.LCPI0_19:
	.short	61905                   # 0xf1d1
.LCPI0_20:
	.short	4621                    # 0x120d
.LCPI0_22:
	.short	36037                   # 0x8cc5
.LCPI0_23:
	.short	63226                   # 0xf6fa
	.section	.rodata.cst32,"aM",@progbits,32
	.p2align	5
.LCPI0_21:
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
	.globl	CRYPTO_NAMESPACE(crypto_decode_653x4621)
	.type	CRYPTO_NAMESPACE(crypto_decode_653x4621),@function
CRYPTO_NAMESPACE(crypto_decode_653x4621): # @CRYPTO_NAMESPACE(crypto_decode_653x4621)
	.cfi_startproc
# %bb.0:
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$1248, %rsp             # imm = 0x4E0
	.cfi_def_cfa_offset 1264
	.cfi_offset %rbx, -16
	movzbl	993(%rsi), %ecx
	leal	-172(%rcx), %edx
	shrl	$15, %edx
	movl	$86, %r9d
	andl	%r9d, %edx
	addl	%edx, %ecx
	addl	$-172, %ecx
	movl	%ecx, %edx
	shrl	$15, %edx
	andl	%r9d, %edx
	addl	%ecx, %edx
	imull	$396, %edx, %eax        # imm = 0x18C
	shrl	$16, %eax
	imull	$-1316749312, %edx, %ecx # imm = 0xB1840000
	sarl	$16, %ecx
	imull	$835, %ecx, %ecx        # imm = 0x343
	shrl	$16, %ecx
	subl	%ecx, %eax
	movzbl	992(%rsi), %ecx
	addl	%ecx, %eax
	movswl	%ax, %edx
	imull	$396, %edx, %edx        # imm = 0x18C
	shrl	$16, %edx
	imull	$-20092, %eax, %eax     # imm = 0xB184
	cwtl
	imull	$835, %eax, %eax        # imm = 0x343
	shrl	$16, %eax
	subl	%eax, %edx
	movzbl	991(%rsi), %r8d
	addl	%r8d, %edx
	movswl	%dx, %r10d
	shrl	$15, %r10d
	movl	$835, %eax              # imm = 0x343
	andl	%r10d, %eax
	addl	%edx, %eax
	shll	$8, %ecx
	orl	%r8d, %ecx
	subl	%eax, %ecx
	imull	$8555, %ecx, %ecx       # imm = 0x216B
	leal	-6708(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$6708, %edx             # imm = 0x1A34
	leal	(%rdx,%rcx), %r8d
	addl	$-6708, %r8d            # imm = 0xE5CC
	movw	%r8w, -110(%rsp)
	movswl	%ax, %r10d
	imull	$3088, %r10d, %edx      # imm = 0xC10
	shrl	$16, %edx
	imull	$-2268, %eax, %eax      # imm = 0xF724
	cwtl
	imull	$7396, %eax, %eax       # imm = 0x1CE4
	shrl	$16, %eax
	subl	%eax, %edx
	movzbl	990(%rsi), %r11d
	addl	%r11d, %edx
	movswl	%dx, %eax
	imull	$3088, %eax, %eax       # imm = 0xC10
	shrl	$16, %eax
	imull	$-2268, %edx, %edx      # imm = 0xF724
	movswl	%dx, %edx
	imull	$7396, %edx, %edx       # imm = 0x1CE4
	shrl	$16, %edx
	subl	%edx, %eax
	movzbl	989(%rsi), %edx
	addl	%edx, %eax
	movswl	%ax, %ecx
	shrl	$15, %ecx
	movl	$7396, %ebx             # imm = 0x1CE4
	andl	%ebx, %ecx
	addl	%eax, %ecx
	shll	$14, %r10d
	shll	$6, %r11d
	orl	%r10d, %r11d
	movswl	%cx, %eax
	subl	%eax, %edx
	shrl	$2, %edx
	addl	%r11d, %edx
	imull	$-18679, %edx, %ecx     # imm = 0xB709
	leal	-7396(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%ebx, %edx
	addl	%ecx, %edx
	addl	$-7396, %edx            # imm = 0xE31C
	movw	%ax, -114(%rsp)
	movw	%dx, -112(%rsp)
	movswl	%r8w, %eax
	movl	%eax, %ecx
	sarl	$14, %ecx
	imull	$-762, %r8d, %ebx       # imm = 0xFD06
	movswl	%bx, %ebx
	imull	$86, %ebx, %ebx
	shrl	$16, %ebx
	subl	%ebx, %ecx
	movswl	%cx, %ebx
	shrl	$15, %ebx
	andl	%r9d, %ebx
	addl	%ecx, %ebx
	movswl	%bx, %ecx
	subl	%ecx, %eax
	shrl	%eax
	imull	$-16765, %eax, %eax     # imm = 0xBE83
	leal	-78(%rax), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	andl	$78, %ebx
	addl	%ebx, %eax
	addl	$-78, %eax
	movw	%cx, -100(%rsp)
	movw	%ax, -98(%rsp)
	xorl	%ecx, %ecx
.LBB0_1:                                # =>This Inner Loop Header: Depth=1
	movswl	%dx, %ebx
	movl	%ebx, %eax
	sarl	$14, %eax
	imull	$-762, %edx, %edx       # imm = 0xFD06
	movswl	%dx, %edx
	imull	$86, %edx, %edx
	shrl	$16, %edx
	subl	%edx, %eax
	movswl	%ax, %edx
	shrl	$15, %edx
	andl	%r9d, %edx
	addl	%eax, %edx
	movswl	%dx, %eax
	subl	%eax, %ebx
	shrl	%ebx
	imull	$-16765, %ebx, %edx     # imm = 0xBE83
	leal	-86(%rdx), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	andl	%r9d, %ebx
	addl	%ebx, %edx
	addl	$-86, %edx
	movw	%ax, -104(%rsp,%rcx,4)
	movw	%dx, -102(%rsp,%rcx,4)
	cmpq	$-1, %rcx
	je	.LBB0_3
# %bb.2:                                #   in Loop: Header=BB0_1 Depth=1
	movzwl	-114(%rsp,%rcx,2), %edx
	decq	%rcx
	jmp	.LBB0_1
.LBB0_3:
	movzwl	-98(%rsp), %eax
	movw	%ax, -76(%rsp)
	xorl	%eax, %eax
	movl	$2370, %r8d             # imm = 0x942
.LBB0_4:                                # =>This Inner Loop Header: Depth=1
	movswl	-100(%rsp,%rax,2), %r9d
	imull	$-14, %r9d, %ebx
	shrl	$16, %ebx
	imull	$-7079, %r9d, %ecx      # imm = 0xE459
	movswl	%cx, %ecx
	imull	$2370, %ecx, %ecx       # imm = 0x942
	shrl	$16, %ecx
	subl	%ecx, %ebx
	movzbl	988(%rsi,%rax,2), %r10d
	addl	%r10d, %ebx
	movswl	%bx, %edx
	imull	$-14, %edx, %edx
	shrl	$16, %edx
	imull	$-7079, %ebx, %ebx      # imm = 0xE459
	movswl	%bx, %ebx
	imull	$2370, %ebx, %ebx       # imm = 0x942
	shrl	$16, %ebx
	subl	%ebx, %edx
	movzbl	987(%rsi,%rax,2), %ebx
	addl	%ebx, %edx
	movswl	%dx, %ecx
	shrl	$15, %ecx
	andl	%r8d, %ecx
	addl	%edx, %ecx
	shll	$15, %r9d
	shll	$7, %r10d
	orl	%r9d, %r10d
	movswl	%cx, %ecx
	subl	%ecx, %ebx
	shrl	%ebx
	addl	%r10d, %ebx
	imull	$-8351, %ebx, %edx      # imm = 0xDF61
	leal	-2370(%rdx), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	andl	%r8d, %ebx
	addl	%ebx, %edx
	addl	$-2370, %edx            # imm = 0xF6BE
	movw	%cx, -80(%rsp,%rax,4)
	movw	%dx, -78(%rsp,%rax,4)
	decq	%rax
	cmpq	$-5, %rax
	jne	.LBB0_4
# %bb.5:
	movzwl	-76(%rsp), %eax
	movw	%ax, -24(%rsp)
	xorl	%eax, %eax
	movl	$12461, %r8d            # imm = 0x30AD
.LBB0_6:                                # =>This Inner Loop Header: Depth=1
	movswl	-78(%rsp,%rax,2), %edx
	imull	$4710, %edx, %ebx       # imm = 0x1266
	shrl	$16, %ebx
	imull	$-1346, %edx, %edx      # imm = 0xFABE
	movswl	%dx, %edx
	imull	$12461, %edx, %edx      # imm = 0x30AD
	shrl	$16, %edx
	subl	%edx, %ebx
	movzbl	978(%rsi,%rax,2), %edx
	addl	%edx, %ebx
	movswl	%bx, %ecx
	imull	$4710, %ecx, %ecx       # imm = 0x1266
	shrl	$16, %ecx
	imull	$-1346, %ebx, %ebx      # imm = 0xFABE
	movswl	%bx, %ebx
	imull	$12461, %ebx, %ebx      # imm = 0x30AD
	shrl	$16, %ebx
	subl	%ebx, %ecx
	movzbl	977(%rsi,%rax,2), %r9d
	addl	%r9d, %ecx
	movswl	%cx, %ebx
	shrl	$15, %ebx
	andl	%r8d, %ebx
	addl	%ecx, %ebx
	shll	$8, %edx
	orl	%r9d, %edx
	subl	%ebx, %edx
	imull	$-19675, %edx, %ecx     # imm = 0xB325
	leal	-12461(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	%r8d, %edx
	addl	%edx, %ecx
	addl	$-12461, %ecx           # imm = 0xCF53
	movw	%bx, -28(%rsp,%rax,4)
	movw	%cx, -26(%rsp,%rax,4)
	decq	%rax
	cmpq	$-10, %rax
	jne	.LBB0_6
# %bb.7:
	vpbroadcastw	.LCPI0_0(%rip), %ymm0 # ymm0 = [65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068,65068]
	movzwl	-24(%rsp), %eax
	vpbroadcastw	.LCPI0_1(%rip), %ymm1 # ymm1 = [56142,56142,56142,56142,56142,56142,56142,56142,56142,56142,56142,56142,56142,56142,56142,56142]
	movw	%ax, 64(%rsp)
	vpbroadcastw	.LCPI0_2(%rip), %ymm2 # ymm2 = [1786,1786,1786,1786,1786,1786,1786,1786,1786,1786,1786,1786,1786,1786,1786,1786]
	movl	$4, %eax
	vpbroadcastw	.LCPI0_3(%rip), %ymm3 # ymm3 = [52693,52693,52693,52693,52693,52693,52693,52693,52693,52693,52693,52693,52693,52693,52693,52693]
.LBB0_8:                                # =>This Inner Loop Header: Depth=1
	vmovdqu	-64(%rsp,%rax,2), %ymm4
	vpmovzxbw	939(%rsi,%rax), %ymm5 # ymm5 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm0, %ymm4, %ymm6
	vpmullw	%ymm1, %ymm4, %ymm7
	vpmulhw	%ymm2, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm6
	vpsraw	$15, %ymm6, %ymm7
	vpand	%ymm2, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm6
	vpsllw	$7, %ymm4, %ymm4
	vpsubw	%ymm6, %ymm5, %ymm5
	vpsraw	$1, %ymm5, %ymm5
	vpaddw	%ymm4, %ymm5, %ymm4
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
	movswl	64(%rsp), %r8d
	imull	$248, %r8d, %edx
	shrl	$16, %edx
	imull	$-24818, %r8d, %ecx     # imm = 0x9F0E
	movswl	%cx, %ecx
	imull	$676, %ecx, %ecx        # imm = 0x2A4
	shrl	$16, %ecx
	subl	%ecx, %edx
	movzbl	938(%rsi), %r9d
	addl	%r9d, %edx
	movswl	%dx, %ebx
	imull	$248, %ebx, %ebx
	shrl	$16, %ebx
	imull	$-24818, %edx, %edx     # imm = 0x9F0E
	movswl	%dx, %edx
	imull	$676, %edx, %edx        # imm = 0x2A4
	shrl	$16, %edx
	subl	%edx, %ebx
	movzbl	937(%rsi), %edx
	addl	%edx, %ebx
	movswl	%bx, %eax
	shrl	$15, %eax
	movl	$676, %ecx              # imm = 0x2A4
	andl	%eax, %ecx
	addl	%ebx, %ecx
	shll	$14, %r8d
	shll	$6, %r9d
	orl	%r8d, %r9d
	movswl	%cx, %eax
	subl	%eax, %edx
	shrl	$2, %edx
	addl	%r9d, %edx
	imull	$-23655, %edx, %ecx     # imm = 0xA399
	leal	-7510(%rcx), %edx
	movswl	%dx, %edx
	shrl	$15, %edx
	andl	$7510, %edx             # imm = 0x1D56
	addl	%edx, %ecx
	addl	$-7510, %ecx            # imm = 0xE2AA
	vpbroadcastw	.LCPI0_4(%rip), %ymm0 # ymm0 = [248,248,248,248,248,248,248,248,248,248,248,248,248,248,248,248]
	movw	%ax, 240(%rsp)
	vpbroadcastw	.LCPI0_5(%rip), %ymm1 # ymm1 = [40718,40718,40718,40718,40718,40718,40718,40718,40718,40718,40718,40718,40718,40718,40718,40718]
	movw	%cx, 242(%rsp)
	vpbroadcastw	.LCPI0_6(%rip), %ymm2 # ymm2 = [676,676,676,676,676,676,676,676,676,676,676,676,676,676,676,676]
	movl	$24, %eax
	vpbroadcastw	.LCPI0_7(%rip), %ymm3 # ymm3 = [41881,41881,41881,41881,41881,41881,41881,41881,41881,41881,41881,41881,41881,41881,41881,41881]
.LBB0_11:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	-16(%rsp,%rax,2), %ymm4
	vpmovzxbw	897(%rsi,%rax), %ymm5 # ymm5 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
	vpmulhw	%ymm0, %ymm4, %ymm6
	vpmullw	%ymm1, %ymm4, %ymm7
	vpmulhw	%ymm2, %ymm7, %ymm7
	vpsubw	%ymm7, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm6
	vpsraw	$15, %ymm6, %ymm7
	vpand	%ymm2, %ymm7, %ymm7
	vpaddw	%ymm6, %ymm7, %ymm6
	vpsllw	$6, %ymm4, %ymm4
	vpsubw	%ymm6, %ymm5, %ymm5
	vpsraw	$2, %ymm5, %ymm5
	vpaddw	%ymm4, %ymm5, %ymm4
	vpmullw	%ymm3, %ymm4, %ymm4
	vpcmpgtw	%ymm4, %ymm2, %ymm5
	vpandn	%ymm2, %ymm5, %ymm5
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
	movswl	242(%rsp), %eax
	movl	%eax, %edx
	shll	$6, %edx
	negl	%edx
	shrl	$16, %edx
	imull	$25206, %eax, %ecx      # imm = 0x6276
	movswl	%cx, %ecx
	imull	$416, %ecx, %ebx        # imm = 0x1A0
	shrl	$16, %ebx
	movzbl	896(%rsi), %ecx
	addl	%ecx, %edx
	subl	%ebx, %edx
	leal	-416(%rdx), %ebx
	movswl	%bx, %ebx
	shrl	$15, %ebx
	movl	$416, %r8d              # imm = 0x1A0
	andl	%r8d, %ebx
	addl	%ebx, %edx
	addl	$-416, %edx             # imm = 0xFE60
	movswl	%dx, %ebx
	shrl	$15, %ebx
	andl	%r8d, %ebx
	addl	%edx, %ebx
	movswl	%bx, %edx
	subl	%edx, %ecx
	shrl	$5, %ecx
	leal	(%rcx,%rax,8), %eax
	imull	$20165, %eax, %eax      # imm = 0x4EC5
	leal	-4621(%rax), %ecx
	movswl	%cx, %ecx
	shrl	$15, %ecx
	andl	$4621, %ecx             # imm = 0x120D
	vpbroadcastw	.LCPI0_8(%rip), %ymm0 # ymm0 = [65472,65472,65472,65472,65472,65472,65472,65472,65472,65472,65472,65472,65472,65472,65472,65472]
	addl	%ecx, %eax
	addl	$-4621, %eax            # imm = 0xEDF3
	vpbroadcastw	.LCPI0_9(%rip), %ymm1 # ymm1 = [25206,25206,25206,25206,25206,25206,25206,25206,25206,25206,25206,25206,25206,25206,25206,25206]
	movw	%dx, 580(%rsp)
	vpbroadcastw	.LCPI0_10(%rip), %ymm2 # ymm2 = [416,416,416,416,416,416,416,416,416,416,416,416,416,416,416,416]
	movw	%ax, 582(%rsp)
	vpbroadcastw	.LCPI0_11(%rip), %ymm3 # ymm3 = [65120,65120,65120,65120,65120,65120,65120,65120,65120,65120,65120,65120,65120,65120,65120,65120]
	movl	$65, %eax
	vpbroadcastw	.LCPI0_12(%rip), %ymm4 # ymm4 = [20165,20165,20165,20165,20165,20165,20165,20165,20165,20165,20165,20165,20165,20165,20165,20165]
.LBB0_14:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	80(%rsp,%rax,2), %ymm5
	vpmovzxbw	815(%rsi,%rax), %ymm6 # ymm6 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
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
	vpsllw	$3, %ymm5, %ymm5
	vpsubw	%ymm7, %ymm6, %ymm6
	vpsraw	$5, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm2, %ymm6
	vpandn	%ymm2, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm7, %ymm6 # ymm6 = ymm7[0],ymm5[0],ymm7[1],ymm5[1],ymm7[2],ymm5[2],ymm7[3],ymm5[3],ymm7[8],ymm5[8],ymm7[9],ymm5[9],ymm7[10],ymm5[10],ymm7[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm7, %ymm5 # ymm5 = ymm7[4],ymm5[4],ymm7[5],ymm5[5],ymm7[6],ymm5[6],ymm7[7],ymm5[7],ymm7[12],ymm5[12],ymm7[13],ymm5[13],ymm7[14],ymm5[14],ymm7[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm6, %ymm7
	vmovdqu	%ymm7, 256(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 288(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_16
# %bb.15:                               #   in Loop: Header=BB0_14 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_14
.LBB0_16:
	vpbroadcastw	.LCPI0_13(%rip), %ymm0 # ymm0 = [65488,65488,65488,65488,65488,65488,65488,65488,65488,65488,65488,65488,65488,65488,65488,65488]
	vpbroadcastw	.LCPI0_14(%rip), %ymm1 # ymm1 = [14072,14072,14072,14072,14072,14072,14072,14072,14072,14072,14072,14072,14072,14072,14072,14072]
	movzwl	582(%rsp), %eax
	vpbroadcastw	.LCPI0_15(%rip), %ymm2 # ymm2 = [326,326,326,326,326,326,326,326,326,326,326,326,326,326,326,326]
	movw	%ax, 1244(%rsp)
	vpbroadcastw	.LCPI0_16(%rip), %ymm3 # ymm3 = [65210,65210,65210,65210,65210,65210,65210,65210,65210,65210,65210,65210,65210,65210,65210,65210]
	movl	$147, %eax
	vpbroadcastw	.LCPI0_17(%rip), %ymm4 # ymm4 = [45835,45835,45835,45835,45835,45835,45835,45835,45835,45835,45835,45835,45835,45835,45835,45835]
.LBB0_17:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	256(%rsp,%rax,2), %ymm5
	vpmovzxbw	652(%rsi,%rax), %ymm6 # ymm6 = mem[0],zero,mem[1],zero,mem[2],zero,mem[3],zero,mem[4],zero,mem[5],zero,mem[6],zero,mem[7],zero,mem[8],zero,mem[9],zero,mem[10],zero,mem[11],zero,mem[12],zero,mem[13],zero,mem[14],zero,mem[15],zero
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
	vpsllw	$7, %ymm5, %ymm5
	vpsubw	%ymm7, %ymm6, %ymm6
	vpsraw	$1, %ymm6, %ymm6
	vpaddw	%ymm5, %ymm6, %ymm5
	vpmullw	%ymm4, %ymm5, %ymm5
	vpcmpgtw	%ymm5, %ymm2, %ymm6
	vpandn	%ymm2, %ymm6, %ymm6
	vpsubw	%ymm6, %ymm5, %ymm5
	vpunpcklwd	%ymm5, %ymm7, %ymm6 # ymm6 = ymm7[0],ymm5[0],ymm7[1],ymm5[1],ymm7[2],ymm5[2],ymm7[3],ymm5[3],ymm7[8],ymm5[8],ymm7[9],ymm5[9],ymm7[10],ymm5[10],ymm7[11],ymm5[11]
	vpunpckhwd	%ymm5, %ymm7, %ymm5 # ymm5 = ymm7[4],ymm5[4],ymm7[5],ymm5[5],ymm7[6],ymm5[6],ymm7[7],ymm5[7],ymm7[12],ymm5[12],ymm7[13],ymm5[13],ymm7[14],ymm5[14],ymm7[15],ymm5[15]
	vinserti128	$1, %xmm5, %ymm6, %ymm7
	vmovdqu	%ymm7, 592(%rsp,%rax,4)
	vperm2i128	$49, %ymm5, %ymm6, %ymm5 # ymm5 = ymm6[2,3],ymm5[2,3]
	vmovdqu	%ymm5, 624(%rsp,%rax,4)
	testq	%rax, %rax
	je	.LBB0_19
# %bb.18:                               #   in Loop: Header=BB0_17 Depth=1
	decq	%rax
	andq	$-16, %rax
	jmp	.LBB0_17
.LBB0_19:
	movl	$-2310, %eax            # imm = 0xF6FA
	addl	1244(%rsp), %eax
	vpbroadcastw	.LCPI0_18(%rip), %ymm0 # ymm0 = [63901,63901,63901,63901,63901,63901,63901,63901,63901,63901,63901,63901,63901,63901,63901,63901]
	vpbroadcastw	.LCPI0_19(%rip), %ymm1 # ymm1 = [61905,61905,61905,61905,61905,61905,61905,61905,61905,61905,61905,61905,61905,61905,61905,61905]
	movw	%ax, 1304(%rdi)
	vpbroadcastw	.LCPI0_20(%rip), %ymm2 # ymm2 = [4621,4621,4621,4621,4621,4621,4621,4621,4621,4621,4621,4621,4621,4621,4621,4621]
	movl	$310, %eax              # imm = 0x136
	vpbroadcastw	.LCPI0_22(%rip), %ymm3 # ymm3 = [36037,36037,36037,36037,36037,36037,36037,36037,36037,36037,36037,36037,36037,36037,36037,36037]
	vmovdqa	.LCPI0_21(%rip), %ymm4  # ymm4 = [255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0,255,0]
	vpbroadcastw	.LCPI0_23(%rip), %ymm5 # ymm5 = [63226,63226,63226,63226,63226,63226,63226,63226,63226,63226,63226,63226,63226,63226,63226,63226]
.LBB0_20:                               # =>This Inner Loop Header: Depth=1
	vmovdqu	592(%rsp,%rax,2), %ymm6
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
	addq	$1248, %rsp             # imm = 0x4E0
	.cfi_def_cfa_offset 16
	popq	%rbx
	.cfi_def_cfa_offset 8
	vzeroupper
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_decode_653x4621), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_decode_653x4621)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
