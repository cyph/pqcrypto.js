all:
	for d in $$(ls packages/*/Makefile | sed 's|/Makefile||g') ; do \
		cd $${d}; \
		make || exit 1; \
		cd -; \
	done

clean:
	for d in $$(ls packages/*/Makefile | sed 's|/Makefile||g') ; do \
		cd $${d}; \
		make clean || exit 1; \
		cd -; \
	done

publish:
	for d in $$(ls packages/*/package.json | sed 's|/package.json||g') ; do \
		cd $${d}; \
		npm publish || exit 1; \
		cd -; \
	done

test:
	for d in $$(ls packages/*/Makefile | sed 's|/Makefile||g') ; do \
		cd $${d}; \
		make test || exit 1; \
		cd -; \
	done

updatelibs:
	grep 'path =' .gitmodules | \
		awk '{print $$3}' | \
		grep -vP '^lib/legacy/' | \
		xargs git submodule update --remote

	for d in $$(ls packages/*/package.json | sed 's|/package.json||g') ; do \
		cd $${d}; \
		rm -rf node_modules package-lock.json; \
		npm install || exit 1; \
		cd -; \
	done
