all:
	make clean

	cp pre.js dist/index.tmp.js
	cat ../../lib/sodiumutil/dist/sodiumutil.js | \
		perl -pe 's/if\(typeof module!=="undefined".*//g' >> dist/index.tmp.js

	cp -a ../../lib/legacy/mcbits/m12_t62_simple mcbits
	grep -rl crypt mcbits | xargs -I% sed -i 's|crypt(|crypt_mcbits(|g' %
	sed -i 's|crypto_encrypt_mcbits|crypto_encrypt|g' mcbits/operations.c
	sed -i 's|_keccakc1024||g' mcbits/operations.c
	sed -i 's|salsa20_ref|salsa20|g' mcbits/operations.c
	sed -i 's|salsa20|chacha20|g' mcbits/operations.c

	cp ../../lib/legacy/supercop/crypto_hash/keccakc1024/simple/Keccak-simple* mcbits/

	bash -c ' \
		args="$$(echo " \
			--memory-init-file 0 \
			-s SINGLE_FILE=1 \
			-DCYPHERTEXT_LEN=216 \
			-s TOTAL_MEMORY=16777216 -s TOTAL_STACK=8388608 \
			-s ASSERTIONS=0 \
			-s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
			-s ALIASING_FUNCTION_POINTERS=1 \
			-s DISABLE_EXCEPTION_CATCHING=1 \
			-s NO_FILESYSTEM=1 \
			-Ilibsodium/src/libsodium/include/sodium \
			-Imcbits \
			-I. \
			libsodium/src/libsodium/randombytes/randombytes.c \
			libsodium/src/libsodium/sodium/core.c \
			libsodium/src/libsodium/sodium/utils.c \
			$$(find libsodium/src/libsodium/crypto_core/salsa -type f -name "*.c" | tr "\n" " ") \
			$$(find libsodium/src/libsodium/crypto_onetimeauth/poly1305 -type f -name "*.c" | tr "\n" " ") \
			$$(find libsodium/src/libsodium/crypto_stream/chacha20 -type f -name "*.c" | tr "\n" " ") \
			$$(find libsodium/src/libsodium/crypto_verify -type f -name "*.c" | tr "\n" " ") \
			$$(ls mcbits/*.c) \
			main.c \
			-s EXPORTED_RUNTIME_METHODS=\"[ \
				'"'"'writeArrayToMemory'"'"' \
			]\" \
			-s EXPORTED_FUNCTIONS=\"[ \
				'"'"'_calloc'"'"', \
				'"'"'_free'"'"', \
				'"'"'_malloc'"'"', \
				'"'"'_mceliecejs_init'"'"', \
				'"'"'_mceliecejs_keypair'"'"', \
				'"'"'_mceliecejs_encrypt'"'"', \
				'"'"'_mceliecejs_decrypt'"'"', \
				'"'"'_mceliecejs_public_key_bytes'"'"', \
				'"'"'_mceliecejs_private_key_bytes'"'"', \
				'"'"'_mceliecejs_encrypted_bytes'"'"', \
				'"'"'_mceliecejs_decrypted_bytes'"'"' \
			]\" \
		" | perl -pe "s/\s+/ /g" | perl -pe "s/\[ /\[/g" | perl -pe "s/ \]/\]/g")"; \
		\
		bash -c "emcc -Oz -s WASM=0 $$args -o dist/index.asm.js"; \
		bash -c "emcc -O3 -s WASM=1 $$args -o dist/index.wasm.js"; \
	'

	echo " \
		var Module = {}; \
		Module.ready = new Promise(function (resolve, reject) { \
			var Module = {}; \
			Module.onAbort = reject; \
			Module.onRuntimeInitialized = function () { \
				try { \
					Module._mceliecejs_public_key_bytes(); \
					resolve(Module); \
				} \
				catch (err) { \
					reject(err); \
				} \
			}; \
	" >> dist/index.tmp.js
	cat dist/index.wasm.js >> dist/index.tmp.js
	echo " \
		}).catch(function () { \
			var Module = {}; \
	" >> dist/index.tmp.js
	cat dist/index.asm.js >> dist/index.tmp.js
	echo " \
			return new Promise(function (resolve, reject) { \
				Module.onAbort = reject; \
				Module.onRuntimeInitialized = function () { resolve(Module); }; \
			}); \
		}).then(function (m) { \
			Object.keys(m).forEach(function (k) { Module[k] = m[k]; }); \
		}); \
	" >> dist/index.tmp.js
	cat post.js >> dist/index.tmp.js

	terser dist/index.tmp.js -cmo dist/index.js

	sed -i 's|use asm||g' dist/index.js
	sed -i 's|require(|eval("require")(|g' dist/index.js

	rm -rf mcbits dist/index.*.js
	chmod -R 777 dist

clean:
	rm -rf dist mcbits 2> /dev/null
	mkdir dist

test:
	npm run test
