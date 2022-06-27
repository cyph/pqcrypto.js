CC=gcc -O2 -fomit-frame-pointer -funroll-all-loops -march=native -mtune=native 
CLIB=-lm

all: sycon_1

sycon_1: sycon

sycon: speed.c sycon.c
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -fr sycon
