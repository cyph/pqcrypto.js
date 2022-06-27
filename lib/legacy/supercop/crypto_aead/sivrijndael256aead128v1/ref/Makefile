CC=gcc
CFLAGES=-std=c99 -Wall -Wextra -Wshadow -fsanitize=address,undefined -fuse-ld=gold -O2
OBJS=genkat_aead.o encrypt.o rijndael256.o rijndael-alg-ref.o 

all: ref

ref: $(OBJS)
	$(CC) -o ref $(OBJS) $(CFLAGES)

genkat_aead.o: ../../../genkat_aead.c
	$(CC) -c ../../../genkat_aead.c $(CFLAGES) -I./

encrypt.o: encrypt.c
	$(CC) -o encrypt.o -c encrypt.c $(CFLAGES) -I../../../

rijndael-alg-ref.o: rijndael-alg-ref.c 
	$(CC) -c rijndael-alg-ref.c $(CFLAGES)

rijndael256.o: rijndael256.c rijndael-alg-ref.c
	$(CC) -c rijndael256.c rijndael-alg-ref.c $(CFLAGES)


.PHONY : clean

clean :
	rm $(OBJS)