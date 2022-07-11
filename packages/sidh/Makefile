all:
	make clean

	bash -c ' \
		args="$$(echo " \
			--memory-init-file 0 \
			-s SINGLE_FILE=1 \
			-DCYPHERTEXT_LEN=123 \
			-D_GENERIC_ -D__NIX__ -D_X86_ \
			-s TOTAL_MEMORY=16777216 -s TOTAL_STACK=8388608 \
			-s ASSERTIONS=0 \
			-s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
			-s ALIASING_FUNCTION_POINTERS=1 \
			-s DISABLE_EXCEPTION_CATCHING=1 \
			-s NO_FILESYSTEM=1 \
			-s ERROR_ON_UNDEFINED_SYMBOLS=0 \
			-Ilibsodium/src/libsodium/include \
			-Ilibsodium/src/libsodium/include/sodium \
			-Isidh/src/P503 \
			$$(find libsodium/src/libsodium -type f -name '\\*.c') \
			sidh/src/sha3/*.c \
			sidh/src/P503/generic/*.c \
			sidh/src/P503/P503.c \
			main.c \
			-s EXPORTED_RUNTIME_METHODS=\"[ \
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
		bash -c "emcc -Oz -s WASM=0 $$args -o dist/index.asm.js"; \
		bash -c "emcc -O3 -s WASM=1 $$args -o dist/index.wasm.js"; \
	'

	cp pre.js dist/index.tmp.js
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

	rm -rf dist/index.*.js
	chmod -R 777 dist

clean:
	rm -rf dist 2> /dev/null
	mkdir dist

test:
	npm run test
