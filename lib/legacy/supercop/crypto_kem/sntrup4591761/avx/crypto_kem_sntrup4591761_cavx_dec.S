	.file	"dec.c"
	.text
	.p2align 4
	.globl	CRYPTO_NAMESPACE(dec)
	.type	CRYPTO_NAMESPACE(dec), @function
CRYPTO_NAMESPACE(dec):
.LFB18:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	xorl	%eax, %eax
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	.cfi_offset 12, -48
	movq	%rsi, %r12
	movq	%rdx, %rsi
	pushq	%rbx
	.cfi_offset 3, -56
	movq	%rdx, %rbx
	andq	$-32, %rsp
	subq	$9536, %rsp
	leaq	320(%rsp), %r14
	movq	%rdi, 56(%rsp)
	leaq	3392(%rsp), %r15
	movq	%r14, %rdi
	leaq	8000(%rsp), %r13
	call	CRYPTO_NAMESPACE(small_decode)@PLT
	leaq	1088(%rsp), %r10
	leaq	191(%rbx), %rsi
	xorl	%eax, %eax
	movq	%r10, %rdi
	movq	%r10, 16(%rsp)
	call	CRYPTO_NAMESPACE(small_decode)@PLT
	leaq	6464(%rsp), %r9
	leaq	382(%rbx), %rsi
	xorl	%eax, %eax
	movq	%r9, %rdi
	movq	%r9, 32(%rsp)
	leaq	4928(%rsp), %rbx
	call	CRYPTO_NAMESPACE(rq_decode)@PLT
	leaq	32(%r12), %rsi
	movq	%r15, %rdi
	xorl	%eax, %eax
	call	CRYPTO_NAMESPACE(rq_decoderounded)@PLT
	movq	%r14, %rdx
	movq	%r13, %rdi
	movq	%r15, %rsi
	leaq	1856(%rsp), %r14
	xorl	%eax, %eax
	call	CRYPTO_NAMESPACE(rq_mult)@PLT
	movq	%r13, %rsi
	movq	%r14, %rdi
	xorl	%eax, %eax
	call	CRYPTO_NAMESPACE(rq_mod3)@PLT
	movq	16(%rsp), %r10
	movq	%r14, %rsi
	xorl	%eax, %eax
	leaq	2624(%rsp), %r13
	movq	%r10, %rdx
	movq	%r13, %rdi
	call	CRYPTO_NAMESPACE(r3_mult)@PLT
	movq	%r13, %rdi
	xorl	%eax, %eax
	call	CRYPTO_NAMESPACE(r3_weightw_mask)@PLT
	movq	32(%rsp), %r9
	movq	%r13, %rdx
	movq	%rbx, %rdi
	movl	%eax, %r14d
	xorl	%eax, %eax
	movq	%r9, %rsi
	call	CRYPTO_NAMESPACE(rq_mult)@PLT
	movq	%rbx, %rsi
	movq	%rbx, %rdi
	xorl	%eax, %eax
	call	CRYPTO_NAMESPACE(rq_round3)@PLT
	vpxor	%xmm2, %xmm2, %xmm2
	xorl	%eax, %eax
	vmovdqa	%ymm2, %ymm3
	.p2align 4,,10
	.p2align 3
.L2:
	vmovdqa	(%rbx,%rax), %ymm4
	vpsubw	(%r15,%rax), %ymm4, %ymm0
	addq	$32, %rax
	vpmovzxwd	%xmm0, %ymm1
	vextracti128	$0x1, %ymm0, %xmm0
	vpmovzxwd	%xmm0, %ymm0
	vpsubd	%ymm1, %ymm3, %ymm1
	vpsubd	%ymm0, %ymm3, %ymm0
	vpor	%ymm1, %ymm0, %ymm0
	vpsrad	$30, %ymm0, %ymm0
	vpor	%ymm0, %ymm2, %ymm2
	cmpq	$1504, %rax
	jne	.L2
	movzwl	6432(%rsp), %r11d
	vmovdqa	%xmm2, %xmm0
	movzwl	6434(%rsp), %ebx
	vextracti128	$0x1, %ymm2, %xmm2
	subw	4896(%rsp), %r11w
	vpor	%xmm2, %xmm0, %xmm2
	subw	4898(%rsp), %bx
	movzwl	6436(%rsp), %r10d
	movzwl	%r11w, %r11d
	subw	4900(%rsp), %r10w
	movzwl	%bx, %ebx
	negl	%ebx
	negl	%r11d
	movzwl	%r10w, %r10d
	movzwl	6438(%rsp), %r9d
	subw	4902(%rsp), %r9w
	orl	%ebx, %r11d
	negl	%r10d
	movzwl	6440(%rsp), %edi
	movzwl	%r9w, %r9d
	subw	4904(%rsp), %di
	orl	%r11d, %r10d
	movzwl	6442(%rsp), %esi
	negl	%r9d
	subw	4906(%rsp), %si
	movzwl	%di, %edi
	movzwl	6444(%rsp), %ecx
	negl	%edi
	subw	4908(%rsp), %cx
	orl	%r10d, %r9d
	movzwl	%si, %esi
	orl	%r9d, %edi
	movzwl	6446(%rsp), %edx
	negl	%esi
	movzwl	%cx, %ecx
	subw	4910(%rsp), %dx
	orl	%edi, %esi
	movzwl	6448(%rsp), %eax
	negl	%ecx
	subw	4912(%rsp), %ax
	vpsrldq	$8, %xmm2, %xmm0
	movzwl	%dx, %edx
	orl	%esi, %ecx
	negl	%edx
	movzwl	%ax, %eax
	vpor	%xmm0, %xmm2, %xmm0
	movq	%r13, %rsi
	negl	%eax
	vpsrldq	$4, %xmm0, %xmm5
	orl	%ecx, %edx
	vmovaps	%xmm0, 16(%rsp)
	orl	%edx, %eax
	leaq	128(%rsp), %rbx
	vmovaps	%xmm5, 32(%rsp)
	sarl	$31, %eax
	movq	%rbx, %rdi
	orl	%eax, %r14d
	xorl	%eax, %eax
	vzeroupper
	call	CRYPTO_NAMESPACE(small_encode)@PLT
	leaq	64(%rsp), %r15
	movl	$191, %edx
	movq	%rbx, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_hash_sha512)@PLT
	movq	%r12, %rsi
	movq	%r15, %rdi
	call	CRYPTO_NAMESPACE(crypto_verify_32)@PLT
	vmovdqa	16(%rsp), %xmm0
	movq	56(%rsp), %rcx
	vpor	32(%rsp), %xmm0, %xmm0
	orl	%eax, %r14d
	vmovd	%xmm0, %eax
	orl	%r14d, %eax
	movl	%eax, %edx
	notl	%edx
	vmovd	%edx, %xmm0
	vpbroadcastb	%xmm0, %ymm0
	vpand	96(%rsp), %ymm0, %ymm0
	vmovdqu	%ymm0, (%rcx)
	vzeroupper
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
.LFE18:
	.size	CRYPTO_NAMESPACE(dec), .-CRYPTO_NAMESPACE(dec)
	.ident	"GCC: (GNU) 9.2.1 20190827 (Red Hat 9.2.1-1)"
	.section	.note.GNU-stack,"",@progbits
