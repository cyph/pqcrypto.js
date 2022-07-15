all:
	make clean

	npx webpack --config webpack.js

	echo " \
		if (typeof module !== 'undefined' && module.exports) { \
			module.exports		= superDilithium; \
		} \
		else { \
			self.superDilithium	= superDilithium; \
		} \
	" >> dist/index.js
	npx terser dist/index.js -cmo dist/index.js

	chmod -R 777 dist

clean:
	rm -rf dist 2> /dev/null
	mkdir dist

test:
	npm run test
