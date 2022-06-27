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
119741c230b3fe5406a373e6de97a2bb  ./params.h
a436e762f74e39ed34854ee11db14470  ./README.txt
4a155d76f2689ca06d7e2dd4b1b96dfa  ./neon_poly_rq_mul.c
38f6e78435cf30a562b4ebfe8ec878ea  ./owcpa.c
deaaaf9aa3b637e6bf119a6309f8b5a9  ./owcpa.h
f340f8b419fc61270bab0ea76e6d1018  ./api.h
fc6dba7613ab3c226c5ec2b7c47c0f97  ./verify.c
5954686b674e2e83beeea9a24b5dbc03  ./sample.h
a8f27ad8127194670a1525da2627a924  ./sample_iid.c
08eb70977885e30c3487bbd002d2c558  ./implementors
3970e4f610ffee8a746aa8bee30d28f0  ./packq.c
c4b12cd8d1f3c328fe10bd01c88cd9f0  ./architectures
5822ca6494ff6f21be0b5c53db7f6269  ./poly_r2_inv.c
940e0ad84d1d62cdc0490050a3a4aa55  ./sample.c
9b74a1ca21184f838bca228290c40d02  ./poly_lift.c
dfe804a49d2d27278bd5d3cb21132d01  ./rq_mul/neon_poly_rq_mul.c
c2a7ecd4b5360fd29e51585cf3dbfcf5  ./rq_mul/neon_matrix_transpose.c
6895ee9d0163dd5979df6b2dae57e9cf  ./rq_mul/neon_matrix_transpose.h
99de300d3ffba862590ffa0bf5d51625  ./rq_mul/neon_batch_multiplication.h
aabd6c3287bc201e39957f5821639811  ./rq_mul/neon_poly_rq_mul.h
84579da93ee608e6e851aaf3b081eb89  ./rq_mul/neon_batch_multiplication.c
b0302140479f271a90fe2ada6e218cce  ./poly_s3_inv.c
e8804edee5585be385bc4ad0ac2f4599  ./poly.c
f7ac46834d9108320523b8ce24eae0b4  ./poly.h
30f0398d933a5120e2f4e8dda90b7cca  ./kem.c
ecd67ddf1dda34790c74d7c96f2fc40e  ./neon_poly_mod.c
03d52993b4afce10aee17325b655d4b4  ./pack3.c
26d7396f27cc8f6448b82a8032427cae  ./verify.h
522398945f768d060d05d83bee5db162  ./kem.h
