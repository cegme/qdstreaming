
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
      Entity(): mentions(std::vector<unsigned long>()), 
                  state(EntityState::NORMAL),
                  count(0L)//,
                  //stringmap(std::unordered_map<std::string, unsigned long>()) 
      {}
    
      // Add a new mention to the data set
      void add (unsigned long mentionid);
      void remove (unsigned long mentionid);
      unsigned long remove_last();

      // Return the index of a random mention chain 
      size_t rand();

      // Return the number of Mentions 
      unsigned long size() const { return count; };

      //virtual std::string to_string() const = 0;

      unsigned long bytes() {
        unsigned long counter = 0L;
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
      unsigned long count;

      // Contains the indexes of the mentions.
      std::vector<unsigned long> mentions;

      // Use this when the number of mentions becomes too large
      // Store the ids of exact token duplicates
      std::unordered_map<std::string, unsigned long> stringmap;

      EntityState state;

      // A function for random mentions
      std::function<size_t()> random_mention;

      void add_normal(unsigned long);
      void remove_normal(unsigned long);

  };
}






#endif  // ENTITY_H
