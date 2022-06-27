all:
	rm -rf dist SIDH libsodium 2> /dev/null
	mkdir dist

	git clone --depth 1 -b stable https://github.com/jedisct1/libsodium
	cd libsodium ; emconfigure ./configure --enable-minimal --disable-shared

	git clone -b v3.0 --depth 1 https://github.com/Microsoft/PQCrypto-SIDH SIDH

	bash -c ' \
		args="$$(echo " \
			--memory-init-file 0 \
			-s SINGLE_FILE=1 \
			-DCYPHERTEXT_LEN=123 \
			-D_GENERIC_ -D__LINUX__ -D_X86_ \
			-s TOTAL_MEMORY=16777216 -s TOTAL_STACK=8388608 \
			-s ASSERTIONS=0 \
			-s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
			-s ALIASING_FUNCTION_POINTERS=1 \
			-s DISABLE_EXCEPTION_CATCHING=1 \
			-s NO_FILESYSTEM=1 \
			-s ERROR_ON_UNDEFINED_SYMBOLS=0 \
			-Ilibsodium/src/libsodium/include \
			-Ilibsodium/src/libsodium/include/sodium \
			-ISIDH/src/P503 \
			$$(find libsodium/src/libsodium -type f -name '\\*.c') \
			SIDH/src/sha3/*.c \
			SIDH/src/P503/generic/*.c \
			SIDH/src/P503/P503.c \
			sidh.c \
			-s EXTRA_EXPORTED_RUNTIME_METHODS=\"[ \
				'"'"'writeArrayToMemory'"'"' \
			]\" \
			-s EXPORTED_FUNCTIONS=\"[ \
				'"'"'_calloc'"'"', \
				'"'"'_free'"'"', \
				'"'"'_malloc'"'"', \
				'"'"'_sidhjs_init'"'"', \
				'"'"'_sidhjs_keypair'"'"', \
				'"'"'_sidhjs_encrypt'"'"', \
				'"'"'_sidhjs_decrypt'"'"', \
				'"'"'_sidhjs_public_key_bytes'"'"', \
				'"'"'_sidhjs_private_key_bytes'"'"', \
				'"'"'_sidhjs_encrypted_bytes'"'"', \
				'"'"'_sidhjs_decrypted_bytes'"'"' \
			]\" \
		" | perl -pe "s/\s+/ /g" | perl -pe "s/\[ /\[/g" | perl -pe "s/ \]/\]/g")"; \
		\
		bash -c "emcc -Oz -s WASM=0 -s RUNNING_JS_OPTS=1 $$args -o dist/sidh.asm.js"; \
		bash -c "emcc -O3 -s WASM=1 $$args -o dist/sidh.wasm.js"; \
	'

	cp pre.js dist/sidh.tmp.js
	echo " \
		var Module = {}; \
		Module.ready = new Promise(function (resolve, reject) { \
			var Module = {}; \
			Module.onAbort = reject; \
			Module.onRuntimeInitialized = function () { \
				try { \
					Module._sidhjs_public_key_bytes(); \
					resolve(Module); \
				} \
				catch (err) { \
					reject(err); \
				} \
			}; \
	" >> dist/sidh.tmp.js
	cat dist/sidh.wasm.js >> dist/sidh.tmp.js
	echo " \
		}).catch(function () { \
			var Module = {}; \
	" >> dist/sidh.tmp.js
	cat dist/sidh.asm.js >> dist/sidh.tmp.js
	echo " \
			return new Promise(function (resolve, reject) { \
				Module.onAbort = reject; \
				Module.onRuntimeInitialized = function () { resolve(Module); }; \
			}); \
		}).then(function (m) { \
			Object.keys(m).forEach(function (k) { Module[k] = m[k]; }); \
		}); \
	" >> dist/sidh.tmp.js
	cat post.js >> dist/sidh.tmp.js

	terser dist/sidh.tmp.js -cmo dist/sidh.js

	sed -i 's|use asm||g' dist/sidh.js
	sed -i 's|require(|eval("require")(|g' dist/sidh.js

	rm -rf SIDH libsodium dist/sidh.*.js

clean:
	rm -rf dist SIDH libsodium
