all:
	rm -rf dist tmp 2> /dev/null
	mkdir dist tmp

	sh -c 'cd ../../lib/libsodium.js ; git show d4fcfd5:wrapper/wrap-template.js' > tmp/wrap-template.js

	cat pre.js > tmp/index.js
	cat tmp/wrap-template.js | tr '\n' '☁' | perl -pe 's/.*Codecs(.*?)Memory management.*/\1/g' | tr '☁' '\n' >> tmp/index.js
	echo >> tmp/index.js
	cat tmp/wrap-template.js | tr '\n' ' ' | perl -pe 's/\s+/ /g' | perl -pe 's/.*(function memcmp.*?)\s+function.*/\1/g' >> tmp/index.js
	echo >> tmp/index.js
	cat tmp/wrap-template.js | tr '\n' ' ' | perl -pe 's/\s+/ /g' | perl -pe 's/.*(function memzero.*?)\s+function.*/\1/g' >> tmp/index.js
	echo >> tmp/index.js
	cat post.js >> tmp/index.js

	npx terser tmp/index.js -cmo dist/index.js
	cat pre.js module.js post.js | npx terser -cmo dist/sodium-wrapper.js

	rm -rf tmp

clean:
	rm -rf dist tmp

test:
	echo
