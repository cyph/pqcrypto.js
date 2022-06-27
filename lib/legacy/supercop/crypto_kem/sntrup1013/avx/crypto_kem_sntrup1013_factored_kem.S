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
	movl	$4052, %esi
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	pushq	%r12
	.cfi_offset 13, -24
	.cfi_offset 12, -32
	movq	%rdi, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$4096, %rsp
	.cfi_offset 3, -40
	movq	%rsp, %rbx
	movq	%rbx, %rdi
	movq	%rbx, %r13
	call	randombytes@PLT
	movq	%rbx, %rsi
	movq	%rbx, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1013xint32)@PLT
	vmovdqa	.LC0(%rip), %ymm1
	leaq	1792(%rsp), %rdx
	movq	%rbx, %rax
	.p2align 4,,10
	.p2align 3
.L2:
	vpand	(%rax), %ymm1, %ymm0
	addq	$32, %rax
	vmovdqa	%ymm0, -32(%rax)
	cmpq	%rdx, %rax
	jne	.L2
	vmovdqa	.LC1(%rip), %ymm2
	vmovdqa	.LC2(%rip), %ymm1
	leaq	4032(%rbx), %rdx
	.p2align 4,,10
	.p2align 3
.L3:
	vpand	(%rax), %ymm2, %ymm0
	addq	$32, %rax
	vpor	%ymm1, %ymm0, %ymm0
	vmovdqa	%ymm0, -32(%rax)
	cmpq	%rdx, %rax
	jne	.L3
	movl	4048(%rsp), %eax
	vmovdqa	.LC3(%rip), %xmm0
	movl	$1024, %esi
	movq	%rbx, %rdi
	vpand	4032(%rsp), %xmm0, %xmm0
	vpor	.LC4(%rip), %xmm0, %xmm0
	movq	$-1, 4084(%rsp)
	movl	$-1, 4092(%rsp)
	andl	$-4, %eax
	vmovdqa	%xmm0, 4032(%rsp)
	orl	$1, %eax
	vpcmpeqd	%xmm0, %xmm0, %xmm0
	movl	%eax, 4048(%rsp)
	vmovdqu	%xmm0, 4052(%rsp)
	vmovdqu	%xmm0, 4068(%rsp)
	vzeroupper
	addq	$3968, %rbx
	call	CRYPTO_NAMESPACE(crypto_sort_uint32)@PLT
	vmovdqa	.LC5(%rip), %ymm0
	movq	%r12, %rax
	vpcmpeqd	%ymm2, %ymm2, %ymm2
	vmovdqa	.LC6(%rip), %ymm1
	vmovdqa	.LC7(%rip), %ymm3
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
	cmpq	%r13, %rbx
	jne	.L4
	movl	4032(%rsp), %eax
	vmovdqa	.LC8(%rip), %xmm1
	vpand	3984(%rsp), %xmm1, %xmm2
	vpand	3968(%rsp), %xmm1, %xmm0
	andl	$3, %eax
	decl	%eax
	vpackusdw	%xmm2, %xmm0, %xmm0
	vpand	4000(%rsp), %xmm1, %xmm2
	movb	%al, 1008(%r12)
	movl	4036(%rsp), %eax
	vpand	4016(%rsp), %xmm1, %xmm1
	andl	$3, %eax
	decl	%eax
	vpackusdw	%xmm1, %xmm2, %xmm1
	vmovdqa	.LC9(%rip), %xmm2
	movb	%al, 1009(%r12)
	movl	4040(%rsp), %eax
	vpand	%xmm0, %xmm2, %xmm0
	vpand	%xmm1, %xmm2, %xmm2
	vpcmpeqd	%xmm1, %xmm1, %xmm1
	andl	$3, %eax
	vpackuswb	%xmm2, %xmm0, %xmm0
	vpand	.LC10(%rip), %xmm0, %xmm0
	decl	%eax
	movb	%al, 1010(%r12)
	movl	4044(%rsp), %eax
	vpaddb	%xmm1, %xmm0, %xmm0
	vmovdqu	%xmm0, 992(%r12)
	andl	$3, %eax
	decl	%eax
	movb	%al, 1011(%r12)
	movl	4048(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 1012(%r12)
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
	subq	$2112, %rsp
	movq	%rcx, 8(%rsp)
	leaq	80(%rsp), %r14
	movq	%r8, (%rsp)
	call	CRYPTO_NAMESPACE(crypto_encode_1013x3)@PLT
	movq	(%rsp), %r8
	movq	%r14, %rdi
	movq	%r8, %rsi
	call	CRYPTO_NAMESPACE(crypto_decode_1013x7177)@PLT
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_1013xint16)@PLT
	movq	8(%rsp), %rdx
	xorl	%ecx, %ecx
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup1013)@PLT
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1013xint16)@PLT
	movq	%r14, %rsi
	movq	%r13, %rdi
	leaq	16(%rsp), %r14
	call	CRYPTO_NAMESPACE(crypto_encode_1013x2393round)@PLT
	movb	$3, (%r15)
	movq	%r15, %rsi
	movq	%r14, %rdi
	movl	$255, %edx
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movq	%r12, %rax
	vmovdqa	16(%rsp), %xmm0
	vmovdqa	32(%rsp), %xmm1
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
	vmovdqa	16(%rsp), %xmm2
	vmovdqa	32(%rsp), %xmm3
	vmovdqu	%xmm2, 1423(%r13)
	vmovdqu	%xmm3, 1439(%r13)
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
	pushq	%rbx
	andq	$-32, %rsp
	subq	$6144, %rsp
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	movq	%rsi, 24(%rsp)
	leaq	2080(%rsp), %r12
	leaq	32(%rsp), %r14
	leaq	6048(%rsp), %rbx
	leaq	1056(%rsp), %r15
	.p2align 4,,10
	.p2align 3
.L21:
	movl	$4052, %esi
	movq	%r12, %rdi
	vzeroupper
	call	randombytes@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1013xint32)@PLT
	vmovdqa	.LC12(%rip), %xmm6
	movq	%r12, %rax
	vmovdqa	.LC11(%rip), %ymm3
	vmovdqa	.LC8(%rip), %xmm5
	vmovdqa	.LC6(%rip), %ymm4
	movq	%r14, %rdx
	vmovdqa	.LC5(%rip), %ymm2
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
	movl	6112(%rsp), %eax
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r14, %rsi
	vpand	6048(%rsp), %xmm6, %xmm0
	movq	%r12, %rdi
	vpand	6064(%rsp), %xmm6, %xmm7
	vpand	6096(%rsp), %xmm6, %xmm8
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	vpslld	$1, %xmm0, %xmm1
	shrl	$30, %eax
	vpaddd	%xmm0, %xmm1, %xmm1
	vpslld	$1, %xmm7, %xmm0
	vpsrld	$30, %xmm1, %xmm1
	decl	%eax
	movb	%al, 1040(%rsp)
	vpaddd	%xmm7, %xmm0, %xmm0
	vpand	%xmm1, %xmm5, %xmm1
	movl	6116(%rsp), %eax
	vpand	6080(%rsp), %xmm6, %xmm7
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
	movb	%al, 1041(%rsp)
	vpslld	$1, %xmm8, %xmm7
	vpsrld	$30, %xmm0, %xmm0
	movl	6120(%rsp), %eax
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
	movb	%al, 1042(%rsp)
	movl	6124(%rsp), %eax
	vpackuswb	%xmm0, %xmm1, %xmm0
	vpaddb	.LC14(%rip), %xmm0, %xmm0
	andl	$1073741823, %eax
	vmovdqa	%xmm0, 1024(%rsp)
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 1043(%rsp)
	movl	6128(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 1044(%rsp)
	vzeroupper
	call	CRYPTO_NAMESPACE(crypto_core_inv3sntrup1013)@PLT
	movzbl	3093(%rsp), %eax
	movl	$1, %esi
	movq	%r15, %rdi
	movb	%al, 1056(%rsp)
	call	crypto_declassify@PLT
	cmpb	$0, 1056(%rsp)
	jne	.L21
	movq	24(%rsp), %rbx
	movq	%r12, %rsi
	leaq	254(%rbx), %rdi
	vzeroupper
	call	CRYPTO_NAMESPACE(crypto_encode_1013x3)@PLT
	movq	%r15, %rdi
	call	Short_random
	movq	%rbx, %rdi
	movq	%r15, %rsi
	call	CRYPTO_NAMESPACE(crypto_encode_1013x3)@PLT
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r15, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_invsntrup1013)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1013xint16)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_1013xint16)@PLT
	movq	%r14, %rdx
	xorl	%ecx, %ecx
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup1013)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1013xint16)@PLT
	movq	%r12, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_1013x7177)@PLT
	movq	%rbx, %rax
	movzbl	507(%rbx), %ebx
	subq	%r13, %rax
	leaq	507(%rax), %rdx
	xorl	%eax, %eax
	cmpq	$30, %rdx
	jbe	.L20
.L19:
	vmovdqu	0(%r13,%rax), %ymm5
	movq	24(%rsp), %rcx
	vmovdqu	%ymm5, 508(%rcx,%rax)
	addq	$32, %rax
	cmpq	$1600, %rax
	jne	.L19
	vmovdqu	1600(%r13), %xmm6
	vmovdqu	%xmm6, 2108(%rcx)
	movzbl	1616(%r13), %eax
	movb	%al, 2124(%rcx)
	movzbl	1617(%r13), %eax
	movb	%al, 2125(%rcx)
	movzbl	1618(%r13), %eax
	movb	%al, 2126(%rcx)
	movzbl	1619(%r13), %eax
	movb	%al, 2127(%rcx)
	movzbl	1620(%r13), %eax
	movb	%al, 2128(%rcx)
	movzbl	1621(%r13), %eax
	movb	%al, 2129(%rcx)
	movzbl	1622(%r13), %eax
	movb	%al, 2130(%rcx)
	vzeroupper
	movq	%rcx, %r15
.L22:
	movb	$4, 507(%r15)
	leaq	507(%r15), %rsi
	movq	%r12, %rdi
	movl	$1624, %edx
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movb	%bl, 507(%r15)
	movl	$254, %esi
	vmovdqa	2080(%rsp), %xmm6
	leaq	2131(%r15), %rdi
	vmovdqu	%xmm6, 2385(%r15)
	vmovdqa	2096(%rsp), %xmm6
	vmovdqu	%xmm6, 2401(%r15)
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
	movq	24(%rsp), %rcx
	movb	%dl, 508(%rcx,%rax)
	incq	%rax
	cmpq	$1623, %rax
	jne	.L20
	movq	%rcx, %r15
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
	movl	$1623, %edx
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movq	%rsi, %rbx
	movq	%r12, %rsi
	subq	$3008, %rsp
	.cfi_def_cfa_offset 3056
	leaq	1376(%rsp), %r13
	leaq	352(%rsp), %r14
	leaq	1377(%rsp), %rdi
	call	memcpy@PLT
	movq	%r13, %rsi
	movl	$1624, %edx
	movq	%r14, %rdi
	movb	$4, 1376(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movq	%r14, %rdi
	vmovdqu	352(%rsp), %xmm0
	vmovdqu	368(%rsp), %xmm1
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
	leaq	1409(%rsp), %rdi
	movq	%rbp, %rsi
	movl	$1455, %edx
	call	memcpy@PLT
	leaq	32(%rsp), %rdi
	movq	%r13, %rsi
	movl	$1488, %edx
	movb	$1, 1376(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	vmovdqu	32(%rsp), %xmm2
	vmovdqu	48(%rsp), %xmm3
	xorl	%eax, %eax
	vmovdqu	%xmm2, (%rbx)
	vmovdqu	%xmm3, 16(%rbx)
	addq	$3008, %rsp
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
	subq	$5632, %rsp
	leaq	3600(%rsp), %r13
	leaq	2144(%rsp), %r12
	movq	%rdi, 16(%rsp)
	movq	%r13, %rdi
	leaq	1120(%rsp), %r15
	call	CRYPTO_NAMESPACE(crypto_decode_1013x2393)@PLT
	movq	%rbx, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1013x3)@PLT
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_1013xint16)@PLT
	xorl	%ecx, %ecx
	movq	%r12, %rdx
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup1013)@PLT
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1013xint16)@PLT
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_1013xint16)@PLT
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_scale3sntrup1013)@PLT
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1013xint16)@PLT
	movq	%r13, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_1013xfreeze3)@PLT
	leaq	254(%rbx), %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1013x3)@PLT
	leaq	96(%rsp), %r11
	xorl	%ecx, %ecx
	movq	%r12, %rdx
	movq	%r15, %rsi
	movq	%r11, %rdi
	movq	%r11, 24(%rsp)
	call	CRYPTO_NAMESPACE(crypto_core_mult3sntrup1013)@PLT
	movq	24(%rsp), %r11
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r11, %rsi
	movq	%r11, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_wforcesntrup1013)@PLT
	movq	24(%rsp), %r11
	movq	%r15, %rdx
	movq	%r12, %rsi
	leaq	2385(%rbx), %r9
	leaq	508(%rbx), %r8
	movq	%r13, %rdi
	movq	%r11, %rcx
	call	Hide
	movq	%r12, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_verify_1455)@PLT
	vmovdqu	2131(%rbx), %ymm3
	movq	%r15, %rsi
	vpxor	1121(%rsp), %ymm3, %ymm1
	vmovd	%eax, %xmm0
	vmovdqu	2163(%rbx), %ymm4
	vmovdqu	2195(%rbx), %ymm5
	movl	%eax, %r12d
	vpbroadcastb	%xmm0, %ymm0
	vmovdqu	2227(%rbx), %ymm6
	vmovdqu	2259(%rbx), %ymm7
	vpand	%ymm0, %ymm1, %ymm1
	vmovdqu	2291(%rbx), %ymm2
	vpxor	1121(%rsp), %ymm1, %ymm1
	vmovdqu	2323(%rbx), %ymm3
	movzbl	1361(%rsp), %edx
	vmovdqu	%ymm1, 1121(%rsp)
	vpxor	1153(%rsp), %ymm4, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1153(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1153(%rsp)
	vpxor	1185(%rsp), %ymm5, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1185(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1185(%rsp)
	vpxor	1217(%rsp), %ymm6, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1217(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1217(%rsp)
	vpxor	1249(%rsp), %ymm7, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1249(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1249(%rsp)
	vpxor	1281(%rsp), %ymm2, %ymm1
	vmovd	%eax, %xmm2
	vpbroadcastb	%xmm2, %xmm2
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1281(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1281(%rsp)
	vpxor	1313(%rsp), %ymm3, %ymm1
	vpand	%ymm0, %ymm1, %ymm0
	vpxor	1313(%rsp), %ymm0, %ymm0
	vmovdqu	1345(%rsp), %xmm1
	vmovdqu	%ymm0, 1313(%rsp)
	vpxor	2355(%rbx), %xmm1, %xmm0
	vpand	%xmm2, %xmm0, %xmm0
	vpxor	%xmm1, %xmm0, %xmm0
	vmovdqu	%xmm0, 1345(%rsp)
	movzbl	2371(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1362(%rsp), %edx
	movb	%al, 1361(%rsp)
	movzbl	2372(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1363(%rsp), %edx
	movb	%al, 1362(%rsp)
	movzbl	2373(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1364(%rsp), %edx
	movb	%al, 1363(%rsp)
	movzbl	2374(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1365(%rsp), %edx
	movb	%al, 1364(%rsp)
	movzbl	2375(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1366(%rsp), %edx
	movb	%al, 1365(%rsp)
	movzbl	2376(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1367(%rsp), %edx
	movb	%al, 1366(%rsp)
	movzbl	2377(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1368(%rsp), %edx
	movb	%al, 1367(%rsp)
	movzbl	2378(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1369(%rsp), %edx
	movb	%al, 1368(%rsp)
	movzbl	2379(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1370(%rsp), %edx
	movb	%al, 1369(%rsp)
	movzbl	2380(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1371(%rsp), %edx
	movb	%al, 1370(%rsp)
	movzbl	2381(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movb	%al, 1371(%rsp)
	movzbl	1372(%rsp), %edx
	movzbl	2382(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1373(%rsp), %edx
	movb	%al, 1372(%rsp)
	movzbl	2383(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1374(%rsp), %edx
	movb	%al, 1373(%rsp)
	movzbl	2384(%rbx), %eax
	leaq	32(%rsp), %rbx
	movq	%rbx, %rdi
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movl	$255, %edx
	movb	%al, 1374(%rsp)
	vzeroupper
	incl	%r12d
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	vmovdqa	32(%rsp), %xmm4
	movq	%r14, %rsi
	movl	$1455, %edx
	vmovdqa	48(%rsp), %xmm5
	leaq	3633(%rsp), %rdi
	vmovdqu	%xmm4, 3601(%rsp)
	vmovdqu	%xmm5, 3617(%rsp)
	call	memcpy@PLT
	movq	%r13, %rsi
	movq	%rbx, %rdi
	movl	$1488, %edx
	movb	%r12b, 3600(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movq	16(%rsp), %r10
	vmovdqa	32(%rsp), %xmm6
	xorl	%eax, %eax
	vmovdqa	48(%rsp), %xmm7
	vmovdqu	%xmm6, (%r10)
	vmovdqu	%xmm7, 16(%r10)
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
