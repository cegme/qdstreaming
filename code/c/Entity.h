
#ifndef Entity_H
#define Entity_H


#include <string>
#include <vector>

#include "gen-cpp/wikilink_constants.h"
#include "gen-cpp/wikilink_types.h"


enum EntityState { NORMAL = 0, LARGE = 0 }; 

class Entity {

  public:
    Entity(): mentions(vector<int>), 
                EntityState(EntityState::NORMAL)
                count(0), 
                unordered_map(std::unordered_map<const char*, unsigned long>) { }

    // Add a new mention to the data set
    void add (WikiLinkItem *wli);
    void remove (WikiLinkItem *wli);
    unsigned long remove_last();

    // Return the index of a random mention chain 
    size_t rand();

    // Return the number of Mentions 
    unsigned long size() const { return count; } ;

    // TODO 
    virtual std::string to_string() const = 0;

  protected: 
    // Initialize the random number generaators 
    void init();

  private:
    unsigned long count;

    // The mentions
    std::vector<unsigned long> mentions;

    // Use this when the number of mentions becomes too large
    // Store the ids of exact token duplicates
    std::unordered_map<const char*, unsigned long>;

    EntityState state;

    // A function for random mentions
    std::function<size_t()> random_mention;

    void add_normal(WikiLinkItem *wli);

};






#endif  // Entity_H
