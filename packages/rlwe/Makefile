all:
	make clean

	cp -a ../../lib/legacy/rlwe ./
	cd rlwe ; mv kex.c tmp ; echo '#include <stdlib.h>' > kex.c ; cat tmp >> kex.c ; rm tmp

	bash -c ' \
		args="$$(echo " \
			--memory-init-file 0 \
			-s SINGLE_FILE=1 \
			-D_GENERIC_ -D__LINUX__ -D_X86_ \
			-s TOTAL_MEMORY=16777216 -s TOTAL_STACK=8388608 \
			-s ASSERTIONS=0 \
			-s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
			-s ALIASING_FUNCTION_POINTERS=1 \
			-s DISABLE_EXCEPTION_CATCHING=1 \
			-s NO_FILESYSTEM=1 \
			-s ERROR_ON_UNDEFINED_SYMBOLS=0 \
			-Ilibsodium/src/libsodium/include/sodium \
			-Irlwe \
			libsodium/src/libsodium/sodium/utils.c \
			libsodium/src/libsodium/randombytes/randombytes.c \
			$$(ls libsodium/src/libsodium/crypto_stream/*.c) \
			$$(ls libsodium/src/libsodium/crypto_stream/chacha20/*.c) \
			$$(ls libsodium/src/libsodium/crypto_stream/chacha20/ref/*.c) \
			$$(ls rlwe/*.c rlwe/generic/*.c) \
			rlwe.c \
			-s EXTRA_EXPORTED_RUNTIME_METHODS=\"[ \
				'"'"'writeArrayToMemory'"'"' \
			]\" \
			-s EXPORTED_FUNCTIONS=\"[ \
				'"'"'_free'"'"', \
				'"'"'_malloc'"'"', \
				'"'"'_rlwejs_init'"'"', \
				'"'"'_rlwejs_keypair_alice'"'"', \
				'"'"'_rlwejs_secret_alice'"'"', \
				'"'"'_rlwejs_secret_bob'"'"', \
				'"'"'_rlwejs_public_key_bytes'"'"', \
				'"'"'_rlwejs_private_key_bytes'"'"', \
				'"'"'_rlwejs_secret_bytes'"'"' \
			]\" \
		" | perl -pe "s/\s+/ /g" | perl -pe "s/\[ /\[/g" | perl -pe "s/ \]/\]/g")"; \
		\
		bash -c "emcc -Oz -s WASM=0 -s RUNNING_JS_OPTS=1 $$args -o dist/rlwe.asm.js"; \
		bash -c "emcc -O3 -s WASM=1 $$args -o dist/rlwe.wasm.js"; \
	'

	cp pre.js dist/rlwe.tmp.js
	echo " \
		var Module = {}; \
		Module.ready = new Promise(function (resolve, reject) { \
			var Module = {}; \
			Module.onAbort = reject; \
			Module.onRuntimeInitialized = function () { \
				try { \
					Module._rlwejs_public_key_bytes(); \
					resolve(Module); \
				} \
				catch (err) { \
					reject(err); \
				} \
			}; \
	" >> dist/rlwe.tmp.js
	cat dist/rlwe.wasm.js >> dist/rlwe.tmp.js
	echo " \
		}).catch(function () { \
			var Module = {}; \
	" >> dist/rlwe.tmp.js
	cat dist/rlwe.asm.js >> dist/rlwe.tmp.js
	echo " \
			return new Promise(function (resolve, reject) { \
				Module.onAbort = reject; \
				Module.onRuntimeInitialized = function () { resolve(Module); }; \
			}); \
		}).then(function (m) { \
			Object.keys(m).forEach(function (k) { Module[k] = m[k]; }); \
		}); \
	" >> dist/rlwe.tmp.js
	cat post.js >> dist/rlwe.tmp.js

	terser dist/rlwe.tmp.js -cmo dist/rlwe.js

	sed -i 's|use asm||g' dist/rlwe.js
	sed -i 's|require(|eval("require")(|g' dist/rlwe.js

	rm -rf rlwe dist/rlwe.*.js
	chmod -R 777 dist

clean:
	rm -rf dist rlwe 2> /dev/null
	mkdir dist
