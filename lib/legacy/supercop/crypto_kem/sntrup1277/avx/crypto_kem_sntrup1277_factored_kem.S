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
	movl	$5108, %esi
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	pushq	%r12
	.cfi_offset 13, -24
	.cfi_offset 12, -32
	movq	%rdi, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$5120, %rsp
	.cfi_offset 3, -40
	movq	%rsp, %rbx
	movq	%rbx, %rdi
	movq	%rbx, %r13
	call	randombytes@PLT
	movq	%rbx, %rsi
	movq	%rbx, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1277xint32)@PLT
	vmovdqa	.LC0(%rip), %ymm1
	leaq	1952(%rsp), %rdx
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
	leaq	1968(%rsp), %rax
	leaq	5104(%rbx), %rdx
	vpand	1952(%rsp), %xmm0, %xmm0
	vmovdqa	.LC3(%rip), %ymm1
	vmovdqa	%xmm0, 1952(%rsp)
	.p2align 4,,10
	.p2align 3
.L3:
	vpand	(%rax), %ymm2, %ymm0
	addq	$32, %rax
	vpor	%ymm1, %ymm0, %ymm0
	vmovdqu	%ymm0, -32(%rax)
	cmpq	%rax, %rdx
	jne	.L3
	movl	5104(%rsp), %eax
	movl	$1280, %esi
	movq	%rbx, %rdi
	movl	$-1, 5116(%rsp)
	andl	$-4, %eax
	orl	$1, %eax
	movl	%eax, 5104(%rsp)
	movq	.LC4(%rip), %rax
	movq	%rax, 5108(%rsp)
	vzeroupper
	addq	$4992, %rbx
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
	movl	5056(%rsp), %eax
	vmovdqa	.LC8(%rip), %xmm1
	vpand	5008(%rsp), %xmm1, %xmm2
	vpand	4992(%rsp), %xmm1, %xmm0
	andl	$3, %eax
	decl	%eax
	vpackusdw	%xmm2, %xmm0, %xmm0
	vpand	5024(%rsp), %xmm1, %xmm2
	movb	%al, 1264(%r12)
	movl	5060(%rsp), %eax
	vpand	5040(%rsp), %xmm1, %xmm1
	andl	$3, %eax
	decl	%eax
	vpackusdw	%xmm1, %xmm2, %xmm1
	vmovdqa	.LC9(%rip), %xmm2
	movb	%al, 1265(%r12)
	movl	5064(%rsp), %eax
	vpand	%xmm0, %xmm2, %xmm0
	vpand	%xmm1, %xmm2, %xmm2
	vpcmpeqd	%xmm1, %xmm1, %xmm1
	andl	$3, %eax
	vpackuswb	%xmm2, %xmm0, %xmm0
	vpand	.LC10(%rip), %xmm0, %xmm0
	decl	%eax
	movb	%al, 1266(%r12)
	movl	5068(%rsp), %eax
	vpaddb	%xmm1, %xmm0, %xmm0
	vmovdqu	%xmm0, 1248(%r12)
	andl	$3, %eax
	decl	%eax
	movb	%al, 1267(%r12)
	movl	5072(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 1268(%r12)
	movl	5076(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 1269(%r12)
	movl	5080(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 1270(%r12)
	movl	5084(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 1271(%r12)
	movl	5088(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 1272(%r12)
	movl	5092(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 1273(%r12)
	movl	5096(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 1274(%r12)
	movl	5100(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 1275(%r12)
	movl	5104(%rsp), %eax
	andl	$3, %eax
	decl	%eax
	movb	%al, 1276(%r12)
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
	subq	$2656, %rsp
	movq	%rcx, 24(%rsp)
	leaq	96(%rsp), %r14
	movq	%r8, 16(%rsp)
	call	CRYPTO_NAMESPACE(crypto_encode_1277x3)@PLT
	movq	16(%rsp), %r8
	movq	%r14, %rdi
	movq	%r8, %rsi
	call	CRYPTO_NAMESPACE(crypto_decode_1277x7879)@PLT
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_1277xint16)@PLT
	movq	24(%rsp), %rdx
	xorl	%ecx, %ecx
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup1277)@PLT
	movq	%r14, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1277xint16)@PLT
	movq	%r14, %rsi
	movq	%r13, %rdi
	leaq	32(%rsp), %r14
	call	CRYPTO_NAMESPACE(crypto_encode_1277x2627round)@PLT
	movb	$3, (%r15)
	movq	%r15, %rsi
	movq	%r14, %rdi
	movl	$321, %edx
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
	vmovdqu	%xmm2, 1815(%r13)
	vmovdqu	%xmm3, 1831(%r13)
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
	subq	$7712, %rsp
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	movq	%rsi, 24(%rsp)
	leaq	2592(%rsp), %r12
	leaq	32(%rsp), %r14
	leaq	7584(%rsp), %rbx
	leaq	1312(%rsp), %r15
	.p2align 4,,10
	.p2align 3
.L21:
	movl	$5108, %esi
	movq	%r12, %rdi
	vzeroupper
	call	randombytes@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1277xint32)@PLT
	vmovdqa	.LC8(%rip), %xmm5
	movq	%r12, %rax
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
	cmpq	%rbx, %rax
	jne	.L17
	movl	7648(%rsp), %eax
	movq	%r14, %rsi
	movq	%r12, %rdi
	vpand	7584(%rsp), %xmm6, %xmm0
	vpand	7600(%rsp), %xmm6, %xmm7
	vpand	7632(%rsp), %xmm6, %xmm8
	andl	$1073741823, %eax
	vpslld	$1, %xmm0, %xmm1
	leal	(%rax,%rax,2), %eax
	vpaddd	%xmm0, %xmm1, %xmm1
	vpslld	$1, %xmm7, %xmm0
	shrl	$30, %eax
	vpsrld	$30, %xmm1, %xmm1
	vpaddd	%xmm7, %xmm0, %xmm0
	decl	%eax
	vpsrld	$30, %xmm0, %xmm0
	vpand	%xmm1, %xmm5, %xmm1
	movb	%al, 1296(%rsp)
	movl	7652(%rsp), %eax
	vpand	%xmm0, %xmm5, %xmm0
	vpand	7616(%rsp), %xmm6, %xmm7
	vpackusdw	%xmm0, %xmm1, %xmm1
	vpand	.LC9(%rip), %xmm1, %xmm1
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	vpslld	$1, %xmm7, %xmm0
	shrl	$30, %eax
	vpaddd	%xmm7, %xmm0, %xmm0
	vpslld	$1, %xmm8, %xmm7
	vpsrld	$30, %xmm0, %xmm0
	decl	%eax
	movb	%al, 1297(%rsp)
	vpaddd	%xmm8, %xmm7, %xmm7
	vpand	%xmm0, %xmm5, %xmm0
	movl	7656(%rsp), %eax
	vpsrld	$30, %xmm7, %xmm7
	andl	$1073741823, %eax
	vpand	%xmm7, %xmm5, %xmm7
	leal	(%rax,%rax,2), %eax
	vpackusdw	%xmm7, %xmm0, %xmm0
	vpand	.LC9(%rip), %xmm0, %xmm0
	shrl	$30, %eax
	decl	%eax
	vpackuswb	%xmm0, %xmm1, %xmm0
	vpaddb	.LC14(%rip), %xmm0, %xmm0
	movb	%al, 1298(%rsp)
	movl	7660(%rsp), %eax
	vmovdqa	%xmm0, 1280(%rsp)
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 1299(%rsp)
	movl	7664(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 1300(%rsp)
	movl	7668(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 1301(%rsp)
	movl	7672(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 1302(%rsp)
	movl	7676(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 1303(%rsp)
	movl	7680(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 1304(%rsp)
	movl	7684(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 1305(%rsp)
	movl	7688(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movb	%al, 1306(%rsp)
	movl	7692(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 1307(%rsp)
	movl	7696(%rsp), %eax
	andl	$1073741823, %eax
	leal	(%rax,%rax,2), %eax
	shrl	$30, %eax
	decl	%eax
	movb	%al, 1308(%rsp)
	vzeroupper
	call	CRYPTO_NAMESPACE(crypto_core_inv3sntrup1277)@PLT
	movzbl	3869(%rsp), %eax
	movl	$1, %esi
	movq	%r15, %rdi
	movb	%al, 1312(%rsp)
	call	crypto_declassify@PLT
	cmpb	$0, 1312(%rsp)
	jne	.L21
	movq	24(%rsp), %rbx
	movq	%r12, %rsi
	leaq	320(%rbx), %rdi
	vzeroupper
	call	CRYPTO_NAMESPACE(crypto_encode_1277x3)@PLT
	movq	%r15, %rdi
	call	Short_random
	movq	%rbx, %rdi
	movq	%r15, %rsi
	call	CRYPTO_NAMESPACE(crypto_encode_1277x3)@PLT
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r15, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_invsntrup1277)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1277xint16)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_1277xint16)@PLT
	movq	%r14, %rdx
	xorl	%ecx, %ecx
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup1277)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1277xint16)@PLT
	movq	%r12, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_1277x7879)@PLT
	movq	%rbx, %rax
	movzbl	639(%rbx), %ebx
	subq	%r13, %rax
	leaq	639(%rax), %rdx
	xorl	%eax, %eax
	cmpq	$30, %rdx
	jbe	.L20
.L19:
	vmovdqu	0(%r13,%rax), %ymm5
	movq	24(%rsp), %rcx
	vmovdqu	%ymm5, 640(%rcx,%rax)
	addq	$32, %rax
	cmpq	$2048, %rax
	jne	.L19
	vmovdqu	2048(%r13), %xmm6
	vmovdqu	%xmm6, 2688(%rcx)
	movzbl	2064(%r13), %eax
	movb	%al, 2704(%rcx)
	movzbl	2065(%r13), %eax
	movb	%al, 2705(%rcx)
	movzbl	2066(%r13), %eax
	movb	%al, 2706(%rcx)
	vzeroupper
	movq	%rcx, %r15
.L22:
	movb	$4, 639(%r15)
	leaq	639(%r15), %rsi
	movq	%r12, %rdi
	movl	$2068, %edx
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movb	%bl, 639(%r15)
	movl	$320, %esi
	vmovdqa	2592(%rsp), %xmm6
	leaq	2707(%r15), %rdi
	vmovdqu	%xmm6, 3027(%r15)
	vmovdqa	2608(%rsp), %xmm6
	vmovdqu	%xmm6, 3043(%r15)
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
	movb	%dl, 640(%rcx,%rax)
	incq	%rax
	cmpq	$2067, %rax
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
	movl	$2067, %edx
	pushq	%rbp
	.cfi_def_cfa_offset 40
	.cfi_offset 6, -40
	movq	%rdi, %rbp
	pushq	%rbx
	.cfi_def_cfa_offset 48
	.cfi_offset 3, -48
	movq	%rsi, %rbx
	movq	%r12, %rsi
	subq	$3792, %rsp
	.cfi_def_cfa_offset 3840
	leaq	1712(%rsp), %r13
	leaq	432(%rsp), %r14
	leaq	1713(%rsp), %rdi
	call	memcpy@PLT
	movq	%r13, %rsi
	movl	$2068, %edx
	movq	%r14, %rdi
	movb	$4, 1712(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movq	%r14, %rdi
	vmovdqu	432(%rsp), %xmm0
	vmovdqu	448(%rsp), %xmm1
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
	leaq	1745(%rsp), %rdi
	movq	%rbp, %rsi
	movl	$1847, %edx
	call	memcpy@PLT
	leaq	32(%rsp), %rdi
	movq	%r13, %rsi
	movl	$1880, %edx
	movb	$1, 1712(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	vmovdqu	32(%rsp), %xmm2
	vmovdqu	48(%rsp), %xmm3
	xorl	%eax, %eax
	vmovdqu	%xmm2, (%rbx)
	vmovdqu	%xmm3, 16(%rbx)
	addq	$3792, %rsp
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
	subq	$7072, %rsp
	leaq	4512(%rsp), %r12
	leaq	2656(%rsp), %r13
	movq	%rdi, 16(%rsp)
	movq	%r12, %rdi
	leaq	1376(%rsp), %r15
	call	CRYPTO_NAMESPACE(crypto_decode_1277x2627)@PLT
	movq	%rbx, %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1277x3)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_1277xint16)@PLT
	xorl	%ecx, %ecx
	movq	%r13, %rdx
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_multsntrup1277)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1277xint16)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_1277xint16)@PLT
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_scale3sntrup1277)@PLT
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1277xint16)@PLT
	movq	%r12, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_encode_1277xfreeze3)@PLT
	leaq	320(%rbx), %rsi
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(crypto_decode_1277x3)@PLT
	leaq	96(%rsp), %r11
	xorl	%ecx, %ecx
	movq	%r13, %rdx
	movq	%r15, %rsi
	movq	%r11, %rdi
	movq	%r11, 24(%rsp)
	call	CRYPTO_NAMESPACE(crypto_core_mult3sntrup1277)@PLT
	movq	24(%rsp), %r11
	xorl	%ecx, %ecx
	xorl	%edx, %edx
	movq	%r11, %rsi
	movq	%r11, %rdi
	call	CRYPTO_NAMESPACE(crypto_core_wforcesntrup1277)@PLT
	movq	24(%rsp), %r11
	movq	%r15, %rdx
	movq	%r13, %rsi
	leaq	3027(%rbx), %r9
	leaq	640(%rbx), %r8
	movq	%r12, %rdi
	movq	%r11, %rcx
	call	Hide
	movq	%r13, %rsi
	movq	%r14, %rdi
	call	CRYPTO_NAMESPACE(crypto_verify_1847)@PLT
	vmovdqu	2707(%rbx), %ymm2
	movq	%r15, %rsi
	vpxor	1377(%rsp), %ymm2, %ymm1
	vmovd	%eax, %xmm0
	vmovdqu	2739(%rbx), %ymm3
	vmovdqu	2771(%rbx), %ymm4
	movl	%eax, %r13d
	vpbroadcastb	%xmm0, %ymm0
	vmovdqu	2803(%rbx), %ymm5
	vmovdqu	2835(%rbx), %ymm6
	movl	$321, %edx
	vpand	%ymm0, %ymm1, %ymm1
	vmovdqu	2867(%rbx), %ymm7
	vpxor	1377(%rsp), %ymm1, %ymm1
	vmovdqu	2899(%rbx), %ymm2
	vmovdqu	%ymm1, 1377(%rsp)
	vpxor	1409(%rsp), %ymm3, %ymm1
	vmovdqu	2931(%rbx), %ymm3
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1409(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1409(%rsp)
	vpxor	1441(%rsp), %ymm4, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1441(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1441(%rsp)
	vpxor	1473(%rsp), %ymm5, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1473(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1473(%rsp)
	vpxor	1505(%rsp), %ymm6, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1505(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1505(%rsp)
	vpxor	1537(%rsp), %ymm7, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1537(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1537(%rsp)
	vpxor	1569(%rsp), %ymm2, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1569(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1569(%rsp)
	vpxor	1601(%rsp), %ymm3, %ymm1
	vpand	%ymm0, %ymm1, %ymm1
	vpxor	1601(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1601(%rsp)
	vmovdqu	2963(%rbx), %ymm4
	vpxor	1633(%rsp), %ymm4, %ymm1
	vmovdqu	2995(%rbx), %ymm5
	leaq	32(%rsp), %rbx
	vpand	%ymm0, %ymm1, %ymm1
	movq	%rbx, %rdi
	vpxor	1633(%rsp), %ymm1, %ymm1
	vmovdqu	%ymm1, 1633(%rsp)
	vpxor	1665(%rsp), %ymm5, %ymm1
	vpand	%ymm0, %ymm1, %ymm0
	vpxor	1665(%rsp), %ymm0, %ymm0
	vmovdqu	%ymm0, 1665(%rsp)
	vzeroupper
	incl	%r13d
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	vmovdqa	32(%rsp), %xmm6
	movq	%r14, %rsi
	movl	$1847, %edx
	vmovdqa	48(%rsp), %xmm7
	leaq	4545(%rsp), %rdi
	vmovdqu	%xmm6, 4513(%rsp)
	vmovdqu	%xmm7, 4529(%rsp)
	call	memcpy@PLT
	movq	%r12, %rsi
	movq	%rbx, %rdi
	movl	$1880, %edx
	movb	%r13b, 4512(%rsp)
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movq	16(%rsp), %r10
	vmovdqa	32(%rsp), %xmm2
	xorl	%eax, %eax
	vmovdqa	48(%rsp), %xmm3
	vmovdqu	%xmm2, (%r10)
	vmovdqu	%xmm3, 16(%r10)
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
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC4:
	.long	-1
	.long	-1
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
