
#ifndef ENTITY_H
#define ENTITY_H


#include <string>
#include <unordered_map>
#include <vector>

#include "gen-cpp/wikilink_constants.h"
#include "gen-cpp/wikilink_types.h"


enum EntityState { NORMAL = 0, LARGE = 0 }; 

namespace dsr {
  class Entity {

    public:
      Entity(): mentions(std::vector<unsigned int>()), 
                  //state(EntityState::NORMAL),
                  state(EntityState::LARGE),
                  count(0)//,
      {}
    
      // Add a new mention to the data set
      void add (unsigned int mentionid);
      void remove (unsigned int mentionid);
      unsigned int remove_last();

      // Return the index of a random mention chain 
      unsigned int rand();

      // Return the number of Mentions 
      unsigned int size() const { return count; };

      //virtual std::string to_string() const = 0;

      unsigned int bytes() {
        unsigned int counter = 0L;
        counter += sizeof(*this); 
        //counter += sizeof(mentions) * mentions.size();
        counter += sizeof(mentions) ;
        if (mentions.size() > 0)
          counter += sizeof(mentions[0]) * mentions.size();
        return counter;
      }

    protected: 
      // Initialize the random number generaators 
      void init();

    private:
      unsigned int count;

      // Contains the indexes of the mentions.
      std::vector<unsigned int> mentions;

      // Use this when the number of mentions becomes too large
      // Store the ids of exact token duplicates
      std::unordered_map<std::string, unsigned int> stringmap;

      EntityState state;

      // A function for random mentions
      std::function<size_t()> random_mention;

      void add_normal(unsigned int);
      void remove_normal(unsigned int);

      void add_unique(unsigned int);

  };
}






#endif  // ENTITY_H
