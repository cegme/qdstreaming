
debug: 
	gcc FileBackedMatrix.c -g -lm

all:
	gcc FileBackedMatrix.c -O3 -march=native -lm

clean:
	rm a.out

cp2tmp:
	cp FileBackedMatrix.c /tmp/
	cp Makefile /tmp/
	cp matrixtest.sh /tmp/
	cp Util.h /tmp/
