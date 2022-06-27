#ifndef COMPOSE_H
#define COMPOSE_H

#include "rainbow.h"

int keypair(unsigned char * sk, unsigned long * sklen,
	unsigned char * pk, unsigned long * pklen);

int shortmessagesigned(unsigned char * m, unsigned long * mlen,
	const unsigned char * sm, unsigned long smlen,
	const unsigned char * pk, unsigned long pklen);

int signedshortmessage(unsigned char * sm, unsigned long * smlen,
	const unsigned char * m, unsigned long mlen,
	const unsigned char * sk, unsigned long sklen);

#endif  /* COMPOSE_H  */
