
#include <algorithm>
#include <random>
#include <sstream>
#include <string>

#include "Entity.h"
#include "Random.h"
#include "Util.h"

//#include "hyperloglog.hpp" //https://github.com/hideo55/cpp-HyperLogLog/blob/master/include/hyperloglog.hpp


void dsr::Entity::remove (unsigned int mentionid) {
  /*if (state == EntityState::NORMAL) {
    mentions[mention_idx] = mentions[count]; // Put the last one in this ones place
    --count;
    init();
  }
  else if (state == EntityState::COMPRESSED) {
    if (stringmap.find(m) != stringmap.end()) {
      if (stringmap[m] == 0) {
        stringmap.erase();
      }
      else {
        stringmap[m] -= 1;
      }
    }
    else {
      stringmap[m] = 1;
    }
  }
  else if (state == EntityState::SORTED) {
    // TODO 
  }*/
}


void dsr::Entity::init() {

  // Initialize the random number generator for selecting mention chains
  //std::default_random_engine generator(42L);
  //std::uniform_int_distribution<size_t> chain_distribution(0, size()-1);
  //random_mention = std::bind(chain_distribution, generator);

  // TODO check the current size, if it is larger than X switch to LARGE

}

void dsr::Entity::add(unsigned int mentionid) {
  if (state == EntityState::NORMAL) {
    if (count < mentions.size()) {
      mentions[count] = mentionid;
    }
    else {
      mentions.push_back(mentionid);
    }

    bf.insert(mentionid);
    add_to_hll(mentionid);

    ++count;
    init();
  }
  else if (state == EntityState::COMPRESSED) {
    if (stringmap.find(mentionid) != stringmap.end()) {
      stringmap[mentionid] += 1;
    }
    else {
      stringmap[mentionid] = 1;
    }
    add_to_hll(mentionid);
  
  }
  else if (state == EntityState::SORTED) {
    // Can I do an insertion sort of something?
  }

}



unsigned int dsr::Entity::rand() {
  // TODO need a new method if it is in the large state
  if (state == EntityState::NORMAL) {
    return random_mention();
  }
  else {
    throw "Unimplemented Random function"; //TODO
  }
}

dsr::Entity dsr::Entity::buildEntity(unsigned int size, int cardinality) {

  dsr::Entity e;


  // If cardinality is -1 do random integers
  if (cardinality == -1) {
    for (unsigned i = 0; i < size; ++i) {
      e.add(RandInt());
    }
  }
  else {
    std::vector<unsigned> ms;
    // Create a block of mentions for each item in the cardinality
    // Evenly spread out the randoms for now
    unsigned block = size / cardinality; // (Assume size > cardinality)
    for (unsigned i = 0; i < cardinality; ++i) {
      auto m = RandInt();
      for (auto b = 0; b < block; ++b) {
        ms.push_back(m);
      }
    }

    std::random_shuffle(ms.begin(), ms.end());
    for(auto &m: ms) {
      e.add(m);
    }
  }

  return e;
}


void dsr::Entity::compress() {
  // NOTE: This object should be locked before compression
  state = EntityState::COMPRESSED;

  // Make this bigger
  //xh = hll::HyperLogLog(16);

  // Remove an entity from a vector and add it to the map
  for (unsigned i = 0; i < count; ++i) {
    auto m = mentions.back();
    mentions.pop_back();
    
    if (stringmap.find(m) != stringmap.end()) {
      stringmap[m] += 1;
    }
    else {
      stringmap[m] = 1;
    }

    // Add to the hll
   //xadd_to_hll(m);

    // No need to build on the entity 
  }

  // Clear up the memory
  mentions.clear();
}

void dsr::Entity::add_to_hll(unsigned int val) {
  auto s = std::to_string(val);
  //xh.add(s.c_str(), s.size());
}


