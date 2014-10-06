
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
    unsigned long docid; // Key
    int mentionidx; // Key
    unsigned long entityid; // ground truth

    std::deque<unsigned long> tokens;
    // TODO store the left and right context
  };

  class ER {

    std::vector<Entity> wikilinkEntities(std::string dbfile);

  private:
    std::hash<std::string> hash_fn;
    //std::unordered_map<std::string, bool>  
    std::vector<Entity> entities;
  };
}

#endif // ER_H
