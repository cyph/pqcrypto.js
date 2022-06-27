	.file	"mult.c"
	.text
	.p2align 4
	.type	mult24x8_float, @function
mult24x8_float:
.LFB5305:
	.cfi_startproc
	vmovaps	(%rsi), %ymm4
	vmovaps	32(%rsi), %ymm5
	movq	%rdi, %rcx
	addq	$192, %rsi
	vmovaps	-128(%rsi), %ymm6
	vmovaps	(%rdx), %ymm12
	leaq	576(%rdi), %r8
	vmovaps	-96(%rsi), %ymm7
	vmovaps	-64(%rsi), %ymm8
	vmovaps	-32(%rsi), %ymm9
	vmovaps	32(%rdx), %ymm13
	vmovaps	64(%rdx), %ymm14
	vmovaps	96(%rdx), %ymm15
#APP
# 150 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vmulps %ymm12,%ymm4,%ymm0 
	vmovups %ymm0,(%rdi) 
	vmulps %ymm12,%ymm5,%ymm10 
	vmulps %ymm12,%ymm6,%ymm1 
	vmulps %ymm12,%ymm7,%ymm2 
	vmulps %ymm12,%ymm8,%ymm3 
	vmulps %ymm12,%ymm9,%ymm0 
	
# 0 "" 2
#NO_APP
	vmovaps	128(%rdx), %ymm12
#APP
# 152 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm13,%ymm4,%ymm10 
	vmovups %ymm10,32(%rdi) 
	vfmadd231ps %ymm13,%ymm5,%ymm1 
	vfmadd231ps %ymm13,%ymm6,%ymm2 
	vfmadd231ps %ymm13,%ymm7,%ymm3 
	vfmadd231ps %ymm13,%ymm8,%ymm0 
	vmulps %ymm13,%ymm9,%ymm10 
	
# 0 "" 2
# 153 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm14,%ymm4,%ymm1 
	vmovups %ymm1,64(%rdi) 
	vfmadd231ps %ymm14,%ymm5,%ymm2 
	vfmadd231ps %ymm14,%ymm6,%ymm3 
	vfmadd231ps %ymm14,%ymm7,%ymm0 
	vfmadd231ps %ymm14,%ymm8,%ymm10 
	vmulps %ymm14,%ymm9,%ymm1 
	
# 0 "" 2
# 154 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm15,%ymm4,%ymm2 
	vmovups %ymm2,96(%rdi) 
	vfmadd231ps %ymm15,%ymm5,%ymm3 
	vfmadd231ps %ymm15,%ymm6,%ymm0 
	vfmadd231ps %ymm15,%ymm7,%ymm10 
	vfmadd231ps %ymm15,%ymm8,%ymm1 
	vmulps %ymm15,%ymm9,%ymm2 
	
# 0 "" 2
# 155 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm12,%ymm4,%ymm3 
	vmovups %ymm3,128(%rdi) 
	vfmadd231ps %ymm12,%ymm5,%ymm0 
	vfmadd231ps %ymm12,%ymm6,%ymm10 
	vfmadd231ps %ymm12,%ymm7,%ymm1 
	vfmadd231ps %ymm12,%ymm8,%ymm2 
	vmulps %ymm12,%ymm9,%ymm3 
	
# 0 "" 2
#NO_APP
	vmovaps	160(%rdx), %ymm13
	vmovaps	192(%rdx), %ymm14
	vmovaps	224(%rdx), %ymm15
	vmovaps	256(%rdx), %ymm12
#APP
# 156 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm13,%ymm4,%ymm0 
	vmovups %ymm0,160(%rdi) 
	vfmadd231ps %ymm13,%ymm5,%ymm10 
	vfmadd231ps %ymm13,%ymm6,%ymm1 
	vfmadd231ps %ymm13,%ymm7,%ymm2 
	vfmadd231ps %ymm13,%ymm8,%ymm3 
	vmulps %ymm13,%ymm9,%ymm0 
	
# 0 "" 2
#NO_APP
	vmovaps	288(%rdx), %ymm13
#APP
# 152 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm14,%ymm4,%ymm10 
	vmovups %ymm10,192(%rdi) 
	vfmadd231ps %ymm14,%ymm5,%ymm1 
	vfmadd231ps %ymm14,%ymm6,%ymm2 
	vfmadd231ps %ymm14,%ymm7,%ymm3 
	vfmadd231ps %ymm14,%ymm8,%ymm0 
	vmulps %ymm14,%ymm9,%ymm10 
	
# 0 "" 2
# 153 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm15,%ymm4,%ymm1 
	vmovups %ymm1,224(%rdi) 
	vfmadd231ps %ymm15,%ymm5,%ymm2 
	vfmadd231ps %ymm15,%ymm6,%ymm3 
	vfmadd231ps %ymm15,%ymm7,%ymm0 
	vfmadd231ps %ymm15,%ymm8,%ymm10 
	vmulps %ymm15,%ymm9,%ymm1 
	
# 0 "" 2
# 154 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm12,%ymm4,%ymm2 
	vmovups %ymm2,256(%rdi) 
	vfmadd231ps %ymm12,%ymm5,%ymm3 
	vfmadd231ps %ymm12,%ymm6,%ymm0 
	vfmadd231ps %ymm12,%ymm7,%ymm10 
	vfmadd231ps %ymm12,%ymm8,%ymm1 
	vmulps %ymm12,%ymm9,%ymm2 
	
# 0 "" 2
# 155 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm13,%ymm4,%ymm3 
	vmovups %ymm3,288(%rdi) 
	vfmadd231ps %ymm13,%ymm5,%ymm0 
	vfmadd231ps %ymm13,%ymm6,%ymm10 
	vfmadd231ps %ymm13,%ymm7,%ymm1 
	vfmadd231ps %ymm13,%ymm8,%ymm2 
	vmulps %ymm13,%ymm9,%ymm3 
	
# 0 "" 2
#NO_APP
	vmovaps	320(%rdx), %ymm14
	vmovaps	384(%rdx), %ymm12
	vmovaps	416(%rdx), %ymm13
	vmovaps	352(%rdx), %ymm15
#APP
# 156 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm14,%ymm4,%ymm0 
	vmovups %ymm0,320(%rdi) 
	vfmadd231ps %ymm14,%ymm5,%ymm10 
	vfmadd231ps %ymm14,%ymm6,%ymm1 
	vfmadd231ps %ymm14,%ymm7,%ymm2 
	vfmadd231ps %ymm14,%ymm8,%ymm3 
	vmulps %ymm14,%ymm9,%ymm0 
	
# 0 "" 2
#NO_APP
	vmovaps	448(%rdx), %ymm14
#APP
# 152 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm15,%ymm4,%ymm10 
	vmovups %ymm10,352(%rdi) 
	vfmadd231ps %ymm15,%ymm5,%ymm1 
	vfmadd231ps %ymm15,%ymm6,%ymm2 
	vfmadd231ps %ymm15,%ymm7,%ymm3 
	vfmadd231ps %ymm15,%ymm8,%ymm0 
	vmulps %ymm15,%ymm9,%ymm10 
	
# 0 "" 2
# 153 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm12,%ymm4,%ymm1 
	vmovups %ymm1,384(%rdi) 
	vfmadd231ps %ymm12,%ymm5,%ymm2 
	vfmadd231ps %ymm12,%ymm6,%ymm3 
	vfmadd231ps %ymm12,%ymm7,%ymm0 
	vfmadd231ps %ymm12,%ymm8,%ymm10 
	vmulps %ymm12,%ymm9,%ymm1 
	
# 0 "" 2
# 154 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm13,%ymm4,%ymm2 
	vmovups %ymm2,416(%rdi) 
	vfmadd231ps %ymm13,%ymm5,%ymm3 
	vfmadd231ps %ymm13,%ymm6,%ymm0 
	vfmadd231ps %ymm13,%ymm7,%ymm10 
	vfmadd231ps %ymm13,%ymm8,%ymm1 
	vmulps %ymm13,%ymm9,%ymm2 
	
# 0 "" 2
# 155 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm14,%ymm4,%ymm3 
	vmovups %ymm3,448(%rdi) 
	vfmadd231ps %ymm14,%ymm5,%ymm0 
	vfmadd231ps %ymm14,%ymm6,%ymm10 
	vfmadd231ps %ymm14,%ymm7,%ymm1 
	vfmadd231ps %ymm14,%ymm8,%ymm2 
	vmulps %ymm14,%ymm9,%ymm3 
	
# 0 "" 2
#NO_APP
	vmovaps	480(%rdx), %ymm15
	vmovaps	512(%rdx), %ymm12
	vmovaps	544(%rdx), %ymm13
	vmovaps	576(%rdx), %ymm14
#APP
# 156 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm15,%ymm4,%ymm0 
	vmovups %ymm0,480(%rdi) 
	vfmadd231ps %ymm15,%ymm5,%ymm10 
	vfmadd231ps %ymm15,%ymm6,%ymm1 
	vfmadd231ps %ymm15,%ymm7,%ymm2 
	vfmadd231ps %ymm15,%ymm8,%ymm3 
	vmulps %ymm15,%ymm9,%ymm0 
	
# 0 "" 2
#NO_APP
	vmovaps	640(%rdx), %ymm11
	vmovaps	608(%rdx), %ymm15
#APP
# 152 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm12,%ymm4,%ymm10 
	vmovups %ymm10,512(%rdi) 
	vfmadd231ps %ymm12,%ymm5,%ymm1 
	vfmadd231ps %ymm12,%ymm6,%ymm2 
	vfmadd231ps %ymm12,%ymm7,%ymm3 
	vfmadd231ps %ymm12,%ymm8,%ymm0 
	vmulps %ymm12,%ymm9,%ymm10 
	
# 0 "" 2
# 153 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm13,%ymm4,%ymm1 
	vmovups %ymm1,544(%rdi) 
	vfmadd231ps %ymm13,%ymm5,%ymm2 
	vfmadd231ps %ymm13,%ymm6,%ymm3 
	vfmadd231ps %ymm13,%ymm7,%ymm0 
	vfmadd231ps %ymm13,%ymm8,%ymm10 
	vmulps %ymm13,%ymm9,%ymm1 
	
# 0 "" 2
# 154 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm14,%ymm4,%ymm2 
	vmovups %ymm2,576(%rdi) 
	vfmadd231ps %ymm14,%ymm5,%ymm3 
	vfmadd231ps %ymm14,%ymm6,%ymm0 
	vfmadd231ps %ymm14,%ymm7,%ymm10 
	vfmadd231ps %ymm14,%ymm8,%ymm1 
	vmulps %ymm14,%ymm9,%ymm2 
	
# 0 "" 2
# 155 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm15,%ymm4,%ymm3 
	vmovups %ymm3,608(%rdi) 
	vfmadd231ps %ymm15,%ymm5,%ymm0 
	vfmadd231ps %ymm15,%ymm6,%ymm10 
	vfmadd231ps %ymm15,%ymm7,%ymm1 
	vfmadd231ps %ymm15,%ymm8,%ymm2 
	vmulps %ymm15,%ymm9,%ymm3 
	
# 0 "" 2
# 156 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm11,%ymm4,%ymm0 
	vmovups %ymm0,640(%rdi) 
	vfmadd231ps %ymm11,%ymm5,%ymm10 
	vfmadd231ps %ymm11,%ymm6,%ymm1 
	vfmadd231ps %ymm11,%ymm7,%ymm2 
	vfmadd231ps %ymm11,%ymm8,%ymm3 
	vmulps %ymm11,%ymm9,%ymm0 
	
# 0 "" 2
#NO_APP
	vmovaps	672(%rdx), %ymm12
	vmovaps	704(%rdx), %ymm13
	vmovaps	736(%rdx), %ymm14
#APP
# 158 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm12,%ymm4,%ymm10 
	vmovups %ymm10,672(%rdi) 
	vfmadd231ps %ymm12,%ymm5,%ymm1 
	vfmadd231ps %ymm12,%ymm6,%ymm2 
	vfmadd231ps %ymm12,%ymm7,%ymm3 
	vfmadd231ps %ymm12,%ymm8,%ymm0 
	vmulps %ymm12,%ymm9,%ymm10 
	
# 0 "" 2
# 159 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm13,%ymm4,%ymm1 
	vmovups %ymm1,704(%rdi) 
	vfmadd231ps %ymm13,%ymm5,%ymm2 
	vfmadd231ps %ymm13,%ymm6,%ymm3 
	vfmadd231ps %ymm13,%ymm7,%ymm0 
	vfmadd231ps %ymm13,%ymm8,%ymm10 
	vmulps %ymm13,%ymm9,%ymm1 
	
# 0 "" 2
# 160 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm14,%ymm4,%ymm2 
	vmovups %ymm2,736(%rdi) 
	vfmadd231ps %ymm14,%ymm5,%ymm3 
	vfmadd231ps %ymm14,%ymm6,%ymm0 
	vfmadd231ps %ymm14,%ymm7,%ymm10 
	vfmadd231ps %ymm14,%ymm8,%ymm1 
	vmulps %ymm14,%ymm9,%ymm2 
	
# 0 "" 2
#NO_APP
	vmovaps	%ymm3, 768(%rdi)
	vmovaps	%ymm0, 800(%rdi)
	vmovaps	%ymm10, 832(%rdi)
	vmovaps	%ymm1, 864(%rdi)
	vmovaps	%ymm2, 896(%rdi)
.L2:
	movq	%rcx, %rax
	addq	$192, %rcx
	vmovaps	(%rsi), %ymm2
	vmovaps	32(%rsi), %ymm3
	vmovaps	64(%rsi), %ymm4
	vmovaps	96(%rsi), %ymm5
	addq	$192, %rsi
	vmovaps	-64(%rsi), %ymm6
	vmovaps	-32(%rsi), %ymm7
	vmovaps	(%rdx), %ymm12
	vmovaps	192(%rax), %ymm0
	vmovaps	224(%rax), %ymm9
	vmovaps	256(%rax), %ymm11
	vmovaps	288(%rax), %ymm1
	vmovaps	320(%rax), %ymm8
	vmovaps	32(%rdx), %ymm13
#APP
# 180 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm12,%ymm2,%ymm0 
	vmovups %ymm0,(%rcx) 
	vmovups 352(%rax),%ymm0 
	vfmadd231ps %ymm12,%ymm3,%ymm9 
	vfmadd231ps %ymm12,%ymm4,%ymm11 
	vfmadd231ps %ymm12,%ymm5,%ymm1 
	vfmadd231ps %ymm12,%ymm6,%ymm8 
	vfmadd231ps %ymm12,%ymm7,%ymm0 
	
# 0 "" 2
# 181 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm13,%ymm2,%ymm9 
	vmovups %ymm9,224(%rax) 
	vmovups 384(%rax),%ymm9 
	vfmadd231ps %ymm13,%ymm3,%ymm11 
	vfmadd231ps %ymm13,%ymm4,%ymm1 
	vfmadd231ps %ymm13,%ymm5,%ymm8 
	vfmadd231ps %ymm13,%ymm6,%ymm0 
	vfmadd231ps %ymm13,%ymm7,%ymm9 
	
# 0 "" 2
#NO_APP
	vmovaps	64(%rdx), %ymm14
	vmovaps	96(%rdx), %ymm15
#APP
# 182 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm14,%ymm2,%ymm11 
	vmovups %ymm11,256(%rax) 
	vmovups 416(%rax),%ymm11 
	vfmadd231ps %ymm14,%ymm3,%ymm1 
	vfmadd231ps %ymm14,%ymm4,%ymm8 
	vfmadd231ps %ymm14,%ymm5,%ymm0 
	vfmadd231ps %ymm14,%ymm6,%ymm9 
	vfmadd231ps %ymm14,%ymm7,%ymm11 
	
# 0 "" 2
#NO_APP
	vmovaps	128(%rdx), %ymm12
#APP
# 183 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm15,%ymm2,%ymm1 
	vmovups %ymm1,288(%rax) 
	vmovups 448(%rax),%ymm1 
	vfmadd231ps %ymm15,%ymm3,%ymm8 
	vfmadd231ps %ymm15,%ymm4,%ymm0 
	vfmadd231ps %ymm15,%ymm5,%ymm9 
	vfmadd231ps %ymm15,%ymm6,%ymm11 
	vfmadd231ps %ymm15,%ymm7,%ymm1 
	
# 0 "" 2
# 184 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm12,%ymm2,%ymm8 
	vmovups %ymm8,320(%rax) 
	vmovups 480(%rax),%ymm8 
	vfmadd231ps %ymm12,%ymm3,%ymm0 
	vfmadd231ps %ymm12,%ymm4,%ymm9 
	vfmadd231ps %ymm12,%ymm5,%ymm11 
	vfmadd231ps %ymm12,%ymm6,%ymm1 
	vfmadd231ps %ymm12,%ymm7,%ymm8 
	
# 0 "" 2
#NO_APP
	vmovaps	160(%rdx), %ymm13
	vmovaps	192(%rdx), %ymm14
	vmovaps	%ymm0, %ymm10
	vmovaps	%ymm11, %ymm0
	vmovaps	224(%rdx), %ymm15
#APP
# 180 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm13,%ymm2,%ymm10 
	vmovups %ymm10,352(%rax) 
	vmovups 512(%rax),%ymm10 
	vfmadd231ps %ymm13,%ymm3,%ymm9 
	vfmadd231ps %ymm13,%ymm4,%ymm0 
	vfmadd231ps %ymm13,%ymm5,%ymm1 
	vfmadd231ps %ymm13,%ymm6,%ymm8 
	vfmadd231ps %ymm13,%ymm7,%ymm10 
	
# 0 "" 2
# 181 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm14,%ymm2,%ymm9 
	vmovups %ymm9,384(%rax) 
	vmovups 544(%rax),%ymm9 
	vfmadd231ps %ymm14,%ymm3,%ymm0 
	vfmadd231ps %ymm14,%ymm4,%ymm1 
	vfmadd231ps %ymm14,%ymm5,%ymm8 
	vfmadd231ps %ymm14,%ymm6,%ymm10 
	vfmadd231ps %ymm14,%ymm7,%ymm9 
	
# 0 "" 2
# 182 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm15,%ymm2,%ymm0 
	vmovups %ymm0,416(%rax) 
	vmovups 576(%rax),%ymm0 
	vfmadd231ps %ymm15,%ymm3,%ymm1 
	vfmadd231ps %ymm15,%ymm4,%ymm8 
	vfmadd231ps %ymm15,%ymm5,%ymm10 
	vfmadd231ps %ymm15,%ymm6,%ymm9 
	vfmadd231ps %ymm15,%ymm7,%ymm0 
	
# 0 "" 2
#NO_APP
	vmovaps	256(%rdx), %ymm12
	vmovaps	288(%rdx), %ymm13
#APP
# 183 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm12,%ymm2,%ymm1 
	vmovups %ymm1,448(%rax) 
	vmovups 608(%rax),%ymm1 
	vfmadd231ps %ymm12,%ymm3,%ymm8 
	vfmadd231ps %ymm12,%ymm4,%ymm10 
	vfmadd231ps %ymm12,%ymm5,%ymm9 
	vfmadd231ps %ymm12,%ymm6,%ymm0 
	vfmadd231ps %ymm12,%ymm7,%ymm1 
	
# 0 "" 2
#NO_APP
	vmovaps	320(%rdx), %ymm14
#APP
# 184 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm13,%ymm2,%ymm8 
	vmovups %ymm8,480(%rax) 
	vmovups 640(%rax),%ymm8 
	vfmadd231ps %ymm13,%ymm3,%ymm10 
	vfmadd231ps %ymm13,%ymm4,%ymm9 
	vfmadd231ps %ymm13,%ymm5,%ymm0 
	vfmadd231ps %ymm13,%ymm6,%ymm1 
	vfmadd231ps %ymm13,%ymm7,%ymm8 
	
# 0 "" 2
# 180 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm14,%ymm2,%ymm10 
	vmovups %ymm10,512(%rax) 
	vmovups 672(%rax),%ymm10 
	vfmadd231ps %ymm14,%ymm3,%ymm9 
	vfmadd231ps %ymm14,%ymm4,%ymm0 
	vfmadd231ps %ymm14,%ymm5,%ymm1 
	vfmadd231ps %ymm14,%ymm6,%ymm8 
	vfmadd231ps %ymm14,%ymm7,%ymm10 
	
# 0 "" 2
#NO_APP
	vmovaps	352(%rdx), %ymm15
	vmovaps	384(%rdx), %ymm12
#APP
# 181 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm15,%ymm2,%ymm9 
	vmovups %ymm9,544(%rax) 
	vmovups 704(%rax),%ymm9 
	vfmadd231ps %ymm15,%ymm3,%ymm0 
	vfmadd231ps %ymm15,%ymm4,%ymm1 
	vfmadd231ps %ymm15,%ymm5,%ymm8 
	vfmadd231ps %ymm15,%ymm6,%ymm10 
	vfmadd231ps %ymm15,%ymm7,%ymm9 
	
# 0 "" 2
#NO_APP
	vmovaps	416(%rdx), %ymm13
#APP
# 182 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm12,%ymm2,%ymm0 
	vmovups %ymm0,576(%rax) 
	vmovups 736(%rax),%ymm0 
	vfmadd231ps %ymm12,%ymm3,%ymm1 
	vfmadd231ps %ymm12,%ymm4,%ymm8 
	vfmadd231ps %ymm12,%ymm5,%ymm10 
	vfmadd231ps %ymm12,%ymm6,%ymm9 
	vfmadd231ps %ymm12,%ymm7,%ymm0 
	
# 0 "" 2
# 183 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm13,%ymm2,%ymm1 
	vmovups %ymm1,608(%rax) 
	vmovups 768(%rax),%ymm1 
	vfmadd231ps %ymm13,%ymm3,%ymm8 
	vfmadd231ps %ymm13,%ymm4,%ymm10 
	vfmadd231ps %ymm13,%ymm5,%ymm9 
	vfmadd231ps %ymm13,%ymm6,%ymm0 
	vfmadd231ps %ymm13,%ymm7,%ymm1 
	
# 0 "" 2
#NO_APP
	vmovaps	448(%rdx), %ymm14
	vmovaps	480(%rdx), %ymm15
#APP
# 184 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm14,%ymm2,%ymm8 
	vmovups %ymm8,640(%rax) 
	vmovups 800(%rax),%ymm8 
	vfmadd231ps %ymm14,%ymm3,%ymm10 
	vfmadd231ps %ymm14,%ymm4,%ymm9 
	vfmadd231ps %ymm14,%ymm5,%ymm0 
	vfmadd231ps %ymm14,%ymm6,%ymm1 
	vfmadd231ps %ymm14,%ymm7,%ymm8 
	
# 0 "" 2
#NO_APP
	vmovaps	512(%rdx), %ymm12
#APP
# 186 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm15,%ymm2,%ymm10 
	vmovups %ymm10,672(%rax) 
	vmovups 832(%rax),%ymm10 
	vfmadd231ps %ymm15,%ymm3,%ymm9 
	vfmadd231ps %ymm15,%ymm4,%ymm0 
	vfmadd231ps %ymm15,%ymm5,%ymm1 
	vfmadd231ps %ymm15,%ymm6,%ymm8 
	vfmadd231ps %ymm15,%ymm7,%ymm10 
	
# 0 "" 2
# 187 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm12,%ymm2,%ymm9 
	vmovups %ymm9,704(%rax) 
	vmovups 864(%rax),%ymm9 
	vfmadd231ps %ymm12,%ymm3,%ymm0 
	vfmadd231ps %ymm12,%ymm4,%ymm1 
	vfmadd231ps %ymm12,%ymm5,%ymm8 
	vfmadd231ps %ymm12,%ymm6,%ymm10 
	vfmadd231ps %ymm12,%ymm7,%ymm9 
	
# 0 "" 2
#NO_APP
	vmovaps	544(%rdx), %ymm13
	vmovaps	576(%rdx), %ymm14
	vmovaps	608(%rdx), %ymm15
	vmovaps	640(%rdx), %ymm12
#APP
# 188 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm13,%ymm2,%ymm0 
	vmovups %ymm0,736(%rax) 
	vmovups 896(%rax),%ymm0 
	vfmadd231ps %ymm13,%ymm3,%ymm1 
	vfmadd231ps %ymm13,%ymm4,%ymm8 
	vfmadd231ps %ymm13,%ymm5,%ymm10 
	vfmadd231ps %ymm13,%ymm6,%ymm9 
	vfmadd231ps %ymm13,%ymm7,%ymm0 
	
# 0 "" 2
# 189 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm14,%ymm2,%ymm1 
	vmovups %ymm1,768(%rax) 
	vfmadd231ps %ymm14,%ymm3,%ymm8 
	vfmadd231ps %ymm14,%ymm4,%ymm10 
	vfmadd231ps %ymm14,%ymm5,%ymm9 
	vfmadd231ps %ymm14,%ymm6,%ymm0 
	vmulps %ymm14,%ymm7,%ymm1 
	
# 0 "" 2
# 190 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm15,%ymm2,%ymm8 
	vmovups %ymm8,800(%rax) 
	vfmadd231ps %ymm15,%ymm3,%ymm10 
	vfmadd231ps %ymm15,%ymm4,%ymm9 
	vfmadd231ps %ymm15,%ymm5,%ymm0 
	vfmadd231ps %ymm15,%ymm6,%ymm1 
	vmulps %ymm15,%ymm7,%ymm8 
	
# 0 "" 2
# 191 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm12,%ymm2,%ymm10 
	vmovups %ymm10,832(%rax) 
	vfmadd231ps %ymm12,%ymm3,%ymm9 
	vfmadd231ps %ymm12,%ymm4,%ymm0 
	vfmadd231ps %ymm12,%ymm5,%ymm1 
	vfmadd231ps %ymm12,%ymm6,%ymm8 
	vmulps %ymm12,%ymm7,%ymm10 
	
# 0 "" 2
#NO_APP
	vmovaps	672(%rdx), %ymm13
	vmovaps	704(%rdx), %ymm14
	vmovaps	736(%rdx), %ymm15
#APP
# 192 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm13,%ymm2,%ymm9 
	vmovups %ymm9,864(%rax) 
	vfmadd231ps %ymm13,%ymm3,%ymm0 
	vfmadd231ps %ymm13,%ymm4,%ymm1 
	vfmadd231ps %ymm13,%ymm5,%ymm8 
	vfmadd231ps %ymm13,%ymm6,%ymm10 
	vmulps %ymm13,%ymm7,%ymm9 
	
# 0 "" 2
# 193 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm14,%ymm2,%ymm0 
	vmovups %ymm0,896(%rax) 
	vfmadd231ps %ymm14,%ymm3,%ymm1 
	vfmadd231ps %ymm14,%ymm4,%ymm8 
	vfmadd231ps %ymm14,%ymm5,%ymm10 
	vfmadd231ps %ymm14,%ymm6,%ymm9 
	vmulps %ymm14,%ymm7,%ymm0 
	
# 0 "" 2
# 194 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vfmadd231ps %ymm15,%ymm2,%ymm1 
	vmovups %ymm1,928(%rax) 
	vfmadd231ps %ymm15,%ymm3,%ymm8 
	vfmadd231ps %ymm15,%ymm4,%ymm10 
	vfmadd231ps %ymm15,%ymm5,%ymm9 
	vfmadd231ps %ymm15,%ymm6,%ymm0 
	vmulps %ymm15,%ymm7,%ymm1 
	
# 0 "" 2
#NO_APP
	vmovaps	%ymm8, 960(%rax)
	vmovaps	%ymm10, 992(%rax)
	vmovaps	%ymm9, 1024(%rax)
	vmovaps	%ymm0, 1056(%rax)
	vmovaps	%ymm1, 1088(%rax)
	cmpq	%rcx, %r8
	jne	.L2
	vxorps	%xmm0, %xmm0, %xmm0
	vmovaps	%ymm0, 1504(%rdi)
	vzeroupper
	ret
	.cfi_endproc
.LFE5305:
	.size	mult24x8_float, .-mult24x8_float
	.p2align 4
	.type	transpose16, @function
transpose16:
.LFB5309:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	andq	$-32, %rsp
	vmovdqa	(%rdi), %ymm0
	vmovdqa	64(%rdi), %ymm5
	vpunpcklwd	32(%rdi), %ymm0, %ymm3
	vpunpckhwd	32(%rdi), %ymm0, %ymm1
	vpunpcklwd	96(%rdi), %ymm5, %ymm2
	vpunpckhwd	96(%rdi), %ymm5, %ymm0
	vmovdqa	192(%rdi), %ymm5
	vpunpcklwd	224(%rdi), %ymm5, %ymm4
	vpunpckldq	%ymm2, %ymm3, %ymm14
	vpunpckldq	%ymm0, %ymm1, %ymm12
	vpunpckhdq	%ymm2, %ymm3, %ymm2
	vpunpckhdq	%ymm0, %ymm1, %ymm0
	vmovdqa	128(%rdi), %ymm1
	vpunpckhwd	160(%rdi), %ymm1, %ymm3
	vpunpcklwd	160(%rdi), %ymm1, %ymm8
	vpunpckhwd	224(%rdi), %ymm5, %ymm1
	vpunpckhdq	%ymm1, %ymm3, %ymm7
	vpunpckldq	%ymm4, %ymm8, %ymm6
	vpunpckldq	%ymm1, %ymm3, %ymm5
	vmovdqa	256(%rdi), %ymm1
	vpunpckhdq	%ymm4, %ymm8, %ymm8
	vpunpckhwd	288(%rdi), %ymm1, %ymm3
	vmovdqa	320(%rdi), %ymm4
	vmovdqa	%ymm7, -32(%rsp)
	vpunpcklwd	352(%rdi), %ymm4, %ymm4
	vpunpcklwd	288(%rdi), %ymm1, %ymm7
	vmovdqa	320(%rdi), %ymm1
	vpunpckhwd	352(%rdi), %ymm1, %ymm1
	vpunpckldq	%ymm4, %ymm7, %ymm13
	vpunpckhdq	%ymm4, %ymm7, %ymm4
	vpunpckldq	%ymm1, %ymm3, %ymm9
	vpunpckhdq	%ymm1, %ymm3, %ymm1
	vmovdqa	384(%rdi), %ymm3
	vpunpcklwd	416(%rdi), %ymm3, %ymm11
	vpunpckhwd	416(%rdi), %ymm3, %ymm15
	vmovdqa	448(%rdi), %ymm3
	vpunpcklwd	480(%rdi), %ymm3, %ymm7
	vpunpckhwd	480(%rdi), %ymm3, %ymm3
	vpunpckldq	%ymm7, %ymm11, %ymm10
	vpunpckhdq	%ymm7, %ymm11, %ymm7
	vpunpckldq	%ymm3, %ymm15, %ymm11
	vpunpckhdq	%ymm3, %ymm15, %ymm3
	vpunpcklqdq	%ymm6, %ymm14, %ymm15
	vpunpckhqdq	%ymm6, %ymm14, %ymm6
	vpunpcklqdq	%ymm10, %ymm13, %ymm14
	vpunpckhqdq	%ymm10, %ymm13, %ymm10
	vperm2i128	$32, %ymm14, %ymm15, %ymm13
	vperm2i128	$49, %ymm14, %ymm15, %ymm15
	vperm2i128	$32, %ymm10, %ymm6, %ymm14
	vperm2i128	$49, %ymm10, %ymm6, %ymm6
	vmovdqa	%ymm13, (%rdi)
	vpunpcklqdq	%ymm11, %ymm9, %ymm10
	vpunpckhqdq	%ymm11, %ymm9, %ymm9
	vmovdqa	%ymm15, 256(%rdi)
	vmovdqa	%ymm14, 32(%rdi)
	vmovdqa	%ymm6, 288(%rdi)
	vpunpcklqdq	%ymm5, %ymm12, %ymm6
	vpunpckhqdq	%ymm5, %ymm12, %ymm5
	vperm2i128	$32, %ymm10, %ymm6, %ymm11
	vperm2i128	$49, %ymm10, %ymm6, %ymm6
	vperm2i128	$32, %ymm9, %ymm5, %ymm10
	vmovdqa	%ymm6, 384(%rdi)
	vperm2i128	$49, %ymm9, %ymm5, %ymm9
	vpunpcklqdq	%ymm7, %ymm4, %ymm6
	vpunpcklqdq	%ymm8, %ymm2, %ymm5
	vpunpckhqdq	%ymm7, %ymm4, %ymm4
	vperm2i128	$32, %ymm6, %ymm5, %ymm7
	vpunpckhqdq	%ymm8, %ymm2, %ymm2
	vmovdqa	%ymm11, 128(%rdi)
	vmovdqa	%ymm7, 64(%rdi)
	vmovdqa	-32(%rsp), %ymm7
	vperm2i128	$49, %ymm6, %ymm5, %ymm5
	vperm2i128	$32, %ymm4, %ymm2, %ymm6
	vperm2i128	$49, %ymm4, %ymm2, %ymm2
	vpunpcklqdq	%ymm3, %ymm1, %ymm4
	vpunpckhqdq	%ymm3, %ymm1, %ymm1
	vmovdqa	%ymm5, 320(%rdi)
	vmovdqa	%ymm2, 352(%rdi)
	vpunpcklqdq	%ymm7, %ymm0, %ymm2
	vpunpckhqdq	%ymm7, %ymm0, %ymm0
	vperm2i128	$32, %ymm4, %ymm2, %ymm5
	vperm2i128	$32, %ymm1, %ymm0, %ymm3
	vperm2i128	$49, %ymm4, %ymm2, %ymm2
	vmovdqa	%ymm10, 160(%rdi)
	vperm2i128	$49, %ymm1, %ymm0, %ymm0
	vmovdqa	%ymm9, 416(%rdi)
	vmovdqa	%ymm6, 96(%rdi)
	vmovdqa	%ymm5, 192(%rdi)
	vmovdqa	%ymm2, 448(%rdi)
	vmovdqa	%ymm3, 224(%rdi)
	vmovdqa	%ymm0, 480(%rdi)
	vzeroupper
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5309:
	.size	transpose16, .-transpose16
	.p2align 4
	.type	mult48x8_float, @function
mult48x8_float:
.LFB5306:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %r9
	movq	%rsi, %r10
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	andq	$-32, %rsp
	subq	$2304, %rsp
	vmovaps	1472(%rsi), %ymm4
	vaddps	704(%rsi), %ymm4, %ymm0
	vmovaps	1472(%rdx), %ymm5
	vmovaps	1504(%rsi), %ymm6
	vmovaps	1504(%rdx), %ymm7
	vmovaps	1408(%rsi), %ymm4
	vmovaps	%ymm0, 2240(%rsp)
	vaddps	704(%rdx), %ymm5, %ymm0
	vmovaps	1408(%rdx), %ymm5
	vmovaps	%ymm0, 704(%rsp)
	vaddps	736(%rsi), %ymm6, %ymm0
	vmovaps	1440(%rsi), %ymm6
	vmovaps	%ymm0, 2272(%rsp)
	vaddps	736(%rdx), %ymm7, %ymm0
	vmovaps	1440(%rdx), %ymm7
	vmovaps	%ymm0, 736(%rsp)
	vaddps	640(%rsi), %ymm4, %ymm0
	vmovaps	1344(%rsi), %ymm4
	vmovaps	%ymm0, 2176(%rsp)
	vaddps	640(%rdx), %ymm5, %ymm0
	vmovaps	576(%rdx), %ymm5
	vmovaps	%ymm0, 640(%rsp)
	vaddps	672(%rsi), %ymm6, %ymm0
	vmovaps	%ymm0, 2208(%rsp)
	vaddps	672(%rdx), %ymm7, %ymm0
	vmovaps	%ymm0, 672(%rsp)
	vaddps	576(%rsi), %ymm4, %ymm0
	vmovaps	%ymm0, 2112(%rsp)
	vaddps	1344(%rdx), %ymm5, %ymm0
	vmovaps	%ymm0, 576(%rsp)
	vmovaps	1376(%rsi), %ymm6
	vaddps	608(%rsi), %ymm6, %ymm0
	vmovaps	1376(%rdx), %ymm7
	vmovaps	512(%rsi), %ymm4
	vmovaps	512(%rdx), %ymm5
	vmovaps	544(%rsi), %ymm6
	vmovaps	%ymm0, 2144(%rsp)
	vaddps	608(%rdx), %ymm7, %ymm0
	vmovaps	544(%rdx), %ymm7
	vmovaps	%ymm0, 608(%rsp)
	vaddps	1280(%rsi), %ymm4, %ymm0
	vmovaps	448(%rsi), %ymm4
	vmovaps	%ymm0, 2048(%rsp)
	vaddps	1280(%rdx), %ymm5, %ymm0
	vmovaps	448(%rdx), %ymm5
	vmovaps	%ymm0, 512(%rsp)
	vaddps	1312(%rsi), %ymm6, %ymm0
	vmovaps	480(%rsi), %ymm6
	vmovaps	%ymm0, 2080(%rsp)
	vaddps	1312(%rdx), %ymm7, %ymm0
	vmovaps	480(%rdx), %ymm7
	vmovaps	%ymm0, 544(%rsp)
	vaddps	1216(%rsi), %ymm4, %ymm0
	vmovaps	384(%rsi), %ymm4
	vmovaps	%ymm0, 1984(%rsp)
	vaddps	1216(%rdx), %ymm5, %ymm0
	vmovaps	%ymm0, 448(%rsp)
	vaddps	1248(%rsi), %ymm6, %ymm0
	vmovaps	%ymm0, 2016(%rsp)
	vaddps	1248(%rdx), %ymm7, %ymm0
	vmovaps	%ymm0, 480(%rsp)
	vaddps	1152(%rsi), %ymm4, %ymm0
	vmovaps	%ymm0, 1920(%rsp)
	vmovaps	384(%rdx), %ymm5
	vaddps	1152(%rdx), %ymm5, %ymm0
	vmovaps	416(%rsi), %ymm6
	vmovaps	416(%rdx), %ymm7
	vmovaps	320(%rsi), %ymm4
	vmovaps	320(%rdx), %ymm5
	vmovaps	%ymm0, 384(%rsp)
	vaddps	1184(%rsi), %ymm6, %ymm0
	vmovaps	352(%rsi), %ymm6
	vmovaps	%ymm0, 1952(%rsp)
	vaddps	1184(%rdx), %ymm7, %ymm0
	vmovaps	352(%rdx), %ymm7
	vmovaps	%ymm0, 416(%rsp)
	vaddps	1088(%rsi), %ymm4, %ymm0
	vmovaps	256(%rsi), %ymm4
	vmovaps	%ymm0, 1856(%rsp)
	vaddps	1088(%rdx), %ymm5, %ymm0
	vmovaps	256(%rdx), %ymm5
	vmovaps	%ymm0, 320(%rsp)
	vaddps	1120(%rsi), %ymm6, %ymm0
	vmovaps	288(%rsi), %ymm6
	vmovaps	%ymm0, 1888(%rsp)
	vaddps	1120(%rdx), %ymm7, %ymm0
	vmovaps	288(%rdx), %ymm7
	vmovaps	%ymm0, 352(%rsp)
	vaddps	1024(%rsi), %ymm4, %ymm0
	vmovaps	%ymm0, 1792(%rsp)
	vaddps	1024(%rdx), %ymm5, %ymm0
	vmovaps	%ymm0, 256(%rsp)
	vaddps	1056(%rsi), %ymm6, %ymm0
	vmovaps	%ymm0, 1824(%rsp)
	vaddps	1056(%rdx), %ymm7, %ymm0
	vmovaps	%ymm0, 288(%rsp)
	vmovaps	192(%rsi), %ymm4
	vaddps	960(%rsi), %ymm4, %ymm0
	vmovaps	192(%rdx), %ymm5
	vmovaps	224(%rsi), %ymm6
	vmovaps	224(%rdx), %ymm7
	vmovaps	128(%rsi), %ymm4
	vmovaps	%ymm0, 1728(%rsp)
	vaddps	960(%rdx), %ymm5, %ymm0
	vmovaps	128(%rdx), %ymm5
	vmovaps	%ymm0, 192(%rsp)
	vaddps	992(%rsi), %ymm6, %ymm0
	vmovaps	160(%rsi), %ymm6
	vmovaps	%ymm0, 1760(%rsp)
	vaddps	992(%rdx), %ymm7, %ymm0
	vmovaps	160(%rdx), %ymm7
	vmovaps	%ymm0, 224(%rsp)
	vaddps	896(%rsi), %ymm4, %ymm0
	vmovaps	64(%rsi), %ymm4
	vmovaps	%ymm0, 1664(%rsp)
	vaddps	896(%rdx), %ymm5, %ymm0
	vmovaps	64(%rdx), %ymm5
	vmovaps	%ymm0, 128(%rsp)
	vaddps	928(%rsi), %ymm6, %ymm0
	vmovaps	96(%rsi), %ymm6
	vmovaps	%ymm0, 1696(%rsp)
	vaddps	928(%rdx), %ymm7, %ymm0
	vmovaps	%ymm0, 160(%rsp)
	vaddps	832(%rsi), %ymm4, %ymm0
	vmovaps	%ymm0, 1600(%rsp)
	vaddps	832(%rdx), %ymm5, %ymm0
	vmovaps	%ymm0, 64(%rsp)
	vaddps	864(%rsi), %ymm6, %ymm0
	vmovaps	%ymm0, 1632(%rsp)
	vmovaps	96(%rdx), %ymm7
	vaddps	864(%rdx), %ymm7, %ymm0
	vmovaps	(%rsi), %ymm4
	vmovaps	(%rdx), %ymm5
	vmovaps	32(%rsi), %ymm6
	vmovaps	32(%rdx), %ymm7
	vmovaps	%ymm0, 96(%rsp)
	vaddps	768(%rsi), %ymm4, %ymm0
	vmovaps	%ymm0, 1536(%rsp)
	vaddps	768(%rdx), %ymm5, %ymm0
	vmovaps	%ymm0, (%rsp)
	vaddps	800(%rsi), %ymm6, %ymm0
	vmovaps	%ymm0, 1568(%rsp)
	vaddps	800(%rdx), %ymm7, %ymm0
	vmovaps	%ymm0, 32(%rsp)
	vzeroupper
	call	mult24x8_float
	leaq	768(%r10), %rsi
	addq	$768, %rdx
	leaq	1536(%rdi), %rdi
	call	mult24x8_float
	leaq	768(%rsp), %r10
	leaq	1536(%rsp), %r11
	movq	%rsp, %rdx
	movq	%r10, %rdi
	movq	%r11, %rsi
	call	mult24x8_float
	vmovaps	.LC0(%rip), %ymm2
	movq	%r9, %rdi
	movq	%r10, %rax
	.p2align 4,,10
	.p2align 3
.L8:
	vmovaps	1536(%rdi), %ymm3
	vmovaps	(%rdi), %ymm1
	addq	$32, %rax
	addq	$32, %rdi
	vfmadd213ps	736(%rdi), %ymm2, %ymm3
	vfmadd213ps	-32(%rax), %ymm2, %ymm1
	vmovaps	2272(%rdi), %ymm0
	vfmadd213ps	736(%rax), %ymm2, %ymm0
	vaddps	%ymm1, %ymm3, %ymm1
	vsubps	%ymm3, %ymm0, %ymm0
	vmovaps	%ymm1, 736(%rdi)
	vmovaps	%ymm0, 1504(%rdi)
	cmpq	%rax, %r11
	jne	.L8
	vzeroupper
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5306:
	.size	mult48x8_float, .-mult48x8_float
	.p2align 4
	.type	mult96x16, @function
mult96x16:
.LFB5308:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsi, %rcx
	movq	$-3072, %r8
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	.cfi_offset 13, -40
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	movq	%rdx, %rbx
	andq	$-32, %rsp
	subq	$13952, %rsp
	vmovdqa	.LC1(%rip), %ymm8
	vmovaps	.LC2(%rip), %ymm7
	leaq	1664(%rsp), %rax
	leaq	7808(%rsp), %r9
	movq	%rax, 56(%rsp)
	movq	%rax, %r15
	leaq	12288(%rsp), %rax
	movq	%r9, %r12
	movq	%rax, 32(%rsp)
	leaq	7680(%rsp), %rax
	leaq	10752(%rsp), %r11
	movq	%rax, (%rsp)
	leaq	3072(%rsp), %rax
	leaq	10880(%rsp), %r14
	movq	%rax, 40(%rsp)
	leaq	1536(%rsp), %rax
	leaq	4736(%rsp), %r13
	movq	%rax, 16(%rsp)
	leaq	3200(%rsp), %rax
	leaq	9344(%rsp), %r10
	movq	%rax, 72(%rsp)
	leaq	12416(%rsp), %rax
	movq	%rax, 64(%rsp)
	leaq	128(%rsp), %rax
	movq	%rax, 24(%rsp)
	leaq	6272(%rsp), %rax
	movq	%rax, 8(%rsp)
	leaq	13824(%rsp), %rax
	movq	%rax, 48(%rsp)
	leaq	9088(%rdi), %rax
	movq	%rax, 80(%rsp)
.L12:
	leaq	3072(%r8), %rax
	.p2align 4,,10
	.p2align 3
.L13:
	vpmovsxwd	3008(%rcx,%rax), %ymm2
#APP
# 64 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vpmovsxbw 3008(%rbx,%rax), %xmm0
# 0 "" 2
#NO_APP
	vpaddd	%ymm8, %ymm2, %ymm2
	vpmovsxwd	%xmm0, %ymm0
	vaddps	%ymm2, %ymm7, %ymm2
	vpaddd	%ymm8, %ymm0, %ymm0
	vaddps	%ymm0, %ymm7, %ymm0
	vmovaps	%ymm2, 6080(%r12,%rax)
	vpmovsxwd	3040(%rcx,%rax), %ymm2
	vmovaps	%ymm0, 3008(%r15,%rax)
#APP
# 64 "../../../crypto_kem/sntrup4591761/cavx/mult.c" 1
	vpmovsxbw 3040(%rbx,%rax), %xmm0
# 0 "" 2
#NO_APP
	vpmovsxwd	%xmm0, %ymm0
	vpaddd	%ymm8, %ymm2, %ymm2
	vpaddd	%ymm8, %ymm0, %ymm0
	vaddps	%ymm2, %ymm7, %ymm2
	vaddps	%ymm0, %ymm7, %ymm0
	vmovaps	%ymm2, 6112(%r12,%rax)
	vmovaps	%ymm0, 3040(%r15,%rax)
	subq	$64, %rax
	cmpq	%rax, %r8
	jne	.L13
	movq	32(%rsp), %rax
	movq	(%rsp), %rdi
	movq	40(%rsp), %rdx
	movq	16(%rsp), %rsi
	.p2align 4,,10
	.p2align 3
.L14:
	vmovaps	(%rax), %ymm3
	addq	$-128, %rax
	addq	$-128, %rdi
	addq	$-128, %rdx
	vaddps	1664(%rax), %ymm3, %ymm0
	vmovaps	128(%rdx), %ymm3
	addq	$-128, %rsi
	vmovaps	%ymm0, 128(%rdi)
	vaddps	1664(%rdx), %ymm3, %ymm0
	vmovaps	160(%rax), %ymm3
	vmovaps	%ymm0, 128(%rsi)
	vaddps	1696(%rax), %ymm3, %ymm0
	vmovaps	160(%rdx), %ymm3
	vmovaps	%ymm0, 160(%rdi)
	vaddps	1696(%rdx), %ymm3, %ymm0
	vmovaps	192(%rax), %ymm3
	vmovaps	%ymm0, 160(%rsi)
	vaddps	1728(%rax), %ymm3, %ymm0
	vmovaps	192(%rdx), %ymm3
	vmovaps	%ymm0, 192(%rdi)
	vaddps	1728(%rdx), %ymm3, %ymm0
	vmovaps	224(%rax), %ymm3
	vmovaps	%ymm0, 192(%rsi)
	vaddps	1760(%rax), %ymm3, %ymm0
	vmovaps	224(%rdx), %ymm3
	vmovaps	%ymm0, 224(%rdi)
	vaddps	1760(%rdx), %ymm3, %ymm0
	vmovaps	%ymm0, 224(%rsi)
	cmpq	%rax, %r11
	jne	.L14
	movq	56(%rsp), %rdx
	movq	%r14, %rsi
	movq	%r9, %rdi
	movq	%r8, 88(%rsp)
	movq	%rcx, 96(%rsp)
	movq	%r10, 104(%rsp)
	movq	%r11, 112(%rsp)
	movq	%r9, 120(%rsp)
	vzeroupper
	call	mult48x8_float
	movq	72(%rsp), %rdx
	movq	64(%rsp), %rsi
	movq	%r14, %rdi
	call	mult48x8_float
	movq	24(%rsp), %rdx
	movq	8(%rsp), %rsi
	movq	%r13, %rdi
	call	mult48x8_float
	movq	120(%rsp), %r9
	movq	88(%rsp), %r8
	movq	%r13, %rdx
	movq	96(%rsp), %rcx
	movq	104(%rsp), %r10
	movq	112(%rsp), %r11
	vmovdqa	.LC7(%rip), %xmm1
	movq	%r9, %rax
	vmovaps	.LC6(%rip), %ymm10
	vmovaps	.LC5(%rip), %ymm4
	vmovaps	.LC4(%rip), %ymm5
	vmovaps	.LC3(%rip), %ymm6
	vmovaps	.LC2(%rip), %ymm7
	vmovdqa	.LC1(%rip), %ymm8
	vmovaps	.LC0(%rip), %ymm9
	.p2align 4,,10
	.p2align 3
.L15:
	vmovaps	3072(%rax), %ymm3
	vmovaps	(%rax), %ymm2
	addq	$32, %rax
	addq	$32, %rdx
	vfmadd213ps	1504(%rax), %ymm9, %ymm3
	vfmadd213ps	-32(%rdx), %ymm9, %ymm2
	vmovaps	4576(%rax), %ymm0
	vfmadd213ps	1504(%rdx), %ymm9, %ymm0
	vaddps	%ymm2, %ymm3, %ymm2
	vsubps	%ymm3, %ymm0, %ymm0
	vmovaps	%ymm2, 1504(%rax)
	vmovaps	%ymm0, 3040(%rax)
	cmpq	%rax, %r10
	jne	.L15
	movq	80(%rsp), %rdi
	movq	48(%rsp), %rax
	leaq	(%rdi,%r8), %rdx
	.p2align 4,,10
	.p2align 3
.L16:
	vmulps	(%rax), %ymm6, %ymm0
	movq	%rax, %rsi
	addq	$-128, %rdx
	addq	$-128, %rax
	vroundps	$8, %ymm0, %ymm0
	vfmadd213ps	128(%rax), %ymm5, %ymm0
	vaddps	%ymm4, %ymm0, %ymm0
	vmovaps	%ymm0, 128(%rax)
	vmovdqa	%xmm0, %xmm3
	vmulps	160(%rax), %ymm6, %ymm0
	vpaddd	%xmm3, %xmm1, %xmm3
	vroundps	$8, %ymm0, %ymm0
	vfmadd213ps	160(%rax), %ymm5, %ymm0
	vfmadd132ps	%ymm10, %ymm4, %ymm0
	vmovaps	%ymm0, 160(%rax)
	vmovdqa	%xmm0, %xmm2
	vmulps	192(%rax), %ymm6, %ymm0
	vpaddd	%xmm2, %xmm1, %xmm2
	vroundps	$8, %ymm0, %ymm0
	vfmadd213ps	192(%rax), %ymm5, %ymm0
	vaddps	%ymm4, %ymm0, %ymm0
	vmovaps	%ymm0, 192(%rax)
	vmovdqa	%xmm0, %xmm11
	vmulps	224(%rax), %ymm6, %ymm0
	vpaddd	%xmm11, %xmm1, %xmm11
	vroundps	$8, %ymm0, %ymm0
	vfmadd213ps	224(%rax), %ymm5, %ymm0
	vfmadd132ps	%ymm10, %ymm4, %ymm0
	vmovaps	%ymm0, 224(%rax)
	vmovdqa	%xmm0, %xmm12
	vpaddd	144(%rax), %xmm1, %xmm0
	vpackssdw	%xmm0, %xmm3, %xmm3
	vpaddd	176(%rax), %xmm1, %xmm0
	vmovaps	%xmm3, 128(%rdx)
	vpackssdw	%xmm0, %xmm2, %xmm2
	vpaddd	208(%rax), %xmm1, %xmm0
	vmovaps	%xmm2, 160(%rdx)
	vpaddd	240(%rax), %xmm1, %xmm2
	vpackssdw	%xmm0, %xmm11, %xmm11
	vpaddd	%xmm12, %xmm1, %xmm0
	vpackssdw	%xmm2, %xmm0, %xmm0
	vmovaps	%xmm11, 192(%rdx)
	vmovaps	%xmm0, 224(%rdx)
	cmpq	%rsi, %r9
	jne	.L16
	addq	$16, %r8
	subq	$8, %rbx
	subq	$16, %r12
	subq	$16, %r15
	cmpq	$-3040, %r8
	jne	.L12
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
.LFE5308:
	.size	mult96x16, .-mult96x16
	.p2align 4
	.type	mult768_mix2_m256i, @function
mult768_mix2_m256i:
.LFB5311:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	addq	$160, %rsi
	vpxor	%xmm5, %xmm5, %xmm5
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r14
	pushq	%r13
	pushq	%r12
	.cfi_offset 14, -24
	.cfi_offset 13, -32
	.cfi_offset 12, -40
	movq	%rdi, %r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$16128, %rsp
	.cfi_offset 3, -48
	vmovdqa	.LC8(%rip), %ymm13
	vmovdqa	.LC9(%rip), %ymm12
	leaq	3840(%rsp), %r13
	leaq	768(%rsp), %r14
	movq	%r13, %rbx
	movq	%r13, %rax
	.p2align 4,,10
	.p2align 3
.L25:
	vmovdqu	192(%rsi), %ymm8
	vmovdqu	(%rsi), %ymm1
	subq	$512, %rax
	subq	$32, %rsi
	vmovdqu	416(%rsi), %ymm0
	vmovdqu	608(%rsi), %ymm4
	vpaddw	%ymm1, %ymm8, %ymm3
	vmovdqu	992(%rsi), %ymm7
	vmovdqu	800(%rsi), %ymm2
	vmovdqa	%ymm1, 3072(%rax)
	vpmulhrsw	%ymm13, %ymm3, %ymm9
	vmovdqu	1184(%rsi), %ymm6
	vpaddw	%ymm0, %ymm1, %ymm11
	vmovdqu	1376(%rsi), %ymm14
	vmovdqa	%ymm0, 3136(%rax)
	vpaddw	%ymm2, %ymm1, %ymm1
	vmovdqa	%ymm2, 3200(%rax)
	vmovdqa	%ymm6, 3264(%rax)
	vmovdqa	%ymm4, 3168(%rax)
	vpmullw	%ymm12, %ymm9, %ymm9
	vmovdqa	%ymm8, 3104(%rax)
	vmovdqa	%ymm7, 3232(%rax)
	vmovdqa	%ymm14, 3296(%rax)
	vpsubw	%ymm9, %ymm3, %ymm3
	vmovdqa	%ymm3, 3328(%rax)
	vpaddw	%ymm0, %ymm4, %ymm3
	vpaddw	%ymm6, %ymm0, %ymm0
	vpmulhrsw	%ymm13, %ymm3, %ymm9
	vpmullw	%ymm12, %ymm9, %ymm9
	vpsubw	%ymm9, %ymm3, %ymm3
	vmovdqa	%ymm3, 3360(%rax)
	vpaddw	%ymm2, %ymm7, %ymm3
	vpaddw	%ymm6, %ymm2, %ymm2
	vpmulhrsw	%ymm13, %ymm3, %ymm9
	vpmulhrsw	%ymm13, %ymm2, %ymm10
	vpmullw	%ymm12, %ymm9, %ymm9
	vpmullw	%ymm12, %ymm10, %ymm10
	vpsubw	%ymm9, %ymm3, %ymm3
	vpsubw	%ymm10, %ymm2, %ymm10
	vmovdqa	%ymm3, 3392(%rax)
	vpaddw	%ymm6, %ymm14, %ymm3
	vpaddw	%ymm14, %ymm7, %ymm2
	vpmulhrsw	%ymm13, %ymm0, %ymm6
	vpmulhrsw	%ymm13, %ymm3, %ymm9
	vmovdqa	%ymm10, 6240(%rax)
	vpmullw	%ymm12, %ymm6, %ymm6
	vpmullw	%ymm12, %ymm9, %ymm9
	vpsubw	%ymm6, %ymm0, %ymm6
	vpsubw	%ymm9, %ymm3, %ymm3
	vpmulhrsw	%ymm13, %ymm1, %ymm9
	vmovdqa	%ymm6, 6400(%rax)
	vmovdqa	%ymm3, 3424(%rax)
	vpmulhrsw	%ymm13, %ymm11, %ymm3
	vpmullw	%ymm12, %ymm9, %ymm9
	vpmullw	%ymm12, %ymm3, %ymm3
	vpsubw	%ymm9, %ymm1, %ymm9
	vpaddw	%ymm7, %ymm8, %ymm1
	vpmulhrsw	%ymm13, %ymm2, %ymm7
	vpsubw	%ymm3, %ymm11, %ymm11
	vpaddw	%ymm4, %ymm8, %ymm3
	vmovdqa	%ymm9, 6336(%rax)
	vpmulhrsw	%ymm13, %ymm3, %ymm0
	vpaddw	%ymm14, %ymm4, %ymm4
	vpaddw	%ymm9, %ymm6, %ymm8
	vmovdqa	%ymm11, 6144(%rax)
	vpmullw	%ymm12, %ymm7, %ymm7
	vpmullw	%ymm12, %ymm0, %ymm0
	vpsubw	%ymm7, %ymm2, %ymm2
	vmovdqa	%ymm2, 6272(%rax)
	vpaddw	%ymm10, %ymm2, %ymm2
	vpsubw	%ymm0, %ymm3, %ymm3
	vpmulhrsw	%ymm13, %ymm1, %ymm0
	vmovdqa	%ymm3, 6176(%rax)
	vpaddw	%ymm11, %ymm3, %ymm3
	vpmullw	%ymm12, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm1, %ymm1
	vpmulhrsw	%ymm13, %ymm4, %ymm0
	vmovdqa	%ymm1, 6368(%rax)
	vpmullw	%ymm12, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm4, %ymm4
	vpmulhrsw	%ymm13, %ymm8, %ymm0
	vpaddw	%ymm1, %ymm4, %ymm7
	vpaddw	%ymm9, %ymm1, %ymm1
	vmovdqa	%ymm4, 6432(%rax)
	vpmullw	%ymm12, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm8, %ymm8
	vpmulhrsw	%ymm13, %ymm7, %ymm0
	vmovdqa	%ymm8, 6528(%rax)
	vpmullw	%ymm12, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm7, %ymm7
	vpmulhrsw	%ymm13, %ymm3, %ymm0
	vmovdqa	%ymm7, 6560(%rax)
	vpmullw	%ymm12, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm3, %ymm3
	vpmulhrsw	%ymm13, %ymm2, %ymm0
	vmovdqa	%ymm3, 6208(%rax)
	vpmullw	%ymm12, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm2, %ymm2
	vpmulhrsw	%ymm13, %ymm1, %ymm0
	vmovdqa	%ymm2, 6304(%rax)
	vpmullw	%ymm12, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm1, %ymm1
	vpaddw	%ymm6, %ymm4, %ymm0
	vmovdqa	%ymm1, 6464(%rax)
	vpmulhrsw	%ymm13, %ymm0, %ymm1
	vmovdqa	%ymm5, 3456(%rax)
	vmovdqa	%ymm5, 3488(%rax)
	vmovdqa	%ymm5, 3520(%rax)
	vmovdqa	%ymm5, 3552(%rax)
	vpmullw	%ymm12, %ymm1, %ymm1
	vmovdqa	%ymm5, 6624(%rax)
	vpsubw	%ymm1, %ymm0, %ymm0
	vmovdqa	%ymm0, 6496(%rax)
	vpaddw	%ymm8, %ymm7, %ymm0
	vpmulhrsw	%ymm13, %ymm0, %ymm1
	vpmullw	%ymm12, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm0, %ymm0
	vmovdqa	%ymm0, 6592(%rax)
	cmpq	%rax, %r14
	jne	.L25
	vmovdqu	64(%rdx), %ymm1
	vmovdqu	160(%rdx), %ymm9
	leaq	9472(%rsp), %rax
	vmovdqu	256(%rdx), %ymm8
	vmovdqu	352(%rdx), %ymm0
	vmovdqu	448(%rdx), %ymm3
	vmovdqu	544(%rdx), %ymm2
	vpaddb	%ymm9, %ymm1, %ymm6
	vmovdqa	%ymm1, 2816(%rsp)
	vmovdqu	736(%rdx), %ymm7
	vmovdqu	640(%rdx), %ymm4
	vpaddb	%ymm8, %ymm1, %ymm10
	vmovdqa	%ymm6, 3072(%rsp)
	vmovdqa	%ymm8, 2880(%rsp)
	vpaddb	%ymm0, %ymm8, %ymm6
	vmovdqa	%ymm6, 3104(%rsp)
	vpaddb	%ymm2, %ymm3, %ymm6
	vmovdqa	%ymm6, 3136(%rsp)
	vpaddb	%ymm7, %ymm4, %ymm6
	vmovdqa	%ymm6, 3168(%rsp)
	vpaddb	%ymm3, %ymm1, %ymm6
	vpaddb	%ymm2, %ymm9, %ymm1
	vmovdqa	%ymm3, 2944(%rsp)
	vpaddb	%ymm4, %ymm3, %ymm3
	vmovdqa	%ymm4, 3008(%rsp)
	vpaddb	%ymm4, %ymm8, %ymm4
	vpaddb	%ymm0, %ymm9, %ymm8
	vmovdqa	%ymm0, 2912(%rsp)
	vpaddb	%ymm7, %ymm0, %ymm0
	vmovdqa	%ymm2, 2976(%rsp)
	vpaddb	%ymm7, %ymm2, %ymm2
	vmovdqa	%ymm9, 2848(%rsp)
	vpaddb	%ymm6, %ymm4, %ymm9
	vmovdqa	%ymm7, 3040(%rsp)
	vpaddb	%ymm1, %ymm0, %ymm7
	vmovdqa	%ymm8, 3360(%rsp)
	vpaddb	%ymm10, %ymm8, %ymm8
	vmovdqa	%ymm1, 3552(%rsp)
	vpaddb	%ymm6, %ymm1, %ymm1
	vmovdqa	%ymm2, 3456(%rsp)
	vpaddb	%ymm3, %ymm2, %ymm2
	vmovdqa	%ymm10, 3328(%rsp)
	vmovdqa	%ymm6, 3520(%rsp)
	vmovdqa	%ymm3, 3424(%rsp)
	vmovdqa	%ymm4, 3584(%rsp)
	vmovdqa	%ymm0, 3616(%rsp)
	vpaddb	%ymm4, %ymm0, %ymm0
	vmovdqa	%ymm9, 3712(%rsp)
	vmovdqa	%ymm7, 3744(%rsp)
	vpaddb	%ymm9, %ymm7, %ymm7
	vmovdqa	%ymm8, 3392(%rsp)
	vmovdqa	%ymm2, 3488(%rsp)
	vmovdqa	%ymm1, 3648(%rsp)
	vmovdqu	32(%rdx), %ymm1
	vmovdqu	128(%rdx), %ymm9
	vmovdqu	224(%rdx), %ymm8
	vmovdqu	416(%rdx), %ymm6
	vmovdqa	%ymm0, 3680(%rsp)
	vmovdqu	320(%rdx), %ymm0
	vmovdqu	512(%rdx), %ymm2
	vpaddb	%ymm9, %ymm1, %ymm4
	vmovdqa	%ymm7, 3776(%rsp)
	vmovdqu	608(%rdx), %ymm3
	vmovdqu	704(%rdx), %ymm7
	vpaddb	%ymm8, %ymm1, %ymm10
	vmovdqa	%ymm4, 2048(%rsp)
	vmovdqa	%ymm1, 1792(%rsp)
	vpaddb	%ymm0, %ymm8, %ymm4
	vmovdqa	%ymm4, 2080(%rsp)
	vpaddb	%ymm2, %ymm6, %ymm4
	vmovdqa	%ymm4, 2112(%rsp)
	vpaddb	%ymm7, %ymm3, %ymm4
	vmovdqa	%ymm4, 2144(%rsp)
	vpaddb	%ymm6, %ymm1, %ymm4
	vpaddb	%ymm2, %ymm9, %ymm1
	vmovdqa	%ymm8, 1856(%rsp)
	vmovdqa	%ymm6, 1920(%rsp)
	vpaddb	%ymm3, %ymm6, %ymm6
	vmovdqa	%ymm3, 1984(%rsp)
	vpaddb	%ymm3, %ymm8, %ymm3
	vpaddb	%ymm0, %ymm9, %ymm8
	vmovdqa	%ymm0, 1888(%rsp)
	vpaddb	%ymm7, %ymm0, %ymm0
	vmovdqa	%ymm9, 1824(%rsp)
	vpaddb	%ymm4, %ymm3, %ymm9
	vmovdqa	%ymm2, 1952(%rsp)
	vpaddb	%ymm7, %ymm2, %ymm2
	vmovdqa	%ymm7, 2016(%rsp)
	vpaddb	%ymm1, %ymm0, %ymm7
	vmovdqa	%ymm10, 2304(%rsp)
	vmovdqa	%ymm4, 2496(%rsp)
	vmovdqa	%ymm6, 2400(%rsp)
	vmovdqa	%ymm3, 2560(%rsp)
	vmovdqa	%ymm8, 2336(%rsp)
	vpaddb	%ymm10, %ymm8, %ymm8
	vmovdqa	%ymm5, 3200(%rsp)
	vmovdqa	%ymm5, 3232(%rsp)
	vmovdqa	%ymm5, 3264(%rsp)
	vmovdqa	%ymm5, 3296(%rsp)
	vmovdqa	%ymm5, 3808(%rsp)
	vmovdqa	%ymm1, 2528(%rsp)
	vpaddb	%ymm4, %ymm1, %ymm1
	vmovdqu	384(%rdx), %ymm4
	vmovdqa	%ymm7, 2720(%rsp)
	vpaddb	%ymm9, %ymm7, %ymm7
	vmovdqa	%ymm0, 2592(%rsp)
	vpaddb	%ymm3, %ymm0, %ymm0
	vmovdqu	480(%rdx), %ymm3
	vmovdqa	%ymm1, 2624(%rsp)
	vmovdqu	(%rdx), %ymm1
	vmovdqa	%ymm7, 2752(%rsp)
	vmovdqu	96(%rdx), %ymm7
	vmovdqa	%ymm2, 2432(%rsp)
	vpaddb	%ymm6, %ymm2, %ymm2
	vmovdqu	288(%rdx), %ymm6
	vmovdqa	%ymm0, 2656(%rsp)
	vmovdqu	192(%rdx), %ymm0
	vmovdqa	%ymm8, 2368(%rsp)
	vpaddb	%ymm7, %ymm1, %ymm8
	vpaddb	%ymm7, %ymm6, %ymm10
	vmovdqa	%ymm9, 2688(%rsp)
	vmovdqu	576(%rdx), %ymm9
	vmovdqa	%ymm2, 2464(%rsp)
	vmovdqu	672(%rdx), %ymm2
	vmovdqa	%ymm8, 1024(%rsp)
	vpaddb	%ymm6, %ymm0, %ymm8
	vmovdqa	%ymm8, 1056(%rsp)
	vpaddb	%ymm3, %ymm4, %ymm8
	vmovdqa	%ymm8, 1088(%rsp)
	vpaddb	%ymm2, %ymm9, %ymm8
	vmovdqa	%ymm8, 1120(%rsp)
	vpaddb	%ymm1, %ymm0, %ymm8
	vmovdqa	%ymm1, 768(%rsp)
	vpaddb	%ymm1, %ymm4, %ymm1
	vmovdqa	%ymm4, 896(%rsp)
	vpaddb	%ymm4, %ymm9, %ymm4
	vmovdqa	%ymm0, 832(%rsp)
	vpaddb	%ymm0, %ymm9, %ymm0
	vmovdqa	%ymm9, 960(%rsp)
	vmovdqa	%ymm8, 1280(%rsp)
	vpaddb	%ymm10, %ymm8, %ymm8
	vmovdqa	%ymm1, 1472(%rsp)
	vmovdqa	%ymm5, 2176(%rsp)
	vmovdqa	%ymm5, 2208(%rsp)
	vmovdqa	%ymm5, 2240(%rsp)
	vmovdqa	%ymm5, 2272(%rsp)
	vmovdqa	%ymm5, 2784(%rsp)
	vmovdqa	%ymm4, 1376(%rsp)
	vmovdqa	%ymm7, 800(%rsp)
	vpaddb	%ymm7, %ymm3, %ymm7
	vmovdqa	%ymm3, 928(%rsp)
	vpaddb	%ymm3, %ymm2, %ymm3
	vmovdqa	%ymm2, 992(%rsp)
	vpaddb	%ymm6, %ymm2, %ymm2
	vmovdqa	%ymm6, 864(%rsp)
	vpaddb	%ymm2, %ymm7, %ymm9
	vpaddb	%ymm0, %ymm1, %ymm6
	vmovdqa	%ymm0, 1536(%rsp)
	vpaddb	%ymm7, %ymm1, %ymm1
	vpaddb	%ymm2, %ymm0, %ymm0
	vmovdqa	%ymm3, 1408(%rsp)
	vpaddb	%ymm3, %ymm4, %ymm3
	vmovdqa	%ymm6, 1664(%rsp)
	vpaddb	%ymm9, %ymm6, %ymm6
	vmovdqa	%ymm10, 1312(%rsp)
	vmovdqa	%ymm7, 1504(%rsp)
	vmovdqa	%ymm2, 1568(%rsp)
	vmovdqa	%ymm9, 1696(%rsp)
	vmovdqa	%ymm8, 1344(%rsp)
	vmovdqa	%ymm3, 1440(%rsp)
	vmovdqa	%ymm1, 1600(%rsp)
	vmovdqa	%ymm0, 1632(%rsp)
	vmovdqa	%ymm6, 1728(%rsp)
	vmovdqa	%ymm5, 1152(%rsp)
	vmovdqa	%ymm5, 1184(%rsp)
	vmovdqa	%ymm5, 1216(%rsp)
	vmovdqa	%ymm5, 1248(%rsp)
	vmovdqa	%ymm5, 1760(%rsp)
	vzeroupper
	.p2align 4,,10
	.p2align 3
.L26:
	movq	%rax, %rdi
	call	transpose16
	subq	$512, %rax
	cmpq	%rdi, %r13
	jne	.L26
	movq	%r14, %rax
	leaq	-2304(%rsp), %rdx
.L27:
	vmovdqa	2048(%rax), %ymm5
	vpunpcklbw	2080(%rax), %ymm5, %ymm3
	subq	$1024, %rax
	vpunpckhbw	3104(%rax), %ymm5, %ymm1
	vmovdqa	3136(%rax), %ymm5
	vpunpckhbw	3168(%rax), %ymm5, %ymm0
	vpunpcklbw	3168(%rax), %ymm5, %ymm2
	vmovdqa	3264(%rax), %ymm4
	vmovdqa	3392(%rax), %ymm10
	vpunpcklwd	%ymm2, %ymm3, %ymm11
	vpunpckhwd	%ymm2, %ymm3, %ymm5
	vpunpcklwd	%ymm0, %ymm1, %ymm6
	vmovdqa	3200(%rax), %ymm2
	vpunpckhwd	%ymm0, %ymm1, %ymm7
	vpunpcklbw	3232(%rax), %ymm2, %ymm3
	vpunpckhbw	3296(%rax), %ymm4, %ymm0
	vmovdqa	%ymm5, 736(%rsp)
	vpunpckhbw	3232(%rax), %ymm2, %ymm1
	vmovdqa	3264(%rax), %ymm2
	vmovdqa	%ymm6, 704(%rsp)
	vpunpcklbw	3296(%rax), %ymm2, %ymm2
	vmovdqa	3456(%rax), %ymm8
	vmovdqa	%ymm7, 672(%rsp)
	vpunpcklbw	3488(%rax), %ymm8, %ymm5
	vpunpcklwd	%ymm2, %ymm3, %ymm4
	vpunpckhwd	%ymm2, %ymm3, %ymm2
	vpunpcklwd	%ymm0, %ymm1, %ymm3
	vmovdqa	%ymm2, 640(%rsp)
	vpunpckhwd	%ymm0, %ymm1, %ymm1
	vmovdqa	3328(%rax), %ymm0
	vpunpckhbw	3360(%rax), %ymm0, %ymm2
	vmovdqa	%ymm3, 608(%rsp)
	vpunpcklbw	3360(%rax), %ymm0, %ymm3
	vmovdqa	3392(%rax), %ymm0
	vmovdqa	%ymm1, 576(%rsp)
	vpunpcklbw	3424(%rax), %ymm0, %ymm0
	vpunpckhbw	3424(%rax), %ymm10, %ymm1
	vpunpcklwd	%ymm0, %ymm3, %ymm10
	vpunpcklwd	%ymm1, %ymm2, %ymm15
	vpunpckhwd	%ymm1, %ymm2, %ymm14
	vpunpckhwd	%ymm0, %ymm3, %ymm0
	vpunpckhbw	3488(%rax), %ymm8, %ymm2
	vmovdqa	%ymm15, 512(%rsp)
	vmovdqa	%ymm0, 544(%rsp)
	vmovdqa	%ymm14, 480(%rsp)
	vmovdqa	3520(%rax), %ymm8
	vpunpckhbw	3552(%rax), %ymm8, %ymm1
	vpunpcklbw	3552(%rax), %ymm8, %ymm3
	vpunpcklwd	%ymm1, %ymm2, %ymm9
	vpunpckhwd	%ymm1, %ymm2, %ymm7
	vmovdqa	3584(%rax), %ymm1
	vpunpckhbw	3616(%rax), %ymm1, %ymm2
	vpunpcklwd	%ymm3, %ymm5, %ymm0
	vpunpckhwd	%ymm3, %ymm5, %ymm8
	vpunpcklbw	3616(%rax), %ymm1, %ymm3
	vmovdqa	3648(%rax), %ymm1
	vpunpcklbw	3680(%rax), %ymm1, %ymm6
	vpunpckhbw	3680(%rax), %ymm1, %ymm1
	vmovdqa	%ymm8, 448(%rsp)
	vmovdqa	%ymm9, 416(%rsp)
	vmovdqa	3712(%rax), %ymm8
	vpunpckhbw	3744(%rax), %ymm8, %ymm5
	vpunpcklwd	%ymm1, %ymm2, %ymm14
	vpunpckhwd	%ymm1, %ymm2, %ymm1
	vpunpcklwd	%ymm6, %ymm3, %ymm9
	vmovdqa	%ymm7, 384(%rsp)
	vmovdqa	%ymm1, 288(%rsp)
	vpunpckhwd	%ymm6, %ymm3, %ymm3
	vpunpcklbw	3744(%rax), %ymm8, %ymm6
	vmovdqa	3776(%rax), %ymm8
	vpunpckhbw	3808(%rax), %ymm8, %ymm1
	vpunpcklbw	3808(%rax), %ymm8, %ymm2
	vmovdqa	%ymm3, 352(%rsp)
	vmovdqa	%ymm14, 320(%rsp)
	vpunpckhwd	%ymm1, %ymm5, %ymm7
	vpunpcklwd	%ymm2, %ymm6, %ymm3
	vpunpckhwd	%ymm2, %ymm6, %ymm8
	vmovdqa	%ymm7, 224(%rsp)
	vpunpcklwd	%ymm1, %ymm5, %ymm2
	vmovdqa	3840(%rax), %ymm1
	vpunpcklbw	3872(%rax), %ymm1, %ymm7
	vpunpckhbw	3872(%rax), %ymm1, %ymm5
	vmovdqa	3904(%rax), %ymm1
	vmovdqa	%ymm8, 256(%rsp)
	vpunpcklbw	3936(%rax), %ymm1, %ymm6
	vpunpckhbw	3936(%rax), %ymm1, %ymm1
	vpunpcklwd	%ymm6, %ymm7, %ymm8
	vpunpckhwd	%ymm6, %ymm7, %ymm14
	vpunpcklwd	%ymm1, %ymm5, %ymm7
	vmovdqa	%ymm14, 192(%rsp)
	vpunpckhwd	%ymm1, %ymm5, %ymm1
	vmovdqa	%ymm1, 160(%rsp)
	vmovdqa	3968(%rax), %ymm1
	vpunpcklbw	4000(%rax), %ymm1, %ymm15
	vpunpckhbw	4000(%rax), %ymm1, %ymm6
	vmovdqa	4032(%rax), %ymm1
	vpunpckhbw	4064(%rax), %ymm1, %ymm5
	vpunpcklbw	4064(%rax), %ymm1, %ymm14
	vpunpcklwd	%ymm14, %ymm15, %ymm1
	vpunpckhwd	%ymm14, %ymm15, %ymm14
	vpunpcklwd	%ymm5, %ymm6, %ymm15
	vmovdqa	%ymm15, %ymm12
	vpunpckhwd	%ymm5, %ymm6, %ymm15
	vpunpckldq	%ymm0, %ymm10, %ymm6
	vpunpckldq	%ymm4, %ymm11, %ymm5
	vpunpckhdq	%ymm0, %ymm10, %ymm0
	vpunpckhdq	%ymm4, %ymm11, %ymm4
	vpunpckldq	%ymm3, %ymm9, %ymm10
	vpunpckldq	%ymm1, %ymm8, %ymm11
	vpunpckhdq	%ymm3, %ymm9, %ymm3
	vpunpckhdq	%ymm1, %ymm8, %ymm1
	vpunpcklqdq	%ymm11, %ymm10, %ymm9
	vmovdqa	%ymm15, %ymm13
	vpunpcklqdq	%ymm6, %ymm5, %ymm15
	vpunpckhqdq	%ymm6, %ymm5, %ymm5
	vpunpcklqdq	%ymm0, %ymm4, %ymm6
	vpunpckhqdq	%ymm0, %ymm4, %ymm4
	vpunpcklqdq	%ymm1, %ymm3, %ymm0
	vpunpckhqdq	%ymm1, %ymm3, %ymm3
	vpunpckhqdq	%ymm11, %ymm10, %ymm11
	vperm2i128	$32, %ymm9, %ymm15, %ymm8
	vperm2i128	$32, %ymm3, %ymm4, %ymm10
	vperm2i128	$49, %ymm9, %ymm15, %ymm15
	vperm2i128	$49, %ymm3, %ymm4, %ymm4
	vperm2i128	$32, %ymm0, %ymm6, %ymm9
	vmovdqa	%ymm10, 3168(%rax)
	vperm2i128	$49, %ymm0, %ymm6, %ymm6
	vperm2i128	$32, %ymm11, %ymm5, %ymm1
	vmovdqa	%ymm4, 3680(%rax)
	vmovdqa	608(%rsp), %ymm4
	vmovdqa	%ymm6, 3648(%rax)
	vperm2i128	$49, %ymm11, %ymm5, %ymm5
	vmovdqa	704(%rsp), %ymm6
	vmovdqa	320(%rsp), %ymm10
	vmovdqa	%ymm15, 3584(%rax)
	vmovdqa	416(%rsp), %ymm15
	vpunpckldq	%ymm4, %ymm6, %ymm0
	vpunpckhdq	%ymm4, %ymm6, %ymm3
	vmovdqa	512(%rsp), %ymm4
	vpunpckldq	%ymm12, %ymm7, %ymm6
	vmovdqa	%ymm1, 3104(%rax)
	vpunpckhdq	%ymm12, %ymm7, %ymm7
	vpunpckldq	%ymm15, %ymm4, %ymm1
	vmovdqa	%ymm5, 3616(%rax)
	vpunpckhdq	%ymm15, %ymm4, %ymm4
	vpunpckldq	%ymm2, %ymm10, %ymm5
	vpunpckhdq	%ymm2, %ymm10, %ymm2
	vpunpcklqdq	%ymm4, %ymm3, %ymm10
	vpunpckhqdq	%ymm4, %ymm3, %ymm3
	vmovdqa	%ymm8, 3072(%rax)
	vpunpcklqdq	%ymm7, %ymm2, %ymm4
	vpunpckhqdq	%ymm7, %ymm2, %ymm2
	vpunpcklqdq	%ymm1, %ymm0, %ymm8
	vmovdqa	%ymm9, 3136(%rax)
	vpunpckhqdq	%ymm1, %ymm0, %ymm0
	vpunpcklqdq	%ymm6, %ymm5, %ymm9
	vperm2i128	$32, %ymm2, %ymm3, %ymm1
	vperm2i128	$49, %ymm2, %ymm3, %ymm2
	vpunpckhqdq	%ymm6, %ymm5, %ymm5
	vperm2i128	$32, %ymm9, %ymm8, %ymm11
	vmovdqa	640(%rsp), %ymm6
	vperm2i128	$49, %ymm9, %ymm8, %ymm8
	vperm2i128	$32, %ymm4, %ymm10, %ymm9
	vmovdqa	%ymm2, 3936(%rax)
	vperm2i128	$49, %ymm4, %ymm10, %ymm10
	vmovdqa	544(%rsp), %ymm2
	vperm2i128	$32, %ymm5, %ymm0, %ymm15
	vmovdqa	448(%rsp), %ymm4
	vperm2i128	$49, %ymm5, %ymm0, %ymm0
	vmovdqa	%ymm11, 3328(%rax)
	vmovdqa	736(%rsp), %ymm5
	vmovdqa	%ymm8, 3840(%rax)
	vmovdqa	%ymm15, 3360(%rax)
	vpunpckhdq	%ymm6, %ymm5, %ymm3
	vmovdqa	256(%rsp), %ymm15
	vmovdqa	%ymm0, 3872(%rax)
	vpunpckldq	%ymm6, %ymm5, %ymm0
	vmovdqa	%ymm9, 3392(%rax)
	vmovdqa	%ymm1, 3424(%rax)
	vpunpckldq	%ymm4, %ymm2, %ymm1
	vpunpckhdq	%ymm4, %ymm2, %ymm4
	vmovdqa	352(%rsp), %ymm2
	vmovdqa	%ymm10, 3904(%rax)
	vpunpcklqdq	%ymm1, %ymm0, %ymm9
	vpunpcklqdq	%ymm4, %ymm3, %ymm11
	vmovdqa	192(%rsp), %ymm10
	vpunpckldq	%ymm15, %ymm2, %ymm5
	vpunpckhdq	%ymm15, %ymm2, %ymm2
	vpunpckhqdq	%ymm1, %ymm0, %ymm0
	vpunpckldq	%ymm14, %ymm10, %ymm8
	vpunpckhdq	%ymm14, %ymm10, %ymm6
	vpunpckhqdq	%ymm4, %ymm3, %ymm3
	vpunpcklqdq	%ymm8, %ymm5, %ymm10
	vpunpcklqdq	%ymm6, %ymm2, %ymm4
	vpunpckhqdq	%ymm8, %ymm5, %ymm5
	vpunpckhqdq	%ymm6, %ymm2, %ymm6
	vperm2i128	$32, %ymm5, %ymm0, %ymm14
	vperm2i128	$49, %ymm5, %ymm0, %ymm0
	vmovdqa	672(%rsp), %ymm5
	vperm2i128	$32, %ymm6, %ymm3, %ymm1
	vperm2i128	$49, %ymm6, %ymm3, %ymm3
	vmovdqa	576(%rsp), %ymm6
	vperm2i128	$32, %ymm10, %ymm9, %ymm7
	vmovdqa	%ymm7, 3200(%rax)
	vperm2i128	$49, %ymm10, %ymm9, %ymm9
	vperm2i128	$32, %ymm4, %ymm11, %ymm10
	vperm2i128	$49, %ymm4, %ymm11, %ymm11
	vpunpckhdq	%ymm6, %ymm5, %ymm8
	vpunpckldq	%ymm6, %ymm5, %ymm7
	vmovdqa	480(%rsp), %ymm5
	vmovdqa	384(%rsp), %ymm6
	vmovdqa	224(%rsp), %ymm4
	vmovdqa	%ymm0, 3744(%rax)
	vpunpckhdq	%ymm6, %ymm5, %ymm2
	vpunpckldq	%ymm6, %ymm5, %ymm0
	vmovdqa	288(%rsp), %ymm5
	vmovdqa	%ymm1, 3296(%rax)
	vmovdqa	%ymm3, 3808(%rax)
	vpunpckldq	%ymm4, %ymm5, %ymm6
	vpunpckhdq	%ymm4, %ymm5, %ymm1
	vmovdqa	160(%rsp), %ymm5
	vpunpcklqdq	%ymm0, %ymm7, %ymm4
	vmovdqa	%ymm9, 3712(%rax)
	vpunpckhqdq	%ymm0, %ymm7, %ymm7
	vpunpckldq	%ymm13, %ymm5, %ymm3
	vpunpckhdq	%ymm13, %ymm5, %ymm5
	vmovdqa	%ymm14, 3232(%rax)
	vpunpcklqdq	%ymm3, %ymm6, %ymm9
	vpunpckhqdq	%ymm3, %ymm6, %ymm0
	vpunpcklqdq	%ymm2, %ymm8, %ymm6
	vmovdqa	%ymm10, 3264(%rax)
	vpunpcklqdq	%ymm5, %ymm1, %ymm3
	vpunpckhqdq	%ymm2, %ymm8, %ymm2
	vpunpckhqdq	%ymm5, %ymm1, %ymm5
	vmovdqa	%ymm11, 3776(%rax)
	vperm2i128	$32, %ymm9, %ymm4, %ymm8
	vperm2i128	$32, %ymm5, %ymm2, %ymm1
	vperm2i128	$49, %ymm9, %ymm4, %ymm4
	vperm2i128	$49, %ymm5, %ymm2, %ymm2
	vperm2i128	$32, %ymm0, %ymm7, %ymm9
	vperm2i128	$49, %ymm0, %ymm7, %ymm0
	vmovdqa	%ymm8, 3456(%rax)
	vperm2i128	$32, %ymm3, %ymm6, %ymm7
	vperm2i128	$49, %ymm3, %ymm6, %ymm3
	vmovdqa	%ymm4, 3968(%rax)
	vmovdqa	%ymm9, 3488(%rax)
	vmovdqa	%ymm0, 4000(%rax)
	vmovdqa	%ymm7, 3520(%rax)
	vmovdqa	%ymm3, 4032(%rax)
	vmovdqa	%ymm1, 3552(%rax)
	vmovdqa	%ymm2, 4064(%rax)
	cmpq	%rax, %rdx
	jne	.L27
	leaq	784(%rsp), %rdx
	leaq	6912(%rsp), %rsi
	leaq	9984(%rsp), %rdi
	vzeroupper
	call	mult96x16
	movq	%r14, %rdx
	movq	%r13, %rsi
	movq	%r13, %rdi
	call	mult96x16
	leaq	15616(%rsp), %rax
	jmp	.L28
	.p2align 4,,10
	.p2align 3
.L35:
	vzeroupper
.L28:
	movq	%rax, %rdi
	call	transpose16
	vmovdqa	.LC8(%rip), %ymm13
	vmovdqa	.LC9(%rip), %ymm12
	subq	$512, %rax
	cmpq	%rdi, %r13
	jne	.L35
	addq	$160, %r12
	.p2align 4,,10
	.p2align 3
.L29:
	vmovdqa	9088(%rbx), %ymm6
	vmovdqa	12160(%rbx), %ymm5
	subq	$512, %rbx
	subq	$32, %r12
	vpaddw	9632(%rbx), %ymm6, %ymm0
	vpaddw	9664(%rbx), %ymm5, %ymm5
	vmovdqa	9440(%rbx), %ymm14
	vmovdqa	9632(%rbx), %ymm7
	vmovdqa	12704(%rbx), %ymm6
	vpaddw	12736(%rbx), %ymm7, %ymm8
	vpsubw	%ymm0, %ymm5, %ymm5
	vmovdqa	12480(%rbx), %ymm7
	vpaddw	12672(%rbx), %ymm6, %ymm0
	vpmulhrsw	%ymm13, %ymm5, %ymm4
	vpaddw	9536(%rbx), %ymm7, %ymm6
	vpaddw	12608(%rbx), %ymm14, %ymm3
	vpsubw	%ymm0, %ymm8, %ymm8
	vmovdqa	12512(%rbx), %ymm2
	vmovdqa	9504(%rbx), %ymm11
	vpsubw	%ymm14, %ymm6, %ymm0
	vpmulhrsw	%ymm13, %ymm8, %ymm9
	vpsubw	%ymm6, %ymm14, %ymm6
	vmovdqa	12576(%rbx), %ymm1
	vpsubw	9408(%rbx), %ymm0, %ymm7
	vpaddw	12480(%rbx), %ymm2, %ymm0
	vpaddw	12544(%rbx), %ymm1, %ymm1
	vmovdqa	12544(%rbx), %ymm2
	vpmullw	%ymm12, %ymm4, %ymm4
	vpaddw	%ymm5, %ymm6, %ymm5
	vmovdqa	%ymm7, 512(%rsp)
	vpsubw	%ymm0, %ymm3, %ymm3
	vpmulhrsw	%ymm7, %ymm13, %ymm0
	vpaddw	9568(%rbx), %ymm2, %ymm7
	vpaddw	12640(%rbx), %ymm11, %ymm2
	vpmullw	%ymm12, %ymm9, %ymm9
	vpsubw	%ymm1, %ymm2, %ymm1
	vpsubw	%ymm11, %ymm7, %ymm2
	vpmulhrsw	%ymm13, %ymm1, %ymm10
	vpsubw	9472(%rbx), %ymm2, %ymm2
	vpsubw	%ymm7, %ymm11, %ymm7
	vmovdqa	9472(%rbx), %ymm11
	vpmullw	%ymm12, %ymm0, %ymm15
	vpmulhrsw	%ymm13, %ymm3, %ymm0
	vpmulhrsw	%ymm13, %ymm2, %ymm2
	vpsubw	%ymm9, %ymm8, %ymm9
	vpmullw	%ymm12, %ymm10, %ymm10
	vpmullw	%ymm12, %ymm0, %ymm0
	vpaddw	9472(%rbx), %ymm0, %ymm0
	vpmullw	%ymm12, %ymm2, %ymm2
	vpsubw	%ymm0, %ymm3, %ymm3
	vpsubw	%ymm10, %ymm1, %ymm10
	vmovdqa	9600(%rbx), %ymm1
	vpsubw	9408(%rbx), %ymm1, %ymm1
	vmovdqa	%ymm10, 448(%rsp)
	vpaddw	%ymm3, %ymm1, %ymm1
	vpaddw	%ymm10, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm9, %ymm3
	vpmulhrsw	%ymm1, %ymm13, %ymm0
	vpaddw	%ymm7, %ymm2, %ymm2
	vmovdqa	%ymm1, 544(%rsp)
	vmovdqa	%ymm3, 576(%rsp)
	vpmulhrsw	%ymm3, %ymm13, %ymm8
	vmovdqa	%ymm0, 128(%rsp)
	vmovdqa	%ymm8, 96(%rsp)
	vpaddw	12512(%rbx), %ymm11, %ymm3
	vmovdqa	9408(%rbx), %ymm6
	vmovdqa	6144(%rbx), %ymm9
	vpaddw	3328(%rbx), %ymm9, %ymm8
	vmovdqa	%ymm15, 480(%rsp)
	vpaddw	%ymm3, %ymm2, %ymm3
	vpsubw	%ymm4, %ymm6, %ymm4
	vmovdqa	12704(%rbx), %ymm6
	vmovdqa	6176(%rbx), %ymm9
	vpaddw	%ymm15, %ymm3, %ymm0
	vpaddw	%ymm4, %ymm5, %ymm5
	vmovdqa	6240(%rbx), %ymm7
	vmovdqa	12320(%rbx), %ymm1
	vpaddw	%ymm0, %ymm5, %ymm5
	vpsubw	12576(%rbx), %ymm6, %ymm0
	vmovdqa	%ymm5, 608(%rsp)
	vpmulhrsw	%ymm5, %ymm13, %ymm14
	vpsubw	%ymm3, %ymm0, %ymm6
	vpaddw	6144(%rbx), %ymm9, %ymm0
	vmovdqa	6208(%rbx), %ymm9
	vpmulhrsw	%ymm6, %ymm13, %ymm11
	vpaddw	3360(%rbx), %ymm9, %ymm9
	vmovdqa	%ymm6, 640(%rsp)
	vmovdqa	9216(%rbx), %ymm6
	vmovdqa	%ymm14, 64(%rsp)
	vmovdqa	3104(%rbx), %ymm14
	vpaddw	6400(%rbx), %ymm14, %ymm2
	vmovdqa	%ymm11, 32(%rsp)
	vpsubw	%ymm0, %ymm2, %ymm2
	vpsubw	%ymm14, %ymm8, %ymm5
	vmovdqa	3168(%rbx), %ymm11
	vpaddw	6208(%rbx), %ymm7, %ymm0
	vmovdqa	12288(%rbx), %ymm7
	vpsubw	%ymm8, %ymm14, %ymm8
	vpmulhrsw	%ymm13, %ymm2, %ymm3
	vpaddw	6432(%rbx), %ymm11, %ymm10
	vpsubw	3072(%rbx), %ymm5, %ymm5
	vpsubw	%ymm11, %ymm9, %ymm15
	vpsubw	3136(%rbx), %ymm15, %ymm15
	vpsubw	%ymm0, %ymm10, %ymm10
	vpmulhrsw	%ymm13, %ymm5, %ymm4
	vpaddw	9280(%rbx), %ymm7, %ymm7
	vpmulhrsw	%ymm13, %ymm10, %ymm0
	vpmulhrsw	%ymm13, %ymm15, %ymm15
	vpsubw	%ymm9, %ymm11, %ymm9
	vpmullw	%ymm12, %ymm3, %ymm3
	vpmullw	%ymm12, %ymm4, %ymm4
	vpmullw	%ymm12, %ymm0, %ymm0
	vpmullw	%ymm12, %ymm15, %ymm15
	vpsubw	%ymm4, %ymm5, %ymm5
	vpsubw	%ymm0, %ymm10, %ymm0
	vpaddw	%ymm15, %ymm9, %ymm15
	vmovdqa	3136(%rbx), %ymm9
	vmovdqa	%ymm0, %ymm10
	vpaddw	9248(%rbx), %ymm6, %ymm0
	vmovdqa	9248(%rbx), %ymm6
	vpaddw	12352(%rbx), %ymm6, %ymm6
	vpsubw	%ymm0, %ymm7, %ymm7
	vpaddw	12288(%rbx), %ymm1, %ymm0
	vpaddw	6176(%rbx), %ymm9, %ymm9
	vpaddw	3072(%rbx), %ymm4, %ymm4
	vmovdqa	%ymm10, 416(%rsp)
	vpsubw	%ymm0, %ymm6, %ymm0
	vpmulhrsw	%ymm13, %ymm7, %ymm6
	vpaddw	3136(%rbx), %ymm3, %ymm3
	vpaddw	%ymm9, %ymm15, %ymm15
	vpaddw	%ymm8, %ymm4, %ymm14
	vmovdqa	12320(%rbx), %ymm9
	vpmulhrsw	%ymm13, %ymm0, %ymm1
	vpaddw	%ymm14, %ymm15, %ymm4
	vpsubw	%ymm3, %ymm2, %ymm2
	vmovdqa	%ymm14, 224(%rsp)
	vpmullw	%ymm12, %ymm6, %ymm6
	vpmullw	%ymm12, %ymm1, %ymm1
	vpsubw	%ymm6, %ymm7, %ymm7
	vpaddw	%ymm4, %ymm7, %ymm7
	vpsubw	6240(%rbx), %ymm9, %ymm4
	vmovdqa	%ymm7, 672(%rsp)
	vpsubw	%ymm1, %ymm0, %ymm1
	vpsubw	%ymm15, %ymm4, %ymm11
	vpmulhrsw	%ymm7, %ymm13, %ymm4
	vmovdqa	6272(%rbx), %ymm15
	vpaddw	3392(%rbx), %ymm15, %ymm15
	vmovdqa	%ymm11, 704(%rsp)
	vpmulhrsw	%ymm11, %ymm13, %ymm9
	vmovdqa	3232(%rbx), %ymm11
	vpmullw	%ymm12, %ymm4, %ymm8
	vmovdqa	%ymm9, (%rsp)
	vpaddw	9216(%rbx), %ymm2, %ymm9
	vpsubw	3072(%rbx), %ymm9, %ymm4
	vpaddw	%ymm10, %ymm2, %ymm2
	vmovdqa	%ymm9, 736(%rsp)
	vpmulhrsw	%ymm13, %ymm4, %ymm0
	vmovdqa	%ymm8, 384(%rsp)
	vpsubw	%ymm8, %ymm7, %ymm6
	vmovdqa	%ymm6, 352(%rsp)
	vpsubw	%ymm2, %ymm1, %ymm6
	vpmulhrsw	%ymm13, %ymm6, %ymm1
	vpmullw	%ymm12, %ymm0, %ymm2
	vpmullw	%ymm12, %ymm1, %ymm1
	vmovdqa	%ymm2, 320(%rsp)
	vpsubw	%ymm2, %ymm4, %ymm3
	vpaddw	6464(%rbx), %ymm11, %ymm4
	vmovdqa	6336(%rbx), %ymm2
	vmovdqa	6304(%rbx), %ymm11
	vpaddw	6368(%rbx), %ymm2, %ymm8
	vmovdqa	%ymm3, 288(%rsp)
	vpaddw	6272(%rbx), %ymm11, %ymm0
	vmovdqa	6336(%rbx), %ymm11
	vpaddw	3424(%rbx), %ymm11, %ymm14
	vmovdqa	3296(%rbx), %ymm11
	vpaddw	6496(%rbx), %ymm11, %ymm11
	vpsubw	%ymm0, %ymm4, %ymm10
	vpsubw	3232(%rbx), %ymm15, %ymm0
	vpsubw	3200(%rbx), %ymm0, %ymm0
	vpmulhrsw	%ymm13, %ymm10, %ymm4
	vmovdqa	%ymm11, 160(%rsp)
	vmovdqa	%ymm8, 192(%rsp)
	vpsubw	%ymm8, %ymm11, %ymm8
	vmovdqa	12384(%rbx), %ymm7
	vpaddw	9376(%rbx), %ymm7, %ymm11
	vpmulhrsw	%ymm13, %ymm8, %ymm3
	vmovdqa	9312(%rbx), %ymm7
	vpmulhrsw	%ymm13, %ymm0, %ymm0
	vpsubw	3296(%rbx), %ymm14, %ymm2
	vpsubw	3264(%rbx), %ymm2, %ymm2
	vpmullw	%ymm12, %ymm4, %ymm4
	vpaddw	3264(%rbx), %ymm4, %ymm4
	vpmulhrsw	%ymm13, %ymm2, %ymm2
	vpmullw	%ymm12, %ymm3, %ymm9
	vpaddw	9344(%rbx), %ymm7, %ymm3
	vmovdqa	9344(%rbx), %ymm7
	vpsubw	%ymm4, %ymm10, %ymm4
	vpaddw	9312(%rbx), %ymm4, %ymm10
	vpmullw	%ymm12, %ymm0, %ymm0
	vpsubw	%ymm3, %ymm11, %ymm11
	vpaddw	3200(%rbx), %ymm0, %ymm0
	vpmullw	%ymm12, %ymm2, %ymm2
	vmovdqa	%ymm9, 256(%rsp)
	vpsubw	%ymm9, %ymm8, %ymm8
	vpaddw	12448(%rbx), %ymm7, %ymm9
	vmovdqa	12416(%rbx), %ymm7
	vpaddw	12384(%rbx), %ymm7, %ymm3
	vpaddw	%ymm8, %ymm4, %ymm4
	vpsubw	%ymm3, %ymm9, %ymm9
	vpmulhrsw	%ymm13, %ymm11, %ymm3
	vpmulhrsw	%ymm13, %ymm9, %ymm7
	vpmullw	%ymm12, %ymm3, %ymm3
	vpmullw	%ymm12, %ymm7, %ymm7
	vpsubw	%ymm3, %ymm11, %ymm11
	vpsubw	%ymm7, %ymm9, %ymm7
	vpsubw	%ymm4, %ymm7, %ymm7
	vpsubw	3200(%rbx), %ymm10, %ymm4
	vpmulhrsw	%ymm13, %ymm4, %ymm9
	vpmulhrsw	%ymm13, %ymm7, %ymm4
	vpmullw	%ymm12, %ymm4, %ymm4
	vpsubw	%ymm4, %ymm7, %ymm4
	vmovdqa	3296(%rbx), %ymm7
	vpsubw	%ymm14, %ymm7, %ymm14
	vpaddw	%ymm2, %ymm14, %ymm2
	vmovdqa	3264(%rbx), %ymm14
	vpaddw	6304(%rbx), %ymm14, %ymm7
	vmovdqa	3232(%rbx), %ymm14
	vpaddw	%ymm7, %ymm2, %ymm2
	vmovdqa	3072(%rbx), %ymm7
	vpsubw	%ymm15, %ymm14, %ymm15
	vmovdqa	12416(%rbx), %ymm14
	vpaddw	%ymm15, %ymm0, %ymm15
	vpsubw	6368(%rbx), %ymm14, %ymm0
	vpaddw	%ymm2, %ymm15, %ymm3
	vpsubw	%ymm2, %ymm0, %ymm2
	vpaddw	%ymm3, %ymm11, %ymm11
	vmovdqa	576(%rsp), %ymm3
	vpmulhrsw	%ymm13, %ymm2, %ymm0
	vpmulhrsw	%ymm13, %ymm11, %ymm14
	vpmullw	%ymm12, %ymm0, %ymm0
	vpmullw	%ymm12, %ymm14, %ymm14
	vpsubw	%ymm0, %ymm2, %ymm2
	vpaddw	3200(%rbx), %ymm1, %ymm0
	vmovdqa	9408(%rbx), %ymm1
	vpsubw	3072(%rbx), %ymm1, %ymm1
	vpsubw	%ymm0, %ymm6, %ymm6
	vpmullw	96(%rsp), %ymm12, %ymm0
	vpsubw	%ymm0, %ymm3, %ymm3
	vpaddw	%ymm6, %ymm1, %ymm1
	vpaddw	%ymm4, %ymm6, %ymm0
	vmovdqa	3072(%rbx), %ymm6
	vpsubw	%ymm0, %ymm3, %ymm0
	vpmulhrsw	%ymm13, %ymm1, %ymm3
	vpsraw	$15, %ymm6, %ymm6
	vpmullw	%ymm12, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm1, %ymm1
	vpmulhrsw	%ymm13, %ymm0, %ymm3
	vpmullw	%ymm12, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm0, %ymm0
	vpaddw	3072(%rbx), %ymm12, %ymm3
	vpblendvb	%ymm6, %ymm3, %ymm7, %ymm6
	vpaddw	.LC10(%rip), %ymm6, %ymm3
	vpaddw	.LC11(%rip), %ymm6, %ymm7
	vpsraw	$15, %ymm3, %ymm3
	vpblendvb	%ymm3, %ymm6, %ymm7, %ymm3
	vpsraw	$15, %ymm1, %ymm7
	vpaddw	%ymm12, %ymm1, %ymm6
	vpblendvb	%ymm7, %ymm6, %ymm1, %ymm1
	vpaddw	.LC10(%rip), %ymm1, %ymm6
	vpaddw	.LC11(%rip), %ymm1, %ymm7
	vmovdqa	%ymm3, 32(%r12)
	vpsraw	$15, %ymm6, %ymm6
	vpblendvb	%ymm6, %ymm1, %ymm7, %ymm1
	vpsraw	$15, %ymm0, %ymm7
	vpaddw	%ymm12, %ymm0, %ymm6
	vmovdqa	%ymm1, 800(%r12)
	vpblendvb	%ymm7, %ymm6, %ymm0, %ymm0
	vpaddw	.LC10(%rip), %ymm0, %ymm6
	vpaddw	.LC11(%rip), %ymm0, %ymm7
	vpsraw	$15, %ymm6, %ymm6
	vpblendvb	%ymm6, %ymm0, %ymm7, %ymm0
	vpsraw	$15, %ymm4, %ymm6
	vpaddw	%ymm12, %ymm4, %ymm7
	vmovdqa	%ymm0, 1568(%r12)
	vpblendvb	%ymm6, %ymm7, %ymm4, %ymm4
	vpaddw	.LC10(%rip), %ymm4, %ymm6
	vpaddw	.LC11(%rip), %ymm4, %ymm7
	vpmullw	(%rsp), %ymm12, %ymm0
	vpsubw	%ymm0, %ymm15, %ymm15
	vpaddw	704(%rsp), %ymm15, %ymm15
	vpaddw	224(%rsp), %ymm15, %ymm1
	vpsraw	$15, %ymm6, %ymm6
	vpblendvb	%ymm6, %ymm4, %ymm7, %ymm4
	vmovdqa	3072(%rbx), %ymm6
	vmovdqa	%ymm4, 2336(%r12)
	vmovdqa	512(%rsp), %ymm4
	vpsubw	480(%rsp), %ymm4, %ymm0
	vmovdqa	640(%rsp), %ymm4
	vpaddw	%ymm0, %ymm1, %ymm1
	vpmullw	32(%rsp), %ymm12, %ymm0
	vpsubw	%ymm0, %ymm4, %ymm3
	vpaddw	%ymm2, %ymm15, %ymm0
	vpsraw	$15, %ymm5, %ymm4
	vpsubw	%ymm0, %ymm3, %ymm0
	vpmulhrsw	%ymm13, %ymm1, %ymm3
	vpmullw	%ymm12, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm1, %ymm1
	vpmulhrsw	%ymm13, %ymm0, %ymm3
	vpmullw	%ymm12, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm0, %ymm0
	vpaddw	%ymm12, %ymm5, %ymm3
	vpblendvb	%ymm4, %ymm3, %ymm5, %ymm5
	vpaddw	.LC10(%rip), %ymm5, %ymm3
	vpaddw	.LC11(%rip), %ymm5, %ymm4
	vpsraw	$15, %ymm3, %ymm3
	vpblendvb	%ymm3, %ymm5, %ymm4, %ymm5
	vpsraw	$15, %ymm1, %ymm4
	vpaddw	%ymm12, %ymm1, %ymm3
	vmovdqa	%ymm5, 224(%r12)
	vpblendvb	%ymm4, %ymm3, %ymm1, %ymm1
	vpaddw	.LC10(%rip), %ymm1, %ymm3
	vpaddw	.LC11(%rip), %ymm1, %ymm4
	vpsraw	$15, %ymm3, %ymm3
	vpblendvb	%ymm3, %ymm1, %ymm4, %ymm1
	vpsraw	$15, %ymm0, %ymm4
	vpaddw	%ymm12, %ymm0, %ymm3
	vmovdqa	%ymm1, 992(%r12)
	vpblendvb	%ymm4, %ymm3, %ymm0, %ymm0
	vpaddw	.LC10(%rip), %ymm0, %ymm3
	vpaddw	.LC11(%rip), %ymm0, %ymm4
	vpsraw	$15, %ymm3, %ymm3
	vpblendvb	%ymm3, %ymm0, %ymm4, %ymm0
	vpsraw	$15, %ymm2, %ymm4
	vpaddw	%ymm12, %ymm2, %ymm3
	vmovdqa	%ymm0, 1760(%r12)
	vpblendvb	%ymm4, %ymm3, %ymm2, %ymm2
	vpaddw	.LC10(%rip), %ymm2, %ymm4
	vpaddw	.LC11(%rip), %ymm2, %ymm3
	vmovdqa	416(%rsp), %ymm0
	vpsraw	$15, %ymm4, %ymm4
	vpblendvb	%ymm4, %ymm2, %ymm3, %ymm2
	vpmullw	%ymm12, %ymm9, %ymm4
	vpsubw	%ymm10, %ymm0, %ymm10
	vpmullw	128(%rsp), %ymm12, %ymm0
	vmovdqa	%ymm2, 2528(%r12)
	vpaddw	3200(%rbx), %ymm4, %ymm4
	vpsubw	%ymm0, %ymm6, %ymm0
	vmovdqa	320(%rsp), %ymm15
	vpaddw	6240(%rbx), %ymm14, %ymm14
	vmovdqa	12576(%rbx), %ymm7
	vpaddw	544(%rsp), %ymm15, %ymm1
	vpaddw	%ymm10, %ymm4, %ymm4
	vmovdqa	256(%rsp), %ymm9
	vmovdqa	192(%rsp), %ymm15
	vmovdqa	288(%rsp), %ymm3
	vmovdqa	352(%rsp), %ymm5
	vpaddw	%ymm1, %ymm0, %ymm0
	vpsubw	736(%rsp), %ymm4, %ymm1
	vpsubw	%ymm4, %ymm9, %ymm4
	vpaddw	%ymm3, %ymm12, %ymm2
	vpaddw	%ymm1, %ymm0, %ymm0
	vpsubw	160(%rsp), %ymm15, %ymm1
	vpaddw	448(%rsp), %ymm1, %ymm1
	vpaddw	%ymm4, %ymm1, %ymm4
	vpmulhrsw	%ymm13, %ymm0, %ymm1
	vpmullw	%ymm12, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm0, %ymm0
	vpmulhrsw	%ymm13, %ymm4, %ymm1
	vpmullw	%ymm12, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm4, %ymm4
	vpsraw	$15, %ymm3, %ymm1
	vpblendvb	%ymm1, %ymm2, %ymm3, %ymm1
	vpaddw	.LC10(%rip), %ymm1, %ymm2
	vpaddw	.LC11(%rip), %ymm1, %ymm3
	vpsraw	$15, %ymm2, %ymm2
	vpblendvb	%ymm2, %ymm1, %ymm3, %ymm2
	vpsraw	$15, %ymm0, %ymm3
	vpaddw	%ymm12, %ymm0, %ymm1
	vmovdqa	%ymm2, 416(%r12)
	vpblendvb	%ymm3, %ymm1, %ymm0, %ymm1
	vpaddw	.LC10(%rip), %ymm1, %ymm0
	vpaddw	.LC11(%rip), %ymm1, %ymm3
	vpsraw	$15, %ymm5, %ymm2
	vpsraw	$15, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm1, %ymm3, %ymm1
	vpsraw	$15, %ymm4, %ymm3
	vpaddw	%ymm12, %ymm4, %ymm0
	vmovdqa	%ymm1, 1184(%r12)
	vpblendvb	%ymm3, %ymm0, %ymm4, %ymm4
	vpaddw	.LC10(%rip), %ymm4, %ymm0
	vpaddw	.LC11(%rip), %ymm4, %ymm3
	vpaddw	608(%rsp), %ymm14, %ymm1
	vpsraw	$15, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm4, %ymm3, %ymm4
	vpsraw	$15, %ymm8, %ymm3
	vpaddw	%ymm12, %ymm8, %ymm0
	vmovdqa	%ymm4, 1952(%r12)
	vpblendvb	%ymm3, %ymm0, %ymm8, %ymm8
	vpaddw	.LC10(%rip), %ymm8, %ymm3
	vpaddw	.LC11(%rip), %ymm8, %ymm0
	vpsraw	$15, %ymm3, %ymm3
	vpblendvb	%ymm3, %ymm8, %ymm0, %ymm8
	vpmullw	64(%rsp), %ymm12, %ymm0
	vmovdqa	%ymm8, 2720(%r12)
	vmovdqa	384(%rsp), %ymm8
	vpsubw	%ymm0, %ymm8, %ymm0
	vpaddw	%ymm1, %ymm0, %ymm0
	vpaddw	672(%rsp), %ymm11, %ymm1
	vpsubw	%ymm14, %ymm11, %ymm11
	vpsubw	%ymm1, %ymm0, %ymm0
	vpsubw	6368(%rbx), %ymm7, %ymm1
	vpaddw	%ymm11, %ymm1, %ymm11
	vpmulhrsw	%ymm13, %ymm0, %ymm1
	vpmullw	%ymm12, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm0, %ymm0
	vpmulhrsw	%ymm13, %ymm11, %ymm1
	vpmullw	%ymm12, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm11, %ymm11
	vpaddw	%ymm5, %ymm12, %ymm1
	vpblendvb	%ymm2, %ymm1, %ymm5, %ymm1
	vpaddw	.LC10(%rip), %ymm1, %ymm2
	vpaddw	.LC11(%rip), %ymm1, %ymm3
	vpsraw	$15, %ymm2, %ymm2
	vpblendvb	%ymm2, %ymm1, %ymm3, %ymm1
	vpsraw	$15, %ymm0, %ymm3
	vpaddw	%ymm12, %ymm0, %ymm2
	vpblendvb	%ymm3, %ymm2, %ymm0, %ymm0
	vpaddw	.LC10(%rip), %ymm0, %ymm2
	vpaddw	.LC11(%rip), %ymm0, %ymm3
	vpsraw	$15, %ymm2, %ymm2
	vpblendvb	%ymm2, %ymm0, %ymm3, %ymm3
	vpsraw	$15, %ymm11, %ymm2
	vpaddw	%ymm12, %ymm11, %ymm0
	vpblendvb	%ymm2, %ymm0, %ymm11, %ymm11
	vpaddw	.LC10(%rip), %ymm11, %ymm0
	vmovdqa	6368(%rbx), %ymm5
	vmovdqa	%ymm1, 608(%r12)
	vpaddw	.LC11(%rip), %ymm11, %ymm2
	vmovdqa	%ymm3, 1376(%r12)
	vpsraw	$15, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm11, %ymm2, %ymm11
	vpsraw	$15, %ymm5, %ymm2
	vpaddw	6368(%rbx), %ymm12, %ymm0
	vmovdqa	6368(%rbx), %ymm5
	vmovdqa	%ymm11, 2144(%r12)
	vpblendvb	%ymm2, %ymm0, %ymm5, %ymm0
	vpaddw	.LC10(%rip), %ymm0, %ymm4
	vpaddw	.LC11(%rip), %ymm0, %ymm2
	vpsraw	$15, %ymm4, %ymm4
	vpblendvb	%ymm4, %ymm0, %ymm2, %ymm0
	vmovdqa	%ymm0, 2912(%r12)
	cmpq	%r14, %rbx
	jne	.L29
	vzeroupper
	leaq	-32(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5311:
	.size	mult768_mix2_m256i, .-mult768_mix2_m256i
	.p2align 4
	.globl	CRYPTO_NAMESPACE(rq_mult)
	.type	CRYPTO_NAMESPACE(rq_mult), @function
CRYPTO_NAMESPACE(rq_mult):
.LFB5312:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	.cfi_offset 3, -24
	movq	%rdi, %rbx
	andq	$-32, %rsp
	subq	$3072, %rsp
	movq	%rsp, %rdi
	call	mult768_mix2_m256i
	vmovdqu	2(%rsp), %xmm2
	vmovdqu	1522(%rsp), %xmm1
	leaq	1522(%rsp), %rcx
	vmovdqu	1524(%rsp), %xmm4
	movswl	(%rsp), %eax
	vpmovsxwd	%xmm2, %xmm3
	vpmovsxwd	%xmm1, %xmm0
	movswl	1522(%rsp), %edx
	vmovdqa	.LC8(%rip), %ymm6
	vpsrldq	$8, %xmm1, %xmm1
	vpsrldq	$8, %xmm2, %xmm2
	vpaddd	%xmm3, %xmm0, %xmm0
	vmovdqa	.LC10(%rip), %ymm5
	vpmovsxwd	%xmm2, %xmm2
	vpmovsxwd	%xmm1, %xmm1
	vpmovsxwd	%xmm4, %xmm3
	addl	%eax, %edx
	vpaddd	%xmm2, %xmm1, %xmm1
	vpsrldq	$8, %xmm4, %xmm2
	vpaddd	%xmm3, %xmm0, %xmm3
	vmovdqa	.LC12(%rip), %xmm4
	vpslld	$3, %xmm3, %xmm0
	imull	$228, %edx, %eax
	vpmovsxwd	%xmm2, %xmm2
	vpaddd	%xmm2, %xmm1, %xmm2
	vpsubd	%xmm3, %xmm0, %xmm0
	vpslld	$3, %xmm2, %xmm1
	vpslld	$3, %xmm0, %xmm0
	vpsubd	%xmm2, %xmm1, %xmm1
	vpaddd	%xmm3, %xmm0, %xmm0
	sarl	$20, %eax
	vpslld	$3, %xmm1, %xmm1
	vpslld	$2, %xmm0, %xmm0
	vpsrad	$20, %xmm0, %xmm0
	imull	$-4591, %eax, %eax
	vpaddd	%xmm2, %xmm1, %xmm1
	vpslld	$2, %xmm1, %xmm1
	vpmulld	%xmm4, %xmm0, %xmm0
	vpsrad	$20, %xmm1, %xmm1
	vpmulld	%xmm4, %xmm1, %xmm1
	addl	%edx, %eax
	vmovdqa	.LC14(%rip), %xmm4
	imull	$58470, %eax, %edx
	vpaddd	%xmm3, %xmm0, %xmm0
	vmovdqa	.LC13(%rip), %xmm3
	addl	$134217728, %edx
	vpaddd	%xmm2, %xmm1, %xmm1
	vpmulld	%xmm3, %xmm0, %xmm2
	sarl	$28, %edx
	vpmulld	%xmm3, %xmm1, %xmm3
	imull	$-4591, %edx, %edx
	addl	%edx, %eax
	leaq	18(%rbx), %rdx
	vpaddd	%xmm4, %xmm2, %xmm2
	movw	%ax, (%rbx)
	leaq	18(%rsp), %rax
	vpaddd	%xmm4, %xmm3, %xmm3
	vmovdqa	.LC15(%rip), %xmm4
	vpsrad	$28, %xmm2, %xmm2
	vpsrad	$28, %xmm3, %xmm3
	vpand	%xmm0, %xmm4, %xmm0
	vpand	%xmm1, %xmm4, %xmm1
	vpand	%xmm3, %xmm4, %xmm3
	vpand	%xmm2, %xmm4, %xmm2
	vpackusdw	%xmm1, %xmm0, %xmm0
	vmovdqa	.LC11(%rip), %ymm4
	vpackusdw	%xmm3, %xmm2, %xmm2
	vmovdqa	.LC9(%rip), %ymm3
	vpmullw	.LC16(%rip), %xmm2, %xmm2
	vpaddw	%xmm0, %xmm2, %xmm0
	vmovups	%xmm0, 2(%rbx)
	.p2align 4,,10
	.p2align 3
.L38:
	vmovdqu	1520(%rax), %ymm7
	addq	$32, %rax
	addq	$32, %rdx
	vpaddw	1490(%rax), %ymm7, %ymm0
	vpaddw	-32(%rax), %ymm0, %ymm0
	vpmulhrsw	%ymm6, %ymm0, %ymm1
	vpmullw	%ymm3, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm0, %ymm0
	vpaddw	%ymm3, %ymm0, %ymm1
	vpsraw	$15, %ymm0, %ymm2
	vpblendvb	%ymm2, %ymm1, %ymm0, %ymm0
	vpaddw	%ymm0, %ymm5, %ymm2
	vpaddw	%ymm0, %ymm4, %ymm1
	vpsraw	$15, %ymm2, %ymm2
	vpblendvb	%ymm2, %ymm0, %ymm1, %ymm0
	vmovdqu	%ymm0, -32(%rdx)
	cmpq	%rax, %rcx
	jne	.L38
	movq	$0, 1522(%rbx)
	xorl	%eax, %eax
	movl	$0, 1530(%rbx)
	movw	%ax, 1534(%rbx)
	vzeroupper
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5312:
	.size	CRYPTO_NAMESPACE(rq_mult), .-CRYPTO_NAMESPACE(rq_mult)
	.p2align 4
	.globl	CRYPTO_NAMESPACE(r3_mult)
	.type	CRYPTO_NAMESPACE(r3_mult), @function
CRYPTO_NAMESPACE(r3_mult):
.LFB5313:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	.cfi_offset 13, -24
	movq	%rdx, %r13
	movl	$1536, %edx
	pushq	%r12
	.cfi_offset 12, -32
	movq	%rsi, %r12
	xorl	%esi, %esi
	pushq	%rbx
	.cfi_offset 3, -40
	movq	%rdi, %rbx
	andq	$-32, %rsp
	subq	$4608, %rsp
	movq	%rsp, %r9
	movq	%r9, %rdi
	call	memset@PLT
	movq	%r12, %rdx
	leaq	1472(%rsp), %rcx
	movq	%rax, %r9
	.p2align 4,,10
	.p2align 3
.L42:
	vpmovsxbw	(%rdx), %ymm0
	vmovdqu	(%rdx), %ymm4
	addq	$64, %rax
	addq	$32, %rdx
	vmovdqa	%ymm0, -64(%rax)
	vextracti128	$0x1, %ymm4, %xmm0
	vpmovsxbw	%xmm0, %ymm0
	vmovdqa	%ymm0, -32(%rax)
	cmpq	%rcx, %rax
	jne	.L42
	movl	$736, %eax
	.p2align 4,,10
	.p2align 3
.L43:
	movsbw	(%r12,%rax), %dx
	movw	%dx, (%r9,%rax,2)
	incq	%rax
	cmpq	$761, %rax
	jne	.L43
	leaq	1536(%rsp), %r12
	movq	%r13, %rdx
	movq	%r9, %rsi
	movq	%r12, %rdi
	vzeroupper
	call	mult768_mix2_m256i
	movswl	1536(%rsp), %eax
	movswl	3058(%rsp), %edx
	vmovdqa	.LC17(%rip), %ymm7
	vmovdqa	.LC18(%rip), %ymm6
	addl	%eax, %edx
	vmovdqa	.LC19(%rip), %ymm5
	vmovdqa	.LC20(%rip), %ymm3
	imull	$10923, %edx, %eax
	vmovdqa	.LC21(%rip), %ymm4
	sarl	$15, %eax
	leal	0(,%rax,4), %ecx
	subl	%ecx, %eax
	addl	%edx, %eax
	imull	$89478485, %eax, %edx
	addl	$134217728, %edx
	sarl	$28, %edx
	leal	0(,%rdx,4), %ecx
	subl	%ecx, %edx
	leaq	3060(%rsp), %rcx
	addl	%edx, %eax
	leaq	4532(%rsp), %rdx
	movb	%al, (%rbx)
	leaq	1(%rbx), %rax
	.p2align 4,,10
	.p2align 3
.L44:
	vmovdqu	-2(%rcx), %ymm1
	vpmovsxwd	-2(%rcx), %ymm0
	addq	$64, %rcx
	addq	$32, %rax
	vpmovsxwd	-1554(%rcx), %ymm12
	vpmovsxwd	-1586(%rcx), %ymm11
	vextracti128	$0x1, %ymm1, %xmm9
	vmovdqu	-1586(%rcx), %ymm1
	vpmovsxwd	%xmm9, %ymm9
	vpaddd	%ymm11, %ymm0, %ymm0
	vpmovsxwd	-64(%rcx), %ymm11
	vextracti128	$0x1, %ymm1, %xmm10
	vmovdqu	-64(%rcx), %ymm1
	vpmovsxwd	%xmm10, %ymm10
	vpaddd	%ymm11, %ymm0, %ymm11
	vmovdqu	-32(%rcx), %ymm0
	vpaddd	%ymm10, %ymm9, %ymm9
	vextracti128	$0x1, %ymm1, %xmm10
	vpmovsxwd	-34(%rcx), %ymm1
	vpmovsxwd	%xmm10, %ymm10
	vpaddd	%ymm12, %ymm1, %ymm1
	vpmovsxwd	-32(%rcx), %ymm12
	vpaddd	%ymm10, %ymm9, %ymm10
	vpmulld	%ymm7, %ymm10, %ymm9
	vpaddd	%ymm12, %ymm1, %ymm12
	vmovdqu	-34(%rcx), %ymm1
	vextracti128	$0x1, %ymm1, %xmm8
	vmovdqu	-1554(%rcx), %ymm1
	vpmovsxwd	%xmm8, %ymm8
	vextracti128	$0x1, %ymm1, %xmm2
	vpsrad	$15, %ymm9, %ymm9
	vpmovsxwd	%xmm2, %ymm2
	vpaddd	%ymm2, %ymm8, %ymm8
	vextracti128	$0x1, %ymm0, %xmm2
	vpmulld	%ymm7, %ymm11, %ymm0
	vpmovsxwd	%xmm2, %ymm2
	vpaddd	%ymm2, %ymm8, %ymm2
	vpsrad	$15, %ymm0, %ymm0
	vpslld	$2, %ymm0, %ymm1
	vpsubd	%ymm1, %ymm0, %ymm0
	vpslld	$2, %ymm9, %ymm1
	vpsubd	%ymm1, %ymm9, %ymm9
	vpmulld	%ymm7, %ymm12, %ymm1
	vpaddd	%ymm11, %ymm0, %ymm0
	vpaddd	%ymm10, %ymm9, %ymm9
	vpsrad	$15, %ymm1, %ymm1
	vpslld	$2, %ymm1, %ymm8
	vpsubd	%ymm8, %ymm1, %ymm1
	vpmulld	%ymm7, %ymm2, %ymm8
	vpaddd	%ymm12, %ymm1, %ymm1
	vpsrad	$15, %ymm8, %ymm8
	vpslld	$2, %ymm8, %ymm10
	vpsubd	%ymm10, %ymm8, %ymm8
	vpmulld	%ymm6, %ymm0, %ymm10
	vpand	%ymm0, %ymm3, %ymm0
	vpaddd	%ymm2, %ymm8, %ymm8
	vpmulld	%ymm6, %ymm9, %ymm2
	vpand	%ymm9, %ymm3, %ymm9
	vpmulld	%ymm6, %ymm8, %ymm11
	vpand	%ymm8, %ymm3, %ymm8
	vpackusdw	%ymm9, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpand	%ymm0, %ymm4, %ymm0
	vpaddd	%ymm5, %ymm10, %ymm10
	vpaddd	%ymm5, %ymm2, %ymm2
	vpsrad	$28, %ymm10, %ymm10
	vpsrad	$28, %ymm2, %ymm2
	vpand	%ymm10, %ymm3, %ymm10
	vpaddd	%ymm5, %ymm11, %ymm11
	vpand	%ymm2, %ymm3, %ymm2
	vpsrad	$28, %ymm11, %ymm11
	vpackusdw	%ymm2, %ymm10, %ymm10
	vpmulld	%ymm6, %ymm1, %ymm2
	vpand	%ymm11, %ymm3, %ymm11
	vpermq	$216, %ymm10, %ymm10
	vpand	%ymm1, %ymm3, %ymm1
	vpand	%ymm10, %ymm4, %ymm10
	vpackusdw	%ymm8, %ymm1, %ymm1
	vpermq	$216, %ymm1, %ymm1
	vpand	%ymm1, %ymm4, %ymm1
	vpaddd	%ymm5, %ymm2, %ymm2
	vpackuswb	%ymm1, %ymm0, %ymm0
	vpsrad	$28, %ymm2, %ymm2
	vpermq	$216, %ymm0, %ymm0
	vpand	%ymm2, %ymm3, %ymm2
	vpackusdw	%ymm11, %ymm2, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vpand	%ymm2, %ymm4, %ymm2
	vpackuswb	%ymm2, %ymm10, %ymm2
	vpermq	$216, %ymm2, %ymm2
	vpaddb	%ymm2, %ymm2, %ymm10
	vpaddb	%ymm2, %ymm10, %ymm2
	vpsubb	%ymm2, %ymm0, %ymm0
	vmovdqu	%ymm0, -32(%rax)
	cmpq	%rdx, %rcx
	jne	.L44
	movswl	4530(%rsp), %esi
	leaq	737(%rbx), %rdi
	leaq	3044(%r12), %r8
	.p2align 4,,10
	.p2align 3
.L45:
	movl	%esi, %eax
	movswl	-1522(%rcx), %edx
	movswl	(%rcx), %esi
	addq	$2, %rcx
	incq	%rdi
	addl	%esi, %edx
	addl	%eax, %edx
	imull	$10923, %edx, %eax
	sarl	$15, %eax
	leal	0(,%rax,4), %r9d
	subl	%r9d, %eax
	addl	%eax, %edx
	imull	$89478485, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	leal	0(,%rax,4), %r9d
	subl	%r9d, %eax
	addl	%edx, %eax
	movb	%al, -1(%rdi)
	cmpq	%rcx, %r8
	jne	.L45
	xorl	%eax, %eax
	movb	$0, 767(%rbx)
	movl	$0, 761(%rbx)
	movw	%ax, 765(%rbx)
	vzeroupper
	leaq	-24(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5313:
	.size	CRYPTO_NAMESPACE(r3_mult), .-CRYPTO_NAMESPACE(r3_mult)
	.section	.rodata.cst32,"aM",@progbits,32
	.align 32
.LC0:
	.long	3212836864
	.long	3212836864
	.long	3212836864
	.long	3212836864
	.long	3212836864
	.long	3212836864
	.long	3212836864
	.long	3212836864
	.align 32
.LC1:
	.long	1262485504
	.long	1262485504
	.long	1262485504
	.long	1262485504
	.long	1262485504
	.long	1262485504
	.long	1262485504
	.long	1262485504
	.align 32
.LC2:
	.long	3409969152
	.long	3409969152
	.long	3409969152
	.long	3409969152
	.long	3409969152
	.long	3409969152
	.long	3409969152
	.long	3409969152
	.align 32
.LC3:
	.long	962881006
	.long	962881006
	.long	962881006
	.long	962881006
	.long	962881006
	.long	962881006
	.long	962881006
	.long	962881006
	.align 32
.LC4:
	.long	3314513920
	.long	3314513920
	.long	3314513920
	.long	3314513920
	.long	3314513920
	.long	3314513920
	.long	3314513920
	.long	3314513920
	.align 32
.LC5:
	.long	1262485504
	.long	1262485504
	.long	1262485504
	.long	1262485504
	.long	1262485504
	.long	1262485504
	.long	1262485504
	.long	1262485504
	.align 32
.LC6:
	.long	1065353216
	.long	1065353216
	.long	1065353216
	.long	1065353216
	.long	1065353216
	.long	1065353216
	.long	1065353216
	.long	1065353216
	.section	.rodata.cst16,"aM",@progbits,16
	.align 16
.LC7:
	.long	-1262485504
	.long	-1262485504
	.long	-1262485504
	.long	-1262485504
	.section	.rodata.cst32
	.align 32
.LC8:
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.value	7
	.align 32
.LC9:
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.value	4591
	.align 32
.LC10:
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.value	-2296
	.align 32
.LC11:
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.section	.rodata.cst16
	.align 16
.LC12:
	.long	-4591
	.long	-4591
	.long	-4591
	.long	-4591
	.align 16
.LC13:
	.long	58470
	.long	58470
	.long	58470
	.long	58470
	.align 16
.LC14:
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.align 16
.LC15:
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.align 16
.LC16:
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.value	-4591
	.section	.rodata.cst32
	.align 32
.LC17:
	.long	10923
	.long	10923
	.long	10923
	.long	10923
	.long	10923
	.long	10923
	.long	10923
	.long	10923
	.align 32
.LC18:
	.long	89478485
	.long	89478485
	.long	89478485
	.long	89478485
	.long	89478485
	.long	89478485
	.long	89478485
	.long	89478485
	.align 32
.LC19:
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.align 32
.LC20:
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.align 32
.LC21:
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
	.ident	"GCC: (GNU) 9.2.1 20190827 (Red Hat 9.2.1-1)"
	.section	.note.GNU-stack,"",@progbits
