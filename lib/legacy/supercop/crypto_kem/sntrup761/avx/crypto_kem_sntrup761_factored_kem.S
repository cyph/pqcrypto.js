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
	movl	$3044, %esi
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	pushq	%r12
	.cfi_offset 13, -24
	.cfi_offset 12, -32
	movq	%rdi, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$3072, %rsp
	.cfi_offset 3, -40
	movq	%rsp, %r13
	movq	%r13, %rdi
	movq	%r13, %rbx
	call	randombytes@PLT
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_761xint32)@PLT
	vmovdqa	.LC0(%rip), %ymm1
	leaq	1120(%rsp), %rdx
	movq	%r13, %rax
	.p2align 4,,10
	.p2align 3
.L2:
	vpand	(%rax), %ymm1, %ymm0
	addq	$32, %rax
	vmovdqa	%ymm0, -32(%rax)
	cmpq	%rax, %rdx
	jne	.L2
	vmovdqa	.LC1(%rip), %xmm0
	vpand	1120(%rsp), %xmm0, %xmm0
	movabsq	$-4294967298, %rax
	leaq	3032(%r13), %rdx
	vmovdqa	.LC2(%rip), %ymm2
	vmovdqa	.LC3(%rip), %ymm1
	andq	%rax, 1136(%rsp)
	leaq	1144(%rsp), %rax
	vmovdqa	%xmm0, 1120(%rsp)
	.p2align 4,,10
	.p2align 3
.L3:
	vpand	(%rax), %ymm2, %ymm0
	addq	$32, %rax
	vpor	%ymm1, %ymm0, %ymm0
	vmovdqu	%ymm0, -32(%rax)
	cmpq	%rdx, %rax
	jne	.L3
	movl	3032(%rsp), %eax
	vpcmpeqd	%xmm0, %xmm0, %xmm0
	movl	$768, %esi
	movq	%r13, %rdi
	movl	$-1, 3060(%rsp)
	movq	$-1, 3064(%rsp)
	andl	$-4, %eax
	vmovdqu	%xmm0, 3044(%rsp)
	orl	$1, %eax
	movl	%eax, 3032(%rsp)
	movl	3036(%rsp), %eax
	andl	$-4, %eax
	orl	$1, %eax
	movl	%eax, 3036(%rsp)
	movl	3040(%rsp), %eax
	andl	$-4, %eax
	orl	$1, %eax
	movl	%eax, 3040(%rsp)
	vzeroupper
	addq	$2944, %r13
	call	CRYPTO_NAMESPACE(crypto_sort_uint32)@PLT
	vmovdqa	.LC4(%rip), %ymm2
	movq	%r12, %rax
	vpcmpeqd	%ymm4, %ymm4, %ymm4
	vmovdqa	.LC5(%rip), %ymm3
	vmovdqa	.LC6(%rip), %ymm5
	.p2align 4,,10
	.p2align 3
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
	cmpq	%r13, %rbx
	jne	.L4
	movl	3008(%rsp), %eax
	vmovdqa	.LC7(%rip), %xmm1
	vpand	2960(%rsp), %xmm1, %xmm2
	vpand	2944(%rsp), %xmm1, %xmm0
	andl	$3, %eax
	decl	%eax
	vpackusdw	%xmm2, %xmm0, %xmm0
	vpand	2976(%rsp), %xmm1, %xmm2
	movb	%al, 752(%r12)
	movl	3012(%rsp), %eax
	vpand	2992(%rsp), %xmm1, %xmm1
	andl	$3, %eax
	decl	%eax
	vpackusdw	%xmm1, %xmm2, %xmm1
	vmovdqa	.LC8(%rip), %xmm2
	movb	%al, 753(%r12)
	movl	3016(%rsp), %eax
	vpand	%xmm0, %xmm2, %xmm0
	vpand	%xmm1, %xmm2, %xmm2
	vpcmpeqd	%xmm1, %xmm1, %xmm1
	andl	$3, %eax
	vpackuswb	%xmm2, %xmm0, %xmm0
	vpand	.LC9(%rip), %xmm0, %xmm0
	decl	%eax
	movb	%al, 754(%r12)
	movl	3020(%rsp), %eax
	vpaddb	%xmm1, %xmm0, %xmm0
	vmovdqu	%xmm0, 736(%r12)
	andl	$3, %eax
	decl	%eax
	movb	%al, 755(%r12)
	movl	3024(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 756(%r12)
	movl	3028(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 757(%r12)
	movl	3032(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 758(%r12)
	movl	3036(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 759(%r12)
	movl	3040(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 760(%r12)
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
	subq	$1632, %rsp
	movq	%rcx, 24(%rsp)
	leaq	96(%rsp), %r14
	movq	%r8, 16(%rsp)
	call	CRYPTO_NAMESPACE(crypto_encode_761x3)@PLT
	movq	16(%rsp), %r8
	movq	%r14, %rdi
	movq	%r8, %rsi
	call	CRYPTO_NAMESPACE(crypto_decode_761x4591)@PLT
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_761xint16)@PLT
	movq	24(%rsp), %rdx
	xorl	%ecx, %ecx
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup761)@PLT
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_761xint16)@PLT
	movq	%r14, %rsi
	movq	%r13, %rdi
	leaq	32(%rsp), %r14
	call	CRYPTO_NAMESPACE(crypto_encode_761x1531round)@PLT
	movb	$3, (%r15)
	movq	%r15, %rsi
	movq	%r14, %rdi
	movl	$192, %edx
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
	vmovdqu	%xmm2, 1007(%r13)
	vmovdqu	%xmm3, 1023(%r13)
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
	subq	$4640, %rsp
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	movq	%rsi, 24(%rsp)
	leaq	1568(%rsp), %r12
	leaq	32(%rsp), %r14
	leaq	4512(%rsp), %rbx
	leaq	800(%rsp), %r15
	.p2align 4,,10
	.p2align 3
.L21:
	movl	$3044, %esi
	movq	%r12, %rdi
	vzeroupper
	call	randombytes@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_761xint32)@PLT
	vmovdqa	.LC7(%rip), %xmm5
	movq	%r12, %rax
	vmovdqa	.LC11(%rip), %xmm6
	vmovdqa	.LC10(%rip), %ymm3
	vmovdqa	.LC5(%rip), %ymm4
	movq	%r14, %rdx
	vmovdqa	.LC4(%rip), %ymm2
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
	vpaddb	.LC12(%rip), %ymm0, %ymm0
	vmovdqa	%ymm0, -32(%rdx)
	cmpq	%rbx, %rax
	jne	.L17
	movl	4576(%rsp), %eax
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r14, %rsi
	vpand	4512(%rsp), %xmm6, %xmm0
	movq	%r12, %rdi
	vpand	4528(%rsp), %xmm6, %xmm7
	vpand	4560(%rsp), %xmm6, %xmm8
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	vpslld	$1, %xmm0, %xmm1
	shrl	$30, %eax
	vpaddd	%xmm0, %xmm1, %xmm1
	vpslld	$1, %xmm7, %xmm0
	vpsrld	$30, %xmm1, %xmm1
	decl	%eax
	movb	%al, 784(%rsp)
	vpaddd	%xmm7, %xmm0, %xmm0
	vpand	%xmm1, %xmm5, %xmm1
	movl	4580(%rsp), %eax
	vpand	4544(%rsp), %xmm6, %xmm7
	vpsrld	$30, %xmm0, %xmm0
	andl	$1073741823, %eax
	vpand	%xmm0, %xmm5, %xmm0
	leal	(%rax,%rax,2), %eax
	vpackusdw	%xmm0, %xmm1, %xmm1
	vpand	.LC8(%rip), %xmm1, %xmm1
	shrl	$30, %eax
	vpslld	$1, %xmm7, %xmm0
	decl	%eax
	vpaddd	%xmm7, %xmm0, %xmm0
	movb	%al, 785(%rsp)
	vpslld	$1, %xmm8, %xmm7
	vpsrld	$30, %xmm0, %xmm0
	movl	4584(%rsp), %eax
	vpaddd	%xmm8, %xmm7, %xmm7
	vpand	%xmm0, %xmm5, %xmm0
	andl	$1073741823, %eax
	vpsrld	$30, %xmm7, %xmm7
	leal	(%rax,%rax,2), %eax
	vpand	%xmm7, %xmm5, %xmm7
	shrl	$30, %eax
	vpackusdw	%xmm7, %xmm0, %xmm0
	vpand	.LC8(%rip), %xmm0, %xmm0
	decl	%eax
	movb	%al, 786(%rsp)
	movl	4588(%rsp), %eax
	vpackuswb	%xmm0, %xmm1, %xmm0
	vpaddb	.LC13(%rip), %xmm0, %xmm0
	andl	$1073741823, %eax
	vmovdqa	%xmm0, 768(%rsp)
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 787(%rsp)
	movl	4592(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 788(%rsp)
	movl	4596(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 789(%rsp)
	movl	4600(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 790(%rsp)
	movl	4604(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 791(%rsp)
	movl	4608(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 792(%rsp)
	vzeroupper
	call	CRYPTO_NAMESPACE(crypto_core_inv3sntrup761)@PLT
	movzbl	2329(%rsp), %eax
	movl	$1, %esi
	movq	%r15, %rdi
	movb	%al, 800(%rsp)
	call	crypto_declassify@PLT
	cmpb	$0, 800(%rsp)
	jne	.L21
	movq	24(%rsp), %rbx
	movq	%r12, %rsi
	leaq	191(%rbx), %rdi
	vzeroupper
	call	CRYPTO_NAMESPACE(crypto_encode_761x3)@PLT
	movq	%r15, %rdi
	call	Short_random
	movq	%rbx, %rdi
	movq	%r15, %rsi
	call	CRYPTO_NAMESPACE(crypto_encode_761x3)@PLT
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r15, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_invsntrup761)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_761xint16)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_761xint16)@PLT
	movq	%r14, %rdx
	xorl	%ecx, %ecx
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup761)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_761xint16)@PLT
	movq	%r12, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_761x4591)@PLT
	movq	%rbx, %rax
	movzbl	381(%rbx), %ebx
	subq	%r13, %rax
	leaq	381(%rax), %rdx
	xorl	%eax, %eax
	cmpq	$30, %rdx
	jbe	.L20
.L19:
	vmovdqu	0(%r13,%rax), %ymm5
	movq	24(%rsp), %rcx
	vmovdqu	%ymm5, 382(%rcx,%rax)
	addq	$32, %rax
	cmpq	$1152, %rax
	jne	.L19
	movzbl	1152(%r13), %eax
	movb	%al, 1534(%rcx)
	movzbl	1153(%r13), %eax
	movb	%al, 1535(%rcx)
	movzbl	1154(%r13), %eax
	movb	%al, 1536(%rcx)
	movzbl	1155(%r13), %eax
	movb	%al, 1537(%rcx)
	movzbl	1156(%r13), %eax
	movb	%al, 1538(%rcx)
	movzbl	1157(%r13), %eax
	movb	%al, 1539(%rcx)
	vzeroupper
	movq	%rcx, %r15
.L22:
	movb	$4, 381(%r15)
	leaq	381(%r15), %rsi
	movq	%r12, %rdi
	movl	$1159, %edx
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movb	%bl, 381(%r15)
	movl	$191, %esi
	vmovdqa	1568(%rsp), %xmm6
	leaq	1540(%r15), %rdi
	vmovdqu	%xmm6, 1731(%r15)
	vmovdqa	1584(%rsp), %xmm6
	vmovdqu	%xmm6, 1747(%r15)
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
	movb	%dl, 382(%rcx,%rax)
	incq	%rax
	cmpq	$1158, %rax
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
	movl	$1158, %edx
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movq	%rsi, %rbx
	movq	%r12, %rsi
	subq	$2224, %rsp
	.cfi_def_cfa_offset 2272
	leaq	1056(%rsp), %r13
	leaq	288(%rsp), %r14
	leaq	1057(%rsp), %rdi
	call	memcpy@PLT
	movq	%r13, %rsi
	movl	$1159, %edx
	movq	%r14, %rdi
	movb	$4, 1056(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movq	%r14, %rdi
	vmovdqu	288(%rsp), %xmm0
	vmovdqu	304(%rsp), %xmm1
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
	leaq	1089(%rsp), %rdi
	movq	%rbp, %rsi
	movl	$1039, %edx
	call	memcpy@PLT
	leaq	32(%rsp), %rdi
	movq	%r13, %rsi
	movl	$1072, %edx
	movb	$1, 1056(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	vmovdqu	32(%rsp), %xmm2
	vmovdqu	48(%rsp), %xmm3
	xorl	%eax, %eax
	vmovdqu	%xmm2, (%rbx)
	vmovdqu	%xmm3, 16(%rbx)
	addq	$2224, %rsp
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
	subq	$4224, %rsp
	leaq	2688(%rsp), %r13
	leaq	1648(%rsp), %r12
	movq	%rdi, 16(%rsp)
	movq	%r13, %rdi
	leaq	864(%rsp), %r15
	call	CRYPTO_NAMESPACE(crypto_decode_761x1531)@PLT
	movq	%rbx, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_761x3)@PLT
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_761xint16)@PLT
	xorl	%ecx, %ecx
	movq	%r12, %rdx
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup761)@PLT
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_761xint16)@PLT
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_761xint16)@PLT
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_scale3sntrup761)@PLT
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_761xint16)@PLT
	movq	%r13, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_761xfreeze3)@PLT
	leaq	191(%rbx), %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_761x3)@PLT
	leaq	96(%rsp), %r11
	xorl	%ecx, %ecx
	movq	%r12, %rdx
	movq	%r15, %rsi
	movq	%r11, %rdi
	movq	%r11, 24(%rsp)
	call	CRYPTO_NAMESPACE(crypto_core_mult3sntrup761)@PLT
	movq	24(%rsp), %r11
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r11, %rsi
	movq	%r11, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_wforcesntrup761)@PLT
	movq	24(%rsp), %r11
	movq	%r15, %rdx
	movq	%r12, %rsi
	leaq	1731(%rbx), %r9
	leaq	382(%rbx), %r8
	movq	%r13, %rdi
	movq	%r11, %rcx
	call	Hide
	movq	%r12, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_verify_1039)@PLT
	vmovdqu	1540(%rbx), %ymm3
	movq	%r15, %rsi
	vpxor	865(%rsp), %ymm3, %ymm0
	vmovd	%eax, %xmm1
	vmovdqu	1572(%rbx), %ymm4
	movl	%eax, %r12d
	vmovdqu	1604(%rbx), %ymm5
	vpbroadcastb	%xmm1, %ymm1
	movzbl	1041(%rsp), %edx
	vmovd	%eax, %xmm2
	movzbl	1716(%rbx), %eax
	vpand	%ymm1, %ymm0, %ymm0
	vmovdqu	1636(%rbx), %ymm6
	vpbroadcastb	%xmm2, %xmm2
	vpxor	865(%rsp), %ymm0, %ymm0
	xorl	%edx, %eax
	vmovdqu	1668(%rbx), %ymm7
	vmovdqu	%ymm0, 865(%rsp)
	andl	%r12d, %eax
	vpxor	897(%rsp), %ymm4, %ymm0
	xorl	%edx, %eax
	movzbl	1042(%rsp), %edx
	vpand	%ymm1, %ymm0, %ymm0
	movb	%al, 1041(%rsp)
	vpxor	897(%rsp), %ymm0, %ymm0
	movzbl	1717(%rbx), %eax
	vmovdqu	%ymm0, 897(%rsp)
	vpxor	929(%rsp), %ymm5, %ymm0
	xorl	%edx, %eax
	vpand	%ymm1, %ymm0, %ymm0
	andl	%r12d, %eax
	vpxor	929(%rsp), %ymm0, %ymm0
	xorl	%edx, %eax
	movzbl	1043(%rsp), %edx
	vmovdqu	%ymm0, 929(%rsp)
	vpxor	961(%rsp), %ymm6, %ymm0
	movb	%al, 1042(%rsp)
	movzbl	1718(%rbx), %eax
	vpand	%ymm1, %ymm0, %ymm0
	vpxor	961(%rsp), %ymm0, %ymm0
	xorl	%edx, %eax
	vmovdqu	%ymm0, 961(%rsp)
	andl	%r12d, %eax
	vpxor	993(%rsp), %ymm7, %ymm0
	xorl	%edx, %eax
	vpand	%ymm1, %ymm0, %ymm0
	movb	%al, 1043(%rsp)
	vpxor	993(%rsp), %ymm0, %ymm0
	vmovdqu	1025(%rsp), %xmm1
	vmovdqu	%ymm0, 993(%rsp)
	vpxor	1700(%rbx), %xmm1, %xmm0
	vpand	%xmm2, %xmm0, %xmm0
	vpxor	%xmm1, %xmm0, %xmm0
	vmovdqu	%xmm0, 1025(%rsp)
	movzbl	1044(%rsp), %edx
	movzbl	1719(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1045(%rsp), %edx
	movb	%al, 1044(%rsp)
	movzbl	1720(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1046(%rsp), %edx
	movb	%al, 1045(%rsp)
	movzbl	1721(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1047(%rsp), %edx
	movb	%al, 1046(%rsp)
	movzbl	1722(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1048(%rsp), %edx
	movb	%al, 1047(%rsp)
	movzbl	1723(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1049(%rsp), %edx
	movb	%al, 1048(%rsp)
	movzbl	1724(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1050(%rsp), %edx
	movb	%al, 1049(%rsp)
	movzbl	1725(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1051(%rsp), %edx
	movb	%al, 1050(%rsp)
	movzbl	1726(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1052(%rsp), %edx
	movb	%al, 1051(%rsp)
	movzbl	1727(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1053(%rsp), %edx
	movb	%al, 1052(%rsp)
	movzbl	1728(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movzbl	1054(%rsp), %edx
	movb	%al, 1053(%rsp)
	movzbl	1729(%rbx), %eax
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movb	%al, 1054(%rsp)
	movzbl	1055(%rsp), %edx
	movzbl	1730(%rbx), %eax
	leaq	32(%rsp), %rbx
	movq	%rbx, %rdi
	xorl	%edx, %eax
	andl	%r12d, %eax
	xorl	%edx, %eax
	movl	$192, %edx
	movb	%al, 1055(%rsp)
	vzeroupper
	incl	%r12d
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	vmovdqa	32(%rsp), %xmm2
	movq	%r14, %rsi
	movl	$1039, %edx
	vmovdqa	48(%rsp), %xmm3
	leaq	2721(%rsp), %rdi
	vmovdqu	%xmm2, 2689(%rsp)
	vmovdqu	%xmm3, 2705(%rsp)
	call	memcpy@PLT
	movq	%r13, %rsi
	movq	%rbx, %rdi
	movl	$1072, %edx
	movb	%r12b, 2688(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movq	16(%rsp), %r10
	vmovdqa	32(%rsp), %xmm4
	xorl	%eax, %eax
	vmovdqa	48(%rsp), %xmm5
	vmovdqu	%xmm4, (%r10)
	vmovdqu	%xmm5, 16(%r10)
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
	.align 32
.LC4:
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.align 32
.LC5:
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
.LC6:
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
.LC7:
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.align 16
.LC8:
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.value	255
	.align 16
.LC9:
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
.LC10:
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
.LC11:
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.long	1073741823
	.section	.rodata.cst32
	.align 32
.LC12:
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
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
