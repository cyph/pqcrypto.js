all:
	for d in packages/* ; do cd $${d} ; make || exit 1 ; cd - ; done

clean:
	for d in packages/* ; do cd $${d} ; make clean || exit 1 ; cd - ; done

publish:
	for d in packages/* ; do cd $${d} ; npm publish || exit 1 ; cd - ; done

test:
	for d in packages/* ; do cd $${d} ; make test || exit 1 ; cd - ; done

updatelibs:
	git submodule update --recursive --remote

	for d in packages/* ; do \
		cd $${d}; \
		rm -rf node_modules package-lock.json; \
		npm install || exit 1; \
		cd -; \
	done
