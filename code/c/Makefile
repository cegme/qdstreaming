
GPP = g++ --std=c++11 -march=native -w  
DEBUG = -g -pg

CPP = $(GPP) $(DEBUG)

EXTRAS = -lfl -lboost_date_time -lboost_program_options -lboost_system -lpthread -lboost_filesystem -lboost_regex -DHAVE_NETINET_IN_H -DHAVE_INTTYPES_H

clustertest: Random.o
	$(CPP) -o clustertest clustertest.cpp Random.o $(EXTRAS)

Random.o: Random.cc
	$(CPP) -c Random.cc

clean:
	rm clustertest
	rm Random.o


matrxtest_debug: 
	gcc FileBackedMatrix.c -g -lm

matrxtest_all:
	gcc FileBackedMatrix.c -O3 -march=native -lm

matrxtest_clean:
	rm a.out

cp2tmp:
	cp FileBackedMatrix.c /tmp/
	cp Makefile /tmp/
	cp matrixtest.sh /tmp/
	cp Util.h /tmp/
