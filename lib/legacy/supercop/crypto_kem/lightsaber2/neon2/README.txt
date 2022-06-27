This implementation is based on SABER latest commit at: https://github.com/KULeuven-COSIC/SABER

Any pull requests, issues are welcome at: https://github.com/cothan/SABER

Best regards, 
-----
Duc Tri Nguyen (CERG GMU)

.
├── api.h
├── apiorig.h
├── architectures
├── cbd.c
├── cbd.h
├── crypto_stream.h
├── fips202.c
├── fips202.h
├── implementors
├── kem.c
├── kem.h
├── pack_unpack.c
├── pack_unpack.h
├── poly.c
├── poly.h
├── README.txt
├── rq_mul
│   ├── neon_batch_multiplication.c
│   ├── neon_batch_multiplication.h
│   ├── neon_matrix_transpose.c
│   ├── neon_matrix_transpose.h
│   ├── neon_poly_rq_mul.c
│   └── neon_poly_rq_mul.h
├── SABER_indcpa.c
├── SABER_indcpa.h
├── SABER_params.h
├── verify.c
└── verify.h

1 directory, 26 files
550e09228e56210c2835b5f390a81220  ./api.h
bcb961cca51f63814fbc1c7b7c1892e5  ./verify.c
7a794e9b2699b2f8548997c09143d130  ./fips202.h
596ed8e81143fa3f7b19f0227ec44c13  ./SABER_params.h
1bcb1fa9d14b9be69d0c7af0041fab04  ./pack_unpack.c
95c065a367424e7db50c232ed9703705  ./SABER_indcpa.c
f462b0d1a5033705a5fd65559cd56567  ./implementors
eae70d6eeba93027ed4a1017611956a6  ./cbd.c
c4b12cd8d1f3c328fe10bd01c88cd9f0  ./architectures
86a1278bf772ee581cd5c299d3c9983d  ./SABER_indcpa.h
1cb977efbfddf5c6475d3f6bfefa1410  ./cbd.h
32bc67ce2f6e9a0d316a6335e466bdc6  ./fips202.c
0f51f61444e621454074d274f3327efe  ./rq_mul/neon_poly_rq_mul.c
c2a7ecd4b5360fd29e51585cf3dbfcf5  ./rq_mul/neon_matrix_transpose.c
6895ee9d0163dd5979df6b2dae57e9cf  ./rq_mul/neon_matrix_transpose.h
99de300d3ffba862590ffa0bf5d51625  ./rq_mul/neon_batch_multiplication.h
7c8ef5cf2ea180f12845fad60df40641  ./rq_mul/neon_poly_rq_mul.h
84579da93ee608e6e851aaf3b081eb89  ./rq_mul/neon_batch_multiplication.c
d9882eee52079852f2c46034e470e754  ./poly.c
8a26b15a57e10367c3fafca63c8a0bda  ./poly.h
a4985af1c963d5c07fd9b10abd1dab41  ./kem.c
cec3111bdf6da7d016b2441b3b9e82b1  ./pack_unpack.h
80c1c93393fa780a28b24724bd487fe2  ./verify.h
70816f11b636da452709b9083732e5c0  ./apiorig.h
3f5868e49e234b1f5eeeb4edec2db560  ./crypto_stream.h
3c0eb574f59b132e6099f08a729dce1d  ./kem.h
