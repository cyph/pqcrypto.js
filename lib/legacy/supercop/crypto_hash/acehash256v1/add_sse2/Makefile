#CC=gcc -O2 -fomit-frame-pointer -funroll-all-loops -march=native -mtune=native -msse3 -mmmx -mavx -mavx2
#CC=gcc -Wall -O2 -fomit-frame-pointer -funroll-all-loops -march=native -mtune=native -msse2 -mmmx -mavx -mavx2
CC=gcc -Wall -O2 -fomit-frame-pointer -funroll-all-loops -march=native -mtune=native -msse2
#CC=gcc -O1 -fomit-frame-pointer -march=haswell -mtune=native -mavx
#ivybridgei, skylake, sandybridge, haswell
all: acesse2_1

acesse2_1:acesse2

acesse2: speed.c ace.c
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -fr acesse2 
