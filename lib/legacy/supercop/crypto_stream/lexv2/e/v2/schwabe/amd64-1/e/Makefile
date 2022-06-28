
name = lex
type = sync
std = -ansi

srcdir = .
root := $(shell \
  cd "$(srcdir)"; root="$(srcdir)"; \
  while [ "`pwd`" != "/" ]; do \
    if [ -r "`pwd`/test/ecrypt-test.mk" ]; then  \
      echo $$root; exit; \
    fi; \
    cd ..; root="$$root"/..; \
  done; \
  echo ".")

include $(root)/test/ecrypt-test.mk

lex.s: crypto_stream_lexv2_e_v2_schwabe_amd64_1_ECRYPT_process_bytes.s crypto_stream_lexv2_e_v2_schwabe_amd64_1_ECRYPT_ivsetup.s table.s
	cat $^ > lex.s
