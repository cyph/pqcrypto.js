# Makefile
# 2017-09-23  Markku-Juhani O. Saarinen <mjos@iki.fi>
# (Reference Implementation)

BIN	= genkat_ref
OBJS	= kem.o hila5_sha3_ref.o \
	../KAT/src/rng.o ../KAT/src/PQCgenKAT_kem.o

CC	?= gcc
CFLAGS	+= -std=c99 -Wall -Ofast -I.
LDLIBS	+= -lcrypto
LDFLAGS	+=

$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJS) $(LDLIBS)

test:	$(BIN)
	./$(BIN)
	@echo "== BEGIN KAT DIFFERENCE =="
	@diff PQCkemKAT_1824.rsp ../KAT/PQCkemKAT_1824.rsp
	@echo "== END KAT DIFFERENCE =="

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS) $(BIN) PQCkemKAT_1824.*
