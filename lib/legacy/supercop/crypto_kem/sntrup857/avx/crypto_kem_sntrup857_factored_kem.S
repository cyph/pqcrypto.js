	.file	"kem.c"
	.text
	.p2align 4
	.type	Short_random, @function
Short_random:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movl	$3428, %esi
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	pushq	%r12
	.cfi_offset 13, -24
	.cfi_offset 12, -32
	movq	%rdi, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$3456, %rsp
	.cfi_offset 3, -40
	movq	%rsp, %r13
	movq	%r13, %rdi
	movq	%r13, %rbx
	call	randombytes@PLT
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_857xint32)@PLT
	vmovdqa	.LC0(%rip), %ymm1
	leaq	1280(%rsp), %rdx
	movq	%r13, %rax
	.p2align 4,,10
	.p2align 3
.L2:
	vpand	(%rax), %ymm1, %ymm0
	addq	$32, %rax
	vmovdqa	%ymm0, -32(%rax)
	cmpq	%rax, %rdx
	jne	.L2
	vmovdqa	.LC1(%rip), %ymm2
	vmovdqa	.LC2(%rip), %ymm1
	movabsq	$-4294967298, %rax
	leaq	3400(%r13), %rdx
	andq	%rax, 1280(%rsp)
	leaq	1288(%rsp), %rax
	.p2align 4,,10
	.p2align 3
.L3:
	vpand	(%rax), %ymm2, %ymm0
	addq	$32, %rax
	vpor	%ymm1, %ymm0, %ymm0
	vmovdqu	%ymm0, -32(%rax)
	cmpq	%rdx, %rax
	jne	.L3
	movl	3416(%rsp), %eax
	vmovdqa	.LC3(%rip), %xmm0
	movl	$857, %esi
	movq	%r13, %rdi
	vpand	3400(%rsp), %xmm0, %xmm0
	vpor	.LC4(%rip), %xmm0, %xmm0
	andl	$-4, %eax
	vmovdqu	%xmm0, 3400(%rsp)
	orl	$1, %eax
	movl	%eax, 3416(%rsp)
	movl	3420(%rsp), %eax
	andl	$-4, %eax
	orl	$1, %eax
	movl	%eax, 3420(%rsp)
	movl	3424(%rsp), %eax
	andl	$-4, %eax
	orl	$1, %eax
	movl	%eax, 3424(%rsp)
	vzeroupper
	addq	$3328, %r13
	call	CRYPTO_NAMESPACE(crypto_sort_uint32)@PLT
	vmovdqa	.LC5(%rip), %ymm2
	movq	%r12, %rax
	vpcmpeqd	%ymm4, %ymm4, %ymm4
	vmovdqa	.LC6(%rip), %ymm3
	vmovdqa	.LC7(%rip), %ymm5
.L4:
	vpand	32(%rbx), %ymm2, %ymm1
	vpand	(%rbx), %ymm2, %ymm0
	subq	$-128, %rbx
	addq	$32, %rax
	vpand	-32(%rbx), %ymm2, %ymm6
	vpackusdw	%ymm1, %ymm0, %ymm0
	vpand	-64(%rbx), %ymm2, %ymm1
	vpermq	$216, %ymm0, %ymm0
	vpackusdw	%ymm6, %ymm1, %ymm1
	vpand	%ymm0, %ymm3, %ymm0
	vpermq	$216, %ymm1, %ymm1
	vpand	%ymm1, %ymm3, %ymm1
	vpackuswb	%ymm1, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpand	%ymm0, %ymm5, %ymm0
	vpaddb	%ymm4, %ymm0, %ymm0
	vmovdqu	%ymm0, -32(%rax)
	cmpq	%rbx, %r13
	jne	.L4
	movl	3392(%rsp), %eax
	vmovdqa	.LC8(%rip), %xmm1
	vpand	3344(%rsp), %xmm1, %xmm2
	vpand	3328(%rsp), %xmm1, %xmm0
	andl	$3, %eax
	decl	%eax
	vpackusdw	%xmm2, %xmm0, %xmm0
	vpand	3360(%rsp), %xmm1, %xmm2
	movb	%al, 848(%r12)
	movl	3396(%rsp), %eax
	vpand	3376(%rsp), %xmm1, %xmm1
	andl	$3, %eax
	decl	%eax
	vpackusdw	%xmm1, %xmm2, %xmm1
	vmovdqa	.LC9(%rip), %xmm2
	movb	%al, 849(%r12)
	movl	3400(%rsp), %eax
	vpand	%xmm0, %xmm2, %xmm0
	vpand	%xmm1, %xmm2, %xmm2
	vpcmpeqd	%xmm1, %xmm1, %xmm1
	andl	$3, %eax
	vpackuswb	%xmm2, %xmm0, %xmm0
	vpand	.LC10(%rip), %xmm0, %xmm0
	decl	%eax
	movb	%al, 850(%r12)
	movl	3404(%rsp), %eax
	vpaddb	%xmm1, %xmm0, %xmm0
	vmovdqu	%xmm0, 832(%r12)
	andl	$3, %eax
	decl	%eax
	movb	%al, 851(%r12)
	movl	3408(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 852(%r12)
	movl	3412(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 853(%r12)
	movl	3416(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 854(%r12)
	movl	3420(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 855(%r12)
	movl	3424(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 856(%r12)
	vzeroupper
	leaq	-24(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	Short_random, .-Short_random
	.p2align 4
	.type	Hide, @function
Hide:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	.cfi_offset 15, -24
	movq	%rdx, %r15
	pushq	%r14
	pushq	%r13
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	movq	%rsi, %r13
	movq	%rcx, %rsi
	pushq	%r12
	.cfi_offset 12, -48
	movq	%rdi, %r12
	leaq	1(%rdx), %rdi
	pushq	%rbx
	.cfi_offset 3, -56
	movq	%r9, %rbx
	andq	$-32, %rsp
	subq	$1824, %rsp
	movq	%rcx, 24(%rsp)
	leaq	96(%rsp), %r14
	movq	%r8, 16(%rsp)
	call	CRYPTO_NAMESPACE(crypto_encode_857x3)@PLT
	movq	16(%rsp), %r8
	movq	%r14, %rdi
	movq	%r8, %rsi
	call	CRYPTO_NAMESPACE(crypto_decode_857x5167)@PLT
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_857xint16)@PLT
	movq	24(%rsp), %rdx
	xorl	%ecx, %ecx
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup857)@PLT
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_857xint16)@PLT
	movq	%r14, %rsi
	movq	%r13, %rdi
	leaq	32(%rsp), %r14
	call	CRYPTO_NAMESPACE(crypto_encode_857x1723round)@PLT
	movb	$3, (%r15)
	movq	%r15, %rsi
	movq	%r14, %rdi
	movl	$216, %edx
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movq	%r12, %rax
	vmovdqa	32(%rsp), %xmm0
	vmovdqa	48(%rsp), %xmm1
	subq	%rbx, %rax
	addq	$32, %rax
	vmovdqu	%xmm0, 1(%r12)
	vmovdqu	%xmm1, 17(%r12)
	cmpq	$30, %rax
	jbe	.L13
	vmovdqu	(%rbx), %ymm4
	vmovdqu	%ymm4, 33(%r12)
	vzeroupper
.L12:
	movb	$2, (%r12)
	movq	%r12, %rsi
	movq	%r14, %rdi
	movl	$65, %edx
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	vmovdqa	32(%rsp), %xmm2
	vmovdqa	48(%rsp), %xmm3
	vmovdqu	%xmm2, 1152(%r13)
	vmovdqu	%xmm3, 1168(%r13)
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
.L13:
	.cfi_restore_state
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L11:
	movzbl	(%rbx,%rax), %edx
	movb	%dl, 33(%r12,%rax)
	incq	%rax
	cmpq	$32, %rax
	jne	.L11
	jmp	.L12
	.cfi_endproc
.LFE8:
	.size	Hide, .-Hide
	.p2align 4
	.globl	CRYPTO_NAMESPACE(keypair)
	.type	CRYPTO_NAMESPACE(keypair), @function
CRYPTO_NAMESPACE(keypair):
.LFB9:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	movq	%rdi, %r13
	pushq	%r12
	.cfi_offset 12, -48
	movq	%rsi, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$5216, %rsp
	.cfi_offset 3, -56
	leaq	896(%rsp), %rax
	leaq	1760(%rsp), %r15
	movq	%rax, 24(%rsp)
	leaq	32(%rsp), %r14
	leaq	5088(%rsp), %rbx
	.p2align 4,,10
	.p2align 3
.L21:
	movl	$3428, %esi
	movq	%r15, %rdi
	vzeroupper
	call	randombytes@PLT
	movq	%r15, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_857xint32)@PLT
	vmovdqa	.LC8(%rip), %xmm5
	movq	%r15, %rax
	vmovdqa	.LC6(%rip), %ymm4
	vmovdqa	.LC5(%rip), %ymm2
	vmovdqa	.LC12(%rip), %xmm6
	movq	%r14, %rdx
	vmovdqa	.LC11(%rip), %ymm3
	.p2align 4,,10
	.p2align 3
.L17:
	vpand	(%rax), %ymm3, %ymm0
	vpand	32(%rax), %ymm3, %ymm7
	subq	$-128, %rax
	addq	$32, %rdx
	vpand	-32(%rax), %ymm3, %ymm8
	vpslld	$1, %ymm0, %ymm1
	vpaddd	%ymm0, %ymm1, %ymm1
	vpslld	$1, %ymm7, %ymm0
	vpaddd	%ymm7, %ymm0, %ymm0
	vpsrld	$30, %ymm1, %ymm1
	vpand	-64(%rax), %ymm3, %ymm7
	vpsrld	$30, %ymm0, %ymm0
	vpand	%ymm1, %ymm2, %ymm1
	vpand	%ymm0, %ymm2, %ymm0
	vpackusdw	%ymm0, %ymm1, %ymm1
	vpslld	$1, %ymm7, %ymm0
	vpaddd	%ymm7, %ymm0, %ymm0
	vpslld	$1, %ymm8, %ymm7
	vpermq	$216, %ymm1, %ymm1
	vpaddd	%ymm8, %ymm7, %ymm7
	vpsrld	$30, %ymm0, %ymm0
	vpand	%ymm1, %ymm4, %ymm1
	vpsrld	$30, %ymm7, %ymm7
	vpand	%ymm0, %ymm2, %ymm0
	vpand	%ymm7, %ymm2, %ymm7
	vpackusdw	%ymm7, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpand	%ymm0, %ymm4, %ymm0
	vpackuswb	%ymm0, %ymm1, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpaddb	.LC13(%rip), %ymm0, %ymm0
	vmovdqa	%ymm0, -32(%rdx)
	cmpq	%rax, %rbx
	jne	.L17
	movl	5152(%rsp), %eax
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r14, %rsi
	vpand	5088(%rsp), %xmm6, %xmm0
	movq	%r15, %rdi
	vpand	5104(%rsp), %xmm6, %xmm7
	vpand	5136(%rsp), %xmm6, %xmm8
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	vpslld	$1, %xmm0, %xmm1
	shrl	$30, %eax
	vpaddd	%xmm0, %xmm1, %xmm1
	vpslld	$1, %xmm7, %xmm0
	vpsrld	$30, %xmm1, %xmm1
	decl	%eax
	movb	%al, 880(%rsp)
	vpaddd	%xmm7, %xmm0, %xmm0
	vpand	%xmm1, %xmm5, %xmm1
	movl	5156(%rsp), %eax
	vpand	5120(%rsp), %xmm6, %xmm7
	vpsrld	$30, %xmm0, %xmm0
	andl	$1073741823, %eax
	vpand	%xmm0, %xmm5, %xmm0
	leal	(%rax,%rax,2), %eax
	vpackusdw	%xmm0, %xmm1, %xmm1
	vpand	.LC9(%rip), %xmm1, %xmm1
	shrl	$30, %eax
	vpslld	$1, %xmm7, %xmm0
	decl	%eax
	vpaddd	%xmm7, %xmm0, %xmm0
	movb	%al, 881(%rsp)
	vpslld	$1, %xmm8, %xmm7
	vpsrld	$30, %xmm0, %xmm0
	movl	5160(%rsp), %eax
	vpaddd	%xmm8, %xmm7, %xmm7
	vpand	%xmm0, %xmm5, %xmm0
	andl	$1073741823, %eax
	vpsrld	$30, %xmm7, %xmm7
	leal	(%rax,%rax,2), %eax
	vpand	%xmm7, %xmm5, %xmm7
	shrl	$30, %eax
	vpackusdw	%xmm7, %xmm0, %xmm0
	vpand	.LC9(%rip), %xmm0, %xmm0
	decl	%eax
	movb	%al, 882(%rsp)
	movl	5164(%rsp), %eax
	vpackuswb	%xmm0, %xmm1, %xmm0
	vpaddb	.LC14(%rip), %xmm0, %xmm0
	andl	$1073741823, %eax
	vmovdqa	%xmm0, 864(%rsp)
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 883(%rsp)
	movl	5168(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 884(%rsp)
	movl	5172(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 885(%rsp)
	movl	5176(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 886(%rsp)
	movl	5180(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 887(%rsp)
	movl	5184(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 888(%rsp)
	vzeroupper
	call	CRYPTO_NAMESPACE(crypto_core_inv3sntrup857)@PLT
	movq	24(%rsp), %rdi
	movl	$1, %esi
	movzbl	2617(%rsp), %eax
	movb	%al, 896(%rsp)
	call	crypto_declassify@PLT
	cmpb	$0, 896(%rsp)
	jne	.L21
	leaq	215(%r12), %rdi
	movq	%r15, %rsi
	vzeroupper
	call	CRYPTO_NAMESPACE(crypto_encode_857x3)@PLT
	movq	24(%rsp), %rbx
	movq	%rbx, %rdi
	call	Short_random
	movq	%rbx, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_857x3)@PLT
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%rbx, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_invsntrup857)@PLT
	movq	%r15, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_857xint16)@PLT
	movq	%r15, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_857xint16)@PLT
	movq	%r14, %rdx
	xorl	%ecx, %ecx
	movq	%r15, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup857)@PLT
	movq	%r15, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_857xint16)@PLT
	movq	%r15, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_857x5167)@PLT
	movq	%r12, %rax
	movzbl	429(%r12), %ebx
	subq	%r13, %rax
	leaq	429(%rax), %rdx
	xorl	%eax, %eax
	cmpq	$30, %rdx
	jbe	.L20
.L19:
	vmovdqu	0(%r13,%rax), %ymm5
	vmovdqu	%ymm5, 430(%r12,%rax)
	addq	$32, %rax
	cmpq	$1312, %rax
	jne	.L19
	movzbl	1312(%r13), %eax
	movb	%al, 1742(%r12)
	movzbl	1313(%r13), %eax
	movb	%al, 1743(%r12)
	movzbl	1314(%r13), %eax
	movb	%al, 1744(%r12)
	movzbl	1315(%r13), %eax
	movb	%al, 1745(%r12)
	movzbl	1316(%r13), %eax
	movb	%al, 1746(%r12)
	movzbl	1317(%r13), %eax
	movb	%al, 1747(%r12)
	movzbl	1318(%r13), %eax
	movb	%al, 1748(%r12)
	movzbl	1319(%r13), %eax
	movb	%al, 1749(%r12)
	movzbl	1320(%r13), %eax
	movb	%al, 1750(%r12)
	movzbl	1321(%r13), %eax
	movb	%al, 1751(%r12)
	vzeroupper
.L22:
	leaq	429(%r12), %rsi
	movq	%r15, %rdi
	movl	$1323, %edx
	movb	$4, 429(%r12)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movb	%bl, 429(%r12)
	movl	$215, %esi
	vmovdqa	1760(%rsp), %xmm6
	leaq	1752(%r12), %rdi
	vmovdqu	%xmm6, 1967(%r12)
	vmovdqa	1776(%rsp), %xmm6
	vmovdqu	%xmm6, 1983(%r12)
	call	randombytes@PLT
	leaq	-40(%rbp), %rsp
	xorl	%eax, %eax
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
	.p2align 4,,10
	.p2align 3
.L20:
	.cfi_restore_state
	movzbl	0(%r13,%rax), %edx
	movb	%dl, 430(%r12,%rax)
	incq	%rax
	cmpq	$1322, %rax
	jne	.L20
	jmp	.L22
	.cfi_endproc
.LFE9:
	.size	CRYPTO_NAMESPACE(keypair), .-CRYPTO_NAMESPACE(keypair)
	.p2align 4
	.globl	CRYPTO_NAMESPACE(enc)
	.type	CRYPTO_NAMESPACE(enc), @function
CRYPTO_NAMESPACE(enc):
.LFB10:
	.cfi_startproc
	pushq	%r14
	.cfi_def_cfa_offset 16
	.cfi_offset 14, -16
	pushq	%r13
	.cfi_def_cfa_offset 24
	.cfi_offset 13, -24
	pushq	%r12
	.cfi_def_cfa_offset 32
	.cfi_offset 12, -32
	movq	%rdx, %r12
	movl	$1322, %edx
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movq	%rsi, %rbx
	movq	%r12, %rsi
	subq	$2512, %rsp
	.cfi_def_cfa_offset 2560
	leaq	1184(%rsp), %r13
	leaq	320(%rsp), %r14
	leaq	1185(%rsp), %rdi
	call	memcpy@PLT
	movq	%r13, %rsi
	movl	$1323, %edx
	movq	%r14, %rdi
	movb	$4, 1184(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movq	%r14, %rdi
	vmovdqu	320(%rsp), %xmm0
	vmovdqu	336(%rsp), %xmm1
	vmovdqu	%xmm0, (%rsp)
	vmovdqu	%xmm1, 16(%rsp)
	call	Short_random
	movq	%rsp, %r9
	movq	%r12, %r8
	movq	%r14, %rcx
	leaq	96(%rsp), %rdx
	movq	%rbp, %rsi
	movq	%r13, %rdi
	call	Hide
	leaq	1217(%rsp), %rdi
	movq	%rbp, %rsi
	movl	$1184, %edx
	call	memcpy@PLT
	leaq	32(%rsp), %rdi
	movq	%r13, %rsi
	movl	$1217, %edx
	movb	$1, 1184(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	vmovdqu	32(%rsp), %xmm2
	vmovdqu	48(%rsp), %xmm3
	xorl	%eax, %eax
	vmovdqu	%xmm2, (%rbx)
	vmovdqu	%xmm3, 16(%rbx)
	addq	$2512, %rsp
	.cfi_def_cfa_offset 48
	popq	%rbx
	.cfi_def_cfa_offset 40
	popq	%rbp
	.cfi_def_cfa_offset 32
	popq	%r12
	.cfi_def_cfa_offset 24
	popq	%r13
	.cfi_def_cfa_offset 16
	popq	%r14
	.cfi_def_cfa_offset 8
	ret
	.cfi_endproc
.LFE10:
	.size	CRYPTO_NAMESPACE(enc), .-CRYPTO_NAMESPACE(enc)
	.p2align 4
	.globl	CRYPTO_NAMESPACE(dec)
	.type	CRYPTO_NAMESPACE(dec), @function
CRYPTO_NAMESPACE(dec):
.LFB11:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	movq	%rsi, %r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	.cfi_offset 13, -40
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	movq	%rdx, %rbx
	andq	$-32, %rsp
	subq	$4736, %rsp
	leaq	3008(%rsp), %r12
	leaq	1824(%rsp), %r13
	movq	%rdi, 16(%rsp)
	movq	%r12, %rdi
	leaq	960(%rsp), %r15
	call	CRYPTO_NAMESPACE(crypto_decode_857x1723)@PLT
	movq	%rbx, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_857x3)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_857xint16)@PLT
	xorl	%ecx, %ecx
	movq	%r13, %rdx
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup857)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_857xint16)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_857xint16)@PLT
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_scale3sntrup857)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_857xint16)@PLT
	movq	%r12, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_857xfreeze3)@PLT
	leaq	215(%rbx), %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_857x3)@PLT
	leaq	96(%rsp), %r11
	xorl	%ecx, %ecx
	movq	%r13, %rdx
	movq	%r15, %rsi
	movq	%r11, %rdi
	movq	%r11, 24(%rsp)
	call	CRYPTO_NAMESPACE(crypto_core_mult3sntrup857)@PLT
	movq	24(%rsp), %r11
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r11, %rsi
	movq	%r11, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_wforcesntrup857)@PLT
	movq	24(%rsp), %r11
	movq	%r15, %rdx
	movq	%r13, %rsi
	leaq	1967(%rbx), %r9
	leaq	430(%rbx), %r8
	movq	%r12, %rdi
	movq	%r11, %rcx
	call	Hide
	movq	%r13, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_verify_1184)@PLT
	vmovdqu	1752(%rbx), %ymm3
	movq	%r15, %rsi
	vpxor	961(%rsp), %ymm3, %ymm1
	vmovd	%eax, %xmm0
	vmovdqu	1784(%rbx), %ymm4
	vmovdqu	1816(%rbx), %ymm5
	movl	%eax, %r13d
	vpbroadcastb	%xmm0, %ymm0
	vmovdqu	1848(%rbx), %ymm6
	vmovdqu	1880(%rbx), %ymm7
	vpand	%ymm0, %ymm1, %ymm1
	movzbl	1169(%rsp), %edx
	vpxor	961(%rsp), %ymm1, %ymm1
	vmovdqu	1912(%rbx), %ymm2
	vmovdqu	%ymm1, 961(%rsp)
	vpxor	993(%rsp), %ymm4, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	993(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 993(%rsp)
	vpxor	1025(%rsp), %ymm5, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1025(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1025(%rsp)
	vpxor	1057(%rsp), %ymm6, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1057(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1057(%rsp)
	vpxor	1089(%rsp), %ymm7, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1089(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1089(%rsp)
	vpxor	1121(%rsp), %ymm2, %ymm1
	vmovd	%eax, %xmm2
	movzbl	1960(%rbx), %eax
	vpbroadcastb	%xmm2, %xmm2
	xorl	%edx, %eax
	vpand	%ymm0, %ymm1, %ymm0
	vpxor	1121(%rsp), %ymm0, %ymm0
	vmovdqu	1153(%rsp), %xmm1
	andl	%r13d, %eax
	vmovdqu	%ymm0, 1121(%rsp)
	xorl	%edx, %eax
	movzbl	1170(%rsp), %edx
	vpxor	1944(%rbx), %xmm1, %xmm0
	movb	%al, 1169(%rsp)
	movzbl	1961(%rbx), %eax
	vpand	%xmm2, %xmm0, %xmm0
	xorl	%edx, %eax
	vpxor	%xmm1, %xmm0, %xmm0
	vmovdqu	%xmm0, 1153(%rsp)
	andl	%r13d, %eax
	xorl	%edx, %eax
	movb	%al, 1170(%rsp)
	movzbl	1171(%rsp), %edx
	movzbl	1962(%rbx), %eax
	xorl	%edx, %eax
	andl	%r13d, %eax
	xorl	%edx, %eax
	movzbl	1172(%rsp), %edx
	movb	%al, 1171(%rsp)
	movzbl	1963(%rbx), %eax
	xorl	%edx, %eax
	andl	%r13d, %eax
	xorl	%edx, %eax
	movzbl	1173(%rsp), %edx
	movb	%al, 1172(%rsp)
	movzbl	1964(%rbx), %eax
	xorl	%edx, %eax
	andl	%r13d, %eax
	xorl	%edx, %eax
	movzbl	1174(%rsp), %edx
	movb	%al, 1173(%rsp)
	movzbl	1965(%rbx), %eax
	xorl	%edx, %eax
	andl	%r13d, %eax
	xorl	%edx, %eax
	movzbl	1175(%rsp), %edx
	movb	%al, 1174(%rsp)
	movzbl	1966(%rbx), %eax
	leaq	32(%rsp), %rbx
	movq	%rbx, %rdi
	xorl	%edx, %eax
	andl	%r13d, %eax
	xorl	%edx, %eax
	movl	$216, %edx
	movb	%al, 1175(%rsp)
	vzeroupper
	incl	%r13d
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	vmovdqa	32(%rsp), %xmm3
	movq	%r14, %rsi
	movl	$1184, %edx
	vmovdqa	48(%rsp), %xmm4
	leaq	3041(%rsp), %rdi
	vmovdqu	%xmm3, 3009(%rsp)
	vmovdqu	%xmm4, 3025(%rsp)
	call	memcpy@PLT
	movq	%r12, %rsi
	movq	%rbx, %rdi
	movl	$1217, %edx
	movb	%r13b, 3008(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movq	16(%rsp), %r10
	vmovdqa	32(%rsp), %xmm5
	xorl	%eax, %eax
	vmovdqa	48(%rsp), %xmm6
	vmovdqu	%xmm5, (%r10)
	vmovdqu	%xmm6, 16(%r10)
	leaq	-40(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	CRYPTO_NAMESPACE(dec), .-CRYPTO_NAMESPACE(dec)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.long	-2
	.long	-2
	.long	-2
	.long	-2
	.long	-2
	.long	-2
	.long	-2
	.long	-2
	.align 32
.LC1:
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.align 32
.LC2:
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.section	.rodata.cst16,"aM",@progbits,16
	.align 16
.LC3:
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.align 16
.LC4:
	.long	1
	.long	1
	.long	1
	.long	1
	.section	.rodata.cst32
	.align 32
.LC5:
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.align 32
.LC6:
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.align 32
.LC7:
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.section	.rodata.cst16
	.align 16
.LC8:
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.align 16
.LC9:
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.align 16
.LC10:
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.section	.rodata.cst32
	.align 32
.LC11:
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.section	.rodata.cst16
	.align 16
.LC12:
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.section	.rodata.cst32
	.align 32
.LC13:
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.section	.rodata.cst16
	.align 16
.LC14:
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
