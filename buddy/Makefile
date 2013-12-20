all: libs src test

libs:
	cd libsrc
	make install
	cd ..

src:
	cd src
	make install
	cd ..

test:
	gcc -g -Wall -o buddy-test buddy-test.o
	gcc -g -Wall -o buddy-unit-test buddy-unit-test.o
	gcc -g -Wall -o malloc-test malloc-test.o

testclean:
	rm --force buddy-test.o buddy-test buddy-unit-test buddy-unit-test.o malloc-test malloc-test.o

clean: testclean
	cd libsrc
	make installclean
	cd ..
	cd src
	make installclean
	cd ..
