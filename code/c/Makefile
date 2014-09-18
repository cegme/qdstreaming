
GPP = g++ --std=c++11 -march=native -w -O3 
DEBUG = -g -pg 

#THRIFT = -I/usr/local/include/thrift
#THRIFT = -L/usr/local/lib/libthriftnb.a -L/usr/local/lib/libthrift.a -L/usr/local/lib/libthriftz -lthrift -lthriftnb
#THRIFT = -L/usr/local/lib -lthrift -lthriftnb -lthrift_c_glib -lthriftz  -lz
#THRIFT = -L/usr/local/lib -lz -lthriftnb -lthrift -lthrift_c_glib -levent -fPIC
THRIFT = -L/usr/local/lib -L/home/cgrant/scratch/thrift/lib -L/usr/local/include/thrift -lz -lthriftz -lthriftnb -lthrift -lthrift_c_glib -levent -fPIC

CPP = $(GPP) $(DEBUG)

BOOST = -lboost_date_time -lboost_program_options -lboost_system -lboost_filesystem -lboost_regex -lboost_iostreams -lboost_thread

EXTRAS = -lfl -lssl -lcrypto -lrt -lpthread -DHAVE_NETINET_IN_H -DHAVE_INTTYPES_H $(BOOST) $(THRIFT)


# Wikilink Stats
wikilinkstats: Util.o wiki-link.o WikiLinkFile.o 
	$(CPP) -o wikilinkstats wikilinkstats.cpp wikilink_constants.o wikilink_types.o WikiLinkFile.o $(EXTRAS) 

wiki-link.o:	
	thrift -r --gen cpp wikilink.thrift
	$(CPP) -c gen-cpp/wikilink_constants.cpp -o wikilink_constants.o $(EXTRAS)
	$(CPP) -c gen-cpp/wikilink_types.cpp -o wikilink_types.o $(EXTRAS)

WikiLinkFile.o: WikiLinkFile.cpp
	$(CPP) -c WikiLinkFile.cpp -o WikiLinkFile.o $(EXTRAS)
	


# Cluster test

clustertest: Random.o
	$(CPP) -o clustertest clustertest.cpp Random.o $(EXTRAS)

Random.o: Random.cc
	$(CPP) -c Random.cc

Util.o: Util.cpp
	$(CPP) -c Util.cpp

clean:
	rm -rf gen-cpp
	rm clustertest
	rm Random.o




# Matrix test

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
