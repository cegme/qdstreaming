
#include "Random.h"
#include "Util.h"

#include <string>
#include <unordered_map>
#include <vector>

#include <boost/program_options.hpp>

enum Algo {ALL = 0, BASELINE = 1, SORTED = 2, TOPK = 3};

struct point {
  std::vector<int> x;
  point(int dim): x(std::vector<int>()) {
    for (int i = 0; i < dim; ++i) {
      x.push_back(RandInt());
    }
  }
};

void create_cluster(std::vector<point>& a, int size, int dimensions) {

  // Clear old cluster
  a.clear();

  for (int i = 0; i < size; ++i) {
    // Call the random constructor
    a.push_back(point(dimensions));
    
  }

}

long baseline(std::vector<point>* a,
                    std::vector<point>* b) {

  clock_t tic = clock();

  size_t asize = a->size();
  size_t bsize = b->size();
  for (size_t i = 0; i != asize; ++i) {
    for (size_t j = 0; j != bsize; ++j) {
    } 
  }
  
  clock_t toc = clock();
  return toc - tic;
  
}

// TODO create statistis collections method


int main (int argc, char** argv) {
  namespace po = boost::program_options;

  int iterations;
  int dimensions;
  int algo; 
  int sizes[6] = { 10, 100, 1000, 10000, 1000000, 10000000 };
  
  std::unordered_map<std::string, std::vector<long> > timer_map;

  boost::program_options::options_description desc("Cluster Improvement test.");
  desc.add_options()
    ("help,h", "Print a helpul help message")
    ("dimension,d", boost::program_options::value<int>(&dimensions)->default_value(2),
       "The dimensions of the points")
    ("algorithms,a", boost::program_options::value<int>(&algo)->default_value((int)Algo::ALL),
       "Choose the algorithm to run, all is default, Choose values 1-3");
    ("iterations,i", boost::program_options::value<int>(&iterations)->default_value(100), "Iterations for each algo");

  boost::program_options::variables_map vm;
  try {
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);
    if (vm.count("help") ) {
      logInfo(desc);
      exit(0);
    }

  }
  catch (boost::program_options::error &e) {
    log_err("Bad parameters");
    logInfo(desc);
    exit(1);
  }


  // Add for timing map

  // Run the test 
  int thesizes = (sizeof(sizes)/sizeof(*sizes));
  for (int a = 0; a < thesizes; ++a) {
    for (int b = 0; b < thesizes; ++b) {
      // Create cluster a
      std::vector<point> ca;
      create_cluster(ca, sizes[a], dimensions);

      // Create cluster b
      std::vector<point> cb;
      create_cluster(cb, sizes[b], dimensions);

      for (int m = BASELINE; m != TOPK; ++m) {
        switch (m) {
          case BASELINE: {
            std::string key("BASELINE ");
              key += std::to_string(a) + " " + std::to_string(b); 
            timer_map[key] = std::vector<long>();
            for (int i = 0; i < iterations; ++i) {
              long time = baseline(&ca, &cb);
              timer_map[key].push_back(time);
            }
            break;
          }
          default:
            break;
        }
      }
    }
    // TODO Print results as csv and clear the cache
  }

  
    

  return 0;
}



