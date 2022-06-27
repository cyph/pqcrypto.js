CC=gcc 
NISTGCCFLAGS =-std=c99 -Wall -Wextra -Wshadow -fsanitize=address,undefined -O2 
LFLAGS=-lm

all: sycon_hash_1

sycon_hash_1: sycon_hash

sycon_hash: genkat_hash.c hash.c sycon.c
	$(CC) $(NISTGCCFLAGS) -o $@ $^ $(LFLAGS)

.PHONY: clean

clean: 
	-rm sycon_hash 
