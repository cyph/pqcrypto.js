.data
.section .rodata
.align 32
const_3_repeating:
.word 0x3
.word 0x3
.word 0x3
.word 0x3
.word 0x3
.word 0x3
.word 0x3
.word 0x3
.word 0x3
.word 0x3
.word 0x3
.word 0x3
.word 0x3
.word 0x3
.word 0x3
.word 0x3
shuf_b8_to_low_doubleword:
.byte 8
.byte 255
.byte 8
.byte 255
.byte 8
.byte 255
.byte 8
.byte 255
.byte 8
.byte 255
.byte 8
.byte 255
.byte 8
.byte 255
.byte 8
.byte 255
.byte 8
.byte 255
.byte 8
.byte 255
.byte 8
.byte 255
.byte 8
.byte 255
.byte 8
.byte 255
.byte 8
.byte 255
.byte 8
.byte 255
.byte 8
.byte 255
mask_ff:
.word 0xff
.word 0xff
.word 0xff
.word 0xff
.word 0xff
.word 0xff
.word 0xff
.word 0xff
.word 0xff
.word 0xff
.word 0xff
.word 0xff
.word 0xff
.word 0xff
.word 0xff
.word 0xff
mask_f:
.word 0xf
.word 0xf
.word 0xf
.word 0xf
.word 0xf
.word 0xf
.word 0xf
.word 0xf
.word 0xf
.word 0xf
.word 0xf
.word 0xf
.word 0xf
.word 0xf
.word 0xf
.word 0xf
mask_3:
.word 0x03
.word 0x03
.word 0x03
.word 0x03
.word 0x03
.word 0x03
.word 0x03
.word 0x03
.word 0x03
.word 0x03
.word 0x03
.word 0x03
.word 0x03
.word 0x03
.word 0x03
.word 0x03
.text
.hidden poly_Rq_to_S3
.global poly_Rq_to_S3
.att_syntax prefix
poly_Rq_to_S3:
vmovdqa const_3_repeating(%rip), %ymm3
vmovdqa 992(%rsi), %ymm4
vpsrlw $10, %ymm4, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm4
vpsrlw $8, %ymm4, %ymm5
vpand mask_ff(%rip), %ymm4, %ymm4
vpaddw %ymm5, %ymm4, %ymm5
vpand mask_f(%rip), %ymm5, %ymm4
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm4, %ymm5
vpand mask_3(%rip), %ymm5, %ymm4
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm4, %ymm5
vpand mask_3(%rip), %ymm5, %ymm4
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm4, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm4
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm4, %ymm5
vpsllw $1, %ymm5, %ymm4
vextracti128 $1, %ymm4, %xmm4
vpshufb shuf_b8_to_low_doubleword(%rip), %ymm4, %ymm4
vinserti128 $1, %xmm4, %ymm4, %ymm4
vmovdqa 0(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 0(%rdi)
vmovdqa 32(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 32(%rdi)
vmovdqa 64(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 64(%rdi)
vmovdqa 96(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 96(%rdi)
vmovdqa 128(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 128(%rdi)
vmovdqa 160(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 160(%rdi)
vmovdqa 192(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 192(%rdi)
vmovdqa 224(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 224(%rdi)
vmovdqa 256(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 256(%rdi)
vmovdqa 288(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 288(%rdi)
vmovdqa 320(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 320(%rdi)
vmovdqa 352(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 352(%rdi)
vmovdqa 384(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 384(%rdi)
vmovdqa 416(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 416(%rdi)
vmovdqa 448(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 448(%rdi)
vmovdqa 480(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 480(%rdi)
vmovdqa 512(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 512(%rdi)
vmovdqa 544(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 544(%rdi)
vmovdqa 576(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 576(%rdi)
vmovdqa 608(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 608(%rdi)
vmovdqa 640(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 640(%rdi)
vmovdqa 672(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 672(%rdi)
vmovdqa 704(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 704(%rdi)
vmovdqa 736(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 736(%rdi)
vmovdqa 768(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 768(%rdi)
vmovdqa 800(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 800(%rdi)
vmovdqa 832(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 832(%rdi)
vmovdqa 864(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 864(%rdi)
vmovdqa 896(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 896(%rdi)
vmovdqa 928(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 928(%rdi)
vmovdqa 960(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 960(%rdi)
vmovdqa 992(%rsi), %ymm1
vpsrlw $10, %ymm1, %ymm0
vpxor %ymm3, %ymm0, %ymm0
vpsllw $11, %ymm0, %ymm0
vpaddw %ymm1, %ymm0, %ymm0
vpaddw %ymm4, %ymm0, %ymm0
vpsrlw $8, %ymm0, %ymm5
vpand mask_ff(%rip), %ymm0, %ymm0
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_f(%rip), %ymm5, %ymm0
vpsrlw $4, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpand mask_3(%rip), %ymm5, %ymm0
vpsrlw $2, %ymm5, %ymm5
vpaddw %ymm5, %ymm0, %ymm5
vpsubw mask_3(%rip), %ymm5, %ymm14
vpsraw $15, %ymm14, %ymm15
vpandn %ymm14, %ymm15, %ymm0
vpand %ymm15, %ymm5, %ymm14
vpxor %ymm14, %ymm0, %ymm5
vmovdqa %ymm5, 992(%rdi)
ret
