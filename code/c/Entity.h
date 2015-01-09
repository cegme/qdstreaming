
#ifndef ENTITY_H
#define ENTITY_H


#include <string>
#include <unordered_map>
#include <vector>

#include "bloom_filter.hpp"
//#include "murmur3.h"
//#include "hyperloglog.hpp" //https://github.com/hideo55/cpp-HyperLogLog/blob/master/include/hyperloglog.hpp

#include "gen-cpp/wikilink_constants.h"
#include "gen-cpp/wikilink_types.h"

#include <sqlite3.h>


enum EntityState { NORMAL = 0, COMPRESSED = 1, SORTED = 2 }; 

namespace dsr {
  class Entity {

    public:
      Entity(): mentions(std::vector<unsigned long int>()), 
                  //state(EntityState::NORMAL),
                  state(EntityState::NORMAL),
                  count(0),
                  total_insertions(0),
                  total_deletions(0),
                  max_velocity(500),
                  stringmap(std::unordered_map<unsigned long int, unsigned long int>())
      {
        /*bloom_parameters parameters;
        parameters.projected_element_count = 10000;
        parameters.false_positive_probability = 0.0001;
        // parameters.random_seet = ??
        parameters.compute_optimal_parameters();*/
 
        // Instantiate Bloom Filter
        //bloom_filter _bf(parameters);
        //this->bf = _bf;

       //xhll::HyperLogLog _h(4);
       //xh = _h;
      }

      /* If isAdd this is a proposal to insert. */ 
      std::pair<double,double> score (unsigned long int mention, bool isAdd, sqlite3_stmt*& stmt, sqlite3 *&db ) ;
      

      // Compare two mentions with the given ids
      static double doCompare(unsigned long int m1, unsigned long int m2);
    
      // Add a new mention to the data set
      void add (unsigned long int mentionid);
      void remove (unsigned long int mentionid);

      // Return the index of a random mention chain 
      unsigned long int rand();

      // Return the number of Mentions 
      unsigned long int size() const { return mentions.size(); };

      unsigned long int bytes() {
        // FIXME this needs to work for both compressed and uncompressed 
        unsigned long int counter = 0L;
        counter += sizeof(*this); 
        //counter += sizeof(mentions) * mentions.size();
        counter += sizeof(mentions) ;
        if (mentions.size() > 0)
          counter += sizeof(mentions[0]) * mentions.size();
        return counter;
      }
 
      static dsr::Entity buildEntity(unsigned long int size, int cardinality);

      void compress();

    //protected: 
      // Initialize the random number generaators 
      void init();

      unsigned long int total_insertions;
      unsigned long int total_deletions;
    //private:
      unsigned long int count;

      // This keep trac of the updates.
      // If there is a removal reset the velovity.
      unsigned long int max_velocity;
      std::list<bool> velocity;

      // Contains the indexes of the mentions.
      std::vector<unsigned long int> mentions;

      // Use this when the number of mentions becomes too large
      // Store the ids of exact token duplicates
      std::unordered_map<unsigned long int, unsigned long int> stringmap;

      EntityState state;

      //bloom_filter bf;
      unsigned long int cardinality() const {
        //bf.cardinality();
      }

      //xhll::HyperLogLog h;

      // A function for random mentions

      void add_to_hll(unsigned long int val);

      void update_velocity(bool isAdd) {

        if (isAdd) {
          velocity.push_back(isAdd);
          if (velocity.size() > max_velocity) {
            velocity.pop_front();
          }
        }
        else {
          // For now, clear if it is false
          velocity.clear();
        }
      }

  };
}



#endif  // ENTITY_H
