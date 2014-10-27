
#include "Optimizer.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include <boost/algorithm/string.hpp>

#include <dlib/svm.h>


/**
  * Using info from: http://dlib.net/krls_ex.cpp.html
  */

int main (int argc, char ** argv) {

  // Create regressors for baseline and early stopping methods

  // Features are A clustersize, B clustersize and the output is the total time
  typedef dlib::matrix<double,2,1> sample_type;

  // Kernel (RBF)
  typedef dlib::radial_basis_kernel<sample_type> kernel_type;
  

  dlib::krls<kernel_type> test(kernel_type(0.1), 1.0); 

  // Read the text file and get the results

  std::string line;
  std::ifstream myfile;
  myfile.open("sortedtest8.csv");
  if (!myfile.is_open()) return false;

  sample_type m;
  while (getline(myfile, line)) {

    std::vector<std::string> strs;
    boost::split(strs, line, boost::is_any_of(","));

    if (strs[0] == "SORTED_TRIANGLE") {
      m(0) = std::stoi(strs[2]); // Cluster A
      m(1) = std::stoi(strs[3]); // Cluster B
      test.train(m, std::stoi(strs[4])); // Add the time
    }
    
     
  }

  // Print some examples
  m(0)=10; m(1)=10;
  std::cout << "m(0)=10; m(1)=10; --> " << test(m) << "\n";
  m(0)=100; m(1)=100;
  std::cout << "m(0)=100; m(1)=100; --> " << test(m) << "\n";
  m(0)=1000; m(1)=1000;
  std::cout << "m(0)=1000; m(1)=1000; --> " << test(m) << "\n";
  m(0)=10000; m(1)=10000;
  std::cout << "m(0)=10000; m(1)=10000; --> " << test(m) << "\n";
  m(0)=100000; m(1)=100000;
  std::cout << "m(0)=100000; m(1)=100000; --> " << test(m) << "\n";
  m(0)=1000000; m(1)=1000000;
  std::cout << "m(0)=1000000; m(1)=1000000; --> " << test(m) << "\n";


  myfile.close();
  

  return 0;
}
