# Compiler setup
CC      = gcc
LDLIBS  = -lcrypto -lkeccak -lm
CFLAGS  = -march=native -mtune=native -O3 -fomit-frame-pointer -fwrapv -DNIST_KAT_GENERATION

# Build the application to generate the KATs
PQCgenKAT_kem:
	@$(CC) $(CFLAGS) -o $@ *.c $(LDLIBS)

# Clean build artefact
clean:
	@rm -f PQCgenKAT_kem

.PHONY: clean
