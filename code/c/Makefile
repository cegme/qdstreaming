
GPP = g++ --std=c++11 -march=native -w -O3
DEBUG = -g -pg 

#THRIFT = -I/usr/local/include/thrift
#THRIFT = -L/usr/local/lib/libthriftnb.a -L/usr/local/lib/libthrift.a -L/usr/local/lib/libthriftz -lthrift -lthriftnb
#THRIFT = -L/usr/local/lib -lthrift -lthriftnb -lthrift_c_glib -lthriftz  -lz
#THRIFT = -L/usr/local/lib -lz -lthriftnb -lthrift -lthrift_c_glib -levent -fPIC
#THRIFT = -L/usr/local/lib -L/home/cgrant/scratch/thrift/lib -L/usr/local/include/thrift -lz -lthriftz -lthriftnb -lthrift -lthrift_c_glib -levent -fPIC
THRIFT = -L/usr/local/lib -lz -lthriftz -lthriftnb -lthrift -lthrift_c_glib -levent -fPIC
SQLITE = -lsqlite3

CPP = $(GPP) $(DEBUG)

BOOST = -lboost_date_time -lboost_program_options -lboost_system -lboost_filesystem -lboost_regex -lboost_iostreams -lboost_thread


DLIB = -I/home/cgrant/projects/dlib/ -I/home/cgrant/projects/dlib/dlib/all/source.cpp

EXTRAS = -lX11 -lfl -lssl -lcrypto -lrt -lpthread -DHAVE_NETINET_IN_H -DHAVE_INTTYPES_H $(BOOST) $(THRIFT) $(SQLITE) $(DLIB)


all: wikilinkstats


experiment_bsa2015: wikilink experiment_bsa2015.o Util.o Entity.o Random.o ER.o WikiLinkUtil.o wikilinkstats
	$(CPP) -o experiment_bsa2015 experiment_bsa2015.cpp Util.o Entity.o Random.o ER.o $(EXTRAS)


experiment_bsa2015.o: experiment_bsa2015.cpp
	$(CPP) -c experiment_bsa2015.cpp $(EXTRAS)


WikiLinkUtil.o: WikiLinkUtil.hpp
	$(CPP) -c WikiLinkUtil.hpp $(EXTRAS)

opttest: Optimizer.o
	$(CPP) -o opttest Optimizer.o $(EXTRAS)


# ER
er: wikilink_constants.o ER.o Util.o Entity.o
	$(CPP) -o er wikilink_constants.o ER.o Util.o Entity.o $(EXTRAS)

compression_test: wikilink_constants.o compression_test.o Entity.o Random.o
	$(CPP) -o compression_test wikilink_constants.o compression_test.o Entity.o Random.o hyperloglog.hpp $(EXTRAS)

Optimizer.o: Optimizer.cpp
	$(CPP) -c Optimizer.cpp $(EXTRAS)

ER.o: ER.cpp
	$(CPP) -c ER.cpp $(EXTRAS)

# Wikilink Stats
wikilinkstats: wikilink wikilinkstats.o wikilink_types.o wikilink_constants.o WikiLinkFile.o Util.o
	$(CPP) -o wikilinkstats wikilinkstats.o wikilink_constants.o wikilink_types.o WikiLinkFile.o Util.o $(EXTRAS) 

wikilinkstats.o: wikilinkstats.cpp
	$(CPP) -c wikilinkstats.cpp $(EXTRAS)

wikilink:
	thrift -r --gen cpp wikilink.thrift

wikilink_constants.o: wikilink
	$(CPP) -c gen-cpp/wikilink_constants.cpp -o wikilink_constants.o $(EXTRAS)

wikilink_types.o:	
	$(CPP) -c gen-cpp/wikilink_types.cpp -o wikilink_types.o $(EXTRAS)

WikiLinkFile.o: WikiLinkFile.cpp
	$(CPP) -c WikiLinkFile.cpp $(EXTRAS)
	
Entity.o: Entity.cpp
	$(CPP) -c Entity.cpp $(EXTRAS)

compression_test.o: compression_test.cpp 
	$(CPP) -c compression_test.cpp $(EXTRAS)

hyperloglog.o: hyperloglog.hpp
	$(CPP) -c hyperloglog.hpp $(EXTRAS)


# Cluster test

clustertest: clustertest.o Random.o Util.o
	$(CPP) -o clustertest clustertest.o Random.o Util.o $(EXTRAS)

clustertest.o: clustertest.cpp
	$(CPP) -c clustertest.cpp $(EXTRAS)

Random.o: Random.cc
	$(CPP) -c Random.cc $(EXTRAS)

Util.o: Util.cpp
	$(CPP) -c Util.cpp $(EXTRAS)

clean:
	rm -rf gen-cpp *.o



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

