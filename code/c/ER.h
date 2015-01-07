
#ifndef ER_H
#define ER_H

#include "Entity.h"

#include <deque>
#include <functional>
#include <unordered_map>
#include <string>
#include <vector>

namespace dsr {
  class Mention {
  public:
    unsigned int docid; // Key
    unsigned int mentionidx; // Key
    unsigned int entityid; // ground truth

    std::deque<unsigned int> tokens;
    // TODO store the left and right context
    
    //static double doCompare (Mention m1, Mention m2);

    unsigned long bytes() {
      unsigned long counter = 0L;
      counter += sizeof(*this);
      for (auto&t:tokens) counter += sizeof(t);
      return counter;
    }
  };

  class ER {

    std::vector<Entity> wikilinkEntities(std::string dbfile);

  public:
    ER(std::vector<Entity>* e): entities(e) {}

    void mcmc (long unsigned int iterations);

  private:
    std::hash<std::string> hash_fn;
    //std::unordered_map<std::string, bool>  
    std::vector<Entity>* entities;
  };
}

#endif // ER_H
