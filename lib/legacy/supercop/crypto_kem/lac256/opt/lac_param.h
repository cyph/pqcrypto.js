//#define TEST_ROW_ERROR_RATE
//security level
#define LAC256
//modulus
#define Q 251
#define BIG_Q 257024//1024*Q 

#if defined(LAC128)
//parameter for LAC_LIGHT
#define STRENGTH "LAC128"
#define DIM_N 512
#define SEED_LEN 32
#define PK_LEN 544 //N+SEED_LEN
#define MESSAGE_LEN 32
#define CIPHER_LEN 1024//N+N
#define C2_VEC_NUM 512
#define PSI//secret and error distribution
#define HASH_TYPE "SHA256"
#endif

#if defined(LAC192)
//parameter for LAC_STANDARD
#define STRENGTH "LAC192"
#define DIM_N 1024
#define SEED_LEN 32
#define PK_LEN 1056//N+SEED_LEN
#define MESSAGE_LEN 48
#define CIPHER_LEN 1536//N+N/2
#define C2_VEC_NUM 512
#define PSI_SQUARE//secret and error distribution
#define HASH_TYPE "SHA384"
#endif

#if defined(LAC256)
//parameter for LAC_HIGH
#define STRENGTH "LAC256"
#define DIM_N 1024
#define SEED_LEN 32
#define PK_LEN 1056//(N+SEED_LEN)
#define MESSAGE_LEN 64
#define CIPHER_LEN 2048//(N+N)
#define C2_VEC_NUM 1024
#define PSI//secret and error distribution
#define HASH_TYPE "SHA512"
#endif
