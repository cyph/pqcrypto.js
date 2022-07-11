all:
	make clean
	mkdir tmp

	cp pre.js tmp/pre.js
	echo >> tmp/pre.js

	cat ../../lib/sodiumutil/dist/sodiumutil.js >> tmp/pre.js

	# Temporarily using /usr/bin/node as workaround for emscripten
	/usr/bin/node -e 'fs.writeFileSync("tmp/post.js", fs.readFileSync("post.js").toString().replace("DEFAULT_WORD_LIST", JSON.stringify(fs.readFileSync("word-list.txt").toString().trim().split("\n"))))'

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
			main.c \
			-s EXPORTED_RUNTIME_METHODS=\"[ \
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
		bash -c "emcc -Oz -s WASM=0 $$args -o tmp/index.asm.js"; \
		bash -c "emcc -O3 -s WASM=1 $$args -o tmp/index.wasm.js"; \
	'

	npx babel --no-babelrc --presets @babel/preset-env tmp/index.asm.js -o tmp/index.asm.js
	npx babel --no-babelrc --presets @babel/preset-env tmp/index.wasm.js -o tmp/index.wasm.js

	cp tmp/pre.js tmp/index.js
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
	" >> tmp/index.js
	cat tmp/index.wasm.js >> tmp/index.js
	echo " \
		}).catch(function () { \
			var Module = {}; \
	" >> tmp/index.js
	cat tmp/index.asm.js >> tmp/index.js
	echo " \
			return new Promise(function (resolve, reject) { \
				Module.onAbort = reject; \
				Module.onRuntimeInitialized = function () { resolve(Module); }; \
			}); \
		}).then(function (m) { \
			Object.keys(m).forEach(function (k) { Module[k] = m[k]; }); \
		}); \
	" >> tmp/index.js
	cat tmp/post.js >> tmp/index.js

	npx terser tmp/index.js -cmo dist/index.js

	sed -i 's|use asm||g' dist/index.js
	sed -i 's|require(|eval("require")(|g' dist/index.js

	cat dist/index.js | perl -pe 's/defaultWordList:.*?],/defaultWordList:\[\],/g' \
		> dist/index.slim.js

	rm -rf tmp
	chmod -R 777 dist

clean:
	rm -rf dist tmp 2> /dev/null
	mkdir dist

test:
	echo
