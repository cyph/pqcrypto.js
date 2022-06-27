all:
	rm -rf dist libsodium mcbits sodiumutil 2> /dev/null
	mkdir dist

	git clone --depth 1 -b stable https://github.com/jedisct1/libsodium
	cd libsodium ; emconfigure ./configure --enable-minimal --disable-shared

	git clone --depth 1 https://github.com/cyph/sodiumutil
	cp pre.js dist/mceliece.tmp.js
	cat sodiumutil/dist/sodiumutil.js | \
		perl -pe 's/if\(typeof module!=="undefined".*//g' >> dist/mceliece.tmp.js

	wget https://tungchou.github.io/code/mcbits_new_2.0.tar.gz
	tar xzf mcbits_new_2.0.tar.gz
	mv mcbits_new/m12_t62_simple mcbits
	rm -rf mcbits_new mcbits_new_2.0.tar.gz
	grep -rl crypt mcbits | xargs -I% sed -i 's|crypt(|crypt_mcbits(|g' %
	sed -i 's|crypto_encrypt_mcbits|crypto_encrypt|g' mcbits/operations.c
	sed -i 's|_keccakc1024||g' mcbits/operations.c
	sed -i 's|salsa20_ref|salsa20|g' mcbits/operations.c
	sed -i 's|salsa20|chacha20|g' mcbits/operations.c

	wget https://bench.cr.yp.to/supercop/supercop-20190816.tar.xz
	unxz < supercop-20190816.tar.xz | tar -xf -
	mv supercop-20190816/crypto_hash/keccakc1024/simple/Keccak-simple* mcbits/
	rm -rf supercop*

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
			mceliece.c \
			-s EXTRA_EXPORTED_RUNTIME_METHODS=\"[ \
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
		bash -c "emcc -Oz -s WASM=0 -s RUNNING_JS_OPTS=1 $$args -o dist/mceliece.asm.js"; \
		bash -c "emcc -O3 -s WASM=1 $$args -o dist/mceliece.wasm.js"; \
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
	" >> dist/mceliece.tmp.js
	cat dist/mceliece.wasm.js >> dist/mceliece.tmp.js
	echo " \
		}).catch(function () { \
			var Module = {}; \
	" >> dist/mceliece.tmp.js
	cat dist/mceliece.asm.js >> dist/mceliece.tmp.js
	echo " \
			return new Promise(function (resolve, reject) { \
				Module.onAbort = reject; \
				Module.onRuntimeInitialized = function () { resolve(Module); }; \
			}); \
		}).then(function (m) { \
			Object.keys(m).forEach(function (k) { Module[k] = m[k]; }); \
		}); \
	" >> dist/mceliece.tmp.js
	cat post.js >> dist/mceliece.tmp.js

	terser dist/mceliece.tmp.js -cmo dist/mceliece.js

	sed -i 's|use asm||g' dist/mceliece.js
	sed -i 's|require(|eval("require")(|g' dist/mceliece.js

	rm -rf libsodium mcbits sodiumutil dist/mceliece.*.js

clean:
	rm -rf dist libsodium mcbits sodiumutil
