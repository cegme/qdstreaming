
#include "Optimizer.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include <boost/algorithm/string.hpp>

#include <dlib/svm.h>




bool earlyStop (const dsr::Entity* es, const dsr::Entity* et) {

  // Get Features 
  int es_count = es->count; 
  int et_count = et->count; 

  // FIXME is this line correct??
  return model(es_count, et_count) > 0: true:false;
}



bool doCompression (const dsr::Entity* et) {

  bool doIt = false;

  // Get Features
  unsigned int et_count = et->count; 
  unsigned int et_cardinality = et->cardinality();
  unsigned int et_insertions = et->total_insertions;
  unsigned int et_deletions = et->total_deletions;


  float cardinality_ratio = et_cardinality / et_count;

  // Only compress if it has been at least a little bit active
  if (et->velocity.size() == et->max_velocity) {
    doIt = false;
  }

  // If active do not compress
  if (et->total_insertions > et->total_deletions &&
      et->velocity.size() / et->max_velocity < 0.5) {
    doIt = true;
  }

  // If large and not active compress

  // If large and low velocity, compress

  // If high cardinality and not active, compress 


  if (cardinality_ratio > .25) {
    // If this is very diverse, do not do it
    doIt = false;
  }

  return doIT;
}

void loadModel() {

  log_info("Loading model from %s", model_path.c_str());
  dlib::deserialize(model_path) >> model;
}



/**
  * Using info from: http://dlib.net/krls_ex.cpp.html
  */

int main (int argc, char ** argv) {

  // Create regressors for baseline and early stopping methods

  // Features are A clustersize, B clustersize and the output is the total time
  typedef dlib::matrix<double,3,1> sample_type;

  // Kernel (RBF)
  typedef dlib::radial_basis_kernel<sample_type> kernel_type;
  

  dlib::krls<kernel_type> test(kernel_type(0.1), 1.0); 

  // Read the text file and get the results

  std::string line;
  std::ifstream myfile;
  myfile.open("sortedtest8.csv");
  if (!myfile.is_open()) return false;

  unsigned int count = 0;
  sample_type m;
  while (getline(myfile, line)) {
    if (count++ < 2) continue; // header

    std::vector<std::string> strs;
    boost::split(strs, line, boost::is_any_of(","));

    m(0) = std::stoi(strs[2]); // Cluster A
    m(1) = std::stoi(strs[3]); // Cluster B
    if (strs[0] == "BASELINE") {
      m(2) = 1;
    }
    else if (strs[0] == "BASELINE_TRIANGLE") {
      m(2) = 2;
    }
    else if (strs[0] == "SORTED") {
      m(2) = 3;
    }
    else if (strs[0] == "SORTED_TRIANGLE") {
      m(2) = 4;
    }
    else if (strs[0] == "BLOCKING") {
      m(2) = 5;
    }
    else {
      m(2) = 6;
    }
    std::cerr << std::stoi(strs[2])<<"," << std::stoi(strs[3]) <<","<< std::stoi(strs[4]) << "::" << m(2) << std::endl;
    test.train(m, std::stoi(strs[4])); // Add the time
    
  }

  // Prints the output of each slgo type 
  auto mprint = [&test] (sample_type &s) -> std::string {
    std::stringstream str;
    str << "[";
    s(2) = 1; str << test(s) << ",";
    s(2) = 2; str << test(s) << ",";
    s(2) = 3; str << test(s) << ",";
    s(2) = 4; str << test(s) << ",";
    s(2) = 5; str << test(s) << "]";

    return str.str();
  };

  // Print some examples
  m(0)=10; m(1)=10; 
  std::cerr << "m(0)=10; m(1)=10; --> " << mprint(m) << "\n";
  m(0)=100; m(1)=100;
  std::cerr << "m(0)=100; m(1)=100; --> " << mprint(m) << "\n";
  m(0)=1000; m(1)=1000;
  std::cerr << "m(0)=1000; m(1)=1000; --> " << mprint(m) << "\n";
  m(0)=10000; m(1)=10000;
  std::cerr << "m(0)=10000; m(1)=10000; --> " << mprint(m) << "\n";
  m(0)=100000; m(1)=100000;
  std::cerr << "m(0)=100000; m(1)=100000; --> " << mprint(m) << "\n";
  m(0)=1000000; m(1)=1000000;
  std::cerr << "m(0)=1000000; m(1)=1000000; --> " << mprint(m) << "\n";


  myfile.close();
  
  // Output the model
  dlib::serialize("saved_rbf_function.dat") << test;

  // Read it and try and use it again
  dlib::krls<kernel_type> test2(kernel_type(0.1), 1.0);
  dlib::deserialize("saved_rbf_function.dat") >> test2;
  std::cerr << "m(0)=1000000; m(1)=1000000; --> " << test2(m) << "\n";
  
  return 0;
}
