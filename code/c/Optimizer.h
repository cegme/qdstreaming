#ifndef _OPTIMIZER_H_
#define _OPTIMIZER_H_

/**
  * This a group of optimizers is used to decide whether 
  * The features for this model are the following:
  * - sorce size
  * - destination size
  * - source entity momentum (Second derivative of the change)
  * - destination entity momentum (Second derivative of the change)
  * - cardinality of source
  * - cardinality of destination
  *
  *
  *
  * (2) for the best feature computation method:
  * - Use a manually pre-trained SVM classifier to decide the fastest method on sample data.
  * - The example is the baseline vs early stopping code
  *
  *
  * (3) Compress if:
  * - Large size and still increasing
  * - Large and low cardinality 
  * - (Probably only compress the destination)
  *
*/

#include "Util.h"
#include "Entity.h"

#include <dlib/svm.h>

typedef dlib::matrix<double,3,1> sample_type;
typedef dlib::radial_basis_kernel<sample_type> kernel_type;


class Optimizer {

  public:
    Optimizer (std::string model_path, 
                    double ctime,
                    double cspace,
                    double cactive,
                    double cvelocity) {
      ctime=0.0;
      cspace=0.0; 
      cactive=0.0;
      cvelocity=0.0;
      
      model_path = "saved_rbf_function.dat";
      //model = dlib::krls<kernel_type>(kernel_type(0.1), 1.0);
      loadModel();

    } 

    std::string model_path;
    double ctime;
    double cspace;
    double cactive;
    double cvelocity;

    
    bool earlyStop (const dsr::Entity* es, const dsr::Entity* et);
    bool doCompression (const dsr::Entity* et);

  private:
    
    //dlib::krls<kernel_type> model;//(kernel_type(0.1), 1.0);

    void loadModel();

};

#endif
