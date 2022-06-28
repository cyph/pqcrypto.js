all:
	make clean

	webpack --mode none --output-library-target var --output-library superSphincs index.js -o dist/index.js

	echo " \
		if (typeof module !== 'undefined' && module.exports) { \
			module.exports		= superSphincs; \
		} \
		else { \
			self.superSphincs	= superSphincs; \
		} \
	" >> dist/index.js
	terser dist/index.js -cmo dist/index.js

	chmod -R 777 dist

clean:
	rm -rf dist 2> /dev/null
	mkdir dist
