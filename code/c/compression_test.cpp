
#include "Entity.h"

#include "Random.h"
#include "Util.h"

#include <boost/program_options.hpp>

/**
  * This code is to test the compression of entities.
  * The objective is to use synthetic data to create
  * the following graphs:
  *
  * (1) x-axis: entity size, y-axis: time to compression
  * (2) x-axis: entity size, y-axis: time to insertion, time to deletion (compressed and uncompressed)
  *
  * To create this graph we will create an entity of increasing size and for each
  * size perform and time the following operations:
  *
  * (1) compress, decompress
  * (2) insert, remove  (on both compressed and decompressed data)
  *
  *
  *
  * Usage: make compression_test
  *       ./compression_test | tee compression_test.csv
  */





int main (int argc, char** argv) {
  namespace po = boost::program_options;

  // An array with sizes increasing to 100K
  std::vector<unsigned int> entity_sizes = {10, 100, 5000, 7000, 10000, 50000, 70000, 100000, 500000, 700000, 1000000};
  std::vector<float> cardinality_percentage = {0.2, 0.4, 0.6, 0.8};

  unsigned int insertions = 100000;

  clock_t tic, toc;
  std::cout << "Entity Size,Cardinality,Insertion time("<< insertions << "),"
            << "Compression Time,Size Uncompressed (bytes),Size Compressed (bytes)\n";

  for (auto &es: entity_sizes) {
    long instime = 0L, comprtime = 0L;
    unsigned int bytes_compressed = 0L, bytes_uncompressed  = 0L;

    // Different cardinality sizes
    for (auto &c: cardinality_percentage) {

      { // Time the insertions
        auto  e = dsr::Entity::buildEntity(es, c * es);
        clock_t tic = clock();
        for (auto iter = 0; iter < insertions; ++iter) {
          auto val = RandInt();
          e.add(val);
        }
        clock_t toc = clock();
        instime = toc - tic;
      }

      { // The time to compression
        auto  e = dsr::Entity::buildEntity(es, c * es);
        bytes_compressed =  e.bytes();
        clock_t tic = clock();
        e.compress();
        clock_t toc = clock();
        comprtime = toc - tic;
        bytes_uncompressed =  e.bytes();
      }

      std::cout << es << "," << c << "," 
                << instime << "," << comprtime << ","
                << bytes_uncompressed << "," << bytes_compressed << std::endl; 
    }
  }





  return 0;
}






