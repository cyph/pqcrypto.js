//
//  encrypt.c
//  Limdolen
//

#define CRYPTO_KEYBYTES 16  // key length
#define CRYPTO_NSECBYTES 0  // always zero
#define CRYPTO_NPUBBYTES 16 // nonce
#define CRYPTO_ABYTES 16    // tag length
#define CRYPTO_NOOVERLAP 1  // must be there b/c of supercop
