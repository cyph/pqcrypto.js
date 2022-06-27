all:
	rm -rf dist c_src erlang-binding libsodium openssl supercop* 2> /dev/null
	mkdir dist

	git clone --depth 1 -b stable https://github.com/jedisct1/libsodium
	cd libsodium ; emconfigure ./configure --enable-minimal --disable-shared

	git clone --depth 1 -b OpenSSL_1_0_2-stable https://github.com/openssl/openssl
	cd openssl ; emconfigure ./config no-asm no-threads no-shared no-dso no-sse2

	wget https://bench.cr.yp.to/supercop/supercop-20191221.tar.xz
	unxz < supercop-20191221.tar.xz | tar -xf -
	mv supercop-20191221 c_src
	rm supercop-20191221.tar.xz

	sed -i 's|crypto_hash|crypto_hash_blake256_ref|g' c_src/crypto_hash/blake256/ref/hash.c
	sed -i 's|sigma|sigma_blake256_ref|g' c_src/crypto_hash/blake256/ref/hash.c
	sed -i 's|crypto_hash_blake256_ref.h|crypto_hash.h|g' c_src/crypto_hash/blake256/ref/hash.c
	sed -i 's|cst|cst_blake256_ref|g' c_src/crypto_hash/blake256/ref/hash.c

	sed -i 's|crypto_hash|crypto_hash_blake512_ref|g' c_src/crypto_hash/blake512/ref/hash.c
	sed -i 's|sigma|sigma_blake512_ref|g' c_src/crypto_hash/blake512/ref/hash.c
	sed -i 's|crypto_hash_blake512_ref.h|crypto_hash.h|g' c_src/crypto_hash/blake512/ref/hash.c
	sed -i 's|cst|cst_blake512_ref|g' c_src/crypto_hash/blake512/ref/hash.c

	sed -i 's|crypto_sign|crypto_sign_sphincs|g' c_src/crypto_sign/sphincs256/ref/sign.c
	sed -i 's|crypto_sign_sphincs.h|crypto_sign.h|g' c_src/crypto_sign/sphincs256/ref/sign.c

	git clone --depth 1 https://github.com/ahf/sphincs erlang-binding
	mv erlang-binding/c_src/crypto_core/include/* c_src/include/
	mv erlang-binding/c_src/crypto_stream/chacha12/ref/api.c c_src/crypto_stream/chacha12/e/ref/stream.c
	rm -rf erlang-binding

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
			-Iopenssl/include \
			-Ic_src -Ic_src/include -Ic_src/crypto_stream/chacha12/e/ref \
			-include c_src/crypto_stream/chacha12/e/ref/namespace.h \
			libsodium/src/libsodium/randombytes/randombytes.c \
			c_src/crypto_hash/blake256/ref/hash.c \
			c_src/crypto_hash/blake512/ref/hash.c \
			c_src/crypto_stream/chacha12/e/ref/stream.c \
			c_src/crypto_stream/chacha12/e/ref/chacha.c \
			$$(find c_src/crypto_sign/sphincs256/ref -name '"'"'*.c'"'"' -type f) \
			sphincs.c \
			-s EXTRA_EXPORTED_RUNTIME_METHODS=\"[ \
				'"'"'writeArrayToMemory'"'"' \
			]\" \
			-s EXPORTED_FUNCTIONS=\"[ \
				'"'"'_free'"'"', \
				'"'"'_malloc'"'"', \
				'"'"'_sphincsjs_init'"'"', \
				'"'"'_sphincsjs_keypair'"'"', \
				'"'"'_sphincsjs_sign'"'"', \
				'"'"'_sphincsjs_open'"'"', \
				'"'"'_sphincsjs_public_key_bytes'"'"', \
				'"'"'_sphincsjs_secret_key_bytes'"'"', \
				'"'"'_sphincsjs_signature_bytes'"'"' \
			]\" \
		" | perl -pe "s/\s+/ /g" | perl -pe "s/\[ /\[/g" | perl -pe "s/ \]/\]/g")"; \
		\
		bash -c "emcc -Oz -s WASM=0 -s RUNNING_JS_OPTS=1 $$args -o dist/sphincs.asm.js"; \
		bash -c "emcc -O3 -s WASM=1 $$args -o dist/sphincs.wasm.js"; \
	'

	cp pre.js dist/sphincs.tmp.js
	echo " \
		var Module = {}; \
		Module.ready = new Promise(function (resolve, reject) { \
			var Module = {}; \
			Module.onAbort = reject; \
			Module.onRuntimeInitialized = function () { \
				try { \
					Module._sphincsjs_public_key_bytes(); \
					resolve(Module); \
				} \
				catch (err) { \
					reject(err); \
				} \
			}; \
	" >> dist/sphincs.tmp.js
	cat dist/sphincs.wasm.js >> dist/sphincs.tmp.js
	echo " \
		}).catch(function () { \
			var Module = {}; \
	" >> dist/sphincs.tmp.js
	cat dist/sphincs.asm.js >> dist/sphincs.tmp.js
	echo " \
			return new Promise(function (resolve, reject) { \
				Module.onAbort = reject; \
				Module.onRuntimeInitialized = function () { resolve(Module); }; \
			}); \
		}).then(function (m) { \
			Object.keys(m).forEach(function (k) { Module[k] = m[k]; }); \
		}); \
	" >> dist/sphincs.tmp.js
	cat post.js >> dist/sphincs.tmp.js

	terser dist/sphincs.tmp.js -cmo dist/sphincs.js

	sed -i 's|use asm||g' dist/sphincs.js
	sed -i 's|require(|eval("require")(|g' dist/sphincs.js

	rm -rf c_src libsodium openssl dist/sphincs.*.js

clean:
	rm -rf dist c_src erlang-binding libsodium openssl supercop*
