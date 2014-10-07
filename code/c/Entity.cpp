
#include <algorithm>
#include <random>
#include <sstream>

#include "Entity.h"
#include "Util.h"



void dsr::Entity::remove (unsigned long mentionid) {
  if (state == EntityState::NORMAL) {
    remove_normal(mentionid);
  }
  else if (state == EntityState::LARGE) {
    // TODO
  }
}

void dsr::Entity::remove_normal (unsigned long mention_idx) {
  mentions[mention_idx] = mentions[count]; // Put the last one in this ones place
  --count;
  init();
}


unsigned long dsr::Entity::remove_last() {
  --count;
  init();
  return 1;
}

void dsr::Entity::init() {

  // Initialize the random number generator for selecting mention chains
  std::default_random_engine generator(42L);
  std::uniform_int_distribution<size_t> chain_distribution(0, size()-1);
  random_mention = std::bind(chain_distribution, generator);

  // TODO check the current size, if it is larger than X switch to LARGE

}

void dsr::Entity::add(unsigned long mentionid) {
  if (state == EntityState::NORMAL) {
    add_normal(mentionid);
  }
  else if (state == EntityState::LARGE) {
    // TODO implement insertion to the compressed map
  }
}


void dsr::Entity::add_normal (unsigned long mentionid) {
  if (count < mentions.size())
    mentions[count] = mentionid;
  else 
    mentions.push_back(mentionid); 

  ++count;
  init();
}


unsigned long dsr::Entity::rand() {
  // TODO need a new method if it is in the large state
  if (state == EntityState::NORMAL) {
    return random_mention();
  }
  else {
    throw "Unimplemented Random function"; //TODO
  }
}





