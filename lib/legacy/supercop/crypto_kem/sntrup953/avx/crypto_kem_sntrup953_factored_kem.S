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
	movl	$3812, %esi
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	pushq	%r12
	.cfi_offset 13, -24
	.cfi_offset 12, -32
	movq	%rdi, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$3840, %rsp
	.cfi_offset 3, -40
	movq	%rsp, %rbx
	movq	%rbx, %rdi
	movq	%rbx, %r13
	call	randombytes@PLT
	movq	%rbx, %rsi
	movq	%rbx, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_953xint32)@PLT
	vmovdqa	.LC0(%rip), %ymm1
	leaq	1568(%rsp), %rdx
	movq	%rbx, %rax
	.p2align 4,,10
	.p2align 3
.L2:
	vpand	(%rax), %ymm1, %ymm0
	addq	$32, %rax
	vmovdqa	%ymm0, -32(%rax)
	cmpq	%rax, %rdx
	jne	.L2
	vmovdqa	.LC1(%rip), %xmm0
	vmovdqa	.LC2(%rip), %ymm2
	leaq	1584(%rsp), %rax
	leaq	3792(%rbx), %rdx
	vpand	1568(%rsp), %xmm0, %xmm0
	vmovdqa	.LC3(%rip), %ymm1
	vmovdqa	%xmm0, 1568(%rsp)
	.p2align 4,,10
	.p2align 3
.L3:
	vpand	(%rax), %ymm2, %ymm0
	addq	$32, %rax
	vpor	%ymm1, %ymm0, %ymm0
	vmovdqu	%ymm0, -32(%rax)
	cmpq	%rax, %rdx
	jne	.L3
	movl	3808(%rsp), %eax
	vmovdqa	.LC4(%rip), %xmm0
	movl	$960, %esi
	movq	%rbx, %rdi
	vpand	3792(%rsp), %xmm0, %xmm0
	vpor	.LC5(%rip), %xmm0, %xmm0
	movl	$-1, 3812(%rsp)
	movq	$-1, 3816(%rsp)
	andl	$-4, %eax
	movq	$-1, 3824(%rsp)
	orl	$1, %eax
	movl	%eax, 3808(%rsp)
	movq	$-1, 3832(%rsp)
	vmovdqa	%xmm0, 3792(%rsp)
	vzeroupper
	addq	$3712, %rbx
	call	CRYPTO_NAMESPACE(crypto_sort_uint32)@PLT
	vmovdqa	.LC6(%rip), %ymm0
	movq	%r12, %rax
	vpcmpeqd	%ymm2, %ymm2, %ymm2
	vmovdqa	.LC7(%rip), %ymm1
	vmovdqa	.LC8(%rip), %ymm3
.L4:
	vpand	32(%r13), %ymm0, %ymm5
	vpand	0(%r13), %ymm0, %ymm4
	subq	$-128, %r13
	addq	$32, %rax
	vpand	-32(%r13), %ymm0, %ymm6
	vpackusdw	%ymm5, %ymm4, %ymm4
	vpand	-64(%r13), %ymm0, %ymm5
	vpermq	$216, %ymm4, %ymm4
	vpackusdw	%ymm6, %ymm5, %ymm5
	vpand	%ymm4, %ymm1, %ymm4
	vpermq	$216, %ymm5, %ymm5
	vpand	%ymm5, %ymm1, %ymm5
	vpackuswb	%ymm5, %ymm4, %ymm4
	vpermq	$216, %ymm4, %ymm4
	vpand	%ymm4, %ymm3, %ymm4
	vpaddb	%ymm2, %ymm4, %ymm4
	vmovdqu	%ymm4, -32(%rax)
	cmpq	%rbx, %r13
	jne	.L4
	movl	3776(%rsp), %eax
	vmovdqa	.LC9(%rip), %xmm1
	vpand	3728(%rsp), %xmm1, %xmm2
	vpand	3712(%rsp), %xmm1, %xmm0
	andl	$3, %eax
	decl	%eax
	vpackusdw	%xmm2, %xmm0, %xmm0
	vpand	3744(%rsp), %xmm1, %xmm2
	movb	%al, 944(%r12)
	movl	3780(%rsp), %eax
	vpand	3760(%rsp), %xmm1, %xmm1
	andl	$3, %eax
	decl	%eax
	vpackusdw	%xmm1, %xmm2, %xmm1
	vmovdqa	.LC10(%rip), %xmm2
	movb	%al, 945(%r12)
	movl	3784(%rsp), %eax
	vpand	%xmm0, %xmm2, %xmm0
	vpand	%xmm1, %xmm2, %xmm2
	vpcmpeqd	%xmm1, %xmm1, %xmm1
	andl	$3, %eax
	vpackuswb	%xmm2, %xmm0, %xmm0
	vpand	.LC11(%rip), %xmm0, %xmm0
	decl	%eax
	movb	%al, 946(%r12)
	movl	3788(%rsp), %eax
	vpaddb	%xmm1, %xmm0, %xmm0
	vmovdqu	%xmm0, 928(%r12)
	andl	$3, %eax
	decl	%eax
	movb	%al, 947(%r12)
	movl	3792(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 948(%r12)
	movl	3796(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 949(%r12)
	movl	3800(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 950(%r12)
	movl	3804(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 951(%r12)
	movl	3808(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 952(%r12)
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
	subq	$2016, %rsp
	movq	%rcx, 24(%rsp)
	leaq	96(%rsp), %r14
	movq	%r8, 16(%rsp)
	call	CRYPTO_NAMESPACE(crypto_encode_953x3)@PLT
	movq	16(%rsp), %r8
	movq	%r14, %rdi
	movq	%r8, %rsi
	call	CRYPTO_NAMESPACE(crypto_decode_953x6343)@PLT
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_953xint16)@PLT
	movq	24(%rsp), %rdx
	xorl	%ecx, %ecx
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup953)@PLT
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_953xint16)@PLT
	movq	%r14, %rsi
	movq	%r13, %rdi
	leaq	32(%rsp), %r14
	call	CRYPTO_NAMESPACE(crypto_encode_953x2115round)@PLT
	movb	$3, (%r15)
	movq	%r15, %rsi
	movq	%r14, %rdi
	movl	$240, %edx
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
	vmovdqu	%xmm2, 1317(%r13)
	vmovdqu	%xmm3, 1333(%r13)
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
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	movq	%rsi, %r14
	pushq	%r13
	.cfi_offset 13, -40
	movq	%rdi, %r13
	pushq	%r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$5792, %rsp
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	leaq	992(%rsp), %rax
	leaq	1952(%rsp), %r15
	movq	%rax, 24(%rsp)
	leaq	32(%rsp), %r12
	leaq	5664(%rsp), %rbx
	.p2align 4,,10
	.p2align 3
.L21:
	movl	$3812, %esi
	movq	%r15, %rdi
	vzeroupper
	call	randombytes@PLT
	movq	%r15, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_953xint32)@PLT
	vmovdqa	.LC13(%rip), %xmm6
	movq	%r15, %rax
	vmovdqa	.LC12(%rip), %ymm3
	vmovdqa	.LC9(%rip), %xmm5
	vmovdqa	.LC7(%rip), %ymm4
	movq	%r12, %rdx
	vmovdqa	.LC6(%rip), %ymm2
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
	vpaddb	.LC14(%rip), %ymm0, %ymm0
	vmovdqa	%ymm0, -32(%rdx)
	cmpq	%rax, %rbx
	jne	.L17
	movl	5728(%rsp), %eax
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r12, %rsi
	vpand	5664(%rsp), %xmm6, %xmm0
	movq	%r15, %rdi
	vpand	5680(%rsp), %xmm6, %xmm7
	vpand	5712(%rsp), %xmm6, %xmm8
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	vpslld	$1, %xmm0, %xmm1
	shrl	$30, %eax
	vpaddd	%xmm0, %xmm1, %xmm1
	vpslld	$1, %xmm7, %xmm0
	vpsrld	$30, %xmm1, %xmm1
	decl	%eax
	movb	%al, 976(%rsp)
	vpaddd	%xmm7, %xmm0, %xmm0
	vpand	%xmm1, %xmm5, %xmm1
	movl	5732(%rsp), %eax
	vpand	5696(%rsp), %xmm6, %xmm7
	vpsrld	$30, %xmm0, %xmm0
	andl	$1073741823, %eax
	vpand	%xmm0, %xmm5, %xmm0
	leal	(%rax,%rax,2), %eax
	vpackusdw	%xmm0, %xmm1, %xmm1
	vpand	.LC10(%rip), %xmm1, %xmm1
	shrl	$30, %eax
	vpslld	$1, %xmm7, %xmm0
	decl	%eax
	vpaddd	%xmm7, %xmm0, %xmm0
	movb	%al, 977(%rsp)
	vpslld	$1, %xmm8, %xmm7
	vpsrld	$30, %xmm0, %xmm0
	movl	5736(%rsp), %eax
	vpaddd	%xmm8, %xmm7, %xmm7
	vpand	%xmm0, %xmm5, %xmm0
	andl	$1073741823, %eax
	vpsrld	$30, %xmm7, %xmm7
	leal	(%rax,%rax,2), %eax
	vpand	%xmm7, %xmm5, %xmm7
	shrl	$30, %eax
	vpackusdw	%xmm7, %xmm0, %xmm0
	vpand	.LC10(%rip), %xmm0, %xmm0
	decl	%eax
	movb	%al, 978(%rsp)
	movl	5740(%rsp), %eax
	vpackuswb	%xmm0, %xmm1, %xmm0
	vpaddb	.LC15(%rip), %xmm0, %xmm0
	andl	$1073741823, %eax
	vmovdqa	%xmm0, 960(%rsp)
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 979(%rsp)
	movl	5744(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 980(%rsp)
	movl	5748(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 981(%rsp)
	movl	5752(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 982(%rsp)
	movl	5756(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 983(%rsp)
	movl	5760(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 984(%rsp)
	vzeroupper
	call	CRYPTO_NAMESPACE(crypto_core_inv3sntrup953)@PLT
	movq	24(%rsp), %rdi
	movl	$1, %esi
	movzbl	2905(%rsp), %eax
	movb	%al, 992(%rsp)
	call	crypto_declassify@PLT
	cmpb	$0, 992(%rsp)
	jne	.L21
	leaq	239(%r14), %rdi
	movq	%r15, %rsi
	vzeroupper
	call	CRYPTO_NAMESPACE(crypto_encode_953x3)@PLT
	movq	24(%rsp), %rbx
	movq	%rbx, %rdi
	call	Short_random
	movq	%rbx, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_953x3)@PLT
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%rbx, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_invsntrup953)@PLT
	movq	%r15, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_953xint16)@PLT
	movq	%r15, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_953xint16)@PLT
	movq	%r12, %rdx
	xorl	%ecx, %ecx
	movq	%r15, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup953)@PLT
	movq	%r15, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_953xint16)@PLT
	movq	%r15, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_953x6343)@PLT
	movq	%r14, %rax
	movzbl	477(%r14), %ebx
	subq	%r13, %rax
	leaq	477(%rax), %rdx
	xorl	%eax, %eax
	cmpq	$30, %rdx
	jbe	.L20
.L19:
	vmovdqu	0(%r13,%rax), %ymm5
	vmovdqu	%ymm5, 478(%r14,%rax)
	addq	$32, %rax
	cmpq	$1504, %rax
	jne	.L19
	movzbl	1504(%r13), %eax
	movb	%al, 1982(%r14)
	vzeroupper
.L22:
	movb	$4, 477(%r14)
	leaq	477(%r14), %rsi
	movq	%r15, %rdi
	movl	$1506, %edx
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movb	%bl, 477(%r14)
	movl	$239, %esi
	vmovdqa	1952(%rsp), %xmm6
	leaq	1983(%r14), %rdi
	vmovdqu	%xmm6, 2222(%r14)
	vmovdqa	1968(%rsp), %xmm6
	vmovdqu	%xmm6, 2238(%r14)
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
	movb	%dl, 478(%r14,%rax)
	incq	%rax
	cmpq	$1505, %rax
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
	movl	$1505, %edx
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movq	%rsi, %rbx
	movq	%r12, %rsi
	subq	$2816, %rsp
	.cfi_def_cfa_offset 2864
	leaq	1296(%rsp), %r13
	leaq	336(%rsp), %r14
	leaq	1297(%rsp), %rdi
	call	memcpy@PLT
	movq	%r13, %rsi
	movl	$1506, %edx
	movq	%r14, %rdi
	movb	$4, 1296(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movq	%r14, %rdi
	vmovdqu	336(%rsp), %xmm0
	vmovdqu	352(%rsp), %xmm1
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
	leaq	1329(%rsp), %rdi
	movq	%rbp, %rsi
	movl	$1349, %edx
	call	memcpy@PLT
	leaq	32(%rsp), %rdi
	movq	%r13, %rsi
	movl	$1382, %edx
	movb	$1, 1296(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	vmovdqu	32(%rsp), %xmm2
	vmovdqu	48(%rsp), %xmm3
	xorl	%eax, %eax
	vmovdqu	%xmm2, (%rbx)
	vmovdqu	%xmm3, 16(%rbx)
	addq	$2816, %rsp
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
	subq	$5312, %rsp
	leaq	3392(%rsp), %r13
	leaq	2032(%rsp), %r12
	movq	%rdi, 16(%rsp)
	movq	%r13, %rdi
	leaq	1056(%rsp), %r15
	call	CRYPTO_NAMESPACE(crypto_decode_953x2115)@PLT
	movq	%rbx, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_953x3)@PLT
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_953xint16)@PLT
	xorl	%ecx, %ecx
	movq	%r12, %rdx
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup953)@PLT
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_953xint16)@PLT
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_953xint16)@PLT
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_scale3sntrup953)@PLT
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_953xint16)@PLT
	movq	%r13, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_953xfreeze3)@PLT
	leaq	239(%rbx), %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_953x3)@PLT
	leaq	96(%rsp), %r11
	xorl	%ecx, %ecx
	movq	%r12, %rdx
	movq	%r15, %rsi
	movq	%r11, %rdi
	movq	%r11, 24(%rsp)
	call	CRYPTO_NAMESPACE(crypto_core_mult3sntrup953)@PLT
	movq	24(%rsp), %r11
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r11, %rsi
	movq	%r11, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_wforcesntrup953)@PLT
	movq	24(%rsp), %r11
	movq	%r15, %rdx
	movq	%r12, %rsi
	leaq	2222(%rbx), %r9
	leaq	478(%rbx), %r8
	movq	%r13, %rdi
	movq	%r11, %rcx
	call	Hide
	movq	%r12, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_verify_1349)@PLT
	vmovdqu	1983(%rbx), %ymm2
	movq	%r15, %rsi
	vpxor	1057(%rsp), %ymm2, %ymm1
	vmovd	%eax, %xmm0
	vmovdqu	2015(%rbx), %ymm3
	vmovdqu	2047(%rbx), %ymm4
	movl	%eax, %r12d
	vpbroadcastb	%xmm0, %ymm0
	vmovdqu	2079(%rbx), %ymm5
	vmovdqu	2111(%rbx), %ymm6
	vpand	%ymm0, %ymm1, %ymm1
	vmovdqu	2143(%rbx), %ymm7
	vpxor	1057(%rsp), %ymm1, %ymm1
	movzbl	1281(%rsp), %edx
	movzbl	2207(%rbx), %eax
	vmovdqu	%ymm1, 1057(%rsp)
	vmovdqu	2175(%rbx), %ymm2
	vpxor	1089(%rsp), %ymm3, %ymm1
	xorl	%edx, %eax
	vpand	%ymm0, %ymm1, %ymm1
	andl	%r12d, %eax
	vpxor	1089(%rsp), %ymm1, %ymm1
	xorl	%edx, %eax
	movzbl	1282(%rsp), %edx
	vmovdqu	%ymm1, 1089(%rsp)
	vpxor	1121(%rsp), %ymm4, %ymm1
	movb	%al, 1281(%rsp)
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1121(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1121(%rsp)
	vpxor	1153(%rsp), %ymm5, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1153(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1153(%rsp)
	vpxor	1185(%rsp), %ymm6, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1185(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1185(%rsp)
	vpxor	1217(%rsp), %ymm7, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1217(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1217(%rsp)
	vpxor	1249(%rsp), %ymm2, %ymm1
	vpand	%ymm0, %ymm1, %ymm0
	vpxor	1249(%rsp), %ymm0, %ymm0
	vmovdqu	%ymm0, 1249(%rsp)
	movzbl	2208(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1283(%rsp), %edx
	movb	%al, 1282(%rsp)
	movzbl	2209(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1284(%rsp), %edx
	movb	%al, 1283(%rsp)
	movzbl	2210(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1285(%rsp), %edx
	movb	%al, 1284(%rsp)
	movzbl	2211(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1286(%rsp), %edx
	movb	%al, 1285(%rsp)
	movzbl	2212(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1287(%rsp), %edx
	movb	%al, 1286(%rsp)
	movzbl	2213(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1288(%rsp), %edx
	movb	%al, 1287(%rsp)
	movzbl	2214(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1289(%rsp), %edx
	movb	%al, 1288(%rsp)
	movzbl	2215(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1290(%rsp), %edx
	movb	%al, 1289(%rsp)
	movzbl	2216(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1291(%rsp), %edx
	movb	%al, 1290(%rsp)
	movzbl	2217(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1292(%rsp), %edx
	movb	%al, 1291(%rsp)
	movzbl	2218(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movb	%al, 1292(%rsp)
	movzbl	1293(%rsp), %edx
	movzbl	2219(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1294(%rsp), %edx
	movb	%al, 1293(%rsp)
	movzbl	2220(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1295(%rsp), %edx
	movb	%al, 1294(%rsp)
	movzbl	2221(%rbx), %eax
	leaq	32(%rsp), %rbx
	movq	%rbx, %rdi
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movl	$240, %edx
	movb	%al, 1295(%rsp)
	vzeroupper
	incl	%r12d
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	vmovdqa	32(%rsp), %xmm3
	movq	%r14, %rsi
	movl	$1349, %edx
	vmovdqa	48(%rsp), %xmm4
	leaq	3425(%rsp), %rdi
	vmovdqu	%xmm3, 3393(%rsp)
	vmovdqu	%xmm4, 3409(%rsp)
	call	memcpy@PLT
	movq	%r13, %rsi
	movq	%rbx, %rdi
	movl	$1382, %edx
	movb	%r12b, 3392(%rsp)
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
	.section	.rodata.cst16,"aM",@progbits,16
	.align 16
.LC1:
	.long	-2
	.long	-2
	.long	-2
	.long	-2
	.section	.rodata.cst32
	.align 32
.LC2:
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.align 32
.LC3:
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.long	1
	.section	.rodata.cst16
	.align 16
.LC4:
	.long	-4
	.long	-4
	.long	-4
	.long	-4
	.align 16
.LC5:
	.long	1
	.long	1
	.long	1
	.long	1
	.section	.rodata.cst32
	.align 32
.LC6:
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.align 32
.LC7:
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
.LC8:
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
.LC9:
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.align 16
.LC10:
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.align 16
.LC11:
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
.LC12:
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
.LC13:
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.section	.rodata.cst32
	.align 32
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
.LC15:
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
