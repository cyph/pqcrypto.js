.data
.section .rodata
.p2align 5

.globl CRYPTO_NAMESPACE(scale)
.globl CRYPTO_NAMESPACE(121665)
.globl CRYPTO_NAMESPACE(alpha26)
.globl CRYPTO_NAMESPACE(alpha51)
.globl CRYPTO_NAMESPACE(alpha77)
.globl CRYPTO_NAMESPACE(alpha102)
.globl CRYPTO_NAMESPACE(alpha128)
.globl CRYPTO_NAMESPACE(alpha153)
.globl CRYPTO_NAMESPACE(alpha179)
.globl CRYPTO_NAMESPACE(alpha204)
.globl CRYPTO_NAMESPACE(alpha230)
.globl CRYPTO_NAMESPACE(alpha255)
.globl CRYPTO_NAMESPACE(in0offset)
.globl CRYPTO_NAMESPACE(in1offset)
.globl CRYPTO_NAMESPACE(in2offset)
.globl CRYPTO_NAMESPACE(in3offset)
.globl CRYPTO_NAMESPACE(in4offset)
.globl CRYPTO_NAMESPACE(in5offset)
.globl CRYPTO_NAMESPACE(in6offset)
.globl CRYPTO_NAMESPACE(in7offset)
.globl CRYPTO_NAMESPACE(in8offset)
.globl CRYPTO_NAMESPACE(in9offset)
.globl CRYPTO_NAMESPACE(out0offset)
.globl CRYPTO_NAMESPACE(out1offset)
.globl CRYPTO_NAMESPACE(out2offset)
.globl CRYPTO_NAMESPACE(out3offset)
.globl CRYPTO_NAMESPACE(out4offset)
.globl CRYPTO_NAMESPACE(out5offset)
.globl CRYPTO_NAMESPACE(out6offset)
.globl CRYPTO_NAMESPACE(out7offset)
.globl CRYPTO_NAMESPACE(out8offset)
.globl CRYPTO_NAMESPACE(out9offset)
.globl CRYPTO_NAMESPACE(two0)
.globl CRYPTO_NAMESPACE(two1)
.globl CRYPTO_NAMESPACE(zero)
.globl CRYPTO_NAMESPACE(rounding)

CRYPTO_NAMESPACE(scale):
	.long 0x0,0x30430000
CRYPTO_NAMESPACE(121665):
	.long 0x0,0x40fdb410
CRYPTO_NAMESPACE(in0offset):
	.long 0x0,0x43300000
CRYPTO_NAMESPACE(in1offset):
	.long 0x0,0x45300000
CRYPTO_NAMESPACE(in2offset):
	.long 0x0,0x46b00000
CRYPTO_NAMESPACE(in3offset):
	.long 0x0,0x48300000
CRYPTO_NAMESPACE(in4offset):
	.long 0x0,0x49b00000
CRYPTO_NAMESPACE(in5offset):
	.long 0x0,0x4b300000
CRYPTO_NAMESPACE(in6offset):
	.long 0x0,0x4d300000
CRYPTO_NAMESPACE(in7offset):
	.long 0x0,0x4eb00000
CRYPTO_NAMESPACE(in8offset):
	.long 0x0,0x50300000
CRYPTO_NAMESPACE(in9offset):
	.long 0x0,0x51b00000
CRYPTO_NAMESPACE(alpha26):
	.long 0x0,0x45880000
CRYPTO_NAMESPACE(alpha51):
	.long 0x0,0x47180000
CRYPTO_NAMESPACE(alpha77):
	.long 0x0,0x48b80000
CRYPTO_NAMESPACE(alpha102):
	.long 0x0,0x4a480000
CRYPTO_NAMESPACE(alpha128):
	.long 0x0,0x4be80000
CRYPTO_NAMESPACE(alpha153):
	.long 0x0,0x4d780000
CRYPTO_NAMESPACE(alpha179):
	.long 0x0,0x4f180000
CRYPTO_NAMESPACE(alpha204):
	.long 0x0,0x50a80000
CRYPTO_NAMESPACE(alpha230):
	.long 0x0,0x52480000
CRYPTO_NAMESPACE(alpha255):
	.long 0x0,0x53d80000
CRYPTO_NAMESPACE(two0):
	.long 0x0,0x3ff00000
CRYPTO_NAMESPACE(two1):
	.long 0x0,0x40000000
CRYPTO_NAMESPACE(zero):
	.long 0x0,0x0
CRYPTO_NAMESPACE(out0offset):
	.long 0x1fffffed,0x43380000
CRYPTO_NAMESPACE(out1offset):
	.long 0xffffff8,0x44d80000
CRYPTO_NAMESPACE(out2offset):
	.long 0x1ffffff8,0x46680000
CRYPTO_NAMESPACE(out3offset):
	.long 0xffffff8,0x48080000
CRYPTO_NAMESPACE(out4offset):
	.long 0x1ffffff8,0x49980000
CRYPTO_NAMESPACE(out5offset):
	.long 0xffffff8,0x4b380000
CRYPTO_NAMESPACE(out6offset):
	.long 0x1ffffff8,0x4cc80000
CRYPTO_NAMESPACE(out7offset):
	.long 0xffffff8,0x4e680000
CRYPTO_NAMESPACE(out8offset):
	.long 0x1ffffff8,0x4ff80000
CRYPTO_NAMESPACE(out9offset):
	.long 0x1fffff8,0x51980000
CRYPTO_NAMESPACE(rounding):
	.byte 0x7f
	.byte 0x13
