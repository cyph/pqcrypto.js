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
			-s RUNNING_JS_OPTS=1 \
			-s ASSERTIONS=0 \
			-s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
			-s ALIASING_FUNCTION_POINTERS=1 \
			-s DISABLE_EXCEPTION_CATCHING=1 \
			-s NO_FILESYSTEM=1 \
			-Ilibsodium/src/libsodium/include/sodium \
			-Iopenssl -Iopenssl/include -Iopenssl/crypto \
			libsodium/src/libsodium/randombytes/randombytes.c \
			openssl/crypto/rand/rand_err.o \
			$$(find openssl/crypto -type f -name "*.o" -not -path "openssl/crypto/rand/*" | tr "\n" " ") \
			rsasign.c \
			-s EXTRA_EXPORTED_RUNTIME_METHODS=\"[ \
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
		bash -c "emcc -Oz -s WASM=0 $$args -o dist/rsasign.tmp.js"; \
	'

	cp pre.js dist/rsasign.module.js
	cat dist/rsasign.tmp.js >> dist/rsasign.module.js
	cat post.js | \
		sed "s|RSASIGNJS_PUBLEN|$(RSASIGNJS_PUBLEN)|g" | \
		sed "s|RSASIGNJS_PRIVLEN|$(RSASIGNJS_PRIVLEN)|g" | \
		sed "s|RSASIGNJS_SIGLEN|$(RSASIGNJS_SIGLEN)|g" \
	>> dist/rsasign.module.js

	sed -i 's|use asm||g' dist/rsasign.module.js
	sed -i 's|require(|eval("require")(|g' dist/rsasign.module.js
	sed -i 's|eval("require")(.pem-jwk-norecompute.)|require("pem-jwk-norecompute")|g' dist/rsasign.module.js
	sed -i 's|eval("require")(.sodiumutil.)|require("sodiumutil")|g' dist/rsasign.module.js

	webpack --mode none --output-library-target var --output-library rsaSign dist/rsasign.module.js -o dist/rsasign.js

	terser dist/rsasign.module.js -cmo dist/rsasign.module.js
	terser dist/rsasign.js -cmo dist/rsasign.js

	rm -rf dist/rsasign.tmp.js
	chmod -R 777 dist

clean:
	rm -rf dist 2> /dev/null
	mkdir dist
