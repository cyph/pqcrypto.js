SHELL := /bin/bash

all:
	rm -rf dist tmp 2> /dev/null
	mkdir dist tmp

	wget https://raw.githubusercontent.com/jedisct1/libsodium.js/d4fcfd5/wrapper/wrap-template.js -O tmp/wrap-template.js
	if [ "$$(shasum -a 512 tmp/wrap-template.js | awk '{print $$1}')" != '733340b46210573257785dc749e86e14ad9d1bda68206e94182266ee8a4b84bbf7d71f008c9a26fc9dbfe6de40ae07bc5646c108066aed6d9781f064190fcf62' ] ; then exit 1 ; fi

	cat pre.js > tmp/sodiumutil.js
	cat tmp/wrap-template.js | tr '\n' '☁' | perl -pe 's/.*Codecs(.*?)Memory management.*/\1/g' | tr '☁' '\n' >> tmp/sodiumutil.js
	echo >> tmp/sodiumutil.js
	cat tmp/wrap-template.js | tr '\n' ' ' | perl -pe 's/\s+/ /g' | perl -pe 's/.*(function memcmp.*?)\s+function.*/\1/g' >> tmp/sodiumutil.js
	echo >> tmp/sodiumutil.js
	cat tmp/wrap-template.js | tr '\n' ' ' | perl -pe 's/\s+/ /g' | perl -pe 's/.*(function memzero.*?)\s+function.*/\1/g' >> tmp/sodiumutil.js
	echo >> tmp/sodiumutil.js
	cat post.js >> tmp/sodiumutil.js

	uglifyjs tmp/sodiumutil.js -cmo dist/sodiumutil.js

	rm -rf tmp

clean:
	rm -rf dist tmp
