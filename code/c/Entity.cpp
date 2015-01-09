
#include <algorithm>
#include <random>
#include <sstream>
#include <string>

#include "Entity.h"
#include "Random.h"
#include "Util.h"

#include <sqlite3.h> 

//#include "hyperloglog.hpp" //https://github.com/hideo55/cpp-HyperLogLog/blob/master/include/hyperloglog.hpp

std::pair<double,double> dsr::Entity::score (unsigned long int mention, bool isAdd, sqlite3_stmt*& stmt, sqlite3 *&db ) {
  // This is the baseline_triangle method

    auto doCompare = [&db,&stmt] (unsigned long int m1, unsigned long int m2) -> double {

      int rc;

      sqlite3_bind_int(stmt, 1, m1);

      rc = sqlite3_step(stmt);
      if (rc != SQLITE_ROW) log_err("Error geting mention: %lu, %s", m1, sqlite3_errmsg(db));
      std::string mention1 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

      // Get Mention 2
      sqlite3_reset(stmt);
      sqlite3_bind_int(stmt, 1, m2);

      rc = sqlite3_step(stmt);
      if (rc != SQLITE_ROW) log_err("Error geting mention: %lu, %s", m2, sqlite3_errmsg(db));
      std::string mention2 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));

      sqlite3_reset(stmt);

      double score = 0.0;

      // Same string
      if (mention1 == mention2)
        score += 50.0;
      else
        score -= 10.0;

      // Same size
      if (mention1.size() == mention2.size())
        score += 5.0;
      else
        score -= 5.0;

      bool overlap = false;
      // Overlapping tokens separated by spaces
      for (unsigned int long i = 0; i < mention1.size(); i = mention1.find(' ', i)) {
        if (i !=0) ++i; // skip the space

        // Find next space, stop at the boundary
        unsigned int end = MIN(mention1.size(), mention1.find(' ', i+1)); 
        if (end - i <= 4) continue;

        for (unsigned int long j = 0; j < mention2.size(); j = mention2.find(' ', j)) {
          if (j !=0) ++j; 

          // Find matching tokens
          if (std::equal(mention1.begin()+i,mention1.begin()+end, mention2.begin()+j)) {
            score += (end - i) * 2;
            overlap = true;
            //if (mention1 != mention2)
              //log_info("The overlap: <%s|%s> --> %s|", mention1.c_str(), mention2.c_str(), mention1.substr(i, end-i).c_str()); 
          }
        }
      }

      if (! overlap)
        score -= 10;
      //else 
        //if (mention1 != mention2)
          //log_info("overlap! %s, %s", mention1.c_str(), mention2.c_str());

      return score;
    };

  // ----------------------------------------------------------
  double score_with = 0.0, score_without = 0.0;
  double pairs_with = 0.0, pairs_without = 0.0;

  auto sz = size();

  if (isAdd) {
    // New mention is not currently in this entity

    double temp_score = 0.0;
    for (int i = 0; i < sz; ++i) {
      for (int j = i+1; j < sz; ++j) {
        temp_score += doCompare(mentions[i], mentions[j]);
        score_without += temp_score;
        score_with += temp_score;
        pairs_with += 1;
        pairs_without += 1;
      }
      if (i != mention) {
        pairs_with += 1;
        score_with += doCompare(mentions[i], mention);
      }

    }
    return {score_with/(pairs_with), score_without/(pairs_without)};
  }
  else {
    // New mention is currently in this entity

    double temp_score = 0.0;
    for (int i = 0; i < sz; ++i) {
      for (int j = i+1; j < sz; ++j) {
        temp_score += doCompare(mentions[i], mentions[j]);
        score_with += temp_score;
        pairs_with += 1.0;
        if (i != mention && j != mention) {
          score_without += temp_score;
          pairs_without += 1.0;
        }
      }
    }
    
    return {score_with/(pairs_with), score_without/(pairs_without)};
  }
  // ----------------------------------------------------------

}




void dsr::Entity::remove (unsigned long int mentionid) {
  update_velocity(false);
  if (state == EntityState::NORMAL) {
    if (mentions.size() == 1) {
      assert(mentionid == mentions[0]);
      mentions.pop_back();
      ++total_deletions;
      --count;
    }
    else {
      // Find where this mention is 
      auto ele = std::find(mentions.begin(), mentions.begin()+count, mentionid);
      mentions[ele-mentions.begin()] = mentions.back();
      mentions.pop_back();
      init();
      ++total_deletions;
      --count;
    }
  }
  else if (state == EntityState::COMPRESSED) {
    if (stringmap.find(mentionid) != stringmap.end()) {
      if (stringmap[mentionid] == 0) {
        stringmap.erase(mentionid);
      }
      else {
        stringmap[mentionid] -= 1;
      }
    }
  }
  else if (state == EntityState::SORTED) {
    // TODO 
  }
  assert(count == mentions.size());
}


void dsr::Entity::init() {

  // Initialize the random number generator for selecting mention chains
  /*std::default_random_engine generator(42L);
  std::uniform_int_distribution<size_t> chain_distribution(0, size()-1);
  random_mention = std::bind(chain_distribution, generator);
  */
  // TODO check the current size, if it is larger than X switch to LARGE

}

void dsr::Entity::add(unsigned long int mentionid) {
  update_velocity(true);

  if (mentionid == 0) {
    log_err("Bad error!: mentionid = 0, entity# %lu, %lu ", mentions.size(), count);
  }
  if (state == EntityState::NORMAL) {
    mentions.push_back(mentionid);
    ++count;
    ++total_insertions;
    init();
    assert(mentions.back() != 0);
  }
  else if (state == EntityState::COMPRESSED) {
  /*  if (stringmap.find(mentionid) != stringmap.end()) {
      stringmap[mentionid] += 1;
    }
    else {
      stringmap[mentionid] = 1;
    }
    add_to_hll(mentionid);
  */
  }
  else if (state == EntityState::SORTED) {
    // Can I do an insertion sort of something?
  }

  assert(count == mentions.size());
}



unsigned long int dsr::Entity::rand() {
  // TODO need a new method if it is in the large state
  if (state == EntityState::NORMAL) {
    auto r = mentions[RandInt() % mentions.size()];
    return r;
  }
  else if (state == EntityState::COMPRESSED) {
    unsigned long int b = RandInt() % stringmap.bucket_count();
    return stringmap.begin(b)->first;
  }
  else {
    throw "Unimplemented Random function"; //TODO
  }
}

dsr::Entity dsr::Entity::buildEntity(unsigned long int size, int cardinality) {

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

void dsr::Entity::add_to_hll(unsigned long int val) {
  auto s = std::to_string(val);
  //xh.add(s.c_str(), s.size());
}


