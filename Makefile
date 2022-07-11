all:
	for d in packages/* ; do cd $${d} ; make || exit 1 ; cd - ; done

clean:
	for d in packages/* ; do cd $${d} ; make clean || exit 1 ; cd - ; done

test:
	for d in packages/* ; do cd $${d} ; make test || exit 1 ; cd - ; done
