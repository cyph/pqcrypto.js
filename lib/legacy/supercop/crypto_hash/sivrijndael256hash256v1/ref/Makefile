CC=gcc
CFLAGES=-std=c99 -Wall -Wextra -Wshadow -fsanitize=address,undefined -fuse-ld=gold -O2
OBJS=genkat_hash.o hash.o rijndael256.o rijndael-alg-ref.o 

all: ref

ref: $(OBJS)
	$(CC) -o ref $(OBJS) $(CFLAGES)

genkat_hash.o: ../../../genkat_hash.c
	$(CC) -c ../../../genkat_hash.c $(CFLAGES) -I./

hash.o: hash.c
	$(CC) -o hash.o -c hash.c $(CFLAGES) -I../../../

rijndael-alg-ref.o: rijndael-alg-ref.c 
	$(CC) -c rijndael-alg-ref.c $(CFLAGES)

rijndael256.o: rijndael256.c rijndael-alg-ref.c
	$(CC) -c rijndael256.c rijndael-alg-ref.c $(CFLAGES)

.PHONY : clean

clean :
	rm $(OBJS)