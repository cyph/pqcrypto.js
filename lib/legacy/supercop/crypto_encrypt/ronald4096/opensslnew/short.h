#ifndef SHORT_H
#define SHORT_H

#define shortplaintext CRYPTO_NAMESPACE(shortplaintext)
#define shortciphertext CRYPTO_NAMESPACE(shortciphertext)

extern int shortplaintext(unsigned char *,unsigned long long *,
  const unsigned char *,unsigned long long,const unsigned char *,unsigned long long);
extern int shortciphertext(unsigned char *,unsigned long long *,
  const unsigned char *,unsigned long long,const unsigned char *,unsigned long long);

#endif
