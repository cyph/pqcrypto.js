all:
	make clean

	webpack --mode none --output-library-target var --output-library superSphincs supersphincs.js -o dist/supersphincs.js

	echo " \
		if (typeof module !== 'undefined' && module.exports) { \
			module.exports		= superSphincs; \
		} \
		else { \
			self.superSphincs	= superSphincs; \
		} \
	" >> dist/supersphincs.js
	terser dist/supersphincs.js -cmo dist/supersphincs.js

	chmod -R 777 dist

clean:
	rm -rf dist 2> /dev/null
	mkdir dist
