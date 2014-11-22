
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


enum EntityState { NORMAL = 0, COMPRESSED = 1, SORTED = 2 }; 

namespace dsr {
  class Entity {

    public:
      Entity(): mentions(std::vector<unsigned int>()), 
                  //state(EntityState::NORMAL),
                  state(EntityState::NORMAL),
                  count(0),
                  stringmap(std::unordered_map<unsigned int, unsigned int>())
      {
        bloom_parameters parameters;
        parameters.projected_element_count = 10000;
        parameters.false_positive_probability = 0.0001;
        // parameters.random_seet = ??
        parameters.compute_optimal_parameters();
 
        // Instantiate Bloom Filter
        bloom_filter _bf(parameters);
        bf = _bf;

       //xhll::HyperLogLog _h(4);
       //xh = _h;
      }
    
      // Add a new mention to the data set
      void add (unsigned int mentionid);
      void remove (unsigned int mentionid);

      // Return the index of a random mention chain 
      unsigned int rand();

      // Return the number of Mentions 
      unsigned int size() const { return count; };

      //virtual std::string to_string() const = 0;

      unsigned int bytes() {
        // FIXME this needs to work for both compressed and uncompressed 
        unsigned int counter = 0L;
        counter += sizeof(*this); 
        //counter += sizeof(mentions) * mentions.size();
        counter += sizeof(mentions) ;
        if (mentions.size() > 0)
          counter += sizeof(mentions[0]) * mentions.size();
        return counter;
      }
 
      static dsr::Entity buildEntity(unsigned int size, int cardinality);

      void compress();

    protected: 
      // Initialize the random number generaators 
      void init();

    private:
      unsigned int count;

      // Contains the indexes of the mentions.
      std::vector<unsigned int> mentions;

      // Use this when the number of mentions becomes too large
      // Store the ids of exact token duplicates
      std::unordered_map<unsigned int, unsigned int> stringmap;

      EntityState state;

      bloom_filter bf;

      //xhll::HyperLogLog h;

      // A function for random mentions
      std::function<size_t()> random_mention;

      void add_to_hll(unsigned int val);

      void remove_normal(unsigned int);

  };
}






#endif  // ENTITY_H
