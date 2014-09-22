
#include <algorithm>
#include <random>
#include <sstream>

#include "Entity.h"
#include "Util.h"



void Entity::remove (WikiLinkItem *wli) {
  if (state == EntityState::NORMAL) {
    remove_norrmal(wli);
  }
  else if (state == EntityState::LARGE) {
    // TODO
  }
}

void Entity::remove_normal (unsigned mention_idx) {
  mentions[mention_idx] = mentions[count];
  --count;
  init();
}


unsigned long Entity::remove_last() {
  --count;
  init();
  return 1;
}

void Entity::init() {

  // Initialize the random number generator for selecting mention chains
  std::default_random_engine generator(42L);
  std::uniform_int_distribution<size_t> chain_distribution(0, size()-1);
  random_mention = std::bind(chain_distribution, generator);

  // TODO check the current size, if it is larger than X switch to LARGE

}

void Entity::add(WikiLinkItem *wli) {
  if (state == EntityState::NORMAL) {
    add_norrmal(wli);
  }
  else if (state == EntityState::LARGE) {
    // TODO implement insertion to the compressed map
  }
}


void Entity::add_normal (WikiLinkItem *wli) {
  mentions.push_back(m); 
  init();
}


unsigned long Entity::rand() {
  // TODO need a new method if it is in the large state
  if (state == EntityState::NORMAL) {
    return random_mention();
  }
  else {
    throw "Unimplemente Random function"; //TODO
  }
}

