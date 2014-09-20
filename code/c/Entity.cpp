
#include <algorithm>
#include <random>
#include <sstream>

#include "Entity.h"
#include "Util.h"



void Entity::remove (WikiLinkItem *wli) {
  /*MentionChain *m = chains[mention_idx];
  chains.erase(chains.begin()+mention_idx);
  init();
  return m;*/
}

unsigned long Entity::remove_last() {
  /*MentionChain *m = chains[chains.size()-1];
  chains.erase(chains.end()-1);
  init();
  return m;*/
  return 0L;
}

void Entity::init() {

  // Initialize the random number generator for selecting mention chains
  std::default_random_engine generator(42L);
  std::uniform_int_distribution<size_t> chain_distribution(0, size()-1);
  random_mention = std::bind(chain_distribution, generator);

  // TODO check the current size, if it is larger than 

}

void Entity::add(WikiLinkItem *wli) {
  if (state == EntityState::NORMAL) {

    // TODO
    //mentions.push_back(m); 
    init();
  }
  else if (state == EntityState::LARGE) {
    // TODO implement insertion to the compressed map
    init();
  }
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

