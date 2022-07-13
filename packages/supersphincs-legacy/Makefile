all:
	make clean

	cp nacl-sha512.js dist/
	patch -o dist/old-api.js index.js old-api.js.patch
	patch -o dist/old-api.d.ts index.d.ts old-api.d.ts.patch
	npx webpack --config webpack.js
	npx webpack --config old-api.webpack.js

	echo " \
		if (typeof module !== 'undefined' && module.exports) { \
			module.exports		= superSphincs; \
		} \
		else { \
			self.superSphincs	= superSphincs; \
		} \
	" >> dist/index.js
	npx terser dist/index.js -cmo dist/index.js

	chmod -R 777 dist

clean:
	rm -rf dist 2> /dev/null
	mkdir dist

test:
	npm run test
