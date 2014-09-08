
#include "Random.h"
#include "Util.h"

#include <iostream>
#include <numeric>
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

  static double doCompare(point left, point right) {
    // Assume other is the same dimension or larger
    double sum = 0.0;
    for (size_t i = 0; i < left.x.size(); ++i) {
      sum +=  pow(left.x[i] - right.x[i], 2); 
    }
    return sqrt(sum);
  }
};

/**
  * Welford's method for computing variance.
  * It would be good to generalize this for
  * any container type
  */
double variance(std::vector<long>& vec) {
  assert(vec.size() >= 2);
  size_t N = 0;
  long M = 0, S = 0, Mprev = 0; 
  for (auto x: vec) {
    ++N;
    Mprev = M;
    M += (x - Mprev) / N;
    S += (x - Mprev) * (x - M);
  }
  return S / (N - 1);
}

void create_cluster(std::vector<point>& a, int size, int dimensions) {

  // Clear old cluster
  a.clear();

  for (int i = 0; i < size; ++i) {
    // Call the random constructor
    a.push_back(point(dimensions));
  }
}

long baseline_method(std::vector<point> a,
              std::vector<point> b,
              std::vector<int> qn,
              bool & accept) {
  // TODO pass vectors by reference, don't make a copy
  // TODO need a vector of results for each query node. migrate accept to a vector
  accept = false; // This method sets the accept parameters
  size_t asize = a.size();
  size_t bsize = b.size();
  int qnsize = qn.size();

  clock_t tic = clock();

  for (int q = 0; q < qnsize; ++q) {
    double ascore_with = 0.0, ascore_without = 0.0;

    // Compute a with and without q
    for (size_t i = 0; i != asize; ++i) {
      for (size_t j = 0; j != asize; ++j) {
        // compare the two vectors
        double score = point::doCompare(a[i],a[j]);
        if (i == qn[q] || j == qn[q]) ascore_with += score;
        else ascore_without += score;
      } 
    }
    // Get exact scire width
    ascore_with += ascore_without;

    double bscore_with = 0.0, bscore_without = 0.0;
    // Compute b with and without q
    for (size_t i = 0; i != bsize; ++i) {
      for (size_t j = 0; j != bsize; ++j) {
        // compare the two vectors
        double score = point::doCompare(b[i],b[j]);
        bscore_without += score;
      } 
      double score = point::doCompare(b[i], a[qn[q]]);
      bscore_with = score;
      score = point::doCompare(a[qn[q]], b[i]);
      bscore_with = score;
    }
    bscore_with = point::doCompare(a[qn[q]], a[qn[q]]);

    double score_with = (ascore_with/(asize*asize)) + (bscore_without/(bsize*bsize));
    double score_without = (ascore_without/((asize-1)*(asize-1))) + (bscore_with/((bsize+1)*(bsize+1)));
    accept = score_with < score_without;
  }
  
  clock_t toc = clock();
  return toc - tic;
}


long sorted_method(std::vector<point> a,
              std::vector<point> b,
              std::vector<int> qn,
              bool & accept) {

  size_t asize = a.size();
  size_t bsize = b.size();
  int qnsize = qn.size();

  clock_t tic = clock();

  // TODO need to keep track of the correct and incorrect decision
  for (int q = 0; q < qnsize; ++q) {
    // TODO sort the points based on the query node
    
  }
  
  clock_t toc = clock();
  return toc - tic;
}


int main (int argc, char** argv) {
  namespace po = boost::program_options;

  int iterations;
  int dimensions;
  int algo; 
  int sizes[5] = { 10, 100, 1000, 10000, 1000000/*, 10000000*/ };
  int querynodes;
  
  std::unordered_map<std::string, std::vector<long> > timer_map;

  boost::program_options::options_description desc("Cluster Improvement test.");
  desc.add_options()
    ("help,h", "Print a helpul help message")
    ("dimension,d", po::value<int>(&dimensions)->default_value(2),
       "The dimensions of the points")
    ("algorithms,a", po::value<int>(&algo)->default_value((int)Algo::ALL),
       "Choose the algorithm to run, all is default, Choose values 1-3")
    ("querynodes,q", po::value<int>(&querynodes)->default_value(1),
      "The number of query nodes to test for each set")
    ("iterations,i", po::value<int>(&iterations)->default_value(100), "Iterations for each algo");

  boost::program_options::variables_map vm;
  try {
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
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

  // Print header
  // Name, N, a clustersize, b clustersize, Sum, Variance
  std::cout << "Method," << "Samples," << "A Cluster Size," << "B Cluster Size," << "Sum," << "Variance\n";

  // Run the test 
  int thesizes = (sizeof(sizes)/sizeof(*sizes));
  for (int a = 0; a < thesizes; ++a) {

    // Create cluster a
    std::vector<point> ca;
    create_cluster(ca, sizes[a], dimensions);

    // Get $querynode querynodes
    auto qn = [&ca,querynodes] () -> std::vector<int> {
      std::vector<int> qn;
      // Do sampling with replacement. We'll allow duplicate nodes
      for (int i = 0; i < querynodes; ++i) { 
        qn.push_back(RandInt() % ca.size());
      }
      return qn;
    }();

    for (int b = 0; b < thesizes; ++b) {
      // Create cluster b
      std::vector<point> cb;
      create_cluster(cb, sizes[b], dimensions);

      // Need to know the optimal decision
      // Always run baseline first to get it.
      bool accept; 

      for (int m = BASELINE; m != TOPK; ++m) {
        switch (m) {
          case BASELINE: {
            std::string key("BASELINE");
            timer_map[key] = std::vector<long>();
            for (int i = 0; i < iterations; ++i) {
              long time = baseline_method(ca, cb, qn, accept);
              timer_map[key].push_back(time);
            }
            break;
          }
          case SORTED: {
            std::string key("SORTED");
            timer_map[key] = std::vector<long>();
            for (int i = 0; i < iterations; ++i) {
              long time = sorted_method(ca, cb, qn, accept);
              timer_map[key].push_back(time);
            }
            break;
          }
          case TOPK: {
            break;
          }
          default:
            break;
        }
      }

      // Print results as csv and clear the cache
      for (auto e : timer_map) {
        // Name, N, a clustersize, b clustersize, Sum, Variance
        std::cout << e.first << "," << iterations << ","
          << sizes[a] << "," << sizes[b]
          << "," << std::accumulate(e.second.begin(), e.second.end(), 0L)
          << "," << variance(e.second)
          << std::endl;
      }
      timer_map.clear();
    }
  }

  return 0;
}



