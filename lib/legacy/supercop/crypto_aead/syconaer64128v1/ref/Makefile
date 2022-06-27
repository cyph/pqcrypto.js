CC=gcc 
GCCFLAGS =-std=c99 -Wall -Wextra -Wshadow -fsanitize=address,undefined -O2 
LFLAGS=-lm
all: sycon128_1 
sycon128_1: sycon128 
sycon128: genkat_aead.c encrypt.c sycon.c
	$(CC) $(GCCFLAGS) -o $@ $^ $(LFLAGS)

.PHONY: clean

clean: 
	-rm sycon128
