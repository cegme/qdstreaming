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


class Optimizer {

// Model
  



// Stats

// Training row

// Test row


};

#endif
