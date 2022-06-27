	.text
	.file	"decode.c"
	.globl	CRYPTO_NAMESPACE(crypto_decode_653xint32) # -- Begin function CRYPTO_NAMESPACE(crypto_decode_653xint32)
	.type	CRYPTO_NAMESPACE(crypto_decode_653xint32),@function
CRYPTO_NAMESPACE(crypto_decode_653xint32): # @CRYPTO_NAMESPACE(crypto_decode_653xint32)
	.cfi_startproc
# %bb.0:
	cmpq	%rsi, %rdi
	je	.LBB0_1
# %bb.2:
	movl	$2612, %edx             # imm = 0xA34
	jmp	memcpy@PLT              # TAILCALL
.LBB0_1:
	retq
.Lfunc_end0:
	.size	CRYPTO_NAMESPACE(crypto_decode_653xint32), .Lfunc_end0-CRYPTO_NAMESPACE(crypto_decode_653xint32)
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 10.0.1 (Fedora 10.0.1-3.fc32)"
	.section	".note.GNU-stack","",@progbits
