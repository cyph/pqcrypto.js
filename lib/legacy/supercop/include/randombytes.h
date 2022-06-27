#ifndef randombytes_h
#define randombytes_h

#ifdef __cplusplus
extern "C" {
#endif

extern void randombytes(unsigned char *,unsigned long long);
extern unsigned long long randombytes_calls;
extern unsigned long long randombytes_bytes;

#ifdef __cplusplus
}
#endif

#endif
