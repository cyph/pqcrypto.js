default: ntt.S

ntt.S: ../avx/ntt.c
	cp ../avx/ntt.c .
	touch crypto_core.h
	gcc-7 -fPIC -fPIE -fwrapv -O3 -march=haswell -mtune=skylake -D'CRYPTO_NAMESPACE(x)=x' -S -I. ntt.c
	mv ntt.s ntt.S
	sed -i 's/\<invntt512_7681\>/CRYPTO_NAMESPACE(invntt512_7681)/g' ntt.S
	sed -i 's/\<invntt512_10753\>/CRYPTO_NAMESPACE(invntt512_10753)/g' ntt.S
	sed -i 's/\<ntt512_7681\>/CRYPTO_NAMESPACE(ntt512_7681)/g' ntt.S
	sed -i 's/\<ntt512_10753\>/CRYPTO_NAMESPACE(ntt512_10753)/g' ntt.S
	rm ntt.c crypto_core.h
	touch -r ../avx/ntt.c ntt.S
