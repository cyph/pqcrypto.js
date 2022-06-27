all:
	rm -rf dist tmp 2> /dev/null
	mkdir dist tmp

	cp pre.js tmp/pre.js
	echo >> tmp/pre.js

	curl -s https://raw.githubusercontent.com/cyph/sodiumutil/master/dist/sodiumutil.js >> tmp/pre.js

	node -e 'fs.writeFileSync("tmp/post.js", fs.readFileSync("post.js").toString().replace("DEFAULT_WORD_LIST", JSON.stringify(fs.readFileSync("word-list.txt").toString().trim().split("\n"))))'

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
			xkcd-passphrase.c \
			-s EXTRA_EXPORTED_RUNTIME_METHODS=\"[ \
				'"'"'UTF8ToString'"'"', \
				'"'"'writeAsciiToMemory'"'"' \
			]\" \
			-s EXPORTED_FUNCTIONS=\"[ \
				'"'"'_asm_test'"'"', \
				'"'"'_free'"'"', \
				'"'"'_malloc'"'"', \
				'"'"'_generate'"'"' \
			]\" \
		" | perl -pe "s/\s+/ /g" | perl -pe "s/\[ /\[/g" | perl -pe "s/ \]/\]/g")"; \
		\
		bash -c "emcc -Oz -s WASM=0 -s RUNNING_JS_OPTS=1 $$args -o tmp/xkcd-passphrase.asm.js"; \
		bash -c "emcc -O3 -s WASM=1 $$args -o tmp/xkcd-passphrase.wasm.js"; \
	'

	cp tmp/pre.js tmp/xkcd-passphrase.js
	echo " \
		var Module = {}; \
		Module.ready = new Promise(function (resolve, reject) { \
			var Module = {}; \
			Module.onAbort = reject; \
			Module.onRuntimeInitialized = function () { \
				try { \
					Module._asm_test(); \
					resolve(Module); \
				} \
				catch (err) { \
					reject(err); \
				} \
			}; \
	" >> tmp/xkcd-passphrase.js
	cat tmp/xkcd-passphrase.wasm.js >> tmp/xkcd-passphrase.js
	echo " \
		}).catch(function () { \
			var Module = {}; \
	" >> tmp/xkcd-passphrase.js
	cat tmp/xkcd-passphrase.asm.js >> tmp/xkcd-passphrase.js
	echo " \
			return new Promise(function (resolve, reject) { \
				Module.onAbort = reject; \
				Module.onRuntimeInitialized = function () { resolve(Module); }; \
			}); \
		}).then(function (m) { \
			Object.keys(m).forEach(function (k) { Module[k] = m[k]; }); \
		}); \
	" >> tmp/xkcd-passphrase.js
	cat tmp/post.js >> tmp/xkcd-passphrase.js

	terser tmp/xkcd-passphrase.js -cmo dist/xkcd-passphrase.js

	sed -i 's|use asm||g' dist/xkcd-passphrase.js
	sed -i 's|require(|eval("require")(|g' dist/xkcd-passphrase.js

	cat dist/xkcd-passphrase.js | perl -pe 's/defaultWordList:.*?],/defaultWordList:\[\],/g' \
		> dist/xkcd-passphrase.slim.js

	rm -rf tmp

clean:
	rm -rf dist tmp
