#ifndef SHORT_H
#define SHORT_H

#define signatureofshorthash CRYPTO_NAMESPACE(signatureofshorthash)
#define verification CRYPTO_NAMESPACE(verification)

extern int signatureofshorthash(unsigned char *,unsigned long long *,
  const unsigned char *,unsigned long long,
  const unsigned char *,unsigned long long);
extern int verification(const unsigned char *,unsigned long long,
  const unsigned char *,unsigned long long,
  const unsigned char *,unsigned long long);

#endif
