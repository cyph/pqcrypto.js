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
ca1c8cecfe3fd03c1ef9beda2fecee92  ./params.h
27926dc690826a815e9cd073b957cdb2  ./README.txt
3316482ce5da4303c107f6d5641f08e4  ./neon_poly_rq_mul.c
38f6e78435cf30a562b4ebfe8ec878ea  ./owcpa.c
deaaaf9aa3b637e6bf119a6309f8b5a9  ./owcpa.h
983c6e50e76f14322bf5e9b008a02cbf  ./api.h
8eb43600849edf4dc9d68a7dfaa52fac  ./verify.c
2528b2860721dadf4d9b42ffbf21d9d3  ./sample.h
a8f27ad8127194670a1525da2627a924  ./sample_iid.c
08eb70977885e30c3487bbd002d2c558  ./implementors
476217b661a16a39a24967e22058dd20  ./packq.c
c4b12cd8d1f3c328fe10bd01c88cd9f0  ./architectures
5822ca6494ff6f21be0b5c53db7f6269  ./poly_r2_inv.c
940e0ad84d1d62cdc0490050a3a4aa55  ./sample.c
9b74a1ca21184f838bca228290c40d02  ./poly_lift.c
fc23663aa6c00f77f075e5c2fa68df43  ./rq_mul/neon_poly_rq_mul.c
dabd97b103209cf624472a0d94af52b3  ./rq_mul/neon_matrix_transpose.c
78780469c2988a357803925d71544372  ./rq_mul/neon_matrix_transpose.h
13fdc0cde498b80687a490a914deaceb  ./rq_mul/neon_batch_multiplication.h
8c26083071b3fe56f2cd75385482ff59  ./rq_mul/neon_poly_rq_mul.h
3ce65fb0a2fc8502afff4b4836ab2e3c  ./rq_mul/neon_batch_multiplication.c
b0302140479f271a90fe2ada6e218cce  ./poly_s3_inv.c
e8804edee5585be385bc4ad0ac2f4599  ./poly.c
0363aab3f33ea271a711247726041e69  ./poly.h
30f0398d933a5120e2f4e8dda90b7cca  ./kem.c
0372aa3da377496f94ea6ee8fdb67da3  ./neon_poly_mod.c
03d52993b4afce10aee17325b655d4b4  ./pack3.c
26d7396f27cc8f6448b82a8032427cae  ./verify.h
b3acbb4db22891e6019558f7c15f5179  ./kem.h
