
#include "Random.h"
#include "Util.h"

#include <vector>

#include <boost/program_options.hpp>
 
enum Algo {ALL = 0, RANDOM = 1, SORTED = 2, TOPK = 3};

struct point {
  std::vector<int> x;
  point(int dim): x(std::vector<int>()) {
    for (int i = 0; i < dim; ++i) {
      x.push_back(RandomInt());
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

// TODO create algorithms
// TODO create statistis collections method


int main(int argc, char** argv) {
  namespace po = boost::program_options;

  int iterations;
  int dimensions;
  Algo algo; 
  int* sizes = { 10, 100, 1000, 10000, 1000000, 10000000 };

  po::options_description desc("Cluster Improvement test.");
  desc.add_options()
    ("help,h", "Print a helpul help message")
    ("dimension,d", po::value<int>(&dimensions)->default_value(2),
       "The dimensions of the points")
    ("algorithms,a", po::value<Algo>(&algo)->default_value(Algo::ALL),
       "Choose the algorithm to run, all is default, Choose values 1-3");
    ("iterations,i", po:value<int>(&iterations)->default_value(100), "Iterations for each algo");

  // Run the test 
  for (int a = 0; a < sizes; ++i) {
    for (int b = 0; b < sizes; ++i) {
      // Create cluster a
      std::vector<point> a;
      create_cluster(a, sizes[a], dimensions);

      // Create cluster b
      std::vector<point> b;
      create_cluster(b, sizes[b], dimensions);

      for (int m = RANDOM; m != TOPK; ++m) {
        switch (m) {
          case RANDOM:
            random(a, b, iterations);

      }
    }
  }
    

  return 0;
}
