all:
	rm -rf dist node_modules package-lock.json 2> /dev/null
	mkdir dist node_modules

	npm install

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

	rm -rf node_modules package-lock.json

clean:
	rm -rf dist node_modules package-lock.json
