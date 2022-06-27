.text
.p2align 5
.globl _CRYPTO_NAMESPACE(init)
.globl CRYPTO_NAMESPACE(init)
_CRYPTO_NAMESPACE(init):
CRYPTO_NAMESPACE(init):
mov %esp,%eax
and $31,%eax
add $0,%eax
sub %eax,%esp
fldcw CRYPTO_NAMESPACE(rounding)
add %eax,%esp
ret
