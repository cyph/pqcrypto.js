	.file	"encode.c"
	.text
	.globl	CRYPTO_NAMESPACE(crypto_encode_1013x2393round)
	.type	CRYPTO_NAMESPACE(crypto_encode_1013x2393round), @function
CRYPTO_NAMESPACE(crypto_encode_1013x2393round):
.LFB5342:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %r8
	movl	$32, %ecx
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	andq	$-32, %rsp
	subq	$904, %rsp
	.cfi_offset 3, -24
	leaq	-110(%rsp), %rax
	movq	%rax, %r11
	vmovdqa	.LC0(%rip), %ymm8
	vmovdqa	.LC1(%rip), %ymm7
	vmovdqa	.LC2(%rip), %ymm6
	vmovdqa	.LC3(%rip), %ymm5
	vmovdqa	.LC4(%rip), %ymm1
	vmovdqa	.LC5(%rip), %ymm4
	vmovdqa	.LC6(%rip), %ymm3
.L3:
	decq	%rcx
	jne	.L2
	subq	$24, %rsi
	subq	$12, %rax
	subq	$12, %rdi
.L2:
	vpmulhrsw	(%rsi), %ymm8, %ymm0
	vpaddw	%ymm0, %ymm0, %ymm9
	addq	$64, %rsi
	addq	$32, %rax
	vpaddw	%ymm7, %ymm9, %ymm9
	vpmulhrsw	-32(%rsi), %ymm8, %ymm10
	vpaddw	%ymm10, %ymm10, %ymm2
	addq	$32, %rdi
	vpaddw	%ymm0, %ymm9, %ymm9
	vpaddw	%ymm7, %ymm2, %ymm2
	vpand	%ymm6, %ymm9, %ymm9
	vpaddw	%ymm10, %ymm2, %ymm2
	vpmulhw	%ymm5, %ymm9, %ymm9
	vpand	%ymm6, %ymm2, %ymm2
	vpmulhw	%ymm5, %ymm2, %ymm2
	vpsrld	$16, %ymm9, %ymm0
	vpand	%ymm1, %ymm9, %ymm9
	vpmulld	%ymm4, %ymm0, %ymm0
	vpsrld	$16, %ymm2, %ymm10
	vpand	%ymm1, %ymm2, %ymm2
	vpaddd	%ymm9, %ymm0, %ymm0
	vpmulld	%ymm4, %ymm10, %ymm9
	vpshufb	%ymm3, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpaddd	%ymm2, %ymm9, %ymm2
	vpshufb	%ymm3, %ymm2, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vperm2i128	$49, %ymm2, %ymm0, %ymm9
	vinserti128	$1, %xmm2, %ymm0, %ymm0
	vmovdqu	%ymm9, -32(%rax)
	vmovdqu	%ymm0, -32(%rdi)
	testq	%rcx, %rcx
	jne	.L3
	movswl	2024(%r8), %eax
	imull	$10923, %eax, %eax
	addl	$16384, %eax
	sarl	$15, %eax
	leal	(%rax,%rax,2), %eax
	addl	$3588, %eax
	andl	$16383, %eax
	imull	$10923, %eax, %eax
	sarl	$15, %eax
	movw	%ax, 902(%rsp)
.L4:
	movzwl	2(%r11,%rcx,4), %esi
	movzwl	(%r11,%rcx,4), %r8d
	imull	$88, %esi, %esi
	addl	%r8d, %esi
	movw	%si, (%r11,%rcx,2)
	incq	%rcx
	cmpq	$253, %rcx
	jne	.L4
	vmovdqa	.LC7(%rip), %ymm5
	movw	%ax, 396(%rsp)
	movq	%r11, %rcx
	movq	%r11, %rax
	movl	$8, %esi
.L6:
	decq	%rsi
	jne	.L5
	subq	$4, %rax
	subq	$2, %rcx
	subq	$2, %rdi
.L5:
	vmovdqu	(%rax), %ymm7
	vpand	(%rax), %ymm1, %ymm4
	addq	$32, %rcx
	addq	$64, %rax
	addq	$32, %rdi
	vpsrld	$16, %ymm7, %ymm0
	vmovdqu	-32(%rax), %ymm7
	vpmulld	%ymm5, %ymm0, %ymm0
	vpsrld	$16, %ymm7, %ymm2
	vpmulld	%ymm5, %ymm2, %ymm2
	vpaddd	%ymm4, %ymm0, %ymm0
	vpand	-32(%rax), %ymm1, %ymm4
	vpshufb	%ymm3, %ymm0, %ymm0
	vpaddd	%ymm4, %ymm2, %ymm2
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm2, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vperm2i128	$49, %ymm2, %ymm0, %ymm4
	vinserti128	$1, %xmm2, %ymm0, %ymm0
	vmovdqu	%ymm4, -32(%rcx)
	vmovdqu	%ymm0, -32(%rdi)
	testq	%rsi, %rsi
	jne	.L6
	vmovdqa	.LC8(%rip), %ymm5
	movq	%r11, %rcx
	movq	%r11, %rsi
	movl	$8, %eax
	vmovdqa	.LC9(%rip), %ymm2
.L8:
	decq	%rax
	jne	.L7
	subq	$4, %rsi
	subq	$2, %rcx
	decq	%rdi
.L7:
	vmovdqu	(%rsi), %ymm6
	vpand	(%rsi), %ymm1, %ymm4
	addq	$16, %rcx
	addq	$32, %rsi
	addq	$8, %rdi
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm5, %ymm0, %ymm0
	vpaddd	%ymm4, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, -16(%rcx)
	vextracti128	$0x1, %ymm0, %xmm0
	vmovd	%xmm0, -8(%rdi)
	vpextrd	$2, %xmm0, -4(%rdi)
	testq	%rax, %rax
	jne	.L8
	movl	$2, %r9d
	movw	142(%rsp), %cx
	movq	%r11, %r8
	vmovdqa	.LC10(%rip), %ymm6
	movl	$2, %r10d
	movw	%cx, 16(%rsp)
	movq	%r11, %rcx
.L10:
	decq	%r10
	movq	%rdi, %rsi
	jne	.L9
	subq	$4, %rcx
	subq	$2, %r8
	subq	$2, %rsi
.L9:
	vmovdqu	(%rcx), %ymm7
	vpand	(%rcx), %ymm1, %ymm5
	addq	$32, %r8
	addq	$64, %rcx
	leaq	32(%rsi), %rdi
	movl	$1, %r10d
	vpsrld	$16, %ymm7, %ymm0
	vmovdqu	-32(%rcx), %ymm7
	vpmulld	%ymm6, %ymm0, %ymm0
	vpsrld	$16, %ymm7, %ymm4
	vpmulld	%ymm6, %ymm4, %ymm4
	vpaddd	%ymm5, %ymm0, %ymm0
	vpand	-32(%rcx), %ymm1, %ymm5
	vpshufb	%ymm3, %ymm0, %ymm0
	vpaddd	%ymm5, %ymm4, %ymm4
	vpermq	$216, %ymm0, %ymm0
	vpshufb	%ymm3, %ymm4, %ymm4
	vpermq	$216, %ymm4, %ymm4
	vperm2i128	$49, %ymm4, %ymm0, %ymm5
	vinserti128	$1, %xmm4, %ymm0, %ymm0
	vmovdqu	%ymm5, -32(%r8)
	decl	%r9d
	vmovdqu	%ymm0, (%rsi)
	je	.L25
	movl	$1, %r9d
	jmp	.L10
.L25:
	movzwl	16(%rsp), %ecx
	movzwl	14(%rsp), %edi
	vmovdqa	.LC11(%rip), %ymm4
	imull	$3278, %ecx, %ecx
	addl	%edi, %ecx
	leaq	33(%rsi), %rdi
	movb	%cl, 32(%rsi)
	shrl	$8, %ecx
	xorl	%esi, %esi
	movw	%cx, -48(%rsp)
.L11:
	vmovdqu	(%r11,%rsi,2), %ymm6
	vpand	(%r11,%rsi,2), %ymm1, %ymm3
	movq	%rdi, %rcx
	addq	$8, %rdi
	vpsrld	$16, %ymm6, %ymm0
	vpmulld	%ymm4, %ymm0, %ymm0
	vpaddd	%ymm3, %ymm0, %ymm0
	vpshufb	%ymm2, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%xmm0, (%r11,%rsi)
	vextracti128	$0x1, %ymm0, %xmm0
	addq	$16, %rsi
	vmovd	%xmm0, -8(%rdi)
	vpextrd	$2, %xmm0, -4(%rdi)
	cmpq	$32, %rsi
	jne	.L11
	xorl	%esi, %esi
.L12:
	movzwl	2(%r11,%rsi,4), %r8d
	movzwl	(%r11,%rsi,4), %r9d
	imull	$106, %r8d, %r8d
	addl	%r9d, %r8d
	movw	%r8w, (%r11,%rsi,2)
	incq	%rsi
	cmpq	$7, %rsi
	jne	.L12
	movzwl	-80(%rsp), %esi
	movzwl	-82(%rsp), %r8d
	imull	$106, %esi, %esi
	addl	%r8d, %esi
	movb	%sil, (%rdi)
	shrl	$8, %esi
	movw	%si, -96(%rsp)
.L13:
	movzwl	2(%r11,%rax,4), %ebx
	movzwl	(%r11,%rax,4), %esi
	imull	$11236, %ebx, %ebx
	addl	%esi, %ebx
	movb	%bl, 9(%rcx,%rax,2)
	movb	%bh, 10(%rcx,%rax,2)
	shrl	$16, %ebx
	movw	%bx, (%r11,%rax,2)
	incq	%rax
	cmpq	$4, %rax
	jne	.L13
	movzwl	-108(%rsp), %edx
	movzwl	-110(%rsp), %eax
	movzwl	-106(%rsp), %esi
	imull	$1927, %edx, %edx
	addl	%eax, %edx
	movzwl	-104(%rsp), %eax
	movb	%dl, 17(%rcx)
	shrl	$8, %edx
	imull	$1927, %eax, %eax
	movzwl	%dx, %edx
	addl	%esi, %eax
	movb	%al, 18(%rcx)
	shrl	$8, %eax
	movzwl	%ax, %eax
	imull	$14506, %eax, %eax
	addl	%edx, %eax
	movl	%eax, 19(%rcx)
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5342:
	.size	CRYPTO_NAMESPACE(crypto_encode_1013x2393round), .-CRYPTO_NAMESPACE(crypto_encode_1013x2393round)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.value	10923
	.align 32
.LC1:
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.value	3588
	.align 32
.LC2:
	.quad	4611474908973580287
	.quad	4611474908973580287
	.quad	4611474908973580287
	.quad	4611474908973580287
	.align 32
.LC3:
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.value	21846
	.align 32
.LC4:
	.quad	281470681808895
	.quad	281470681808895
	.quad	281470681808895
	.quad	281470681808895
	.align 32
.LC5:
	.long	2393
	.long	2393
	.long	2393
	.long	2393
	.long	2393
	.long	2393
	.long	2393
	.long	2393
	.align 32
.LC6:
	.byte	0
	.byte	1
	.byte	4
	.byte	5
	.byte	8
	.byte	9
	.byte	12
	.byte	13
	.byte	2
	.byte	3
	.byte	6
	.byte	7
	.byte	10
	.byte	11
	.byte	14
	.byte	15
	.byte	0
	.byte	1
	.byte	4
	.byte	5
	.byte	8
	.byte	9
	.byte	12
	.byte	13
	.byte	2
	.byte	3
	.byte	6
	.byte	7
	.byte	10
	.byte	11
	.byte	14
	.byte	15
	.align 32
.LC7:
	.long	7744
	.long	7744
	.long	7744
	.long	7744
	.long	7744
	.long	7744
	.long	7744
	.long	7744
	.align 32
.LC8:
	.long	916
	.long	916
	.long	916
	.long	916
	.long	916
	.long	916
	.long	916
	.long	916
	.align 32
.LC9:
	.byte	1
	.byte	2
	.byte	5
	.byte	6
	.byte	9
	.byte	10
	.byte	13
	.byte	14
	.byte	0
	.byte	4
	.byte	8
	.byte	12
	.byte	0
	.byte	4
	.byte	8
	.byte	12
	.byte	1
	.byte	2
	.byte	5
	.byte	6
	.byte	9
	.byte	10
	.byte	13
	.byte	14
	.byte	0
	.byte	4
	.byte	8
	.byte	12
	.byte	0
	.byte	4
	.byte	8
	.byte	12
	.align 32
.LC10:
	.long	3278
	.long	3278
	.long	3278
	.long	3278
	.long	3278
	.long	3278
	.long	3278
	.long	3278
	.align 32
.LC11:
	.long	164
	.long	164
	.long	164
	.long	164
	.long	164
	.long	164
	.long	164
	.long	164
	.ident	"GCC: (GNU) 10.3.1 20210422 (Red Hat 10.3.1-1)"
	.section	.note.GNU-stack,"",@progbits
