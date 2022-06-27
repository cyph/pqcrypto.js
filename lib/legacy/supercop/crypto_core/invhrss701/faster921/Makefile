core.s: ../faster/core.c
	echo '#define crypto_core CRYPTO_NAMESPACETOP' > crypto_core.h
	gcc -fPIC -fPIE -fwrapv -O3 -march=haswell -mtune=haswell -S -I. ../faster/core.c
	rm crypto_core.h
