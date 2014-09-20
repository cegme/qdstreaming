
#include "Random.h"
#include "Util.h"

#include <algorithm>
#include <future>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/program_options.hpp>

// Finite population control to reduce the sampling variation of the mean.
#define fpc(N,n) sqrt((N-n)/(N-1))
// Size of the upper triangle n*n matrix
#define TRISIZE(n) ((n/2)*(n-1))

enum Algo {ALL = 0, BASELINE = 1, BASELINE_TRIANGLE = 2, SORTED = 3, SORTED_TRIANGLE = 4, BLOCKING = 5, LAST = 6};

struct point {
  std::vector<int> x;
  point (): x(std::vector<int>()) { }

  point(const point &o): x(std::vector<int>()) {
    if (o.dim() > 0) {
      //for (auto i : o.x) {
      for (int i = 0; i < o.x.size(); ++i) {
        x.push_back(o.x[i]);
      }
    }
  }

  ~point() {
    x.clear();
    x.resize(0);
  }

  point(int dim): x(std::vector<int>()) {
    for (int i = 0; i < dim; ++i) {
      x.push_back(RandInt() % 100);
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

  inline int dim (void) const { return x.size(); }

  std::ostream& operator<< (std::ostream &strm) const {
    strm << "<"; 
    for (int i = 0; i < x.size()-1; ++i) {
      strm << x[i] << ",";
    }
    if (x.size() > 0) strm << x.back();
    strm << ">"; 
    return strm;
  }

  std::string to_string() const {
    std::stringstream strm;
    strm << "<"; 
    for (int i = 0; i < x.size()-1; ++i) {
      strm << x[i] << ",";
    }
    if (x.size() > 0) strm << x.back();
    strm << ">"; 
    return strm.str();

  }
  
  static double doCompare (const point& left, const point& right) {
    // Assume other is the same dimension or larger
    double sum = 0.0;
    int leftxsize = left.x.size();
    int rightxsize = right.x.size();
    for (size_t i = 0; i < leftxsize && i < rightxsize; ++i) {
      sum +=  pow(left.x[i] - right.x[i], 2);
    }
    //log_info("doCompare: %f, sum: %f", sqrt(sum), sum);
    //if (sqrt(sum) > 0.0) std::cerr << left.to_string() << "::" << right.to_string() << "::" << sqrt(sum) << "\n";
    return sqrt(sum);
  }

};


/**
  * Welford's method for computing variance.
  * It would be good to generalize this for
  * any container type
  */
double variance(const std::vector<long>& vec) {
  Stats s;
  for (auto a : vec) {
    s.add((long double)a);
  }
  return s.variance();
}


void create_cluster(std::vector<point>& a, int size, int dimensions) {

  // Clear old cluster
  a.clear();

  for (int i = 0; i < size; ++i) {
    // Call the random constructor
    a.push_back(point(dimensions));
  }
}


long baseline_method (const std::vector<point> &a,
              const std::vector<point> &b,
              std::vector<int> qn,
              std::vector<bool> & accept) {
  // TODO pass vectors by reference, don't make a copy
  // TODO make another version of this that does triangle parsing instead of the full n^2
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
    // Get exact score width
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
    accept.push_back(score_with < score_without);
  }
  
  clock_t toc = clock();
  return (double)(toc - tic);
}


long baseline_triangle_method (const std::vector<point> &a,
              const std::vector<point> &b,
              std::vector<int> qn,
              std::vector<bool> & accept) {

  size_t asize = a.size();
  size_t bsize = b.size();
  int qnsize = qn.size();

  clock_t tic = clock();

  // Use http://www.boost.org/doc/libs/1_56_0/libs/numeric/ublas/doc/triangular.htm?
  for (int q = 0; q < qnsize; ++q) {
    double ascore_with = 0.0, ascore_without = 0.0;

    // Compute a with and without q
    for (size_t i = 0; i < asize; ++i) {
      for (size_t j = i+1; j < asize; ++j) {
        // compare the two vectors
        double score = point::doCompare(a[i],a[j]);
        if (i == qn[q] || j == qn[q]) ascore_with += score;
        else ascore_without += score;
      } 
    }
    // Get exact score width
    ascore_with += ascore_without;

    double bscore_with = 0.0, bscore_without = 0.0;
    // Compute b with and without q
    for (size_t i = 0; i < bsize; ++i) {
      for (size_t j = i+1; j < bsize; ++j) {
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

    double score_with = (ascore_with/TRISIZE(asize)) + (bscore_without/TRISIZE(bsize));
    double score_without = (ascore_without/TRISIZE(asize-1)) + (bscore_with/TRISIZE(bsize+1));
    accept.push_back(score_with < score_without);
  }
  clock_t toc = clock();
  return (double)(toc - tic);
}


long sorted_triangle_method(const std::vector<point> &_a,
              const std::vector<point> &_b,
              const std::vector<int> &qn,
              double conf,
              std::vector<bool> & accept) {

  int debug_iter = 0;

  size_t asize = _a.size();
  size_t bsize = _b.size();
  int qnsize = qn.size();

  clock_t tic = clock();
  
  // Copy all of the query nodes first
  std::vector<point> qs;
  for (int q = 0; q < qnsize; ++q) {
    qs.push_back(_a[q]);
  }

  // Don't mutilate the vector so create new ones
  std::vector<point> a(asize), b(bsize);
  std::copy(_a.cbegin(), _a.cend(), a.begin());
  std::copy(_b.cbegin(), _b.cend(), b.begin());

  // Need to keep track of the correct and incorrect decision
  for (int q = 0; q < qnsize; ++q) {

    // Sort the vectors based on the query node
    point qnode = qs[q];
    const auto sortcomparator = [qnode] (point p1, point p2) -> bool const {
      return point::doCompare(p1,qnode) < point::doCompare(p2,qnode);
    };
    std::sort (begin(a), end(a), sortcomparator); 
    std::sort (begin(b), end(b), sortcomparator);

    // Compute a with and without q
    Stats astats_with, astats_without;
    //log_info("mean: %Lf,%Lf", astats_with.mean, astats_without.mean);
    bool done = false;
    for (size_t i = 0; i < asize && !done; ++i) {
      for (size_t j = i+1; j < asize && !done; ++j) {
        // compare the two vectors
        double score = point::doCompare(a[i],a[j]);
        if (a[i] == qs[q] || a[j] == qs[q]) astats_with.add(score);
        else { astats_without.add(score); astats_with.add(score); }
        ++debug_iter;
      }
      if (astats_with.std_err() > (1-conf) || astats_without.std_err() > (1-conf)) { done = true; }
    }


    debug_iter = 0;
    // Compute b with and without q
    Stats bstats_with, bstats_without;
    done = false;
    for (size_t i = 0; i < bsize && !done; ++i) {
      for (size_t j = i+1; j < bsize && !done; ++j) {
        // compare the two vectors
        double score = point::doCompare(b[i],b[j]);
        bstats_without.add(score);
        bstats_with.add(score);
        ++debug_iter;
      }
      double score = point::doCompare(b[i], qs[q]);
      bstats_with.add(score);
      score = point::doCompare(qs[q], b[i]);
      bstats_with.add(score);
      if (bstats_with.std_err() > (1-conf) || bstats_without.std_err() > (1-conf)) { done = true;}
    }
    bstats_with.add( point::doCompare(qs[q], qs[q]) );
    
    
    double score_with = astats_with.mean + bstats_without.mean;
    double score_without = astats_without.mean + bstats_with.mean;
    accept.push_back(score_with < score_without);

    // Repare the clusters
    std::copy(_b.cbegin(), _b.cend(), b.begin());
    std::copy(_a.cbegin(), _a.cend(), a.begin());
  }
  
  clock_t toc = clock();
  return (double)(toc - tic);
}


long sorted_method(const std::vector<point> &_a,
              const std::vector<point> &_b,
              const std::vector<int> &qn,
              double conf,
              std::vector<bool> & accept) {

  int debug_iter = 0;

  size_t asize = _a.size();
  size_t bsize = _b.size();
  int qnsize = qn.size();

  clock_t tic = clock();
  
  // Copy all of the query nodes first
  std::vector<point> qs;
  for (int q = 0; q < qnsize; ++q) {
    qs.push_back(_a[q]);
  }

  // Don't mutilate the vector so create new ones
  std::vector<point> a(asize), b(bsize);
  std::copy(_a.cbegin(), _a.cend(), a.begin());
  std::copy(_b.cbegin(), _b.cend(), b.begin());

  // Need to keep track of the correct and incorrect decision
  for (int q = 0; q < qnsize; ++q) {

    // Sort the vectors based on the query node
    point qnode = qs[q];
    const auto sortcomparator = [qnode] (point p1, point p2) -> bool const {
      return point::doCompare(p1,qnode) < point::doCompare(p2,qnode);
    };
    std::sort (begin(a), end(a), sortcomparator); 
    std::sort (begin(b), end(b), sortcomparator);

    // Compute a with and without q
    Stats astats_with, astats_without;
    //log_info("mean: %Lf,%Lf", astats_with.mean, astats_without.mean);
    bool done = false;
    for (size_t i = 0; i < asize && !done; ++i) {
      for (size_t j = 0; j < asize && !done; ++j) {
        // compare the two vectors
        double score = point::doCompare(a[i],a[j]);
        if (a[i] == qs[q] || a[j] == qs[q]) astats_with.add(score);
        else { astats_without.add(score); astats_with.add(score); }
        ++debug_iter;
      }
      if (astats_with.std_err() > (1.0-conf) || astats_without.std_err() > (1.0-conf)) { done = true; }
    }


    debug_iter = 0;
    // Compute b with and without q
    Stats bstats_with, bstats_without;
    done = false;
    for (size_t i = 0; i < bsize && !done; ++i) {
      for (size_t j = 0; j < bsize && !done; ++j) {
        // compare the two vectors
        double score = point::doCompare(b[i],b[j]);
        bstats_without.add(score);
        bstats_with.add(score);
        ++debug_iter;
      }
      double score = point::doCompare(b[i], qs[q]);
      bstats_with.add(score);
      score = point::doCompare(qs[q], b[i]);
      bstats_with.add(score);
      if (bstats_with.std_err() > (1-conf) || bstats_without.std_err() > (1-conf)) { done = true;}
    }
    bstats_with.add( point::doCompare(qs[q], qs[q]) );
    
    
    double score_with = astats_with.mean + bstats_without.mean;
    double score_without = astats_without.mean + bstats_with.mean;
    accept.push_back(score_with < score_without);

    // Repare the clusters
    std::copy(_b.cbegin(), _b.cend(), b.begin());
    std::copy(_a.cbegin(), _a.cend(), a.begin());
  }
  
  clock_t toc = clock();
  return (double)(toc - tic);
}

long blocking_method(const std::vector<point> &_a,
              const std::vector<point> &_b,
              const std::vector<int> &qn,
              double conf,
              std::vector<bool> & accept) {

  size_t asize = _a.size();
  size_t bsize = _b.size();
  int qnsize = qn.size();

  clock_t tic = clock();
  
  // Copy all of the query nodes first
  std::vector<point> qs;
  for (int q = 0; q < qnsize; ++q) {
    qs.push_back(_a[q]);
  }

  // Don't mutilate the vector so create new ones
  std::vector<point> a(asize), b(bsize);
  std::copy(_a.cbegin(), _a.cend(), a.begin());
  std::copy(_b.cbegin(), _b.cend(), b.begin());


  // Need to keep track of the correct and incorrect decision
  for (int q = 0; q < qnsize; ++q) {
    // Break the upper triangle range into two
    // smaller triangles and a square. Sum each part.

    // Triangle #1 (top left)
   auto atriangle1 = [&qn,&q,&a,asize] () -> std::pair<double,double> {
      double ascore_with = 0.0, ascore_without = 0.0;
      for (int i = 0; i < asize/2; ++i) {
        for (int j = i+1; j < asize/2; ++j) {
          double score = point::doCompare(a[i],a[j]);
          if (i == qn[q] || j == qn[q]) ascore_with += score;
          else ascore_without += score;
        }
      }
      return std::make_pair(ascore_with, ascore_without);
    };

    // Triangle #2 (bottom right)
   auto atriangle2 = [&qn,&q,&a,asize] () -> std::pair<double,double> {
      double ascore_with = 0.0, ascore_without = 0.0;
      for (int i = asize/2 +1; i < asize; ++i) {
        for (int j = i+1; j < asize; ++j) {
          double score = point::doCompare(a[i],a[j]);
          if (i == qn[q] || j == qn[q]) ascore_with += score;
          else ascore_without += score;
        }
      }
      return std::make_pair(ascore_with, ascore_without);
    };

    // The Square

   auto asq = [&qn,&q,&a,asize] () -> std::pair<double,double> {
      double ascore_with = 0.0, ascore_without = 0.0;
      for (int i = asize/2 + 1; i < asize; ++i) {
        for (int j = 0; j < asize/2; ++j) {
          double score = point::doCompare(a[i],a[j]);
          if (i == qn[q] || j == qn[q]) ascore_with += score;
          else ascore_without += score;
        }
      }
      return std::make_pair(ascore_with, ascore_without);
    };

    

   
    // TODO the b version of above
    auto btriangle1 = [&qn,&q,&a,&b,&bsize] () -> std::pair<double,double> {
      double bscore_with = 0.0, bscore_without = 0.0;
      for (int i = 0; i < bsize/2; ++i) {
        for (int j = i+1; j < bsize/2; ++j) {
          double score = point::doCompare(b[i],b[j]);
          bscore_without += score;
        }
      }
      return std::make_pair(bscore_with, bscore_without);
    };

    auto btriangle2 = [&qn,&q,&a,&b,&bsize] () -> std::pair<double,double> {
      double bscore_with = 0.0, bscore_without = 0.0;
      for (int i = bsize/2 +1; i < bsize; ++i) {
        for (int j = i+1; j < bsize; ++j) {
          double score = point::doCompare(b[i],b[j]);
          bscore_without += score;
        }
        double score = point::doCompare(b[i], a[qn[q]]);
        bscore_with += score;
        score = point::doCompare(a[qn[q]], b[i]);
        bscore_with += score;
      }
      return std::make_pair(bscore_with, bscore_without);
    };

     auto bsq = [&qn,&q,&a,&b,bsize] () -> std::pair<double,double> {
      double bscore_with = 0.0, bscore_without = 0.0;
      for (int i = bsize/2 + 1; i < bsize; ++i) {
        for (int j = 0; j < bsize/2; ++j) {
          double score = point::doCompare(b[i],b[j]);
          bscore_without += score;
        }
        double score = point::doCompare(b[i], a[qn[q]]);
        bscore_with += score;
        score = point::doCompare(a[qn[q]], b[i]);
        bscore_with += score;
      }
      bscore_with += point::doCompare(a[qn[q]], a[qn[q]]);
      return std::make_pair(bscore_with, bscore_without);
    };


    // Run and sum the As and Bs
    double bscore_with = 0.0, bscore_without = 0.0;
    double ascore_with = 0.0, ascore_without = 0.0;
    {
      std::vector<std::future<std::pair<double,double>>> avec;
      avec.push_back(std::async(std::launch::async, atriangle1));
      avec.push_back(std::async(std::launch::async, atriangle2));
      avec.push_back(std::async(std::launch::async, asq));

      for (auto &f : avec) {
        auto a = f.get();
        ascore_with += a.first;
        ascore_without += a.second;
      }

      std::vector<std::future<std::pair<double,double>>> bvec;
      bvec.push_back(std::async(std::launch::async, btriangle1));
      bvec.push_back(std::async(std::launch::async, btriangle2));
      bvec.push_back(std::async(std::launch::async, bsq));

      for (auto &f : bvec) {
        auto b = f.get();
        bscore_with += b.first;
        bscore_without += b.second;
      }
    }
    double score_with = (ascore_with/TRISIZE(asize)) + (bscore_without/TRISIZE(bsize));
    double score_without = (ascore_without/TRISIZE(asize-1)) + (bscore_with/TRISIZE(bsize+1));
    accept.push_back(score_with < score_without);
    
  }

  clock_t toc = clock();
  return (double)(toc - tic);
}


int main (int argc, char** argv) {
  namespace po = boost::program_options;

  //int sizes[5] = { 10, 100, 1000, 10000/*, 1000000, 10000000*/ };
  std::pair<int,int>  absizes[25] = {std::make_pair(10,10), std::make_pair(10,100), std::make_pair(100,10), std::make_pair(100,100), 
                                  std::make_pair(1000,10), std::make_pair(10,1000), std::make_pair(1000,100), std::make_pair(100,1000), std::make_pair(1000,1000),
                                  std::make_pair(10000,10), std::make_pair(10,10000), std::make_pair(10000,100), std::make_pair(100,10000), std::make_pair(10000,1000), std::make_pair(1000,10000), std::make_pair(10000,10000),
                                  std::make_pair(100000,10), std::make_pair(10,100000), std::make_pair(100000,100), std::make_pair(100,100000), std::make_pair(100000,1000), std::make_pair(1000,100000), std::make_pair(100000,10000), std::make_pair(10000,100000), std::make_pair(100000,100000)};
  int dimensions;
  int algo; 
  int querynodes;
  int iterations;
  double conf;
  
  std::unordered_map<std::string, std::vector<long> > timer_map;
  std::unordered_map<std::string, std::vector<std::vector<bool>>> accuracy_map;

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
      return 0;
    }
  }
  catch (boost::program_options::error &e) {
    log_err("Bad parameters");
    logInfo(desc);
    return 1;
  }

  log_info("#Params: dimensions=%d; algorithms=%d; querynodes=%d; conf=%f; iterations=%d",\
    dimensions, algo, querynodes, conf, iterations);
  std::cout << "#dimensions="<< dimensions <<"; algorithms=" << algo << "; querynodes="<< querynodes <<"; conf="<< conf <<"; iterations="<< iterations << "; clocks_per_sec=" << CLOCKS_PER_SEC << "\n";

  // Print header
  // Name, N, a clustersize, b clustersize, Sum, Variance
  std::cout << "Method," << "Samples," << "A Cluster Size," << "B Cluster Size," << "Time," << "Variance," << "Accuracy," << "Conf\n";

  // Run the test 
  int thesizes = (sizeof(absizes)/sizeof(*absizes));
  //for (int a = 0; a < thesizes; ++a) {
  for (auto ab : absizes) { 
    int a = ab.first;

    // Create cluster a
    std::vector<point> ca;
    create_cluster(ca, a, dimensions);

    // Get $querynode querynodes
    auto qn = [&ca,querynodes] () -> std::vector<int> {
      std::vector<int> qn;
      // Do sampling with replacement. We'll allow duplicate nodes
      for (int i = 0; i < querynodes; ++i) { 
        qn.push_back(RandInt() % ca.size());
      }
      return qn;
    }();

    int b = ab.second;
    // Create cluster b
    std::vector<point> cb;
    create_cluster(cb, b, dimensions);

    // Need to know the optimal decision
    // Always run baseline first to get it.
    for (int m = BASELINE_TRIANGLE; m != LAST; ++m) {
      switch (m) {
        case BASELINE: {
          std::vector<bool> baseaccept; 
          std::string key("BASELINE");
          timer_map[key] = std::vector<long>();
          accuracy_map[key] = std::vector<std::vector<bool>>();
          for (int i = 0; i < iterations; ++i) {
            long time = baseline_method(ca, cb, qn, baseaccept);
            timer_map[key].push_back(time);
            accuracy_map[key].push_back(baseaccept);
          }
          break;
        }
        case BASELINE_TRIANGLE: {
          std::vector<bool> baseaccept; 
          std::string key("BASELINE_TRIANGLE");
          timer_map[key] = std::vector<long>();
          accuracy_map[key] = std::vector<std::vector<bool>>();
          for (int i = 0; i < iterations; ++i) {
            long time = baseline_triangle_method(ca, cb, qn, baseaccept);
            timer_map[key].push_back(time);
            accuracy_map[key].push_back(baseaccept);
          }
          break;
        }

        case SORTED: {
          std::string key("SORTED");
          std::vector<bool> thisaccept; 
          timer_map[key] = std::vector<long>();
          accuracy_map[key] = std::vector<std::vector<bool>>();
          for (int i = 0; i < iterations; ++i) {
            long time = sorted_method(ca, cb, qn, conf, thisaccept);
            timer_map[key].push_back(time);
            accuracy_map[key].push_back(thisaccept);
          }
          break;
        }

        case SORTED_TRIANGLE: {
          std::string key("SORTED_TRIANGLE");
          std::vector<bool> thisaccept; 
          timer_map[key] = std::vector<long>();
          accuracy_map[key] = std::vector<std::vector<bool>>();
          for (int i = 0; i < iterations; ++i) {
            long time = sorted_triangle_method(ca, cb, qn, conf, thisaccept);
            timer_map[key].push_back(time);
            accuracy_map[key].push_back(thisaccept);
          }
          break;
        }
        case BLOCKING: {
          std::string key("BLOCKING");
          std::vector<bool> thisaccept; 
          timer_map[key] = std::vector<long>();
          accuracy_map[key] = std::vector<std::vector<bool>>();
          for (int i = 0; i < iterations; ++i) {
            long time = blocking_method(ca, cb, qn, conf, thisaccept);
            timer_map[key].push_back(time);
            accuracy_map[key].push_back(thisaccept);
          }
          break;
        }
        
        default:
          break;
      }
    }

    // Compute Accuracy
    auto accuracy = [&accuracy_map,iterations,querynodes] (const std::vector<std::vector<bool>> &a) -> float {
      auto base = accuracy_map["BASELINE_TRIANGLE"];
      float s = 0.0;
      for (int i = 0; i < iterations; ++i) {
        for (int j = 0; j < querynodes; ++j) {
          if (base[i][j] == a[i][j]) s += 1.0; 
        }
      }
      return s/(iterations*querynodes);
    };

    // Print results as csv and clear the cache
    for (auto e : timer_map) {
      // Name, N, a clustersize, b clustersize, Time, Variance, Accuracy, confidence
      std::cout << e.first << "," << iterations << ","
        << a << "," << b
        << "," << std::accumulate(e.second.begin(), e.second.end(), 0L)
        << "," << variance(e.second)
        << "," << accuracy(accuracy_map[e.first])
        << "," << conf
        << std::endl;
    }
    timer_map.clear();
  }

  return 0;
}



