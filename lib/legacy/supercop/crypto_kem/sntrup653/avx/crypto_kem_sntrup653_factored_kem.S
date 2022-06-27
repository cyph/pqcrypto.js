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
	movl	$2612, %esi
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	pushq	%r12
	.cfi_offset 13, -24
	.cfi_offset 12, -32
	movq	%rdi, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$2624, %rsp
	.cfi_offset 3, -40
	movq	%rsp, %rbx
	movq	%rbx, %rdi
	movq	%rbx, %r13
	call	randombytes@PLT
	movq	%rbx, %rsi
	movq	%rbx, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_653xint32)@PLT
	vmovdqa	.LC0(%rip), %ymm1
	leaq	1152(%rsp), %rax
	movq	%rbx, %rdx
	.p2align 4,,10
	.p2align 3
.L2:
	vpand	(%rdx), %ymm1, %ymm0
	addq	$32, %rdx
	vmovdqa	%ymm0, -32(%rdx)
	cmpq	%rdx, %rax
	jne	.L2
	vmovdqa	.LC1(%rip), %ymm2
	vmovdqa	.LC2(%rip), %ymm1
	leaq	2592(%rbx), %rdx
	.p2align 4,,10
	.p2align 3
.L3:
	vpand	(%rax), %ymm2, %ymm0
	addq	$32, %rax
	vpor	%ymm1, %ymm0, %ymm0
	vmovdqa	%ymm0, -32(%rax)
	cmpq	%rax, %rdx
	jne	.L3
	movl	2608(%rsp), %eax
	vmovdqa	.LC3(%rip), %xmm0
	movl	$653, %esi
	movq	%rbx, %rdi
	vpand	2592(%rsp), %xmm0, %xmm0
	vpor	.LC4(%rip), %xmm0, %xmm0
	andl	$-4, %eax
	vmovdqa	%xmm0, 2592(%rsp)
	orl	$1, %eax
	movl	%eax, 2608(%rsp)
	vzeroupper
	addq	$2560, %rbx
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
	movzbl	2588(%rsp), %eax
	movzbl	2584(%rsp), %edx
	vmovq	.LC8(%rip), %xmm0
	vmovq	.LC9(%rip), %xmm1
	salq	$8, %rax
	orq	%rdx, %rax
	movzbl	2580(%rsp), %edx
	salq	$8, %rax
	orq	%rdx, %rax
	movzbl	2576(%rsp), %edx
	salq	$8, %rax
	orq	%rdx, %rax
	movzbl	2572(%rsp), %edx
	salq	$8, %rax
	orq	%rdx, %rax
	movzbl	2568(%rsp), %edx
	salq	$8, %rax
	orq	%rdx, %rax
	movzbl	2564(%rsp), %edx
	salq	$8, %rax
	orq	%rdx, %rax
	movzbl	2560(%rsp), %edx
	salq	$8, %rax
	orq	%rdx, %rax
	vmovq	%rax, %xmm7
	movl	2592(%rsp), %eax
	vpand	%xmm7, %xmm0, %xmm0
	andl	$3, %eax
	vpaddb	%xmm1, %xmm0, %xmm0
	decl	%eax
	movb	%al, 648(%r12)
	movl	2596(%rsp), %eax
	vmovq	%xmm0, 640(%r12)
	andl	$3, %eax
	decl	%eax
	movb	%al, 649(%r12)
	movl	2600(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 650(%r12)
	movl	2604(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 651(%r12)
	movl	2608(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 652(%r12)
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
	subq	$1408, %rsp
	movq	%rcx, 24(%rsp)
	leaq	96(%rsp), %r14
	movq	%r8, 16(%rsp)
	call	CRYPTO_NAMESPACE(crypto_encode_653x3)@PLT
	movq	16(%rsp), %r8
	movq	%r14, %rdi
	movq	%r8, %rsi
	call	CRYPTO_NAMESPACE(crypto_decode_653x4621)@PLT
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_653xint16)@PLT
	movq	24(%rsp), %rdx
	xorl	%ecx, %ecx
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup653)@PLT
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_653xint16)@PLT
	movq	%r14, %rsi
	movq	%r13, %rdi
	leaq	32(%rsp), %r14
	call	CRYPTO_NAMESPACE(crypto_encode_653x1541round)@PLT
	movb	$3, (%r15)
	movq	%r15, %rsi
	movq	%r14, %rdi
	movl	$165, %edx
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
	vmovdqu	%xmm2, 865(%r13)
	vmovdqu	%xmm3, 881(%r13)
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
	movq	%rdi, %r14
	pushq	%r13
	pushq	%r12
	.cfi_offset 13, -40
	.cfi_offset 12, -48
	movq	%rsi, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$3968, %rsp
	.cfi_offset 3, -56
	leaq	688(%rsp), %rax
	leaq	1344(%rsp), %r15
	movq	%rax, 24(%rsp)
	leaq	32(%rsp), %r13
	leaq	3904(%rsp), %rbx
.L21:
	movl	$2612, %esi
	movq	%r15, %rdi
	vzeroupper
	call	randombytes@PLT
	movq	%r15, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_653xint32)@PLT
	movq	%r15, %rax
	movq	%r13, %rdx
	vpcmpeqd	%ymm5, %ymm5, %ymm5
	vmovdqa	.LC10(%rip), %ymm3
	vmovdqa	.LC6(%rip), %ymm4
	vmovdqa	.LC5(%rip), %ymm2
	.p2align 4,,10
	.p2align 3
.L17:
	vpand	(%rax), %ymm3, %ymm0
	vpand	32(%rax), %ymm3, %ymm6
	subq	$-128, %rax
	addq	$32, %rdx
	vpand	-32(%rax), %ymm3, %ymm7
	vpslld	$1, %ymm0, %ymm1
	vpaddd	%ymm0, %ymm1, %ymm1
	vpslld	$1, %ymm6, %ymm0
	vpaddd	%ymm6, %ymm0, %ymm0
	vpsrld	$30, %ymm1, %ymm1
	vpand	-64(%rax), %ymm3, %ymm6
	vpsrld	$30, %ymm0, %ymm0
	vpand	%ymm1, %ymm2, %ymm1
	vpand	%ymm0, %ymm2, %ymm0
	vpackusdw	%ymm0, %ymm1, %ymm1
	vpslld	$1, %ymm6, %ymm0
	vpaddd	%ymm6, %ymm0, %ymm0
	vpslld	$1, %ymm7, %ymm6
	vpermq	$216, %ymm1, %ymm1
	vpaddd	%ymm7, %ymm6, %ymm6
	vpsrld	$30, %ymm0, %ymm0
	vpand	%ymm1, %ymm4, %ymm1
	vpsrld	$30, %ymm6, %ymm6
	vpand	%ymm0, %ymm2, %ymm0
	vpand	%ymm6, %ymm2, %ymm6
	vpackusdw	%ymm6, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpand	%ymm0, %ymm4, %ymm0
	vpackuswb	%ymm0, %ymm1, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpaddb	%ymm0, %ymm5, %ymm0
	vmovdqa	%ymm0, -32(%rdx)
	cmpq	%rax, %rbx
	jne	.L17
	movl	3904(%rsp), %eax
	movq	%r13, %rsi
	movq	%r15, %rdi
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 672(%rsp)
	movl	3908(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 673(%rsp)
	movl	3912(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 674(%rsp)
	movl	3916(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 675(%rsp)
	movl	3920(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 676(%rsp)
	movl	3924(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 677(%rsp)
	movl	3928(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 678(%rsp)
	movl	3932(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 679(%rsp)
	movl	3936(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 680(%rsp)
	movl	3940(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 681(%rsp)
	movl	3944(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movb	%al, 682(%rsp)
	movl	3948(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 683(%rsp)
	movl	3952(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 684(%rsp)
	vzeroupper
	call	CRYPTO_NAMESPACE(crypto_core_inv3sntrup653)@PLT
	movq	24(%rsp), %rdi
	movl	$1, %esi
	movzbl	1997(%rsp), %eax
	movb	%al, 688(%rsp)
	call	crypto_declassify@PLT
	cmpb	$0, 688(%rsp)
	jne	.L21
	leaq	164(%r12), %rdi
	movq	%r15, %rsi
	vzeroupper
	call	CRYPTO_NAMESPACE(crypto_encode_653x3)@PLT
	movq	24(%rsp), %rbx
	movq	%rbx, %rdi
	call	Short_random
	movq	%rbx, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_653x3)@PLT
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%rbx, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_invsntrup653)@PLT
	movq	%r15, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_653xint16)@PLT
	movq	%r15, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_653xint16)@PLT
	movq	%r13, %rdx
	xorl	%ecx, %ecx
	movq	%r15, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup653)@PLT
	movq	%r15, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_653xint16)@PLT
	movq	%r15, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_653x4621)@PLT
	movq	%r12, %rax
	movzbl	327(%r12), %ebx
	subq	%r14, %rax
	leaq	327(%rax), %rdx
	xorl	%eax, %eax
	cmpq	$30, %rdx
	jbe	.L20
.L19:
	vmovdqu	(%r14,%rax), %ymm5
	vmovdqu	%ymm5, 328(%r12,%rax)
	addq	$32, %rax
	cmpq	$992, %rax
	jne	.L19
	movzbl	992(%r14), %eax
	movb	%al, 1320(%r12)
	movzbl	993(%r14), %eax
	movb	%al, 1321(%r12)
	vzeroupper
.L22:
	leaq	327(%r12), %rsi
	movq	%r15, %rdi
	movl	$995, %edx
	movb	$4, 327(%r12)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movb	%bl, 327(%r12)
	movl	$164, %esi
	vmovdqa	1344(%rsp), %xmm5
	leaq	1322(%r12), %rdi
	vmovdqu	%xmm5, 1486(%r12)
	vmovdqa	1360(%rsp), %xmm5
	vmovdqu	%xmm5, 1502(%r12)
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
	movzbl	(%r14,%rax), %edx
	movb	%dl, 328(%r12,%rax)
	incq	%rax
	cmpq	$994, %rax
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
	movl	$994, %edx
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movq	%rsi, %rbx
	movq	%r12, %rsi
	subq	$1936, %rsp
	.cfi_def_cfa_offset 1984
	leaq	928(%rsp), %r13
	leaq	272(%rsp), %r14
	leaq	929(%rsp), %rdi
	call	memcpy@PLT
	movq	%r13, %rsi
	movl	$995, %edx
	movq	%r14, %rdi
	movb	$4, 928(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movq	%r14, %rdi
	vmovdqu	272(%rsp), %xmm0
	vmovdqu	288(%rsp), %xmm1
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
	leaq	961(%rsp), %rdi
	movq	%rbp, %rsi
	movl	$897, %edx
	call	memcpy@PLT
	leaq	32(%rsp), %rdi
	movq	%r13, %rsi
	movl	$930, %edx
	movb	$1, 928(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	vmovdqu	32(%rsp), %xmm2
	vmovdqu	48(%rsp), %xmm3
	xorl	%eax, %eax
	vmovdqu	%xmm2, (%rbx)
	vmovdqu	%xmm3, 16(%rbx)
	addq	$1936, %rsp
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
	subq	$3616, %rsp
	leaq	2304(%rsp), %r12
	leaq	1392(%rsp), %r13
	movq	%rdi, (%rsp)
	movq	%r12, %rdi
	leaq	736(%rsp), %r15
	call	CRYPTO_NAMESPACE(crypto_decode_653x1541)@PLT
	movq	%rbx, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_653x3)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_653xint16)@PLT
	xorl	%ecx, %ecx
	movq	%r13, %rdx
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup653)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_653xint16)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_653xint16)@PLT
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_scale3sntrup653)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_653xint16)@PLT
	movq	%r12, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_653xfreeze3)@PLT
	leaq	164(%rbx), %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_653x3)@PLT
	leaq	80(%rsp), %r11
	xorl	%ecx, %ecx
	movq	%r13, %rdx
	movq	%r15, %rsi
	movq	%r11, %rdi
	movq	%r11, 8(%rsp)
	call	CRYPTO_NAMESPACE(crypto_core_mult3sntrup653)@PLT
	movq	8(%rsp), %r11
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r11, %rsi
	movq	%r11, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_wforcesntrup653)@PLT
	movq	8(%rsp), %r11
	movq	%r15, %rdx
	movq	%r13, %rsi
	leaq	1486(%rbx), %r9
	leaq	328(%rbx), %r8
	movq	%r12, %rdi
	movq	%r11, %rcx
	call	Hide
	movq	%r13, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_verify_897)@PLT
	movzbl	897(%rsp), %edx
	movq	%r15, %rsi
	vmovdqu	1322(%rbx), %ymm2
	movl	%eax, %r13d
	vmovd	%eax, %xmm1
	vpxor	737(%rsp), %ymm2, %ymm0
	movzbl	1482(%rbx), %eax
	vpbroadcastb	%xmm1, %ymm1
	vmovdqu	1354(%rbx), %ymm3
	vmovdqu	1386(%rbx), %ymm4
	xorl	%edx, %eax
	vpand	%ymm1, %ymm0, %ymm0
	vpxor	737(%rsp), %ymm0, %ymm0
	vmovdqu	1418(%rbx), %ymm5
	andl	%r13d, %eax
	vmovdqu	1450(%rbx), %ymm6
	vmovdqu	%ymm0, 737(%rsp)
	xorl	%edx, %eax
	vpxor	769(%rsp), %ymm3, %ymm0
	movzbl	898(%rsp), %edx
	movb	%al, 897(%rsp)
	movzbl	1483(%rbx), %eax
	vpand	%ymm1, %ymm0, %ymm0
	vpxor	769(%rsp), %ymm0, %ymm0
	xorl	%edx, %eax
	vmovdqu	%ymm0, 769(%rsp)
	andl	%r13d, %eax
	vpxor	801(%rsp), %ymm4, %ymm0
	xorl	%edx, %eax
	movzbl	899(%rsp), %edx
	movb	%al, 898(%rsp)
	vpand	%ymm1, %ymm0, %ymm0
	movzbl	1484(%rbx), %eax
	vpxor	801(%rsp), %ymm0, %ymm0
	xorl	%edx, %eax
	vmovdqu	%ymm0, 801(%rsp)
	andl	%r13d, %eax
	vpxor	833(%rsp), %ymm5, %ymm0
	xorl	%edx, %eax
	movzbl	900(%rsp), %edx
	vpand	%ymm1, %ymm0, %ymm0
	movb	%al, 899(%rsp)
	vpxor	833(%rsp), %ymm0, %ymm0
	movzbl	1485(%rbx), %eax
	leaq	16(%rsp), %rbx
	vmovdqu	%ymm0, 833(%rsp)
	movq	%rbx, %rdi
	vpxor	865(%rsp), %ymm6, %ymm0
	xorl	%edx, %eax
	vpand	%ymm1, %ymm0, %ymm0
	andl	%r13d, %eax
	vpxor	865(%rsp), %ymm0, %ymm0
	xorl	%edx, %eax
	movl	$165, %edx
	movb	%al, 900(%rsp)
	vmovdqu	%ymm0, 865(%rsp)
	vzeroupper
	incl	%r13d
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	vmovdqa	16(%rsp), %xmm7
	movq	%r14, %rsi
	movl	$897, %edx
	vmovdqa	32(%rsp), %xmm2
	leaq	2337(%rsp), %rdi
	vmovdqu	%xmm7, 2305(%rsp)
	vmovdqu	%xmm2, 2321(%rsp)
	call	memcpy@PLT
	movq	%r12, %rsi
	movq	%rbx, %rdi
	movl	$930, %edx
	movb	%r13b, 2304(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movq	(%rsp), %r10
	vmovdqa	16(%rsp), %xmm3
	xorl	%eax, %eax
	vmovdqa	32(%rsp), %xmm4
	vmovdqu	%xmm3, (%r10)
	vmovdqu	%xmm4, 16(%r10)
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
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC8:
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.byte	3
	.align 8
.LC9:
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.byte	-1
	.section	.rodata.cst32
	.align 32
.LC10:
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
