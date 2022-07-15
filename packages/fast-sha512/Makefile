all:
	make clean

	cp nacl-sha512.js dist/
	npx webpack --config webpack.js

	echo " \
		if (typeof module !== 'undefined' && module.exports) { \
			module.exports	= fastSHA512; \
		} \
		else { \
			self.fastSHA512	= fastSHA512; \
		} \
	" >> dist/index.js
	npx terser dist/index.js -cmo dist/index.js

	chmod -R 777 dist

clean:
	rm -rf dist 2> /dev/null
	mkdir dist

test:
	echo
