CC=gcc -g -Wall -O2 -fomit-frame-pointer -march=native -mtune=native
LIB=-lm


all: sycon_1

sycon_1: sycon

sycon: speed.c sycon.c
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -fr sycon
