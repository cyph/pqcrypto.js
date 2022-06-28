all:
	for d in packages/* ; do cd $${d} ; make || exit 1 ; cd - ; done

clean:
	for d in packages/* ; do cd $${d} ; make clean || exit 1 ; cd - ; done
