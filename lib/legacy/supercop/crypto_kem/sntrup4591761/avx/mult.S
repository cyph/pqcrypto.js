	.file	"mult.c"
	.text
	.p2align 4,,15
	.type	mult24x8_float, @function
mult24x8_float:
.LFB5196:
	.cfi_startproc
	vmovaps	(%rsi), %ymm4
	vmovaps	32(%rsi), %ymm5
	vmovaps	64(%rsi), %ymm6
	vmovaps	96(%rsi), %ymm7
	vmovaps	128(%rsi), %ymm8
	vmovaps	160(%rsi), %ymm9
	vmovaps	(%rdx), %ymm12
	vmovaps	32(%rdx), %ymm13
#APP
# 149 "mult.c" 1
	vmulps %ymm12,%ymm4,%ymm0 
	vmovups %ymm0,(%rdi) 
	vmulps %ymm12,%ymm5,%ymm10 
	vmulps %ymm12,%ymm6,%ymm1 
	vmulps %ymm12,%ymm7,%ymm2 
	vmulps %ymm12,%ymm8,%ymm3 
	vmulps %ymm12,%ymm9,%ymm0 
	
# 0 "" 2
#NO_APP
	vmovaps	64(%rdx), %ymm14
#APP
# 151 "mult.c" 1
	vfmadd231ps %ymm13,%ymm4,%ymm10 
	vmovups %ymm10,32(%rdi) 
	vfmadd231ps %ymm13,%ymm5,%ymm1 
	vfmadd231ps %ymm13,%ymm6,%ymm2 
	vfmadd231ps %ymm13,%ymm7,%ymm3 
	vfmadd231ps %ymm13,%ymm8,%ymm0 
	vmulps %ymm13,%ymm9,%ymm10 
	
# 0 "" 2
#NO_APP
	vmovaps	96(%rdx), %ymm15
#APP
# 152 "mult.c" 1
	vfmadd231ps %ymm14,%ymm4,%ymm1 
	vmovups %ymm1,64(%rdi) 
	vfmadd231ps %ymm14,%ymm5,%ymm2 
	vfmadd231ps %ymm14,%ymm6,%ymm3 
	vfmadd231ps %ymm14,%ymm7,%ymm0 
	vfmadd231ps %ymm14,%ymm8,%ymm10 
	vmulps %ymm14,%ymm9,%ymm1 
	
# 0 "" 2
#NO_APP
	vmovaps	128(%rdx), %ymm12
#APP
# 153 "mult.c" 1
	vfmadd231ps %ymm15,%ymm4,%ymm2 
	vmovups %ymm2,96(%rdi) 
	vfmadd231ps %ymm15,%ymm5,%ymm3 
	vfmadd231ps %ymm15,%ymm6,%ymm0 
	vfmadd231ps %ymm15,%ymm7,%ymm10 
	vfmadd231ps %ymm15,%ymm8,%ymm1 
	vmulps %ymm15,%ymm9,%ymm2 
	
# 0 "" 2
#NO_APP
	addq	$192, %rsi
#APP
# 154 "mult.c" 1
	vfmadd231ps %ymm12,%ymm4,%ymm3 
	vmovups %ymm3,128(%rdi) 
	vfmadd231ps %ymm12,%ymm5,%ymm0 
	vfmadd231ps %ymm12,%ymm6,%ymm10 
	vfmadd231ps %ymm12,%ymm7,%ymm1 
	vfmadd231ps %ymm12,%ymm8,%ymm2 
	vmulps %ymm12,%ymm9,%ymm3 
	
# 0 "" 2
#NO_APP
	movq	%rdi, %rax
	vmovaps	160(%rdx), %ymm13
	vmovaps	192(%rdx), %ymm14
#APP
# 155 "mult.c" 1
	vfmadd231ps %ymm13,%ymm4,%ymm0 
	vmovups %ymm0,160(%rdi) 
	vfmadd231ps %ymm13,%ymm5,%ymm10 
	vfmadd231ps %ymm13,%ymm6,%ymm1 
	vfmadd231ps %ymm13,%ymm7,%ymm2 
	vfmadd231ps %ymm13,%ymm8,%ymm3 
	vmulps %ymm13,%ymm9,%ymm0 
	
# 0 "" 2
#NO_APP
	vmovaps	224(%rdx), %ymm15
#APP
# 151 "mult.c" 1
	vfmadd231ps %ymm14,%ymm4,%ymm10 
	vmovups %ymm10,192(%rdi) 
	vfmadd231ps %ymm14,%ymm5,%ymm1 
	vfmadd231ps %ymm14,%ymm6,%ymm2 
	vfmadd231ps %ymm14,%ymm7,%ymm3 
	vfmadd231ps %ymm14,%ymm8,%ymm0 
	vmulps %ymm14,%ymm9,%ymm10 
	
# 0 "" 2
#NO_APP
	vmovaps	256(%rdx), %ymm12
#APP
# 152 "mult.c" 1
	vfmadd231ps %ymm15,%ymm4,%ymm1 
	vmovups %ymm1,224(%rdi) 
	vfmadd231ps %ymm15,%ymm5,%ymm2 
	vfmadd231ps %ymm15,%ymm6,%ymm3 
	vfmadd231ps %ymm15,%ymm7,%ymm0 
	vfmadd231ps %ymm15,%ymm8,%ymm10 
	vmulps %ymm15,%ymm9,%ymm1 
	
# 0 "" 2
#NO_APP
	vmovaps	288(%rdx), %ymm13
#APP
# 153 "mult.c" 1
	vfmadd231ps %ymm12,%ymm4,%ymm2 
	vmovups %ymm2,256(%rdi) 
	vfmadd231ps %ymm12,%ymm5,%ymm3 
	vfmadd231ps %ymm12,%ymm6,%ymm0 
	vfmadd231ps %ymm12,%ymm7,%ymm10 
	vfmadd231ps %ymm12,%ymm8,%ymm1 
	vmulps %ymm12,%ymm9,%ymm2 
	
# 0 "" 2
#NO_APP
	leaq	576(%rdi), %rcx
#APP
# 154 "mult.c" 1
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
#APP
# 155 "mult.c" 1
	vfmadd231ps %ymm14,%ymm4,%ymm0 
	vmovups %ymm0,320(%rdi) 
	vfmadd231ps %ymm14,%ymm5,%ymm10 
	vfmadd231ps %ymm14,%ymm6,%ymm1 
	vfmadd231ps %ymm14,%ymm7,%ymm2 
	vfmadd231ps %ymm14,%ymm8,%ymm3 
	vmulps %ymm14,%ymm9,%ymm0 
	
# 0 "" 2
#NO_APP
	vmovaps	416(%rdx), %ymm13
	vmovaps	448(%rdx), %ymm14
	vmovaps	352(%rdx), %ymm15
#APP
# 151 "mult.c" 1
	vfmadd231ps %ymm15,%ymm4,%ymm10 
	vmovups %ymm10,352(%rdi) 
	vfmadd231ps %ymm15,%ymm5,%ymm1 
	vfmadd231ps %ymm15,%ymm6,%ymm2 
	vfmadd231ps %ymm15,%ymm7,%ymm3 
	vfmadd231ps %ymm15,%ymm8,%ymm0 
	vmulps %ymm15,%ymm9,%ymm10 
	
# 0 "" 2
# 152 "mult.c" 1
	vfmadd231ps %ymm12,%ymm4,%ymm1 
	vmovups %ymm1,384(%rdi) 
	vfmadd231ps %ymm12,%ymm5,%ymm2 
	vfmadd231ps %ymm12,%ymm6,%ymm3 
	vfmadd231ps %ymm12,%ymm7,%ymm0 
	vfmadd231ps %ymm12,%ymm8,%ymm10 
	vmulps %ymm12,%ymm9,%ymm1 
	
# 0 "" 2
# 153 "mult.c" 1
	vfmadd231ps %ymm13,%ymm4,%ymm2 
	vmovups %ymm2,416(%rdi) 
	vfmadd231ps %ymm13,%ymm5,%ymm3 
	vfmadd231ps %ymm13,%ymm6,%ymm0 
	vfmadd231ps %ymm13,%ymm7,%ymm10 
	vfmadd231ps %ymm13,%ymm8,%ymm1 
	vmulps %ymm13,%ymm9,%ymm2 
	
# 0 "" 2
# 154 "mult.c" 1
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
#APP
# 155 "mult.c" 1
	vfmadd231ps %ymm15,%ymm4,%ymm0 
	vmovups %ymm0,480(%rdi) 
	vfmadd231ps %ymm15,%ymm5,%ymm10 
	vfmadd231ps %ymm15,%ymm6,%ymm1 
	vfmadd231ps %ymm15,%ymm7,%ymm2 
	vfmadd231ps %ymm15,%ymm8,%ymm3 
	vmulps %ymm15,%ymm9,%ymm0 
	
# 0 "" 2
#NO_APP
	vmovaps	544(%rdx), %ymm13
#APP
# 151 "mult.c" 1
	vfmadd231ps %ymm12,%ymm4,%ymm10 
	vmovups %ymm10,512(%rdi) 
	vfmadd231ps %ymm12,%ymm5,%ymm1 
	vfmadd231ps %ymm12,%ymm6,%ymm2 
	vfmadd231ps %ymm12,%ymm7,%ymm3 
	vfmadd231ps %ymm12,%ymm8,%ymm0 
	vmulps %ymm12,%ymm9,%ymm10 
	
# 0 "" 2
#NO_APP
	vmovaps	576(%rdx), %ymm14
#APP
# 152 "mult.c" 1
	vfmadd231ps %ymm13,%ymm4,%ymm1 
	vmovups %ymm1,544(%rdi) 
	vfmadd231ps %ymm13,%ymm5,%ymm2 
	vfmadd231ps %ymm13,%ymm6,%ymm3 
	vfmadd231ps %ymm13,%ymm7,%ymm0 
	vfmadd231ps %ymm13,%ymm8,%ymm10 
	vmulps %ymm13,%ymm9,%ymm1 
	
# 0 "" 2
#NO_APP
	vmovaps	608(%rdx), %ymm15
#APP
# 153 "mult.c" 1
	vfmadd231ps %ymm14,%ymm4,%ymm2 
	vmovups %ymm2,576(%rdi) 
	vfmadd231ps %ymm14,%ymm5,%ymm3 
	vfmadd231ps %ymm14,%ymm6,%ymm0 
	vfmadd231ps %ymm14,%ymm7,%ymm10 
	vfmadd231ps %ymm14,%ymm8,%ymm1 
	vmulps %ymm14,%ymm9,%ymm2 
	
# 0 "" 2
#NO_APP
	vmovaps	640(%rdx), %ymm11
#APP
# 154 "mult.c" 1
	vfmadd231ps %ymm15,%ymm4,%ymm3 
	vmovups %ymm3,608(%rdi) 
	vfmadd231ps %ymm15,%ymm5,%ymm0 
	vfmadd231ps %ymm15,%ymm6,%ymm10 
	vfmadd231ps %ymm15,%ymm7,%ymm1 
	vfmadd231ps %ymm15,%ymm8,%ymm2 
	vmulps %ymm15,%ymm9,%ymm3 
	
# 0 "" 2
# 155 "mult.c" 1
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
# 157 "mult.c" 1
	vfmadd231ps %ymm12,%ymm4,%ymm10 
	vmovups %ymm10,672(%rdi) 
	vfmadd231ps %ymm12,%ymm5,%ymm1 
	vfmadd231ps %ymm12,%ymm6,%ymm2 
	vfmadd231ps %ymm12,%ymm7,%ymm3 
	vfmadd231ps %ymm12,%ymm8,%ymm0 
	vmulps %ymm12,%ymm9,%ymm10 
	
# 0 "" 2
# 158 "mult.c" 1
	vfmadd231ps %ymm13,%ymm4,%ymm1 
	vmovups %ymm1,704(%rdi) 
	vfmadd231ps %ymm13,%ymm5,%ymm2 
	vfmadd231ps %ymm13,%ymm6,%ymm3 
	vfmadd231ps %ymm13,%ymm7,%ymm0 
	vfmadd231ps %ymm13,%ymm8,%ymm10 
	vmulps %ymm13,%ymm9,%ymm1 
	
# 0 "" 2
# 159 "mult.c" 1
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
	vmovaps	(%rsi), %ymm2
	vmovaps	32(%rsi), %ymm3
	vmovaps	64(%rsi), %ymm4
	vmovaps	96(%rsi), %ymm5
	vmovaps	128(%rsi), %ymm6
	vmovaps	160(%rsi), %ymm7
	vmovaps	192(%rax), %ymm0
	vmovaps	224(%rax), %ymm9
	vmovaps	256(%rax), %ymm11
	vmovaps	288(%rax), %ymm1
	vmovaps	320(%rax), %ymm8
	vmovaps	(%rdx), %ymm12
	vmovaps	32(%rdx), %ymm13
#APP
# 179 "mult.c" 1
	vfmadd231ps %ymm12,%ymm2,%ymm0 
	vmovups %ymm0,192(%rax) 
	vmovups 352(%rax),%ymm0 
	vfmadd231ps %ymm12,%ymm3,%ymm9 
	vfmadd231ps %ymm12,%ymm4,%ymm11 
	vfmadd231ps %ymm12,%ymm5,%ymm1 
	vfmadd231ps %ymm12,%ymm6,%ymm8 
	vfmadd231ps %ymm12,%ymm7,%ymm0 
	
# 0 "" 2
# 180 "mult.c" 1
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
# 181 "mult.c" 1
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
# 182 "mult.c" 1
	vfmadd231ps %ymm15,%ymm2,%ymm1 
	vmovups %ymm1,288(%rax) 
	vmovups 448(%rax),%ymm1 
	vfmadd231ps %ymm15,%ymm3,%ymm8 
	vfmadd231ps %ymm15,%ymm4,%ymm0 
	vfmadd231ps %ymm15,%ymm5,%ymm9 
	vfmadd231ps %ymm15,%ymm6,%ymm11 
	vfmadd231ps %ymm15,%ymm7,%ymm1 
	
# 0 "" 2
# 183 "mult.c" 1
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
	vmovaps	224(%rdx), %ymm15
	vmovaps	%ymm11, %ymm0
#APP
# 179 "mult.c" 1
	vfmadd231ps %ymm13,%ymm2,%ymm10 
	vmovups %ymm10,352(%rax) 
	vmovups 512(%rax),%ymm10 
	vfmadd231ps %ymm13,%ymm3,%ymm9 
	vfmadd231ps %ymm13,%ymm4,%ymm0 
	vfmadd231ps %ymm13,%ymm5,%ymm1 
	vfmadd231ps %ymm13,%ymm6,%ymm8 
	vfmadd231ps %ymm13,%ymm7,%ymm10 
	
# 0 "" 2
# 180 "mult.c" 1
	vfmadd231ps %ymm14,%ymm2,%ymm9 
	vmovups %ymm9,384(%rax) 
	vmovups 544(%rax),%ymm9 
	vfmadd231ps %ymm14,%ymm3,%ymm0 
	vfmadd231ps %ymm14,%ymm4,%ymm1 
	vfmadd231ps %ymm14,%ymm5,%ymm8 
	vfmadd231ps %ymm14,%ymm6,%ymm10 
	vfmadd231ps %ymm14,%ymm7,%ymm9 
	
# 0 "" 2
# 181 "mult.c" 1
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
# 182 "mult.c" 1
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
# 183 "mult.c" 1
	vfmadd231ps %ymm13,%ymm2,%ymm8 
	vmovups %ymm8,480(%rax) 
	vmovups 640(%rax),%ymm8 
	vfmadd231ps %ymm13,%ymm3,%ymm10 
	vfmadd231ps %ymm13,%ymm4,%ymm9 
	vfmadd231ps %ymm13,%ymm5,%ymm0 
	vfmadd231ps %ymm13,%ymm6,%ymm1 
	vfmadd231ps %ymm13,%ymm7,%ymm8 
	
# 0 "" 2
# 179 "mult.c" 1
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
# 180 "mult.c" 1
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
# 181 "mult.c" 1
	vfmadd231ps %ymm12,%ymm2,%ymm0 
	vmovups %ymm0,576(%rax) 
	vmovups 736(%rax),%ymm0 
	vfmadd231ps %ymm12,%ymm3,%ymm1 
	vfmadd231ps %ymm12,%ymm4,%ymm8 
	vfmadd231ps %ymm12,%ymm5,%ymm10 
	vfmadd231ps %ymm12,%ymm6,%ymm9 
	vfmadd231ps %ymm12,%ymm7,%ymm0 
	
# 0 "" 2
# 182 "mult.c" 1
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
# 183 "mult.c" 1
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
# 185 "mult.c" 1
	vfmadd231ps %ymm15,%ymm2,%ymm10 
	vmovups %ymm10,672(%rax) 
	vmovups 832(%rax),%ymm10 
	vfmadd231ps %ymm15,%ymm3,%ymm9 
	vfmadd231ps %ymm15,%ymm4,%ymm0 
	vfmadd231ps %ymm15,%ymm5,%ymm1 
	vfmadd231ps %ymm15,%ymm6,%ymm8 
	vfmadd231ps %ymm15,%ymm7,%ymm10 
	
# 0 "" 2
# 186 "mult.c" 1
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
#APP
# 187 "mult.c" 1
	vfmadd231ps %ymm13,%ymm2,%ymm0 
	vmovups %ymm0,736(%rax) 
	vmovups 896(%rax),%ymm0 
	vfmadd231ps %ymm13,%ymm3,%ymm1 
	vfmadd231ps %ymm13,%ymm4,%ymm8 
	vfmadd231ps %ymm13,%ymm5,%ymm10 
	vfmadd231ps %ymm13,%ymm6,%ymm9 
	vfmadd231ps %ymm13,%ymm7,%ymm0 
	
# 0 "" 2
#NO_APP
	vmovaps	608(%rdx), %ymm15
#APP
# 188 "mult.c" 1
	vfmadd231ps %ymm14,%ymm2,%ymm1 
	vmovups %ymm1,768(%rax) 
	vfmadd231ps %ymm14,%ymm3,%ymm8 
	vfmadd231ps %ymm14,%ymm4,%ymm10 
	vfmadd231ps %ymm14,%ymm5,%ymm9 
	vfmadd231ps %ymm14,%ymm6,%ymm0 
	vmulps %ymm14,%ymm7,%ymm1 
	
# 0 "" 2
#NO_APP
	vmovaps	640(%rdx), %ymm12
#APP
# 189 "mult.c" 1
	vfmadd231ps %ymm15,%ymm2,%ymm8 
	vmovups %ymm8,800(%rax) 
	vfmadd231ps %ymm15,%ymm3,%ymm10 
	vfmadd231ps %ymm15,%ymm4,%ymm9 
	vfmadd231ps %ymm15,%ymm5,%ymm0 
	vfmadd231ps %ymm15,%ymm6,%ymm1 
	vmulps %ymm15,%ymm7,%ymm8 
	
# 0 "" 2
#NO_APP
	addq	$192, %rax
#APP
# 190 "mult.c" 1
	vfmadd231ps %ymm12,%ymm2,%ymm10 
	vmovups %ymm10,640(%rax) 
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
# 191 "mult.c" 1
	vfmadd231ps %ymm13,%ymm2,%ymm9 
	vmovups %ymm9,672(%rax) 
	vfmadd231ps %ymm13,%ymm3,%ymm0 
	vfmadd231ps %ymm13,%ymm4,%ymm1 
	vfmadd231ps %ymm13,%ymm5,%ymm8 
	vfmadd231ps %ymm13,%ymm6,%ymm10 
	vmulps %ymm13,%ymm7,%ymm9 
	
# 0 "" 2
# 192 "mult.c" 1
	vfmadd231ps %ymm14,%ymm2,%ymm0 
	vmovups %ymm0,704(%rax) 
	vfmadd231ps %ymm14,%ymm3,%ymm1 
	vfmadd231ps %ymm14,%ymm4,%ymm8 
	vfmadd231ps %ymm14,%ymm5,%ymm10 
	vfmadd231ps %ymm14,%ymm6,%ymm9 
	vmulps %ymm14,%ymm7,%ymm0 
	
# 0 "" 2
# 193 "mult.c" 1
	vfmadd231ps %ymm15,%ymm2,%ymm1 
	vmovups %ymm1,736(%rax) 
	vfmadd231ps %ymm15,%ymm3,%ymm8 
	vfmadd231ps %ymm15,%ymm4,%ymm10 
	vfmadd231ps %ymm15,%ymm5,%ymm9 
	vfmadd231ps %ymm15,%ymm6,%ymm0 
	vmulps %ymm15,%ymm7,%ymm1 
	
# 0 "" 2
#NO_APP
	vmovaps	%ymm8, 768(%rax)
	vmovaps	%ymm10, 800(%rax)
	vmovaps	%ymm9, 832(%rax)
	vmovaps	%ymm0, 864(%rax)
	vmovaps	%ymm1, 896(%rax)
	addq	$192, %rsi
	cmpq	%rax, %rcx
	jne	.L2
	vxorps	%xmm0, %xmm0, %xmm0
	vmovaps	%ymm0, 1504(%rdi)
	vzeroupper
	ret
	.cfi_endproc
.LFE5196:
	.size	mult24x8_float, .-mult24x8_float
	.p2align 4,,15
	.type	transpose16, @function
transpose16:
.LFB5200:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	andq	$-32, %rsp
	vmovdqa	(%rdi), %ymm0
	vmovdqa	32(%rdi), %ymm1
	vmovdqa	96(%rdi), %ymm4
	vpunpcklwd	%ymm1, %ymm0, %ymm3
	vpunpckhwd	%ymm1, %ymm0, %ymm1
	vmovdqa	64(%rdi), %ymm0
	vmovdqa	224(%rdi), %ymm5
	vpunpcklwd	%ymm4, %ymm0, %ymm2
	vpunpckhwd	%ymm4, %ymm0, %ymm0
	vpunpckldq	%ymm2, %ymm3, %ymm14
	vpunpckldq	%ymm0, %ymm1, %ymm12
	vpunpckhdq	%ymm2, %ymm3, %ymm2
	vpunpckhdq	%ymm0, %ymm1, %ymm0
	vmovdqa	128(%rdi), %ymm3
	vmovdqa	160(%rdi), %ymm1
	vmovdqa	352(%rdi), %ymm9
	vpunpcklwd	%ymm1, %ymm3, %ymm8
	vpunpckhwd	%ymm1, %ymm3, %ymm3
	vmovdqa	192(%rdi), %ymm1
	vmovdqa	384(%rdi), %ymm15
	vpunpcklwd	%ymm5, %ymm1, %ymm4
	vpunpckhwd	%ymm5, %ymm1, %ymm1
	vpunpckldq	%ymm1, %ymm3, %ymm5
	vpunpckhdq	%ymm1, %ymm3, %ymm7
	vmovdqa	288(%rdi), %ymm1
	vmovdqa	256(%rdi), %ymm3
	vmovdqa	%ymm7, -32(%rsp)
	vpunpcklwd	%ymm1, %ymm3, %ymm7
	vpunpckhwd	%ymm1, %ymm3, %ymm3
	vmovdqa	320(%rdi), %ymm1
	vpunpckldq	%ymm4, %ymm8, %ymm6
	vpunpckhdq	%ymm4, %ymm8, %ymm8
	vpunpcklwd	%ymm9, %ymm1, %ymm4
	vpunpckhwd	%ymm9, %ymm1, %ymm1
	vpunpckldq	%ymm1, %ymm3, %ymm9
	vpunpckhdq	%ymm1, %ymm3, %ymm1
	vmovdqa	416(%rdi), %ymm3
	vmovdqa	480(%rdi), %ymm10
	vpunpcklwd	%ymm3, %ymm15, %ymm11
	vpunpckhwd	%ymm3, %ymm15, %ymm15
	vmovdqa	448(%rdi), %ymm3
	vpunpckldq	%ymm4, %ymm7, %ymm13
	vpunpckhdq	%ymm4, %ymm7, %ymm4
	vpunpcklwd	%ymm10, %ymm3, %ymm7
	vpunpckhwd	%ymm10, %ymm3, %ymm3
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
	vmovdqa	%ymm6, 288(%rdi)
	vpunpcklqdq	%ymm11, %ymm9, %ymm10
	vpunpcklqdq	%ymm5, %ymm12, %ymm6
	vpunpckhqdq	%ymm11, %ymm9, %ymm9
	vpunpckhqdq	%ymm5, %ymm12, %ymm5
	vperm2i128	$32, %ymm10, %ymm6, %ymm11
	vperm2i128	$49, %ymm10, %ymm6, %ymm6
	vmovdqa	%ymm6, 384(%rdi)
	vperm2i128	$32, %ymm9, %ymm5, %ymm10
	vpunpcklqdq	%ymm7, %ymm4, %ymm6
	vperm2i128	$49, %ymm9, %ymm5, %ymm9
	vpunpckhqdq	%ymm7, %ymm4, %ymm4
	vpunpcklqdq	%ymm8, %ymm2, %ymm5
	vpunpckhqdq	%ymm8, %ymm2, %ymm2
	vperm2i128	$32, %ymm6, %ymm5, %ymm7
	vperm2i128	$49, %ymm6, %ymm5, %ymm5
	vperm2i128	$32, %ymm4, %ymm2, %ymm6
	vperm2i128	$49, %ymm4, %ymm2, %ymm2
	vmovdqa	%ymm5, 320(%rdi)
	vmovdqa	%ymm2, 352(%rdi)
	vmovdqa	%ymm13, (%rdi)
	vmovdqa	%ymm15, 256(%rdi)
	vmovdqa	%ymm14, 32(%rdi)
	vmovdqa	%ymm11, 128(%rdi)
	vmovdqa	%ymm10, 160(%rdi)
	vmovdqa	%ymm9, 416(%rdi)
	vmovdqa	%ymm7, 64(%rdi)
	vmovdqa	%ymm6, 96(%rdi)
	vmovdqa	-32(%rsp), %ymm7
	vpunpcklqdq	%ymm3, %ymm1, %ymm4
	vpunpcklqdq	%ymm7, %ymm0, %ymm2
	vpunpckhqdq	%ymm3, %ymm1, %ymm1
	vpunpckhqdq	%ymm7, %ymm0, %ymm0
	vperm2i128	$32, %ymm4, %ymm2, %ymm5
	vperm2i128	$32, %ymm1, %ymm0, %ymm3
	vperm2i128	$49, %ymm4, %ymm2, %ymm2
	vperm2i128	$49, %ymm1, %ymm0, %ymm0
	vmovdqa	%ymm5, 192(%rdi)
	vmovdqa	%ymm2, 448(%rdi)
	vmovdqa	%ymm3, 224(%rdi)
	vmovdqa	%ymm0, 480(%rdi)
	vzeroupper
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5200:
	.size	transpose16, .-transpose16
	.p2align 4,,15
	.type	mult48x8_float, @function
mult48x8_float:
.LFB5197:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdi, %r8
	movq	%rsi, %r9
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	andq	$-32, %rsp
	subq	$2304, %rsp
	vmovaps	1472(%rsi), %ymm4
	vmovaps	1472(%rdx), %ymm5
	vaddps	704(%rsi), %ymm4, %ymm0
	vmovaps	1504(%rsi), %ymm6
	vmovaps	1504(%rdx), %ymm7
	vmovaps	%ymm0, 2240(%rsp)
	vaddps	704(%rdx), %ymm5, %ymm0
	vmovaps	1408(%rsi), %ymm4
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
	vmovaps	1376(%rdx), %ymm7
	vaddps	608(%rsi), %ymm6, %ymm0
	vmovaps	512(%rsi), %ymm4
	vmovaps	512(%rdx), %ymm5
	vmovaps	%ymm0, 2144(%rsp)
	vaddps	608(%rdx), %ymm7, %ymm0
	vmovaps	544(%rsi), %ymm6
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
	vmovaps	416(%rsi), %ymm6
	vaddps	1152(%rdx), %ymm5, %ymm0
	vmovaps	416(%rdx), %ymm7
	vmovaps	320(%rsi), %ymm4
	vmovaps	%ymm0, 384(%rsp)
	vaddps	1184(%rsi), %ymm6, %ymm0
	vmovaps	320(%rdx), %ymm5
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
	vmovaps	192(%rdx), %ymm5
	vaddps	960(%rsi), %ymm4, %ymm0
	vmovaps	224(%rsi), %ymm6
	vmovaps	224(%rdx), %ymm7
	vmovaps	%ymm0, 1728(%rsp)
	vaddps	960(%rdx), %ymm5, %ymm0
	vmovaps	128(%rsi), %ymm4
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
	vmovaps	(%rsi), %ymm4
	vaddps	864(%rdx), %ymm7, %ymm0
	vmovaps	(%rdx), %ymm5
	vmovaps	32(%rsi), %ymm6
	vmovaps	%ymm0, 96(%rsp)
	vaddps	768(%rsi), %ymm4, %ymm0
	vmovaps	32(%rdx), %ymm7
	vmovaps	%ymm0, 1536(%rsp)
	vaddps	768(%rdx), %ymm5, %ymm0
	vmovaps	%ymm0, (%rsp)
	vaddps	800(%rsi), %ymm6, %ymm0
	vmovaps	%ymm0, 1568(%rsp)
	vaddps	800(%rdx), %ymm7, %ymm0
	vmovaps	%ymm0, 32(%rsp)
	vzeroupper
	call	mult24x8_float
	leaq	768(%r9), %rsi
	leaq	768(%rdx), %rdx
	leaq	1536(%rdi), %rdi
	call	mult24x8_float
	leaq	768(%rsp), %rax
	leaq	1536(%rsp), %r9
	movq	%rax, %rdi
	movq	%rsp, %rdx
	movq	%r9, %rsi
	call	mult24x8_float
	vmovaps	.LC0(%rip), %ymm2
	movq	%r8, %rdi
	leaq	768(%rsp), %rax
	.p2align 4,,10
	.p2align 3
.L8:
	vmovaps	1536(%rdi), %ymm3
	vmovaps	(%rdi), %ymm1
	vmovaps	2304(%rdi), %ymm0
	vfmadd213ps	768(%rdi), %ymm2, %ymm3
	vfmadd213ps	(%rax), %ymm2, %ymm1
	vfmadd213ps	768(%rax), %ymm2, %ymm0
	addq	$32, %rax
	addq	$32, %rdi
	vaddps	%ymm1, %ymm3, %ymm1
	vsubps	%ymm3, %ymm0, %ymm0
	vmovaps	%ymm1, 736(%rdi)
	vmovaps	%ymm0, 1504(%rdi)
	cmpq	%rax, %r9
	jne	.L8
	vzeroupper
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5197:
	.size	mult48x8_float, .-mult48x8_float
	.p2align 4,,15
	.type	mult96x16, @function
mult96x16:
.LFB5199:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rdx, %r10
	leaq	16(%rdx), %r11
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r15
	pushq	%r14
	.cfi_offset 15, -24
	.cfi_offset 14, -32
	leaq	6016(%rdi), %r14
	pushq	%r13
	pushq	%r12
	pushq	%rbx
	.cfi_offset 13, -40
	.cfi_offset 12, -48
	.cfi_offset 3, -56
	movq	%rsi, %rbx
	andq	$-32, %rsp
	subq	$13824, %rsp
	leaq	7680(%rsp), %r12
	vmovdqa	.LC1(%rip), %ymm8
	vmovaps	.LC2(%rip), %ymm7
	leaq	6080(%r12), %r13
.L12:
	leaq	1536(%rsp), %rdi
	xorl	%eax, %eax
	leaq	4544(%rsp), %rsi
	leaq	6112(%r12), %rcx
	leaq	3040(%rdi), %rdx
	.p2align 4,,10
	.p2align 3
.L13:
	vpmovsxwd	3008(%rbx,%rax), %ymm2
#APP
# 63 "mult.c" 1
	vpmovsxbw 3008(%r10,%rax), %xmm0
# 0 "" 2
#NO_APP
	vpaddd	%ymm8, %ymm2, %ymm2
	vpmovsxwd	%xmm0, %ymm0
	vaddps	%ymm2, %ymm7, %ymm2
	vpaddd	%ymm8, %ymm0, %ymm0
	vaddps	%ymm0, %ymm7, %ymm0
	vmovaps	%ymm2, 0(%r13,%rax)
	vpmovsxwd	3040(%rbx,%rax), %ymm2
	vmovaps	%ymm0, (%rsi,%rax)
#APP
# 63 "mult.c" 1
	vpmovsxbw 3040(%r10,%rax), %xmm0
# 0 "" 2
#NO_APP
	vpmovsxwd	%xmm0, %ymm0
	vpaddd	%ymm8, %ymm2, %ymm2
	vpaddd	%ymm8, %ymm0, %ymm0
	vaddps	%ymm2, %ymm7, %ymm2
	vaddps	%ymm0, %ymm7, %ymm0
	vmovaps	%ymm2, (%rcx,%rax)
	vmovaps	%ymm0, (%rdx,%rax)
	subq	$64, %rax
	cmpq	$-3072, %rax
	jne	.L13
	leaq	4480(%r12), %rax
	leaq	7552(%rsp), %rsi
	leaq	2944(%rsp), %rdx
	leaq	1408(%rsp), %rcx
	leaq	2944(%r12), %rdi
	.p2align 4,,10
	.p2align 3
.L14:
	vmovaps	(%rax), %ymm3
	addq	$-128, %rax
	vaddps	1664(%rax), %ymm3, %ymm0
	vmovaps	(%rdx), %ymm3
	addq	$-128, %rsi
	vmovaps	%ymm0, 128(%rsi)
	vaddps	1536(%rdx), %ymm3, %ymm0
	vmovaps	160(%rax), %ymm3
	addq	$-128, %rdx
	vmovaps	%ymm0, (%rcx)
	vaddps	1696(%rax), %ymm3, %ymm0
	vmovaps	160(%rdx), %ymm3
	addq	$-128, %rcx
	vmovaps	%ymm0, 160(%rsi)
	vaddps	1696(%rdx), %ymm3, %ymm0
	vmovaps	192(%rax), %ymm3
	vmovaps	%ymm0, 160(%rcx)
	vaddps	1728(%rax), %ymm3, %ymm0
	vmovaps	192(%rdx), %ymm3
	vmovaps	%ymm0, 192(%rsi)
	vaddps	1728(%rdx), %ymm3, %ymm0
	vmovaps	224(%rax), %ymm3
	vmovaps	%ymm0, 192(%rcx)
	vaddps	1760(%rax), %ymm3, %ymm0
	vmovaps	224(%rdx), %ymm3
	vmovaps	%ymm0, 224(%rsi)
	vaddps	1760(%rdx), %ymm3, %ymm0
	vmovaps	%ymm0, 224(%rcx)
	cmpq	%rax, %rdi
	jne	.L14
	leaq	3072(%r12), %r15
	leaq	1536(%rsp), %rdx
	movq	%r15, %rsi
	movq	%r12, %rdi
	vzeroupper
	call	mult48x8_float
	leaq	3072(%rsp), %rdx
	leaq	4608(%r12), %rsi
	movq	%r15, %rdi
	call	mult48x8_float
	leaq	4608(%rsp), %rax
	movq	%rsp, %rdx
	movq	%rax, %rdi
	leaq	6144(%rsp), %rsi
	call	mult48x8_float
	vmovdqa	.LC7(%rip), %xmm1
	vmovaps	.LC6(%rip), %ymm10
	vmovaps	.LC5(%rip), %ymm4
	vmovaps	.LC4(%rip), %ymm5
	vmovaps	.LC3(%rip), %ymm6
	vmovaps	.LC2(%rip), %ymm7
	vmovdqa	.LC1(%rip), %ymm8
	vmovaps	.LC0(%rip), %ymm9
	leaq	4608(%rsp), %rcx
	leaq	1536(%r12), %rdx
	movq	%r12, %rax
	.p2align 4,,10
	.p2align 3
.L15:
	vmovaps	3072(%rax), %ymm3
	vmovaps	(%rax), %ymm2
	vmovaps	4608(%rax), %ymm0
	vfmadd213ps	1536(%rax), %ymm9, %ymm3
	vfmadd213ps	(%rcx), %ymm9, %ymm2
	vfmadd213ps	1536(%rcx), %ymm9, %ymm0
	addq	$32, %rax
	addq	$32, %rcx
	vaddps	%ymm2, %ymm3, %ymm2
	vsubps	%ymm3, %ymm0, %ymm0
	vmovaps	%ymm2, 1504(%rax)
	vmovaps	%ymm0, 3040(%rax)
	cmpq	%rax, %rdx
	jne	.L15
	leaq	6016(%r12), %rax
	movq	%r14, %rcx
	jmp	.L16
	.p2align 4,,10
	.p2align 3
.L18:
	movq	%rdx, %rax
.L16:
	vmovaps	(%rax), %ymm2
	vmovaps	64(%rax), %ymm11
	vmulps	%ymm6, %ymm2, %ymm0
	vmovaps	96(%rax), %ymm12
	leaq	-128(%rax), %rdx
	addq	$-128, %rcx
	vroundps	$8, %ymm0, %ymm0
	vfmadd132ps	%ymm5, %ymm2, %ymm0
	vmovaps	32(%rax), %ymm2
	vaddps	%ymm4, %ymm0, %ymm0
	vmovdqa	%xmm0, %xmm3
	vmovaps	%ymm0, (%rax)
	vmulps	%ymm6, %ymm2, %ymm0
	vpaddd	%xmm3, %xmm1, %xmm3
	vroundps	$8, %ymm0, %ymm0
	vfmadd132ps	%ymm5, %ymm2, %ymm0
	vfmadd132ps	%ymm10, %ymm4, %ymm0
	vmovdqa	%xmm0, %xmm2
	vmovaps	%ymm0, 32(%rax)
	vmulps	%ymm6, %ymm11, %ymm0
	vpaddd	%xmm2, %xmm1, %xmm2
	vroundps	$8, %ymm0, %ymm0
	vfmadd132ps	%ymm5, %ymm11, %ymm0
	vaddps	%ymm4, %ymm0, %ymm0
	vmovdqa	%xmm0, %xmm11
	vmovaps	%ymm0, 64(%rax)
	vmulps	%ymm6, %ymm12, %ymm0
	vpaddd	%xmm11, %xmm1, %xmm11
	vroundps	$8, %ymm0, %ymm0
	vfmadd132ps	%ymm5, %ymm12, %ymm0
	vfmadd132ps	%ymm10, %ymm4, %ymm0
	vmovdqa	%xmm0, %xmm12
	vmovaps	%ymm0, 96(%rax)
	vpaddd	16(%rax), %xmm1, %xmm0
	vpackssdw	%xmm0, %xmm3, %xmm3
	vpaddd	48(%rax), %xmm1, %xmm0
	vmovaps	%xmm3, 128(%rcx)
	vpackssdw	%xmm0, %xmm2, %xmm2
	vpaddd	80(%rax), %xmm1, %xmm0
	vmovaps	%xmm2, 160(%rcx)
	vpaddd	112(%rax), %xmm1, %xmm2
	vpackssdw	%xmm0, %xmm11, %xmm11
	vpaddd	%xmm12, %xmm1, %xmm0
	vpackssdw	%xmm2, %xmm0, %xmm0
	vmovaps	%xmm11, 192(%rcx)
	vmovaps	%xmm0, 224(%rcx)
	cmpq	%rax, %r12
	jne	.L18
	addq	$8, %r10
	addq	$16, %r14
	addq	$16, %rbx
	cmpq	%r10, %r11
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
.LFE5199:
	.size	mult96x16, .-mult96x16
	.p2align 4,,15
	.type	mult768_mix2_m256i, @function
mult768_mix2_m256i:
.LFB5202:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	addq	$160, %rsi
	vpxor	%xmm5, %xmm5, %xmm5
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	.cfi_offset 13, -24
	movq	%rdi, %r13
	pushq	%r12
	pushq	%rbx
	andq	$-32, %rsp
	subq	$16160, %rsp
	.cfi_offset 12, -32
	.cfi_offset 3, -40
	leaq	3872(%rsp), %r12
	leaq	2560(%r12), %rbx
	movq	%rbx, %rax
	vmovdqa	.LC8(%rip), %ymm13
	vmovdqa	.LC9(%rip), %ymm11
	jmp	.L25
	.p2align 4,,10
	.p2align 3
.L30:
	movq	%rcx, %rax
.L25:
	vmovdqu	192(%rsi), %ymm8
	vmovdqu	(%rsi), %ymm1
	vmovdqu	384(%rsi), %ymm0
	vpaddw	%ymm1, %ymm8, %ymm3
	vpmulhrsw	%ymm13, %ymm3, %ymm9
	vmovdqu	576(%rsi), %ymm4
	vmovdqu	960(%rsi), %ymm7
	vmovdqu	768(%rsi), %ymm2
	vmovdqu	1152(%rsi), %ymm6
	vpmullw	%ymm11, %ymm9, %ymm9
	vmovdqu	1344(%rsi), %ymm14
	vpaddw	%ymm0, %ymm1, %ymm12
	vmovdqa	%ymm0, 64(%rax)
	vmovdqa	%ymm1, (%rax)
	vpsubw	%ymm9, %ymm3, %ymm3
	vmovdqa	%ymm3, 256(%rax)
	vpaddw	%ymm0, %ymm4, %ymm3
	vpmulhrsw	%ymm13, %ymm3, %ymm9
	vpaddw	%ymm6, %ymm0, %ymm0
	vpaddw	%ymm2, %ymm1, %ymm1
	vmovdqa	%ymm2, 128(%rax)
	vmovdqa	%ymm6, 192(%rax)
	vpmullw	%ymm11, %ymm9, %ymm9
	vmovdqa	%ymm4, 96(%rax)
	vmovdqa	%ymm8, 32(%rax)
	vmovdqa	%ymm7, 160(%rax)
	vmovdqa	%ymm14, 224(%rax)
	vpsubw	%ymm9, %ymm3, %ymm3
	vmovdqa	%ymm3, 288(%rax)
	vpaddw	%ymm2, %ymm7, %ymm3
	vpmulhrsw	%ymm13, %ymm3, %ymm9
	vpaddw	%ymm6, %ymm2, %ymm2
	vpmulhrsw	%ymm13, %ymm2, %ymm10
	subq	$32, %rsi
	leaq	-512(%rax), %rcx
	vpmullw	%ymm11, %ymm9, %ymm9
	vpmullw	%ymm11, %ymm10, %ymm10
	vpsubw	%ymm9, %ymm3, %ymm3
	vmovdqa	%ymm3, 320(%rax)
	vpaddw	%ymm6, %ymm14, %ymm3
	vpmulhrsw	%ymm13, %ymm3, %ymm9
	vpmulhrsw	%ymm13, %ymm0, %ymm6
	vpsubw	%ymm10, %ymm2, %ymm10
	vpaddw	%ymm14, %ymm7, %ymm2
	vmovdqa	%ymm10, 3168(%rax)
	vpmullw	%ymm11, %ymm9, %ymm9
	vpmullw	%ymm11, %ymm6, %ymm6
	vpsubw	%ymm9, %ymm3, %ymm3
	vmovdqa	%ymm3, 352(%rax)
	vpmulhrsw	%ymm13, %ymm12, %ymm3
	vpmulhrsw	%ymm13, %ymm1, %ymm9
	vpsubw	%ymm6, %ymm0, %ymm6
	vmovdqa	%ymm6, 3328(%rax)
	vpmullw	%ymm11, %ymm3, %ymm3
	vpmullw	%ymm11, %ymm9, %ymm9
	vpsubw	%ymm3, %ymm12, %ymm12
	vpaddw	%ymm4, %ymm8, %ymm3
	vpmulhrsw	%ymm13, %ymm3, %ymm0
	vpsubw	%ymm9, %ymm1, %ymm9
	vpaddw	%ymm7, %ymm8, %ymm1
	vpaddw	%ymm14, %ymm4, %ymm4
	vpmulhrsw	%ymm13, %ymm2, %ymm7
	vpmullw	%ymm11, %ymm0, %ymm0
	vpaddw	%ymm9, %ymm6, %ymm8
	vmovdqa	%ymm12, 3072(%rax)
	vmovdqa	%ymm9, 3264(%rax)
	vpmullw	%ymm11, %ymm7, %ymm7
	vpsubw	%ymm0, %ymm3, %ymm3
	vpmulhrsw	%ymm13, %ymm1, %ymm0
	vmovdqa	%ymm3, 3104(%rax)
	vpaddw	%ymm12, %ymm3, %ymm3
	vpsubw	%ymm7, %ymm2, %ymm2
	vmovdqa	%ymm2, 3200(%rax)
	vpmullw	%ymm11, %ymm0, %ymm0
	vpaddw	%ymm10, %ymm2, %ymm2
	vpsubw	%ymm0, %ymm1, %ymm1
	vpmulhrsw	%ymm13, %ymm4, %ymm0
	vmovdqa	%ymm1, 3296(%rax)
	vpmullw	%ymm11, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm4, %ymm4
	vpmulhrsw	%ymm13, %ymm8, %ymm0
	vpaddw	%ymm1, %ymm4, %ymm7
	vpaddw	%ymm9, %ymm1, %ymm1
	vmovdqa	%ymm4, 3360(%rax)
	vpmullw	%ymm11, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm8, %ymm8
	vpmulhrsw	%ymm13, %ymm7, %ymm0
	vmovdqa	%ymm8, 3456(%rax)
	vpmullw	%ymm11, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm7, %ymm7
	vpmulhrsw	%ymm13, %ymm3, %ymm0
	vmovdqa	%ymm7, 3488(%rax)
	vpmullw	%ymm11, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm3, %ymm3
	vpmulhrsw	%ymm13, %ymm2, %ymm0
	vmovdqa	%ymm3, 3136(%rax)
	vpmullw	%ymm11, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm2, %ymm2
	vpmulhrsw	%ymm13, %ymm1, %ymm0
	vmovdqa	%ymm2, 3232(%rax)
	vpmullw	%ymm11, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm1, %ymm1
	vpaddw	%ymm6, %ymm4, %ymm0
	vmovdqa	%ymm1, 3392(%rax)
	vpmulhrsw	%ymm13, %ymm0, %ymm1
	vmovdqa	%ymm5, 384(%rax)
	vmovdqa	%ymm5, 416(%rax)
	vmovdqa	%ymm5, 448(%rax)
	vmovdqa	%ymm5, 480(%rax)
	vpmullw	%ymm11, %ymm1, %ymm1
	vmovdqa	%ymm5, 3552(%rax)
	vpsubw	%ymm1, %ymm0, %ymm0
	vmovdqa	%ymm0, 3424(%rax)
	vpaddw	%ymm8, %ymm7, %ymm0
	vpmulhrsw	%ymm13, %ymm0, %ymm1
	vpmullw	%ymm11, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm0, %ymm0
	vmovdqa	%ymm0, 3520(%rax)
	cmpq	%rax, %r12
	jne	.L30
	vmovdqu	64(%rdx), %ymm1
	vmovdqu	160(%rdx), %ymm9
	vmovdqu	256(%rdx), %ymm8
	vmovdqu	352(%rdx), %ymm0
	vmovdqu	448(%rdx), %ymm3
	vmovdqu	544(%rdx), %ymm2
	vpaddb	%ymm9, %ymm1, %ymm6
	vmovdqu	736(%rdx), %ymm7
	vmovdqu	640(%rdx), %ymm4
	vmovdqa	%ymm6, 3104(%rsp)
	vpaddb	%ymm0, %ymm8, %ymm6
	vmovdqa	%ymm6, 3136(%rsp)
	vpaddb	%ymm2, %ymm3, %ymm6
	vmovdqa	%ymm6, 3168(%rsp)
	vpaddb	%ymm7, %ymm4, %ymm6
	vpaddb	%ymm8, %ymm1, %ymm10
	vmovdqa	%ymm6, 3200(%rsp)
	vmovdqa	%ymm1, 2848(%rsp)
	vpaddb	%ymm3, %ymm1, %ymm6
	vmovdqa	%ymm8, 2912(%rsp)
	vpaddb	%ymm2, %ymm9, %ymm1
	vmovdqa	%ymm3, 2976(%rsp)
	vmovdqa	%ymm4, 3040(%rsp)
	vpaddb	%ymm4, %ymm3, %ymm3
	vmovdqa	%ymm0, 2944(%rsp)
	vpaddb	%ymm4, %ymm8, %ymm4
	vmovdqa	%ymm2, 3008(%rsp)
	vpaddb	%ymm0, %ymm9, %ymm8
	vpaddb	%ymm7, %ymm2, %ymm2
	vpaddb	%ymm7, %ymm0, %ymm0
	vmovdqa	%ymm9, 2880(%rsp)
	vmovdqa	%ymm7, 3072(%rsp)
	vpaddb	%ymm6, %ymm4, %ymm9
	vpaddb	%ymm1, %ymm0, %ymm7
	vmovdqa	%ymm8, 3392(%rsp)
	vmovdqa	%ymm1, 3584(%rsp)
	vmovdqa	%ymm2, 3488(%rsp)
	vpaddb	%ymm10, %ymm8, %ymm8
	vpaddb	%ymm3, %ymm2, %ymm2
	vpaddb	%ymm6, %ymm1, %ymm1
	vmovdqa	%ymm10, 3360(%rsp)
	vmovdqa	%ymm6, 3552(%rsp)
	vmovdqa	%ymm3, 3456(%rsp)
	vmovdqa	%ymm4, 3616(%rsp)
	vmovdqa	%ymm0, 3648(%rsp)
	vmovdqa	%ymm9, 3744(%rsp)
	vmovdqa	%ymm7, 3776(%rsp)
	vmovdqa	%ymm8, 3424(%rsp)
	vmovdqa	%ymm2, 3520(%rsp)
	vpaddb	%ymm4, %ymm0, %ymm0
	vmovdqa	%ymm1, 3680(%rsp)
	vpaddb	%ymm9, %ymm7, %ymm7
	vmovdqu	32(%rdx), %ymm1
	vmovdqu	128(%rdx), %ymm9
	vmovdqu	224(%rdx), %ymm8
	vmovdqa	%ymm0, 3712(%rsp)
	vmovdqu	320(%rdx), %ymm0
	vmovdqu	416(%rdx), %ymm6
	vmovdqu	512(%rdx), %ymm2
	vpaddb	%ymm9, %ymm1, %ymm4
	vmovdqu	608(%rdx), %ymm3
	vmovdqa	%ymm7, 3808(%rsp)
	vmovdqa	%ymm4, 2080(%rsp)
	vmovdqu	704(%rdx), %ymm7
	vpaddb	%ymm0, %ymm8, %ymm4
	vmovdqa	%ymm4, 2112(%rsp)
	vpaddb	%ymm2, %ymm6, %ymm4
	vmovdqa	%ymm4, 2144(%rsp)
	vpaddb	%ymm7, %ymm3, %ymm4
	vpaddb	%ymm8, %ymm1, %ymm10
	vmovdqa	%ymm4, 2176(%rsp)
	vmovdqa	%ymm1, 1824(%rsp)
	vpaddb	%ymm6, %ymm1, %ymm4
	vmovdqa	%ymm8, 1888(%rsp)
	vpaddb	%ymm2, %ymm9, %ymm1
	vmovdqa	%ymm6, 1952(%rsp)
	vmovdqa	%ymm3, 2016(%rsp)
	vpaddb	%ymm3, %ymm6, %ymm6
	vmovdqa	%ymm0, 1920(%rsp)
	vpaddb	%ymm3, %ymm8, %ymm3
	vpaddb	%ymm0, %ymm9, %ymm8
	vpaddb	%ymm7, %ymm0, %ymm0
	vmovdqa	%ymm9, 1856(%rsp)
	vmovdqa	%ymm2, 1984(%rsp)
	vpaddb	%ymm4, %ymm3, %ymm9
	vmovdqa	%ymm7, 2048(%rsp)
	vpaddb	%ymm7, %ymm2, %ymm2
	vpaddb	%ymm1, %ymm0, %ymm7
	vmovdqa	%ymm10, 2336(%rsp)
	vmovdqa	%ymm4, 2528(%rsp)
	vmovdqa	%ymm6, 2432(%rsp)
	vmovdqa	%ymm3, 2592(%rsp)
	vmovdqa	%ymm8, 2368(%rsp)
	vmovdqa	%ymm5, 3232(%rsp)
	vmovdqa	%ymm5, 3264(%rsp)
	vmovdqa	%ymm5, 3296(%rsp)
	vmovdqa	%ymm5, 3328(%rsp)
	vmovdqa	%ymm5, 3840(%rsp)
	vmovdqa	%ymm1, 2560(%rsp)
	vmovdqa	%ymm7, 2752(%rsp)
	vpaddb	%ymm4, %ymm1, %ymm1
	vpaddb	%ymm9, %ymm7, %ymm7
	vmovdqa	%ymm0, 2624(%rsp)
	vmovdqa	%ymm1, 2656(%rsp)
	vpaddb	%ymm3, %ymm0, %ymm0
	vmovdqu	(%rdx), %ymm1
	vmovdqa	%ymm7, 2784(%rsp)
	vmovdqu	96(%rdx), %ymm7
	vmovdqa	%ymm2, 2464(%rsp)
	vpaddb	%ymm10, %ymm8, %ymm8
	vpaddb	%ymm6, %ymm2, %ymm2
	vmovdqa	%ymm0, 2688(%rsp)
	vmovdqu	288(%rdx), %ymm6
	vmovdqu	192(%rdx), %ymm0
	vmovdqu	384(%rdx), %ymm4
	vmovdqu	480(%rdx), %ymm3
	vmovdqa	%ymm8, 2400(%rsp)
	vpaddb	%ymm7, %ymm1, %ymm8
	vmovdqa	%ymm9, 2720(%rsp)
	vmovdqa	%ymm2, 2496(%rsp)
	vmovdqu	576(%rdx), %ymm9
	vmovdqu	672(%rdx), %ymm2
	vmovdqa	%ymm8, 1056(%rsp)
	vpaddb	%ymm6, %ymm0, %ymm8
	vmovdqa	%ymm8, 1088(%rsp)
	vpaddb	%ymm3, %ymm4, %ymm8
	vmovdqa	%ymm8, 1120(%rsp)
	vpaddb	%ymm2, %ymm9, %ymm8
	vmovdqa	%ymm8, 1152(%rsp)
	vmovdqa	%ymm1, 800(%rsp)
	vpaddb	%ymm1, %ymm0, %ymm8
	vmovdqa	%ymm4, 928(%rsp)
	vpaddb	%ymm1, %ymm4, %ymm1
	vpaddb	%ymm4, %ymm9, %ymm4
	vpaddb	%ymm7, %ymm6, %ymm10
	vmovdqa	%ymm0, 864(%rsp)
	vmovdqa	%ymm9, 992(%rsp)
	vpaddb	%ymm0, %ymm9, %ymm0
	vmovdqa	%ymm8, 1312(%rsp)
	vmovdqa	%ymm1, 1504(%rsp)
	vmovdqa	%ymm5, 2208(%rsp)
	vmovdqa	%ymm5, 2240(%rsp)
	vmovdqa	%ymm5, 2272(%rsp)
	vmovdqa	%ymm5, 2304(%rsp)
	vmovdqa	%ymm5, 2816(%rsp)
	vmovdqa	%ymm4, 1408(%rsp)
	vmovdqa	%ymm7, 832(%rsp)
	vmovdqa	%ymm3, 960(%rsp)
	vpaddb	%ymm7, %ymm3, %ymm7
	vmovdqa	%ymm2, 1024(%rsp)
	vpaddb	%ymm3, %ymm2, %ymm3
	vpaddb	%ymm6, %ymm2, %ymm2
	vpaddb	%ymm2, %ymm7, %ymm9
	vmovdqa	%ymm6, 896(%rsp)
	vpaddb	%ymm0, %ymm1, %ymm6
	vmovdqa	%ymm0, 1568(%rsp)
	vmovdqa	%ymm3, 1440(%rsp)
	vmovdqa	%ymm6, 1696(%rsp)
	vpaddb	%ymm10, %ymm8, %ymm8
	vpaddb	%ymm3, %ymm4, %ymm3
	vpaddb	%ymm7, %ymm1, %ymm1
	vpaddb	%ymm2, %ymm0, %ymm0
	vpaddb	%ymm9, %ymm6, %ymm6
	leaq	5632(%r12), %rax
	vmovdqa	%ymm10, 1344(%rsp)
	vmovdqa	%ymm7, 1536(%rsp)
	vmovdqa	%ymm2, 1600(%rsp)
	vmovdqa	%ymm9, 1728(%rsp)
	vmovdqa	%ymm8, 1376(%rsp)
	vmovdqa	%ymm3, 1472(%rsp)
	vmovdqa	%ymm1, 1632(%rsp)
	vmovdqa	%ymm0, 1664(%rsp)
	vmovdqa	%ymm6, 1760(%rsp)
	vmovdqa	%ymm5, 1184(%rsp)
	vmovdqa	%ymm5, 1216(%rsp)
	vmovdqa	%ymm5, 1248(%rsp)
	vmovdqa	%ymm5, 1280(%rsp)
	vmovdqa	%ymm5, 1792(%rsp)
	vzeroupper
	jmp	.L26
	.p2align 4,,10
	.p2align 3
.L31:
	movq	%rdx, %rax
.L26:
	movq	%rax, %rdi
	call	transpose16
	leaq	-512(%rax), %rdx
	cmpq	%rax, %r12
	jne	.L31
	leaq	800(%rsp), %rax
	leaq	-2272(%rsp), %rdx
.L27:
	vmovdqa	2080(%rax), %ymm0
	vmovdqa	2048(%rax), %ymm1
	vmovdqa	2144(%rax), %ymm4
	vpunpcklbw	%ymm0, %ymm1, %ymm3
	vpunpckhbw	%ymm0, %ymm1, %ymm1
	vmovdqa	2112(%rax), %ymm0
	subq	$1024, %rax
	vpunpcklbw	%ymm4, %ymm0, %ymm2
	vpunpckhbw	%ymm4, %ymm0, %ymm0
	vpunpcklwd	%ymm0, %ymm1, %ymm5
	vpunpckhwd	%ymm0, %ymm1, %ymm6
	vmovdqa	3232(%rax), %ymm0
	vmovdqa	3200(%rax), %ymm1
	vpunpcklwd	%ymm2, %ymm3, %ymm12
	vmovdqa	3296(%rax), %ymm4
	vpunpckhwd	%ymm2, %ymm3, %ymm7
	vpunpcklbw	%ymm0, %ymm1, %ymm3
	vpunpckhbw	%ymm0, %ymm1, %ymm1
	vmovdqa	3264(%rax), %ymm0
	vmovdqa	%ymm5, 736(%rsp)
	vpunpcklbw	%ymm4, %ymm0, %ymm2
	vpunpckhbw	%ymm4, %ymm0, %ymm0
	vpunpcklwd	%ymm2, %ymm3, %ymm4
	vpunpckhwd	%ymm2, %ymm3, %ymm3
	vpunpcklwd	%ymm0, %ymm1, %ymm2
	vpunpckhwd	%ymm0, %ymm1, %ymm0
	vmovdqa	3392(%rax), %ymm5
	vmovdqa	3424(%rax), %ymm1
	vmovdqa	%ymm2, 640(%rsp)
	vmovdqa	%ymm0, 608(%rsp)
	vmovdqa	3328(%rax), %ymm2
	vmovdqa	3360(%rax), %ymm0
	vmovdqa	%ymm3, 672(%rsp)
	vpunpcklbw	%ymm0, %ymm2, %ymm3
	vpunpckhbw	%ymm0, %ymm2, %ymm2
	vpunpcklbw	%ymm1, %ymm5, %ymm0
	vpunpckhbw	%ymm1, %ymm5, %ymm1
	vpunpcklwd	%ymm0, %ymm3, %ymm10
	vpunpckhwd	%ymm0, %ymm3, %ymm15
	vpunpcklwd	%ymm1, %ymm2, %ymm14
	vmovdqa	3488(%rax), %ymm0
	vpunpckhwd	%ymm1, %ymm2, %ymm8
	vmovdqa	3456(%rax), %ymm2
	vmovdqa	3520(%rax), %ymm1
	vpunpcklbw	%ymm0, %ymm2, %ymm5
	vpunpckhbw	%ymm0, %ymm2, %ymm2
	vmovdqa	3552(%rax), %ymm0
	vmovdqa	%ymm6, 704(%rsp)
	vpunpcklbw	%ymm0, %ymm1, %ymm3
	vpunpckhbw	%ymm0, %ymm1, %ymm1
	vpunpckhwd	%ymm3, %ymm5, %ymm9
	vpunpcklwd	%ymm3, %ymm5, %ymm0
	vpunpcklwd	%ymm1, %ymm2, %ymm3
	vpunpckhwd	%ymm1, %ymm2, %ymm2
	vmovdqa	%ymm2, 416(%rsp)
	vmovdqa	3616(%rax), %ymm1
	vmovdqa	3584(%rax), %ymm2
	vmovdqa	3648(%rax), %ymm5
	vmovdqa	%ymm3, 448(%rsp)
	vpunpcklbw	%ymm1, %ymm2, %ymm3
	vpunpckhbw	%ymm1, %ymm2, %ymm2
	vmovdqa	3680(%rax), %ymm1
	vmovdqa	%ymm15, 576(%rsp)
	vpunpcklbw	%ymm1, %ymm5, %ymm6
	vpunpckhbw	%ymm1, %ymm5, %ymm1
	vpunpckhwd	%ymm6, %ymm3, %ymm15
	vmovdqa	%ymm14, 544(%rsp)
	vpunpcklwd	%ymm1, %ymm2, %ymm14
	vpunpckhwd	%ymm1, %ymm2, %ymm1
	vmovdqa	%ymm7, 768(%rsp)
	vmovdqa	%ymm8, 512(%rsp)
	vmovdqa	%ymm9, 480(%rsp)
	vmovdqa	%ymm15, 384(%rsp)
	vmovdqa	3712(%rax), %ymm5
	vmovdqa	%ymm1, 320(%rsp)
	vmovdqa	3744(%rax), %ymm1
	vpunpcklwd	%ymm6, %ymm3, %ymm9
	vpunpcklbw	%ymm1, %ymm5, %ymm6
	vmovdqa	3776(%rax), %ymm3
	vpunpckhbw	%ymm1, %ymm5, %ymm5
	vmovdqa	3808(%rax), %ymm1
	vmovdqa	%ymm14, 352(%rsp)
	vpunpcklbw	%ymm1, %ymm3, %ymm2
	vpunpckhbw	%ymm1, %ymm3, %ymm1
	vpunpckhwd	%ymm2, %ymm6, %ymm8
	vpunpcklwd	%ymm2, %ymm6, %ymm3
	vpunpcklwd	%ymm1, %ymm5, %ymm2
	vpunpckhwd	%ymm1, %ymm5, %ymm1
	vmovdqa	%ymm1, 256(%rsp)
	vmovdqa	3840(%rax), %ymm5
	vmovdqa	3872(%rax), %ymm1
	vmovdqa	3904(%rax), %ymm6
	vpunpcklbw	%ymm1, %ymm5, %ymm14
	vpunpckhbw	%ymm1, %ymm5, %ymm5
	vmovdqa	3936(%rax), %ymm1
	vmovdqa	%ymm8, 288(%rsp)
	vpunpcklbw	%ymm1, %ymm6, %ymm15
	vpunpckhbw	%ymm1, %ymm6, %ymm1
	vpunpckhwd	%ymm1, %ymm5, %ymm6
	vpunpcklwd	%ymm1, %ymm5, %ymm7
	vmovdqa	%ymm6, %ymm11
	vmovdqa	4000(%rax), %ymm1
	vmovdqa	3968(%rax), %ymm6
	vpunpcklwd	%ymm15, %ymm14, %ymm8
	vpunpckhwd	%ymm15, %ymm14, %ymm15
	vmovdqa	4064(%rax), %ymm5
	vmovdqa	%ymm15, 224(%rsp)
	vpunpcklbw	%ymm1, %ymm6, %ymm15
	vpunpckhbw	%ymm1, %ymm6, %ymm6
	vmovdqa	4032(%rax), %ymm1
	vpunpcklbw	%ymm5, %ymm1, %ymm14
	vpunpckhbw	%ymm5, %ymm1, %ymm5
	vpunpcklwd	%ymm14, %ymm15, %ymm1
	vpunpckhwd	%ymm14, %ymm15, %ymm15
	vpunpcklwd	%ymm5, %ymm6, %ymm14
	vpunpckhwd	%ymm5, %ymm6, %ymm6
	vmovdqa	%ymm6, %ymm13
	vpunpckldq	%ymm4, %ymm12, %ymm5
	vpunpckldq	%ymm0, %ymm10, %ymm6
	vpunpckhdq	%ymm4, %ymm12, %ymm4
	vpunpckhdq	%ymm0, %ymm10, %ymm0
	vpunpckldq	%ymm1, %ymm8, %ymm12
	vpunpckldq	%ymm3, %ymm9, %ymm10
	vpunpckhdq	%ymm1, %ymm8, %ymm1
	vpunpckhdq	%ymm3, %ymm9, %ymm3
	vpunpcklqdq	%ymm0, %ymm4, %ymm8
	vpunpcklqdq	%ymm6, %ymm5, %ymm9
	vpunpckhqdq	%ymm0, %ymm4, %ymm4
	vpunpckhqdq	%ymm6, %ymm5, %ymm6
	vpunpcklqdq	%ymm1, %ymm3, %ymm0
	vpunpcklqdq	%ymm12, %ymm10, %ymm5
	vpunpckhqdq	%ymm1, %ymm3, %ymm3
	vpunpckhqdq	%ymm12, %ymm10, %ymm12
	vperm2i128	$32, %ymm5, %ymm9, %ymm1
	vperm2i128	$32, %ymm3, %ymm4, %ymm10
	vperm2i128	$49, %ymm5, %ymm9, %ymm5
	vperm2i128	$32, %ymm12, %ymm6, %ymm9
	vperm2i128	$49, %ymm12, %ymm6, %ymm12
	vmovdqa	%ymm1, 3072(%rax)
	vmovdqa	%ymm5, 3584(%rax)
	vmovdqa	640(%rsp), %ymm1
	vmovdqa	736(%rsp), %ymm5
	vmovdqa	%ymm12, 3616(%rax)
	vmovdqa	%ymm10, 3168(%rax)
	vmovdqa	544(%rsp), %ymm12
	vmovdqa	448(%rsp), %ymm10
	vperm2i128	$49, %ymm3, %ymm4, %ymm4
	vperm2i128	$32, %ymm0, %ymm8, %ymm6
	vpunpckhdq	%ymm1, %ymm5, %ymm3
	vperm2i128	$49, %ymm0, %ymm8, %ymm8
	vmovdqa	%ymm4, 3680(%rax)
	vpunpckldq	%ymm1, %ymm5, %ymm0
	vpunpckhdq	%ymm10, %ymm12, %ymm4
	vpunpckldq	%ymm10, %ymm12, %ymm1
	vmovdqa	352(%rsp), %ymm12
	vmovdqa	%ymm6, 3136(%rax)
	vpunpckldq	%ymm2, %ymm12, %ymm5
	vpunpckldq	%ymm14, %ymm7, %ymm6
	vmovdqa	%ymm9, 3104(%rax)
	vpunpckhdq	%ymm14, %ymm7, %ymm7
	vpunpcklqdq	%ymm6, %ymm5, %ymm9
	vmovdqa	%ymm8, 3648(%rax)
	vpunpckhqdq	%ymm6, %ymm5, %ymm5
	vpunpcklqdq	%ymm1, %ymm0, %ymm8
	vpunpckhdq	%ymm2, %ymm12, %ymm2
	vpunpckhqdq	%ymm1, %ymm0, %ymm0
	vpunpcklqdq	%ymm4, %ymm3, %ymm10
	vperm2i128	$32, %ymm9, %ymm8, %ymm12
	vpunpckhqdq	%ymm4, %ymm3, %ymm3
	vperm2i128	$32, %ymm5, %ymm0, %ymm14
	vpunpcklqdq	%ymm7, %ymm2, %ymm4
	vperm2i128	$49, %ymm9, %ymm8, %ymm8
	vpunpckhqdq	%ymm7, %ymm2, %ymm2
	vperm2i128	$32, %ymm4, %ymm10, %ymm9
	vmovdqa	%ymm12, 3328(%rax)
	vperm2i128	$49, %ymm4, %ymm10, %ymm10
	vmovdqa	%ymm8, 3840(%rax)
	vmovdqa	%ymm14, 3360(%rax)
	vmovdqa	768(%rsp), %ymm7
	vmovdqa	672(%rsp), %ymm4
	vperm2i128	$49, %ymm5, %ymm0, %ymm0
	vperm2i128	$32, %ymm2, %ymm3, %ymm1
	vmovdqa	%ymm0, 3872(%rax)
	vperm2i128	$49, %ymm2, %ymm3, %ymm2
	vpunpckldq	%ymm4, %ymm7, %ymm0
	vpunpckhdq	%ymm4, %ymm7, %ymm3
	vmovdqa	576(%rsp), %ymm7
	vmovdqa	480(%rsp), %ymm4
	vmovdqa	288(%rsp), %ymm14
	vmovdqa	%ymm1, 3424(%rax)
	vpunpckldq	%ymm4, %ymm7, %ymm1
	vpunpckhdq	%ymm4, %ymm7, %ymm4
	vmovdqa	384(%rsp), %ymm7
	vmovdqa	%ymm2, 3936(%rax)
	vpunpckldq	%ymm14, %ymm7, %ymm5
	vpunpckhdq	%ymm14, %ymm7, %ymm2
	vmovdqa	224(%rsp), %ymm14
	vpunpcklqdq	%ymm1, %ymm0, %ymm8
	vpunpckldq	%ymm15, %ymm14, %ymm7
	vmovdqa	%ymm9, 3392(%rax)
	vpunpcklqdq	%ymm7, %ymm5, %ymm9
	vperm2i128	$32, %ymm9, %ymm8, %ymm6
	vpunpckhqdq	%ymm7, %ymm5, %ymm5
	vpunpckhqdq	%ymm1, %ymm0, %ymm0
	vperm2i128	$32, %ymm5, %ymm0, %ymm12
	vmovdqa	%ymm6, 3200(%rax)
	vperm2i128	$49, %ymm5, %ymm0, %ymm0
	vmovdqa	608(%rsp), %ymm6
	vmovdqa	704(%rsp), %ymm5
	vperm2i128	$49, %ymm9, %ymm8, %ymm8
	vpunpckldq	%ymm6, %ymm5, %ymm7
	vmovdqa	%ymm8, 3712(%rax)
	vpunpckhdq	%ymm15, %ymm14, %ymm15
	vpunpckhdq	%ymm6, %ymm5, %ymm8
	vmovdqa	512(%rsp), %ymm5
	vmovdqa	416(%rsp), %ymm6
	vmovdqa	%ymm10, 3904(%rax)
	vpunpcklqdq	%ymm4, %ymm3, %ymm10
	vpunpckhqdq	%ymm4, %ymm3, %ymm3
	vpunpcklqdq	%ymm15, %ymm2, %ymm4
	vperm2i128	$32, %ymm4, %ymm10, %ymm9
	vpunpckhqdq	%ymm15, %ymm2, %ymm15
	vmovdqa	%ymm0, 3744(%rax)
	vpunpckhdq	%ymm6, %ymm5, %ymm2
	vpunpckldq	%ymm6, %ymm5, %ymm0
	vperm2i128	$49, %ymm4, %ymm10, %ymm10
	vmovdqa	320(%rsp), %ymm5
	vmovdqa	256(%rsp), %ymm4
	vperm2i128	$32, %ymm15, %ymm3, %ymm1
	vpunpckldq	%ymm4, %ymm5, %ymm6
	vmovdqa	%ymm1, 3296(%rax)
	vperm2i128	$49, %ymm15, %ymm3, %ymm15
	vpunpckhdq	%ymm4, %ymm5, %ymm1
	vpunpckldq	%ymm13, %ymm11, %ymm3
	vpunpckhdq	%ymm13, %ymm11, %ymm5
	vpunpcklqdq	%ymm0, %ymm7, %ymm4
	vmovdqa	%ymm9, 3264(%rax)
	vpunpckhqdq	%ymm0, %ymm7, %ymm7
	vpunpcklqdq	%ymm3, %ymm6, %ymm9
	vpunpckhqdq	%ymm3, %ymm6, %ymm0
	vpunpcklqdq	%ymm2, %ymm8, %ymm6
	vpunpcklqdq	%ymm5, %ymm1, %ymm3
	vpunpckhqdq	%ymm2, %ymm8, %ymm2
	vpunpckhqdq	%ymm5, %ymm1, %ymm5
	vperm2i128	$32, %ymm9, %ymm4, %ymm8
	vperm2i128	$32, %ymm5, %ymm2, %ymm1
	vperm2i128	$49, %ymm9, %ymm4, %ymm4
	vperm2i128	$49, %ymm5, %ymm2, %ymm2
	vperm2i128	$32, %ymm0, %ymm7, %ymm9
	vperm2i128	$49, %ymm0, %ymm7, %ymm0
	vperm2i128	$32, %ymm3, %ymm6, %ymm7
	vperm2i128	$49, %ymm3, %ymm6, %ymm3
	vmovdqa	%ymm12, 3232(%rax)
	vmovdqa	%ymm10, 3776(%rax)
	vmovdqa	%ymm15, 3808(%rax)
	vmovdqa	%ymm8, 3456(%rax)
	vmovdqa	%ymm4, 3968(%rax)
	vmovdqa	%ymm9, 3488(%rax)
	vmovdqa	%ymm0, 4000(%rax)
	vmovdqa	%ymm7, 3520(%rax)
	vmovdqa	%ymm3, 4032(%rax)
	vmovdqa	%ymm1, 3552(%rax)
	vmovdqa	%ymm2, 4064(%rax)
	cmpq	%rax, %rdx
	jne	.L27
	leaq	816(%rsp), %rdx
	leaq	3072(%r12), %rsi
	leaq	6144(%r12), %rdi
	vzeroupper
	call	mult96x16
	leaq	800(%rsp), %rdx
	movq	%r12, %rsi
	movq	%r12, %rdi
	call	mult96x16
	leaq	11776(%r12), %rax
	jmp	.L28
	.p2align 4,,10
	.p2align 3
.L32:
	movq	%rdx, %rax
	vzeroupper
.L28:
	movq	%rax, %rdi
	call	transpose16
	cmpq	%rax, %r12
	leaq	-512(%rax), %rdx
	vmovdqa	.LC8(%rip), %ymm13
	vmovdqa	.LC9(%rip), %ymm11
	jne	.L32
	leaq	160(%r13), %rdi
	jmp	.L29
	.p2align 4,,10
	.p2align 3
.L33:
	movq	%rax, %rbx
.L29:
	vmovdqa	9600(%rbx), %ymm0
	vmovdqa	6560(%rbx), %ymm7
	vmovdqa	9408(%rbx), %ymm2
	vpaddw	6528(%rbx), %ymm7, %ymm1
	vpaddw	6464(%rbx), %ymm2, %ymm12
	vmovdqa	6368(%rbx), %ymm3
	vpaddw	6592(%rbx), %ymm0, %ymm10
	vpaddw	9664(%rbx), %ymm7, %ymm7
	vpaddw	9632(%rbx), %ymm0, %ymm0
	vpaddw	9536(%rbx), %ymm3, %ymm5
	vpsubw	%ymm0, %ymm7, %ymm7
	vpsubw	%ymm3, %ymm12, %ymm0
	vpsubw	6336(%rbx), %ymm0, %ymm6
	vpaddw	9440(%rbx), %ymm2, %ymm2
	vpmulhrsw	%ymm6, %ymm13, %ymm0
	vpsubw	%ymm2, %ymm5, %ymm5
	vmovdqa	%ymm6, 480(%rsp)
	vmovdqa	6432(%rbx), %ymm2
	vmovdqa	9472(%rbx), %ymm6
	vpsubw	%ymm1, %ymm10, %ymm10
	vpaddw	6496(%rbx), %ymm6, %ymm14
	vpmullw	%ymm11, %ymm0, %ymm4
	vpaddw	9568(%rbx), %ymm2, %ymm1
	vpaddw	9504(%rbx), %ymm6, %ymm6
	vpmulhrsw	%ymm13, %ymm5, %ymm0
	vpsubw	%ymm6, %ymm1, %ymm1
	vpmulhrsw	%ymm13, %ymm1, %ymm15
	vpmulhrsw	%ymm13, %ymm7, %ymm9
	vmovdqa	%ymm4, 768(%rsp)
	vpmullw	%ymm11, %ymm0, %ymm0
	vmovdqa	6400(%rbx), %ymm4
	vpsubw	%ymm2, %ymm14, %ymm6
	vpsubw	%ymm4, %ymm6, %ymm6
	vpmullw	%ymm11, %ymm15, %ymm15
	vpmulhrsw	%ymm13, %ymm6, %ymm6
	vpaddw	%ymm4, %ymm0, %ymm0
	vpmullw	%ymm11, %ymm9, %ymm9
	vpsubw	%ymm0, %ymm5, %ymm0
	vmovdqa	6528(%rbx), %ymm5
	vpmulhrsw	%ymm13, %ymm10, %ymm8
	vpsubw	%ymm15, %ymm1, %ymm15
	vpsubw	6336(%rbx), %ymm5, %ymm1
	vpmullw	%ymm11, %ymm6, %ymm6
	vpaddw	%ymm0, %ymm1, %ymm5
	vpsubw	%ymm9, %ymm7, %ymm9
	vpaddw	%ymm15, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm9, %ymm7
	vpmulhrsw	%ymm7, %ymm13, %ymm0
	vpmullw	%ymm11, %ymm8, %ymm8
	vpsubw	%ymm14, %ymm2, %ymm2
	vpaddw	%ymm2, %ymm6, %ymm6
	vpaddw	9440(%rbx), %ymm4, %ymm2
	vmovdqa	6336(%rbx), %ymm14
	vpaddw	%ymm2, %ymm6, %ymm2
	vpsubw	%ymm12, %ymm3, %ymm3
	vpsubw	%ymm8, %ymm14, %ymm8
	vmovdqa	%ymm0, 96(%rsp)
	vpaddw	%ymm10, %ymm3, %ymm10
	vpaddw	768(%rsp), %ymm2, %ymm0
	vpaddw	%ymm8, %ymm10, %ymm10
	vpaddw	%ymm0, %ymm10, %ymm14
	vmovdqa	9632(%rbx), %ymm10
	vpmulhrsw	%ymm5, %ymm13, %ymm9
	vpsubw	9504(%rbx), %ymm10, %ymm0
	vmovdqa	%ymm15, 448(%rsp)
	vpsubw	%ymm2, %ymm0, %ymm2
	vpmulhrsw	%ymm2, %ymm13, %ymm8
	vmovdqa	%ymm5, 512(%rsp)
	vmovdqa	%ymm9, 128(%rsp)
	vmovdqa	%ymm7, 544(%rsp)
	vmovdqa	%ymm14, 576(%rsp)
	vmovdqa	3072(%rbx), %ymm1
	vmovdqa	32(%rbx), %ymm7
	vpaddw	256(%rbx), %ymm1, %ymm12
	vmovdqa	3136(%rbx), %ymm3
	vpaddw	3104(%rbx), %ymm1, %ymm1
	vmovdqa	%ymm2, 608(%rsp)
	vmovdqa	%ymm8, 32(%rsp)
	vmovdqa	96(%rbx), %ymm2
	vpaddw	3328(%rbx), %ymm7, %ymm8
	vpsubw	%ymm7, %ymm12, %ymm0
	vpsubw	(%rbx), %ymm0, %ymm0
	vpmulhrsw	%ymm14, %ymm13, %ymm10
	vpsubw	%ymm1, %ymm8, %ymm8
	vpaddw	288(%rbx), %ymm3, %ymm14
	vpaddw	3360(%rbx), %ymm2, %ymm1
	vpaddw	3168(%rbx), %ymm3, %ymm3
	vpmulhrsw	%ymm13, %ymm0, %ymm5
	vpsubw	%ymm3, %ymm1, %ymm1
	vpmulhrsw	%ymm13, %ymm1, %ymm4
	vmovdqa	%ymm10, 64(%rsp)
	vmovdqa	64(%rbx), %ymm10
	vpmullw	%ymm11, %ymm5, %ymm5
	vpsubw	%ymm2, %ymm14, %ymm3
	vpmullw	%ymm11, %ymm4, %ymm4
	vpsubw	%ymm10, %ymm3, %ymm3
	vpmulhrsw	%ymm13, %ymm3, %ymm3
	vpsubw	%ymm5, %ymm0, %ymm6
	vmovdqa	%ymm6, 416(%rsp)
	vpsubw	%ymm4, %ymm1, %ymm9
	vmovdqa	9216(%rbx), %ymm6
	vmovdqa	6176(%rbx), %ymm1
	vmovdqa	%ymm9, 736(%rsp)
	vpaddw	6144(%rbx), %ymm1, %ymm4
	vpaddw	6208(%rbx), %ymm6, %ymm9
	vpmullw	%ymm11, %ymm3, %ymm3
	vpsubw	%ymm4, %ymm9, %ymm9
	vpmulhrsw	%ymm13, %ymm9, %ymm4
	vpsubw	%ymm14, %ymm2, %ymm2
	vpaddw	(%rbx), %ymm5, %ymm5
	vpaddw	%ymm2, %ymm3, %ymm2
	vpaddw	3104(%rbx), %ymm10, %ymm3
	vpmullw	%ymm11, %ymm4, %ymm4
	vpsubw	%ymm12, %ymm7, %ymm7
	vpaddw	%ymm3, %ymm2, %ymm3
	vpaddw	%ymm7, %ymm5, %ymm7
	vpmulhrsw	%ymm13, %ymm8, %ymm0
	vpsubw	%ymm4, %ymm9, %ymm9
	vpaddw	%ymm7, %ymm3, %ymm4
	vmovdqa	%ymm7, 224(%rsp)
	vpaddw	%ymm4, %ymm9, %ymm7
	vpmulhrsw	%ymm7, %ymm13, %ymm2
	vmovdqa	9248(%rbx), %ymm15
	vpmullw	%ymm11, %ymm0, %ymm0
	vpaddw	%ymm6, %ymm15, %ymm6
	vpsubw	3168(%rbx), %ymm15, %ymm15
	vpaddw	9280(%rbx), %ymm1, %ymm1
	vpsubw	%ymm3, %ymm15, %ymm9
	vpmullw	%ymm11, %ymm2, %ymm3
	vpmulhrsw	%ymm9, %ymm13, %ymm2
	vpsubw	%ymm6, %ymm1, %ymm1
	vpaddw	%ymm10, %ymm0, %ymm0
	vpmulhrsw	%ymm13, %ymm1, %ymm6
	vpsubw	%ymm3, %ymm7, %ymm4
	vpsubw	%ymm0, %ymm8, %ymm8
	vmovdqa	%ymm7, 640(%rsp)
	vmovdqa	%ymm3, 384(%rsp)
	vmovdqa	%ymm4, 352(%rsp)
	vmovdqa	%ymm9, 672(%rsp)
	vmovdqa	%ymm2, (%rsp)
	vpaddw	6144(%rbx), %ymm8, %ymm10
	vpmullw	%ymm11, %ymm6, %ymm6
	vpsubw	(%rbx), %ymm10, %ymm2
	vpaddw	736(%rsp), %ymm8, %ymm8
	vpmulhrsw	%ymm13, %ymm2, %ymm0
	vmovdqa	224(%rbx), %ymm7
	vpsubw	%ymm6, %ymm1, %ymm6
	vpsubw	%ymm8, %ymm6, %ymm4
	vmovdqa	160(%rbx), %ymm1
	vpmullw	%ymm11, %ymm0, %ymm8
	vmovdqa	3200(%rbx), %ymm0
	vpaddw	3392(%rbx), %ymm1, %ymm12
	vpaddw	320(%rbx), %ymm0, %ymm14
	vpaddw	3232(%rbx), %ymm0, %ymm0
	vpaddw	3424(%rbx), %ymm7, %ymm15
	vpsubw	%ymm0, %ymm12, %ymm12
	vpsubw	%ymm1, %ymm14, %ymm0
	vmovdqa	3264(%rbx), %ymm1
	vmovdqa	%ymm10, 704(%rsp)
	vpaddw	3296(%rbx), %ymm1, %ymm10
	vpaddw	352(%rbx), %ymm1, %ymm5
	vpsubw	%ymm10, %ymm15, %ymm1
	vpmulhrsw	%ymm13, %ymm1, %ymm6
	vmovdqa	6272(%rbx), %ymm9
	vmovdqa	%ymm15, 160(%rsp)
	vpmulhrsw	%ymm13, %ymm12, %ymm3
	vpsubw	%ymm8, %ymm2, %ymm2
	vpmullw	%ymm11, %ymm6, %ymm6
	vmovdqa	%ymm8, 320(%rsp)
	vmovdqa	%ymm10, 192(%rsp)
	vpsubw	%ymm7, %ymm5, %ymm8
	vpsubw	192(%rbx), %ymm8, %ymm8
	vpsubw	%ymm6, %ymm1, %ymm15
	vmovdqa	9312(%rbx), %ymm1
	vmovdqa	%ymm6, 256(%rsp)
	vpaddw	6304(%rbx), %ymm1, %ymm10
	vpaddw	6240(%rbx), %ymm9, %ymm6
	vpaddw	9344(%rbx), %ymm1, %ymm1
	vpaddw	9376(%rbx), %ymm9, %ymm9
	vpsubw	128(%rbx), %ymm0, %ymm0
	vpsubw	%ymm1, %ymm9, %ymm9
	vpsubw	%ymm6, %ymm10, %ymm10
	vpmulhrsw	%ymm13, %ymm8, %ymm8
	vpmulhrsw	%ymm13, %ymm9, %ymm6
	vpmulhrsw	%ymm13, %ymm0, %ymm0
	vpmullw	%ymm11, %ymm3, %ymm3
	vpmulhrsw	%ymm13, %ymm10, %ymm1
	vpmullw	%ymm11, %ymm8, %ymm8
	vpmullw	%ymm11, %ymm6, %ymm6
	vpmullw	%ymm11, %ymm0, %ymm0
	vpaddw	192(%rbx), %ymm3, %ymm3
	vpsubw	%ymm5, %ymm7, %ymm7
	vmovdqa	192(%rbx), %ymm5
	vpsubw	%ymm3, %ymm12, %ymm3
	vpaddw	6240(%rbx), %ymm3, %ymm12
	vpaddw	3232(%rbx), %ymm5, %ymm5
	vmovdqa	%ymm2, 288(%rsp)
	vpmulhrsw	%ymm13, %ymm4, %ymm2
	vpsubw	%ymm6, %ymm9, %ymm6
	vpaddw	%ymm7, %ymm8, %ymm8
	vpmullw	%ymm11, %ymm1, %ymm1
	vpaddw	%ymm15, %ymm3, %ymm3
	vpsubw	%ymm3, %ymm6, %ymm3
	vpaddw	%ymm5, %ymm8, %ymm8
	vpsubw	128(%rbx), %ymm12, %ymm6
	vpaddw	128(%rbx), %ymm0, %ymm0
	vmovdqa	160(%rbx), %ymm5
	vpmullw	%ymm11, %ymm2, %ymm2
	vpsubw	%ymm14, %ymm5, %ymm14
	vpaddw	%ymm14, %ymm0, %ymm14
	vpsubw	%ymm1, %ymm10, %ymm10
	vpaddw	%ymm8, %ymm14, %ymm1
	vpaddw	%ymm1, %ymm10, %ymm10
	vmovdqa	9344(%rbx), %ymm1
	vpaddw	128(%rbx), %ymm2, %ymm0
	vpsubw	3296(%rbx), %ymm1, %ymm5
	vmovdqa	6336(%rbx), %ymm1
	vmovdqa	%ymm12, %ymm9
	vpmulhrsw	%ymm13, %ymm3, %ymm12
	vpsubw	%ymm8, %ymm5, %ymm8
	vpsubw	(%rbx), %ymm1, %ymm1
	vpsubw	%ymm0, %ymm4, %ymm4
	vpmulhrsw	%ymm13, %ymm8, %ymm5
	vpaddw	%ymm4, %ymm1, %ymm1
	vpmulhrsw	%ymm13, %ymm1, %ymm2
	vpmullw	%ymm11, %ymm12, %ymm12
	vpmullw	96(%rsp), %ymm11, %ymm0
	vpmullw	%ymm11, %ymm5, %ymm5
	vpmulhrsw	%ymm13, %ymm6, %ymm6
	vpmullw	%ymm11, %ymm2, %ymm2
	vpsubw	%ymm12, %ymm3, %ymm3
	vmovdqa	544(%rsp), %ymm12
	vpsubw	%ymm5, %ymm8, %ymm8
	vpsubw	%ymm0, %ymm12, %ymm5
	vpaddw	%ymm3, %ymm4, %ymm0
	vpsubw	%ymm0, %ymm5, %ymm0
	vpsubw	%ymm2, %ymm1, %ymm1
	vpmulhrsw	%ymm13, %ymm0, %ymm2
	vmovdqa	(%rbx), %ymm5
	vpaddw	%ymm11, %ymm3, %ymm12
	vpsraw	$15, %ymm5, %ymm4
	vmovdqa	(%rbx), %ymm5
	vpmullw	%ymm11, %ymm2, %ymm2
	vpmullw	%ymm11, %ymm6, %ymm6
	vpmulhrsw	%ymm13, %ymm10, %ymm7
	leaq	-512(%rbx), %rax
	subq	$32, %rdi
	vpsubw	%ymm2, %ymm0, %ymm0
	vpaddw	(%rbx), %ymm11, %ymm2
	vpmullw	%ymm11, %ymm7, %ymm7
	vpblendvb	%ymm4, %ymm2, %ymm5, %ymm4
	vpaddw	.LC10(%rip), %ymm4, %ymm2
	vpaddw	.LC11(%rip), %ymm4, %ymm5
	vpsraw	$15, %ymm2, %ymm2
	vpblendvb	%ymm2, %ymm4, %ymm5, %ymm2
	vpaddw	%ymm11, %ymm1, %ymm4
	vpsraw	$15, %ymm1, %ymm5
	vpblendvb	%ymm5, %ymm4, %ymm1, %ymm1
	vpaddw	.LC10(%rip), %ymm1, %ymm4
	vpaddw	.LC11(%rip), %ymm1, %ymm5
	vpsraw	$15, %ymm4, %ymm4
	vpblendvb	%ymm4, %ymm1, %ymm5, %ymm1
	vpaddw	%ymm11, %ymm0, %ymm4
	vpsraw	$15, %ymm0, %ymm5
	vpblendvb	%ymm5, %ymm4, %ymm0, %ymm0
	vpaddw	.LC10(%rip), %ymm0, %ymm4
	vpaddw	.LC11(%rip), %ymm0, %ymm5
	vpsraw	$15, %ymm4, %ymm4
	vpblendvb	%ymm4, %ymm0, %ymm5, %ymm0
	vpsraw	$15, %ymm3, %ymm4
	vpblendvb	%ymm4, %ymm12, %ymm3, %ymm3
	vpaddw	.LC10(%rip), %ymm3, %ymm5
	vpaddw	.LC11(%rip), %ymm3, %ymm4
	vpsraw	$15, %ymm5, %ymm5
	vpblendvb	%ymm5, %ymm3, %ymm4, %ymm3
	vmovdqa	%ymm0, 1568(%rdi)
	vpmullw	(%rsp), %ymm11, %ymm0
	vpsubw	%ymm0, %ymm14, %ymm14
	vmovdqa	%ymm3, 2336(%rdi)
	vpaddw	672(%rsp), %ymm14, %ymm14
	vmovdqa	480(%rsp), %ymm3
	vmovdqa	%ymm1, 800(%rdi)
	vpsubw	768(%rsp), %ymm3, %ymm0
	vmovdqa	608(%rsp), %ymm12
	vpaddw	224(%rsp), %ymm14, %ymm1
	vmovdqa	%ymm2, 32(%rdi)
	vpaddw	%ymm0, %ymm1, %ymm1
	vpmullw	32(%rsp), %ymm11, %ymm0
	vpsubw	%ymm0, %ymm12, %ymm2
	vpaddw	%ymm8, %ymm14, %ymm0
	vpsubw	%ymm0, %ymm2, %ymm0
	vpmulhrsw	%ymm13, %ymm1, %ymm2
	vmovdqa	416(%rsp), %ymm5
	vpsraw	$15, %ymm5, %ymm3
	vpmullw	%ymm11, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm1, %ymm1
	vpmulhrsw	%ymm13, %ymm0, %ymm2
	vpmullw	%ymm11, %ymm2, %ymm2
	vpsubw	%ymm2, %ymm0, %ymm0
	vpaddw	%ymm5, %ymm11, %ymm2
	vpblendvb	%ymm3, %ymm2, %ymm5, %ymm3
	vpaddw	.LC10(%rip), %ymm3, %ymm2
	vpaddw	.LC11(%rip), %ymm3, %ymm4
	vpsraw	$15, %ymm2, %ymm2
	vpblendvb	%ymm2, %ymm3, %ymm4, %ymm2
	vpaddw	%ymm11, %ymm1, %ymm3
	vpsraw	$15, %ymm1, %ymm4
	vpblendvb	%ymm4, %ymm3, %ymm1, %ymm1
	vpaddw	.LC10(%rip), %ymm1, %ymm3
	vpaddw	.LC11(%rip), %ymm1, %ymm4
	vpsraw	$15, %ymm3, %ymm3
	vpblendvb	%ymm3, %ymm1, %ymm4, %ymm1
	vpaddw	%ymm11, %ymm0, %ymm3
	vpsraw	$15, %ymm0, %ymm4
	vpblendvb	%ymm4, %ymm3, %ymm0, %ymm0
	vpaddw	.LC10(%rip), %ymm0, %ymm3
	vpaddw	.LC11(%rip), %ymm0, %ymm4
	vpsraw	$15, %ymm3, %ymm3
	vpblendvb	%ymm3, %ymm0, %ymm4, %ymm0
	vmovdqa	%ymm0, 1760(%rdi)
	vmovdqa	736(%rsp), %ymm0
	vmovdqa	320(%rsp), %ymm12
	vpaddw	128(%rbx), %ymm6, %ymm6
	vmovdqa	%ymm1, 992(%rdi)
	vpsubw	%ymm9, %ymm0, %ymm1
	vmovdqa	(%rbx), %ymm9
	vpaddw	%ymm1, %ymm6, %ymm6
	vpaddw	512(%rsp), %ymm12, %ymm1
	vpmullw	128(%rsp), %ymm11, %ymm0
	vpsubw	%ymm0, %ymm9, %ymm0
	vmovdqa	192(%rsp), %ymm12
	vpaddw	%ymm1, %ymm0, %ymm0
	vpsubw	704(%rsp), %ymm6, %ymm1
	vpaddw	%ymm11, %ymm8, %ymm3
	vpaddw	%ymm1, %ymm0, %ymm0
	vpsubw	160(%rsp), %ymm12, %ymm1
	vmovdqa	256(%rsp), %ymm12
	vpaddw	448(%rsp), %ymm1, %ymm1
	vpsubw	%ymm6, %ymm12, %ymm6
	vpaddw	%ymm6, %ymm1, %ymm6
	vpmulhrsw	%ymm13, %ymm0, %ymm1
	vpsraw	$15, %ymm8, %ymm4
	vpblendvb	%ymm4, %ymm3, %ymm8, %ymm8
	vpaddw	.LC10(%rip), %ymm8, %ymm4
	vpaddw	.LC11(%rip), %ymm8, %ymm3
	vpmullw	%ymm11, %ymm1, %ymm1
	vmovdqa	%ymm2, 224(%rdi)
	vmovdqa	288(%rsp), %ymm2
	vpsraw	$15, %ymm4, %ymm4
	vpblendvb	%ymm4, %ymm8, %ymm3, %ymm8
	vpsubw	%ymm1, %ymm0, %ymm0
	vpmulhrsw	%ymm13, %ymm6, %ymm1
	vpaddw	%ymm2, %ymm11, %ymm3
	vmovdqa	%ymm8, 2528(%rdi)
	vpmullw	%ymm11, %ymm1, %ymm1
	vpsubw	%ymm1, %ymm6, %ymm6
	vpsraw	$15, %ymm2, %ymm1
	vpblendvb	%ymm1, %ymm3, %ymm2, %ymm1
	vpaddw	.LC10(%rip), %ymm1, %ymm3
	vpaddw	.LC11(%rip), %ymm1, %ymm4
	vpsraw	$15, %ymm3, %ymm2
	vpblendvb	%ymm2, %ymm1, %ymm4, %ymm3
	vpsraw	$15, %ymm0, %ymm2
	vpaddw	%ymm11, %ymm0, %ymm1
	vpblendvb	%ymm2, %ymm1, %ymm0, %ymm1
	vpaddw	.LC10(%rip), %ymm1, %ymm0
	vpaddw	.LC11(%rip), %ymm1, %ymm2
	vpsraw	$15, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm1, %ymm2, %ymm1
	vpaddw	%ymm11, %ymm6, %ymm0
	vpsraw	$15, %ymm6, %ymm2
	vpblendvb	%ymm2, %ymm0, %ymm6, %ymm6
	vpaddw	.LC10(%rip), %ymm6, %ymm0
	vpaddw	.LC11(%rip), %ymm6, %ymm2
	vpsraw	$15, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm6, %ymm2, %ymm6
	vpsraw	$15, %ymm15, %ymm0
	vpaddw	%ymm11, %ymm15, %ymm2
	vpblendvb	%ymm0, %ymm2, %ymm15, %ymm2
	vpaddw	.LC10(%rip), %ymm2, %ymm4
	vpaddw	.LC11(%rip), %ymm2, %ymm0
	vmovdqa	%ymm3, 416(%rdi)
	vpaddw	3168(%rbx), %ymm7, %ymm7
	vpsraw	$15, %ymm4, %ymm4
	vmovdqa	384(%rsp), %ymm3
	vpblendvb	%ymm4, %ymm2, %ymm0, %ymm2
	vmovdqa	9504(%rbx), %ymm5
	vpaddw	576(%rsp), %ymm7, %ymm0
	vpaddw	640(%rsp), %ymm10, %ymm4
	vpmullw	64(%rsp), %ymm11, %ymm9
	vpsubw	%ymm9, %ymm3, %ymm9
	vpaddw	%ymm0, %ymm9, %ymm9
	vpsubw	3296(%rbx), %ymm5, %ymm0
	vpsubw	%ymm4, %ymm9, %ymm9
	vpsubw	%ymm7, %ymm10, %ymm10
	vpaddw	%ymm0, %ymm10, %ymm10
	vpmulhrsw	%ymm13, %ymm9, %ymm0
	vmovdqa	352(%rsp), %ymm7
	vmovdqa	%ymm1, 1184(%rdi)
	vpsraw	$15, %ymm7, %ymm1
	vmovdqa	%ymm2, 2720(%rdi)
	vpmullw	%ymm11, %ymm0, %ymm0
	vmovdqa	%ymm6, 1952(%rdi)
	vpsubw	%ymm0, %ymm9, %ymm9
	vpmulhrsw	%ymm13, %ymm10, %ymm0
	vpmullw	%ymm11, %ymm0, %ymm0
	vpsubw	%ymm0, %ymm10, %ymm10
	vpaddw	%ymm7, %ymm11, %ymm0
	vpblendvb	%ymm1, %ymm0, %ymm7, %ymm0
	vpaddw	.LC10(%rip), %ymm0, %ymm1
	vpaddw	.LC11(%rip), %ymm0, %ymm2
	vpsraw	$15, %ymm1, %ymm1
	vpblendvb	%ymm1, %ymm0, %ymm2, %ymm2
	vpaddw	%ymm11, %ymm9, %ymm0
	vpsraw	$15, %ymm9, %ymm1
	vpblendvb	%ymm1, %ymm0, %ymm9, %ymm9
	vpaddw	.LC10(%rip), %ymm9, %ymm0
	vpaddw	.LC11(%rip), %ymm9, %ymm1
	vpsraw	$15, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm9, %ymm1, %ymm9
	vpaddw	%ymm11, %ymm10, %ymm0
	vpsraw	$15, %ymm10, %ymm1
	vpblendvb	%ymm1, %ymm0, %ymm10, %ymm10
	vpaddw	.LC10(%rip), %ymm10, %ymm0
	vpaddw	.LC11(%rip), %ymm10, %ymm1
	vmovdqa	3296(%rbx), %ymm7
	vpsraw	$15, %ymm0, %ymm0
	vpblendvb	%ymm0, %ymm10, %ymm1, %ymm10
	vpsraw	$15, %ymm7, %ymm1
	vpaddw	3296(%rbx), %ymm11, %ymm0
	vmovdqa	3296(%rbx), %ymm7
	vmovdqa	%ymm2, 608(%rdi)
	vpblendvb	%ymm1, %ymm0, %ymm7, %ymm0
	vpaddw	.LC10(%rip), %ymm0, %ymm3
	vpaddw	.LC11(%rip), %ymm0, %ymm1
	vpsraw	$15, %ymm3, %ymm3
	vpblendvb	%ymm3, %ymm0, %ymm1, %ymm0
	vmovdqa	%ymm9, 1376(%rdi)
	vmovdqa	%ymm10, 2144(%rdi)
	vmovdqa	%ymm0, 2912(%rdi)
	cmpq	%r12, %rbx
	jne	.L33
	vzeroupper
	leaq	-24(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5202:
	.size	mult768_mix2_m256i, .-mult768_mix2_m256i
	.p2align 4,,15
	.globl	rq_mult
	.type	rq_mult, @function
rq_mult:
.LFB5203:
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
	vmovdqu	2(%rsp), %xmm0
	vmovdqu	1522(%rsp), %xmm2
	vmovdqu	1524(%rsp), %xmm3
	vpmovsxwd	%xmm2, %xmm4
	vpmovsxwd	%xmm0, %xmm1
	vpsrldq	$8, %xmm2, %xmm2
	vpsrldq	$8, %xmm0, %xmm0
	vpaddd	%xmm4, %xmm1, %xmm1
	vpmovsxwd	%xmm2, %xmm2
	vpmovsxwd	%xmm3, %xmm4
	vpmovsxwd	%xmm0, %xmm0
	vpaddd	%xmm4, %xmm1, %xmm4
	vpaddd	%xmm2, %xmm0, %xmm0
	vpsrldq	$8, %xmm3, %xmm2
	vpslld	$3, %xmm4, %xmm1
	vpmovsxwd	%xmm2, %xmm2
	vpaddd	%xmm2, %xmm0, %xmm2
	vpsubd	%xmm4, %xmm1, %xmm1
	vpslld	$3, %xmm2, %xmm0
	vpslld	$3, %xmm1, %xmm1
	vpaddd	%xmm4, %xmm1, %xmm1
	vpsubd	%xmm2, %xmm0, %xmm0
	vmovdqa	.LC12(%rip), %xmm3
	vpslld	$2, %xmm1, %xmm1
	vpslld	$3, %xmm0, %xmm0
	vpaddd	%xmm2, %xmm0, %xmm0
	vpsrad	$20, %xmm1, %xmm1
	vpmulld	%xmm3, %xmm1, %xmm1
	vpslld	$2, %xmm0, %xmm0
	vpsrad	$20, %xmm0, %xmm0
	vpmulld	%xmm3, %xmm0, %xmm0
	vmovdqa	.LC14(%rip), %xmm5
	vpaddd	%xmm4, %xmm1, %xmm1
	vmovdqa	.LC13(%rip), %xmm4
	movswl	(%rsp), %eax
	vpaddd	%xmm2, %xmm0, %xmm0
	vpmulld	%xmm4, %xmm1, %xmm2
	movswl	1522(%rsp), %edx
	vmovdqa	.LC8(%rip), %ymm6
	addl	%eax, %edx
	imull	$228, %edx, %eax
	vpaddd	%xmm5, %xmm2, %xmm2
	vpsrad	$28, %xmm2, %xmm2
	vpmulld	%xmm3, %xmm2, %xmm2
	sarl	$20, %eax
	imull	$-4591, %eax, %eax
	leaq	1522(%rsp), %rcx
	vpaddd	%xmm1, %xmm2, %xmm1
	vpmulld	%xmm4, %xmm0, %xmm2
	addl	%edx, %eax
	imull	$58470, %eax, %edx
	vmovdqa	.LC11(%rip), %ymm4
	vpaddd	%xmm5, %xmm2, %xmm2
	vpsrad	$28, %xmm2, %xmm2
	vpmulld	%xmm3, %xmm2, %xmm3
	addl	$134217728, %edx
	sarl	$28, %edx
	imull	$-4591, %edx, %edx
	vmovdqa	.LC15(%rip), %xmm2
	vpaddd	%xmm0, %xmm3, %xmm3
	vpand	%xmm3, %xmm2, %xmm3
	vpand	%xmm1, %xmm2, %xmm0
	addl	%edx, %eax
	vpackusdw	%xmm3, %xmm0, %xmm0
	vmovdqa	.LC10(%rip), %ymm5
	vmovdqa	.LC9(%rip), %ymm3
	movw	%ax, (%rbx)
	vmovups	%xmm0, 2(%rbx)
	leaq	18(%rsp), %rax
	leaq	18(%rbx), %rdx
	.p2align 4,,10
	.p2align 3
.L37:
	vmovdqu	1520(%rax), %ymm7
	addq	$32, %rax
	vpaddw	1490(%rax), %ymm7, %ymm0
	addq	$32, %rdx
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
	jne	.L37
	xorl	%eax, %eax
	movq	$0, 1522(%rbx)
	movl	$0, 1530(%rbx)
	movw	%ax, 1534(%rbx)
	vzeroupper
	movq	-8(%rbp), %rbx
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5203:
	.size	rq_mult, .-rq_mult
	.p2align 4,,15
	.globl	r3_mult
	.type	r3_mult, @function
r3_mult:
.LFB5204:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r13
	.cfi_offset 13, -24
	movq	%rsi, %r13
	xorl	%esi, %esi
	pushq	%r12
	.cfi_offset 12, -32
	movq	%rdx, %r12
	movl	$1536, %edx
	pushq	%rbx
	.cfi_offset 3, -40
	movq	%rdi, %rbx
	andq	$-32, %rsp
	subq	$4608, %rsp
	movq	%rsp, %rdi
	call	memset
	movq	%r13, %r8
	negq	%r8
	andl	$31, %r8d
	je	.L48
	leal	-1(%r8), %ecx
	addq	$2, %rcx
	movl	$1, %eax
	movl	$761, %edi
	.p2align 4,,10
	.p2align 3
.L42:
	movsbw	-1(%r13,%rax), %dx
	movl	%edi, %esi
	movw	%dx, -2(%rsp,%rax,2)
	subl	%eax, %esi
	movl	%eax, %edx
	incq	%rax
	cmpq	%rcx, %rax
	jne	.L42
.L41:
	movl	$761, %r9d
	subl	%r8d, %r9d
	movl	%r9d, %edi
	movl	%r8d, %eax
	shrl	$5, %edi
	leaq	0(%r13,%rax), %r8
	leaq	(%rsp,%rax,2), %rcx
	salq	$5, %rdi
	xorl	%eax, %eax
	.p2align 4,,10
	.p2align 3
.L43:
	vmovdqa	(%r8,%rax), %ymm0
	vpmovsxbw	%xmm0, %ymm1
	vextracti128	$0x1, %ymm0, %xmm0
	vpmovsxbw	%xmm0, %ymm0
	vmovdqu	%ymm1, (%rcx,%rax,2)
	vmovdqu	%ymm0, 32(%rcx,%rax,2)
	addq	$32, %rax
	cmpq	%rax, %rdi
	jne	.L43
	movl	%r9d, %eax
	andl	$-32, %eax
	movl	%eax, %ecx
	addl	%eax, %edx
	notl	%ecx
	cmpl	%eax, %r9d
	je	.L44
	movslq	%edx, %rdx
	leal	(%rcx,%rsi), %edi
	xorl	%eax, %eax
	leaq	(%rsp,%rdx,2), %rsi
	leaq	0(%r13,%rdx), %rcx
	jmp	.L45
	.p2align 4,,10
	.p2align 3
.L49:
	movq	%rdx, %rax
.L45:
	movsbw	(%rcx,%rax), %dx
	movw	%dx, (%rsi,%rax,2)
	leaq	1(%rax), %rdx
	cmpq	%rdi, %rax
	jne	.L49
.L44:
	movq	%r12, %rdx
	movq	%rsp, %rsi
	leaq	1536(%rsp), %rdi
	vzeroupper
	call	mult768_mix2_m256i
	movswl	1536(%rsp), %eax
	movswl	3058(%rsp), %edx
	vmovdqa	.LC16(%rip), %ymm5
	addl	%eax, %edx
	imull	$10923, %edx, %eax
	vmovdqa	.LC17(%rip), %ymm4
	vmovdqa	.LC18(%rip), %ymm3
	sarl	$15, %eax
	leal	0(,%rax,4), %ecx
	subl	%ecx, %eax
	addl	%edx, %eax
	imull	$89478485, %eax, %edx
	vmovdqa	.LC19(%rip), %ymm2
	vmovdqa	.LC20(%rip), %ymm6
	addl	$134217728, %edx
	sarl	$28, %edx
	leal	0(,%rdx,4), %ecx
	subl	%ecx, %edx
	addl	%edx, %eax
	movb	%al, (%rbx)
	leaq	1(%rbx), %rdx
	leaq	3060(%rsp), %rax
	leaq	4532(%rsp), %rcx
	.p2align 4,,10
	.p2align 3
.L46:
	vmovdqu	-1522(%rax), %ymm8
	vmovdqu	-2(%rax), %ymm0
	vmovdqu	(%rax), %ymm7
	vpmovsxwd	%xmm8, %ymm9
	vpmovsxwd	%xmm0, %ymm12
	vpaddd	%ymm9, %ymm12, %ymm12
	vpmovsxwd	%xmm7, %ymm9
	vpaddd	%ymm9, %ymm12, %ymm12
	vextracti128	$0x1, %ymm0, %xmm9
	vpmulld	%ymm5, %ymm12, %ymm0
	vextracti128	$0x1, %ymm8, %xmm11
	vpmovsxwd	%xmm11, %ymm11
	vpmovsxwd	%xmm9, %ymm9
	vmovdqu	30(%rax), %ymm1
	vpaddd	%ymm11, %ymm9, %ymm9
	vextracti128	$0x1, %ymm7, %xmm11
	vpsrad	$15, %ymm0, %ymm0
	vpmovsxwd	%xmm11, %ymm11
	vpaddd	%ymm11, %ymm9, %ymm11
	vpmovsxwd	%xmm1, %ymm8
	vextracti128	$0x1, %ymm1, %xmm7
	vpslld	$2, %ymm0, %ymm1
	vpsubd	%ymm1, %ymm0, %ymm1
	vpmulld	%ymm5, %ymm11, %ymm0
	vmovdqu	-1490(%rax), %ymm10
	vmovdqu	32(%rax), %ymm13
	vpmovsxwd	%xmm10, %ymm14
	vextracti128	$0x1, %ymm10, %xmm10
	vpsrad	$15, %ymm0, %ymm0
	vpaddd	%ymm14, %ymm8, %ymm8
	vpmovsxwd	%xmm10, %ymm10
	vpslld	$2, %ymm0, %ymm9
	vpmovsxwd	%xmm13, %ymm14
	vpmovsxwd	%xmm7, %ymm7
	vpaddd	%ymm14, %ymm8, %ymm14
	vpaddd	%ymm10, %ymm7, %ymm7
	vpsubd	%ymm9, %ymm0, %ymm0
	vextracti128	$0x1, %ymm13, %xmm10
	vpaddd	%ymm11, %ymm0, %ymm9
	vpmovsxwd	%xmm10, %ymm10
	vpmulld	%ymm5, %ymm14, %ymm0
	vpaddd	%ymm10, %ymm7, %ymm10
	vpaddd	%ymm12, %ymm1, %ymm12
	vpmulld	%ymm5, %ymm10, %ymm7
	vpmulld	%ymm4, %ymm12, %ymm1
	vpsrad	$15, %ymm0, %ymm0
	vpslld	$2, %ymm0, %ymm8
	vpsubd	%ymm8, %ymm0, %ymm0
	vpsrad	$15, %ymm7, %ymm7
	vpaddd	%ymm3, %ymm1, %ymm1
	vpaddd	%ymm14, %ymm0, %ymm8
	vpsrad	$28, %ymm1, %ymm1
	vpslld	$2, %ymm7, %ymm0
	vpsubd	%ymm0, %ymm7, %ymm7
	vpslld	$2, %ymm1, %ymm0
	vpsubd	%ymm0, %ymm1, %ymm1
	vpmulld	%ymm4, %ymm9, %ymm0
	vpaddd	%ymm10, %ymm7, %ymm7
	vpaddd	%ymm12, %ymm1, %ymm1
	vpand	%ymm1, %ymm2, %ymm1
	addq	$64, %rax
	vpaddd	%ymm3, %ymm0, %ymm0
	vpsrad	$28, %ymm0, %ymm0
	vpslld	$2, %ymm0, %ymm10
	vpsubd	%ymm10, %ymm0, %ymm0
	vpaddd	%ymm9, %ymm0, %ymm0
	vpand	%ymm0, %ymm2, %ymm0
	vpackusdw	%ymm0, %ymm1, %ymm1
	vpmulld	%ymm4, %ymm8, %ymm0
	vpermq	$216, %ymm1, %ymm1
	vpand	%ymm1, %ymm6, %ymm1
	addq	$32, %rdx
	vpaddd	%ymm3, %ymm0, %ymm0
	vpsrad	$28, %ymm0, %ymm0
	vpslld	$2, %ymm0, %ymm9
	vpsubd	%ymm9, %ymm0, %ymm0
	vpaddd	%ymm8, %ymm0, %ymm0
	vpmulld	%ymm4, %ymm7, %ymm8
	vpand	%ymm0, %ymm2, %ymm0
	vpaddd	%ymm3, %ymm8, %ymm8
	vpsrad	$28, %ymm8, %ymm8
	vpslld	$2, %ymm8, %ymm9
	vpsubd	%ymm9, %ymm8, %ymm8
	vpaddd	%ymm7, %ymm8, %ymm7
	vpand	%ymm7, %ymm2, %ymm7
	vpackusdw	%ymm7, %ymm0, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vpand	%ymm0, %ymm6, %ymm0
	vpackuswb	%ymm0, %ymm1, %ymm0
	vpermq	$216, %ymm0, %ymm0
	vmovdqu	%ymm0, -32(%rdx)
	cmpq	%rax, %rcx
	jne	.L46
	movswl	4530(%rsp), %esi
	movl	$737, %ecx
	.p2align 4,,10
	.p2align 3
.L47:
	movl	%esi, %eax
	movswl	1536(%rsp,%rcx,2), %edx
	movswl	3058(%rsp,%rcx,2), %esi
	addl	%esi, %edx
	addl	%eax, %edx
	imull	$10923, %edx, %eax
	sarl	$15, %eax
	leal	0(,%rax,4), %edi
	subl	%edi, %eax
	addl	%eax, %edx
	imull	$89478485, %edx, %eax
	addl	$134217728, %eax
	sarl	$28, %eax
	leal	0(,%rax,4), %edi
	subl	%edi, %eax
	addl	%edx, %eax
	movb	%al, (%rbx,%rcx)
	incq	%rcx
	cmpq	$761, %rcx
	jne	.L47
	xorl	%eax, %eax
	movl	$0, 761(%rbx)
	movw	%ax, 765(%rbx)
	movb	$0, 767(%rbx)
	vzeroupper
	leaq	-24(%rbp), %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa 7, 8
	ret
.L48:
	.cfi_restore_state
	movl	$761, %esi
	xorl	%edx, %edx
	jmp	.L41
	.cfi_endproc
.LFE5204:
	.size	r3_mult, .-r3_mult
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
	.section	.rodata.cst32
	.align 32
.LC16:
	.long	10923
	.long	10923
	.long	10923
	.long	10923
	.long	10923
	.long	10923
	.long	10923
	.long	10923
	.align 32
.LC17:
	.long	89478485
	.long	89478485
	.long	89478485
	.long	89478485
	.long	89478485
	.long	89478485
	.long	89478485
	.long	89478485
	.align 32
.LC18:
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.long	134217728
	.align 32
.LC19:
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.long	65535
	.align 32
.LC20:
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
	.ident	"GCC: (GNU) 8.2.1 20181105 (Red Hat 8.2.1-5)"
	.section	.note.GNU-stack,"",@progbits
