all:
	make clean

	patch -o dist/rsa-variant.js index.js rsa-variant.js.patch
	patch -o dist/rsa-variant.d.ts index.d.ts rsa-variant.d.ts.patch
	npx webpack --config webpack.js
	npx webpack --config rsa-variant.webpack.js

	echo " \
		if (typeof module !== 'undefined' && module.exports) { \
			module.exports		= superDilithium; \
		} \
		else { \
			self.superDilithium	= superDilithium; \
		} \
	" >> dist/index.js

	npx terser dist/index.js -cmo dist/index.js
	npx terser dist/rsa-variant.bundle.js -cmo dist/rsa-variant.bundle.js

	chmod -R 777 dist

clean:
	rm -rf dist 2> /dev/null
	mkdir dist

test:
	npm run test
