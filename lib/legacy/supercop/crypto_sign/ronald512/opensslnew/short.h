#ifndef SHORT_H
#define SHORT_H

#define signedshortmessage CRYPTO_NAMESPACE(signedshortmessage)
#define shortmessagesigned CRYPTO_NAMESPACE(shortmessagesigned)

extern int signedshortmessage(unsigned char *,unsigned long long *,
  const unsigned char *,unsigned long long,const unsigned char *,unsigned long long);
extern int shortmessagesigned(unsigned char *,unsigned long long *,
  const unsigned char *,unsigned long long,const unsigned char *,unsigned long long);

#endif
