	.text
	.file	"encode.c"
	.globl	CRYPTO_NAMESPACE(crypto_encode_953xint16) # -- Begin function CRYPTO_NAMESPACE(crypto_encode_953xint16)
	.type	CRYPTO_NAMESPACE(crypto_encode_953xint16),@function
CRYPTO_NAMESPACE(crypto_encode_953xint16): # @CRYPTO_NAMESPACE(crypto_encode_953xint16)
	.cfi_startproc
# %bb.0:
	cmpq	%rdi, %rsi
	je	.LBB0_1
# %bb.2:
	movl	$1906, %edx             # imm = 0x772
	jmp	memcpy@PLT              # TAILCALL
.LBB0_1:
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_encode_953xint16), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_encode_953xint16)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
