
#include "Random.h"
#include "Util.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/program_options.hpp>

enum Algo {ALL = 0, BASELINE = 1, SORTED = 2, TOPK = 3};

struct point {
  std::vector<int> x;
  point (): x(std::vector<int>()) { }

  point(const point &o) {
    x = std::vector<int>();
    for (auto i : o.x) {
      x.push_back(i);
    }
  }
  point(int dim): x(std::vector<int>()) {
    for (int i = 0; i < dim; ++i) {
      x.push_back(RandInt());
    }
  }

  point & operator= (const point & p) {
    x = std::vector<int> ();
    if (x != p.x) {
      for (auto i = 0; i != p.x.size(); ++i)
        x.push_back(p.x[i]);
    }
    return *this;
  }

  bool operator==(const point & p) const {
    if (p.x.size() == x.size()) {
      for (int i = 0; i < p.x.size(); ++i) {
        if (p.x[i] != x[i]) return false;    
      }
      return true;
    }
    else return false;
  }
  
  static double doCompare (const point& left, const point& right) {
    // Assume other is the same dimension or larger
    double sum = 0.0;
    for (size_t i = 0; i < left.x.size(); ++i) {
      sum +=  pow(left.x[i] - right.x[i], 2);
    }
    //log_info("doCompare: %f, sum: %f", sqrt(sum), sum);
    return sqrt(sum);
  }

};

/**
  * Use this stats data structure to get online variance calculation.
  */
struct Stats {
  unsigned int n; 
  long double mean;
  long double M2;
  
  Stats (): n(0), mean(0.0), M2(0.0) { } 

  void reset (void) { n = 0; mean = 0.0; M2 = 0.0; }

  void add(double x) {
    ++n;
    auto delta = x - mean;
    mean += (delta / n);
    M2 = M2 + delta * (x - mean);
  }

  double variance (void) {
    if (n > 2) 
      return M2 / (n-1);
    else
      return 0.0;
  } 
  double conf (void) {
    //double v = variance();
    //if (v == 0.0) return 1.0;
    //else return sqrt(v);
    return sqrt(variance())/mean;
  }

};

/**
  * Welford's method for computing variance.
  * It would be good to generalize this for
  * any container type
  */
double variance(std::vector<long>& vec) {
  size_t N = 0;
  double M = 0, S = 0, Mprev = 0; 
  for (auto x: vec) {
    ++N;
    Mprev = M;
    M += (x - Mprev) / N;
    S += (x - Mprev) * (x - M);
  }
  if (N <= 2) return 0.0;
  else return S / (N - 1.0);
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
  // TODO make another version of this that does triangle parsing instead of the full n^2
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
      bscore_with += score;
      score = point::doCompare(a[qn[q]], b[i]);
      bscore_with += score;
    }
    bscore_with += point::doCompare(a[qn[q]], a[qn[q]]);

    double score_with = (ascore_with/(asize*asize)) + (bscore_without/(bsize*bsize));
    double score_without = (ascore_without/((asize-1)*(asize-1))) + (bscore_with/((bsize+1)*(bsize+1)));
    accept = score_with < score_without;
  }
  
  clock_t toc = clock();
  return (double)(toc - tic);
}


long sorted_method(std::vector<point> a,
              std::vector<point> b,
              std::vector<int> qn,
              double conf,
              bool & accept) {

  size_t asize = a.size();
  size_t bsize = b.size();
  int qnsize = qn.size();

  clock_t tic = clock();

  // Don't mutilate the vector so create new ones
  std::vector<point> preservea(asize), preserveb(bsize);
  std::copy(begin(a), end(a), begin(preservea));
  std::copy(begin(b), end(b), begin(preserveb));

  // TODO need to keep track of the correct and incorrect decision
  for (int q = 0; q < qnsize; ++q) {

    // Sort the vectors based on the query node
    point qnode (a[q]); // Copy the query node;
    const auto sortcomparator = [qnode] (point p1, point p2) -> bool const {
      return point::doCompare(p1,qnode) < point::doCompare(p2,qnode);
    };
    std::sort (begin(a), end(a), sortcomparator); 
    std::sort (begin(b), end(b), sortcomparator);

    // Compute a with and without q
    Stats astats_with, astats_without;
    //log_info("mean: %Lf,%Lf", astats_with.mean, astats_without.mean);
    bool done = false;
    for (size_t i = 0; i != asize && !done; ++i) {
      for (size_t j = 0; j != asize && !done; ++j) {
        // compare the two vectors
        double score = point::doCompare(a[i],a[j]);
        if (a[i] == qnode || a[j] == qnode) astats_with.add(score);
        else { astats_without.add(score); astats_with.add(score); }
      }
      if (astats_with.conf() > (1-conf) && astats_without.conf() > (1-conf)) { done = true; }
    }

    // Compute b with and without q
    Stats bstats_with, bstats_without;
    done = false;
    for (size_t i = 0; i != bsize && !done; ++i) {
      for (size_t j = 0; j != bsize && !done; ++j) {
        // compare the two vectors
        double score = point::doCompare(b[i],b[j]);
        bstats_without.add(score);
        bstats_with.add(score);
      }
      double score = point::doCompare(b[i], qnode);
      bstats_with.add(score);
      score = point::doCompare(qnode, b[i]);
      bstats_with.add(score);
      if (bstats_with.conf() > (1-conf) && bstats_without.conf() > (1-conf)) { done = true; }
    }
    bstats_with.add( point::doCompare(qnode, qnode) );
    
    double score_with = astats_with.mean + bstats_without.mean;
    double score_without = astats_without.mean + bstats_with.mean;
    accept = score_with < score_without;   

    // Repare the clusters
    //a.clear(); b.clear(); not necessary
    std::copy(begin(preserveb), end(preserveb), begin(b));
    std::copy(begin(preservea), end(preservea), begin(a));
  }
  
  clock_t toc = clock();
  return (double)(toc - tic);
}


int main (int argc, char** argv) {
  namespace po = boost::program_options;

  int sizes[5] = { 10, 100, 1000, 10000, 1000000/*, 10000000*/ };
  int dimensions;
  int algo; 
  int querynodes;
  int iterations;
  double conf;
  
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
    ("conf,c", po::value<double>(&conf)->default_value(0.95), "The confidence interval for early stopping")
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

  log_info("#Params: dimensions=%d; algorithms=%d; querynodes=%d; conf=%f; iterations=%d",\
    dimensions, algo, querynodes, conf, iterations);
  std::cout << "#dimensions="<< dimensions <<"; algorithms=" << algo << "; querynodes="<< querynodes <<"; conf="<< conf <<"; iterations="<< iterations << "; clocks_per_sec=" << CLOCKS_PER_SEC << "\n";

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
              long time = sorted_method(ca, cb, qn, conf, accept);
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



