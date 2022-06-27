CC=gcc -Wall -O2 -fomit-frame-pointer -funroll-all-loops -march=native -mtune=native
all: spix_1 
spix_1: spix

spix: speed.c spix.c
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -fr spix 
