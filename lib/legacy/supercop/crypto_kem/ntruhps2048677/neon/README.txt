This implementation is based on NTRU latest commit at: https://github.com/jschanck/ntru

Any pull requests, issues are welcome at: https://github.com/cothan/ntru/

Best regards, 
-----
Duc Tri Nguyen (CERG GMU)



.
├── api.h
├── architectures
├── implementors
├── kem.c
├── kem.h
├── neon_poly_mod.c
├── neon_poly_rq_mul.c
├── owcpa.c
├── owcpa.h
├── pack3.c
├── packq.c
├── params.h
├── poly.c
├── poly.h
├── poly_lift.c
├── poly_r2_inv.c
├── poly_s3_inv.c
├── README.txt
├── rq_mul
│   ├── neon_batch_multiplication.c
│   ├── neon_batch_multiplication.h
│   ├── neon_matrix_transpose.c
│   ├── neon_matrix_transpose.h
│   ├── neon_poly_rq_mul.c
│   └── neon_poly_rq_mul.h
├── sample.c
├── sample.h
├── sample_iid.c
├── verify.c
└── verify.h

1 directory, 29 files
dda2cbfb24a387f18cbb34e663894e63  ./params.h
c16edbcedd6a0b1ab7af848d1a15f066  ./README.txt
3b5a8170d0e4847be42cd4ee7ba83277  ./neon_poly_rq_mul.c
38f6e78435cf30a562b4ebfe8ec878ea  ./owcpa.c
deaaaf9aa3b637e6bf119a6309f8b5a9  ./owcpa.h
7424ea6d3d75377ab81fbf25c7100088  ./api.h
8eb43600849edf4dc9d68a7dfaa52fac  ./verify.c
493ff408a09eccd31780cfa7ffc677f2  ./sample.h
a8f27ad8127194670a1525da2627a924  ./sample_iid.c
08eb70977885e30c3487bbd002d2c558  ./implementors
3970e4f610ffee8a746aa8bee30d28f0  ./packq.c
c4b12cd8d1f3c328fe10bd01c88cd9f0  ./architectures
5822ca6494ff6f21be0b5c53db7f6269  ./poly_r2_inv.c
940e0ad84d1d62cdc0490050a3a4aa55  ./sample.c
9b74a1ca21184f838bca228290c40d02  ./poly_lift.c
14d6352b8a8569097d869c13587c56f5  ./rq_mul/neon_poly_rq_mul.c
40fabf34360a0c9b930831b7f3ca7085  ./rq_mul/neon_matrix_transpose.c
78780469c2988a357803925d71544372  ./rq_mul/neon_matrix_transpose.h
13fdc0cde498b80687a490a914deaceb  ./rq_mul/neon_batch_multiplication.h
fdc3526e3a8e4ab2c00c0701ab36c18c  ./rq_mul/neon_poly_rq_mul.h
51c2a94aa4d08182692077b2f9ea78b6  ./rq_mul/neon_batch_multiplication.c
b0302140479f271a90fe2ada6e218cce  ./poly_s3_inv.c
e8804edee5585be385bc4ad0ac2f4599  ./poly.c
0363aab3f33ea271a711247726041e69  ./poly.h
30f0398d933a5120e2f4e8dda90b7cca  ./kem.c
6fd13fa79757d811c80b7273ded7ac5d  ./neon_poly_mod.c
03d52993b4afce10aee17325b655d4b4  ./pack3.c
26d7396f27cc8f6448b82a8032427cae  ./verify.h
b3acbb4db22891e6019558f7c15f5179  ./kem.h
