objects = main.o test_correctness.o test_cpucycles.o test_speed.o \
          ake.o ke.o kem.o encrypt.o ecc.o bch.o bin-lwe.o rand.o rng.o
cflags = -O3 -Wall -mavx2 -DNDEBUG


lac : $(objects)
	gcc -o lac $(objects)  -lcrypto -lz 

main.o: main.c test_correctness.h test_cpucycles.h test_speed.h api.h lac_param.h
	gcc -c main.c $(cflags)

test_correctness.o: test_correctness.c api.h ecc.h rand.h lac_param.h
	gcc -c test_correctness.c $(cflags)

test_cpucycles.o: test_cpucycles.c api.h rand.h ecc.h lac_param.h
	gcc -c test_cpucycles.c $(cflags)

test_speed.o: test_speed.c api.h rand.h ecc.h lac_param.h
	gcc -c test_speed.c $(cflags)

ake.o: ake.c api.h rand.h lac_param.h
	gcc -c ake.c $(cflags)

ke.o: ke.c api.h rand.h lac_param.h
	gcc -c ke.c $(cflags)

kem.o: kem.c api.h rand.h ecc.h bin-lwe.h lac_param.h
	gcc -c kem.c $(cflags)

encrypt.o: encrypt.c api.h rand.h ecc.h bin-lwe.h lac_param.h 
	gcc -c encrypt.c $(cflags)

ecc.o: ecc.c bch.h ecc.h lac_param.h bch128.h bch192.h bch256.h
	gcc -c ecc.c $(cflags)

bch.o: bch.c bch.h 
	gcc -c bch.c $(cflags)

bin-lwe.o: bin-lwe.c lac_param.h rand.h bin-lwe.h 
	gcc -c bin-lwe.c $(cflags)

rand.o: rand.c lac_param.h rand.h
	gcc -c rand.c $(cflags) 

rng.o: rng.c rng.h
	gcc -c rng.c $(cflags) 

clean:
	rm -f lac lac.exemak $(objects)


          