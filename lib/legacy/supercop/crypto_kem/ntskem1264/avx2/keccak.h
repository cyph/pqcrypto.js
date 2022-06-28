/**
 *  keccak.h
 *
 *  The code below is taken from https://github.com/coruus/keccak-tiny/blob/singlefile/keccak-tiny.h
 **/

#ifndef __KECCAK_H
#define __KECCAK_H

void shake_256(const unsigned char *input, unsigned int inputByteLen, unsigned char *output, int outputByteLen);

#endif /* __KECCAK_H */
