#include "lac_param.h"

#if defined(LAC128)
//bch(511,264,59)
#define DATA_LEN 33
#define ECCBUF_LEN 64
#define ECC_LEN 31
#define MAX_ERROR 29
#define CODE_LEN 64
#define LOG_CODE_LEN 9
#endif

#if defined(LAC192)
//bch(511,392,27)
#define DATA_LEN 49
#define ECCBUF_LEN 64
#define ECC_LEN 15
#define MAX_ERROR 13
#define CODE_LEN 64
#define LOG_CODE_LEN 9
#endif

#if defined(LAC256)
//bch(1023,520,111)
#define DATA_LEN 65
#define ECCBUF_LEN 128
#define ECC_LEN 63
#define MAX_ERROR 55
#define CODE_LEN 128
#define LOG_CODE_LEN 10
#endif

//error correction encode
int ecc_enc(const unsigned char *d, unsigned char *c);

//error corrction decode
int ecc_dec(unsigned char *d, const unsigned char *c);

