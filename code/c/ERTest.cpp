
#include "Entity.h"
#include "Random.h"
#include "Util.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

/*
 *
 * face
 * 
 */

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


long baseline_triangle_method (const dsr::Entity * a,
              const dsr::Entity * b,
              unsigned int m) {

  size_t asize = a->size();
  size_t bsize = b->size();

  clock_t tic = clock();

    double ascore_with = 0.0, ascore_without = 0.0;

  if (a->state == EntityState::NORMAL) {
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
  }

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
  
  clock_t toc = clock();
  sample_type m;
  m(0) = asize;
  m(1) = bsize;
  test.train(m, (double) toc - tic);
  return (double)(toc - tic);
}










int main (int argc, char **argv) {


  unsigned int samples = 1e6;
  unsigned int max_mentions = 1e5;
  unsigned int dimensions  = 3;

  // Create a list of mentions (points)
  std::vector<point> mentions;

  // Create an entity vevtoEntities for each of those points
  std::vector<dsr::Entity> entities(max_mentions);


  // A map to map mention locatioms to entity locations
  // If an item is -1 it is a default mapping
  std::vector<unsigned int>memap(max_mentions, -1);

  // Initialize mentions and entities
  for (int i = 0; i < max_mentions; ++i) {
    auto p = point(dimensions);
    mentions.push_back(p);
    
    entities[i].add(i);
  }
  
  // Perform ER process
  while (samples-- > 0) {

    // Sample a mention from a random entity
    unsigned int m;
    unsigned int es;
    unsigned int et;
    do {
      es = RandInt() % entities.size();
    } while (entities[es].size() > 0);
    m = entities[es].rand();
    
    // Sample dst entity
    do { 
      et = RandInt() % max_mentions;
    } while (entities[et].size() > 0); 


    m = RandInt() % max_mentions;
    // Score a merge

    
    // Keep or accept with a small probability

    // TODO if successful
    //  memap[e] = et;
  } 


  // Check convergence


  return 0;
}
