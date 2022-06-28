RSASIGNJS_BITS = 2048
RSASIGNJS_PUBLEN = 450
RSASIGNJS_PRIVLEN = 1700
RSASIGNJS_SIGLEN = 256

all:
	make clean

	bash -c ' \
		args="$$(echo " \
			--memory-init-file 0 \
			-s SINGLE_FILE=1 \
			-DRSASIGNJS_BITS=$(RSASIGNJS_BITS) \
			-DRSASIGNJS_PUBLEN=$(RSASIGNJS_PUBLEN) \
			-DRSASIGNJS_PRIVLEN=$(RSASIGNJS_PRIVLEN) \
			-DRSASIGNJS_SIGLEN=$(RSASIGNJS_SIGLEN) \
			-s TOTAL_MEMORY=16777216 -s TOTAL_STACK=8388608 \
			-s ASSERTIONS=0 \
			-s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
			-s ALIASING_FUNCTION_POINTERS=1 \
			-s DISABLE_EXCEPTION_CATCHING=1 \
			-s NO_FILESYSTEM=1 \
			-Ilibsodium/src/libsodium/include/sodium \
			-Iopenssl -Iopenssl/include -Iopenssl/crypto \
			libsodium/src/libsodium/randombytes/randombytes.c \
			openssl/crypto/rand/drbg_lib.o \
			openssl/crypto/rand/rand_err.o \
			$$(find openssl/crypto -type f -name "*.o" -not -path "openssl/crypto/rand/*" | tr "\n" " ") \
			main.c \
			-s EXPORTED_RUNTIME_METHODS=\"[ \
				'"'"'writeArrayToMemory'"'"' \
			]\" \
			-s EXPORTED_FUNCTIONS=\"[ \
				'"'"'_free'"'"', \
				'"'"'_malloc'"'"', \
				'"'"'_rsasignjs_init'"'"', \
				'"'"'_rsasignjs_keypair'"'"', \
				'"'"'_rsasignjs_sign'"'"', \
				'"'"'_rsasignjs_verify'"'"', \
				'"'"'_rsasignjs_public_key_bytes'"'"', \
				'"'"'_rsasignjs_secret_key_bytes'"'"', \
				'"'"'_rsasignjs_signature_bytes'"'"' \
			]\" \
		" | perl -pe "s/\s+/ /g" | perl -pe "s/\[ /\[/g" | perl -pe "s/ \]/\]/g")"; \
		\
		bash -c "emcc -Oz -s WASM=0 $$args -o dist/index.tmp.js"; \
	'

	cp pre.js dist/index.module.js
	cat dist/index.tmp.js >> dist/index.module.js
	cat post.js | \
		sed "s|RSASIGNJS_PUBLEN|$(RSASIGNJS_PUBLEN)|g" | \
		sed "s|RSASIGNJS_PRIVLEN|$(RSASIGNJS_PRIVLEN)|g" | \
		sed "s|RSASIGNJS_SIGLEN|$(RSASIGNJS_SIGLEN)|g" \
	>> dist/index.module.js

	sed -i 's|use asm||g' dist/index.module.js
	sed -i 's|require(|eval("require")(|g' dist/index.module.js
	sed -i 's|eval("require")(.pem-jwk-norecompute.)|require("pem-jwk-norecompute")|g' dist/index.module.js
	sed -i 's|eval("require")(.sodiumutil.)|require("sodiumutil")|g' dist/index.module.js

	webpack --mode none --output-library-target var --output-library rsaSign dist/index.module.js -o dist/index.js

	terser dist/index.module.js -cmo dist/index.module.js
	terser dist/index.js -cmo dist/index.js

	rm -rf dist/index.tmp.js
	chmod -R 777 dist

clean:
	rm -rf dist 2> /dev/null
	mkdir dist
