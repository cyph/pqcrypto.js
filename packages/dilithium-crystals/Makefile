all:
	make clean

	bash -c ' \
		args="$$(echo " \
			--memory-init-file 0 \
			-s SINGLE_FILE=1 \
			-s TOTAL_MEMORY=16777216 -s TOTAL_STACK=8388608 \
			-s ASSERTIONS=0 \
			-s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
			-s ALIASING_FUNCTION_POINTERS=1 \
			-s DISABLE_EXCEPTION_CATCHING=1 \
			-s NO_FILESYSTEM=1 \
			-s ERROR_ON_UNDEFINED_SYMBOLS=0 \
			-Ilibsodium/src/libsodium/include/sodium \
			-Ipqclean/common \
			-Ipqclean/crypto_sign/dilithium5/clean \
			libsodium/src/libsodium/randombytes/randombytes.c \
			$$( \
				find \
					pqclean/common \
					pqclean/crypto_sign/dilithium5/clean \
				-name '"'"'*.c'"'"' -type f \
				-not \( \
					-path '*/keccak4x/*' -or \
					-name randombytes.c \
				\) \
			) \
			main.c \
			-s EXPORTED_RUNTIME_METHODS=\"[ \
				'"'"'writeArrayToMemory'"'"' \
			]\" \
			-s EXPORTED_FUNCTIONS=\"[ \
				'"'"'_free'"'"', \
				'"'"'_malloc'"'"', \
				'"'"'_dilithiumjs_init'"'"', \
				'"'"'_dilithiumjs_keypair'"'"', \
				'"'"'_dilithiumjs_sign'"'"', \
				'"'"'_dilithiumjs_verify'"'"', \
				'"'"'_dilithiumjs_public_key_bytes'"'"', \
				'"'"'_dilithiumjs_secret_key_bytes'"'"', \
				'"'"'_dilithiumjs_signature_bytes'"'"' \
			]\" \
		" | perl -pe "s/\s+/ /g" | perl -pe "s/\[ /\[/g" | perl -pe "s/ \]/\]/g")"; \
		\
		bash -c "emcc -Oz -s WASM=0 $$args -o dist/index.asm.js"; \
		bash -c "emcc -O3 -s WASM=1 $$args -o dist/index.wasm.js"; \
	'

	npx babel --no-babelrc --presets @babel/preset-env dist/index.asm.js -o dist/index.asm.js
	npx babel --no-babelrc --presets @babel/preset-env dist/index.wasm.js -o dist/index.wasm.js

	cp pre.js dist/index.tmp.js
	echo " \
		var Module = {}; \
		Module.ready = new Promise(function (resolve, reject) { \
			var Module = {}; \
			Module.onAbort = reject; \
			Module.onRuntimeInitialized = function () { \
				try { \
					Module._dilithiumjs_public_key_bytes(); \
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

	npx terser dist/index.tmp.js -cmo dist/index.js

	sed -i 's|use asm||g' dist/index.js
	sed -i 's|require(|eval("require")(|g' dist/index.js

	rm -rf dist/index.*.js
	chmod -R 777 dist

clean:
	rm -rf dist 2> /dev/null
	mkdir dist

test:
	npm run test
