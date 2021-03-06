
#include "sqlite3.h"
#include <boost/algorithm/string.hpp>

#include <cmath>
#include <iostream>

#include "ER.h"
#include "Random.h"
#include "Util.h"

using namespace dsr;

std::vector<Entity> ER::wikilinkEntities(std::string dbfile) {

  // Open db file

  // Do a select all, ordered by the tokens (do to lower)

  // Read mentions (tokenzing and hashing strings)

  // Put all similar tokens in the same Entity

  // Add that entity to the list

  // Return entity

  return std::vector<Entity>();

}


//------------------ Test functions ----------


/*
** This function computes the cardinality of each mention.
*/
void computeUniqueMention (const std::string& dbfile, bool _wikiurl = false) {

  std::map<unsigned int, unsigned int> hist;
  std::hash<std::string> hash_fn;

  std::string currentWikiUrl = "";
  std::string lastMention = "";
  std::vector<unsigned int> uniques;

  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  std::string sql;

  sqlite3_initialize();
  rc = sqlite3_open_v2(dbfile.c_str(), &db, SQLITE_OPEN_READONLY, NULL); 
  //rc = sqlite3_open(dbfile.c_str(), &db); 
  if (rc) {
    log_err("Cannon open the database: %s, %s", sqlite3_errmsg(db), dbfile.c_str());
    exit(-1);
  }
  else {
    log_info("Database opened at %s", dbfile.c_str());
  }

  if (!_wikiurl) {
    sql = "SELECT docid, mention, mentionidx, wikiurl FROM wikilink_mention;";
  }
  else {
    sql = "SELECT docid, mention, mentionidx, wikiurl FROM wikilink_wikiurl;";
  }

  sqlite3_stmt* stmt;
  sqlite3_exec(db, "PRAGMA cache_size = 200000;", NULL, NULL, &zErrMsg);
  sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) {log_err(" Query error"); exit(-1); }


  unsigned int row_counter = 0;
  std::deque<std::string> temp_tokens;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    dsr::Mention m;
    m.docid = sqlite3_column_int(stmt, 0);
    m.mentionidx = sqlite3_column_int64(stmt, 2);
    m.entityid = hash_fn((const char*)sqlite3_column_text(stmt, 3));
    
    std::string tokens = (char *) sqlite3_column_text(stmt, 1);
    boost::trim_if(tokens, boost::is_any_of("\t "));
    boost::split(temp_tokens, tokens, boost::is_any_of("\t "), boost::token_compress_on);
    for(auto& t: temp_tokens) m.tokens.push_back(hash_fn(t));
    temp_tokens.clear();

    std::string wikiurl = (const char *)sqlite3_column_text(stmt, 3);
    if (!_wikiurl) {
      if (currentWikiUrl == "" || currentWikiUrl != wikiurl) {
        // New mentions
        uniques.push_back(1); 
        currentWikiUrl = wikiurl;
        lastMention = tokens;
      }
      else if (currentWikiUrl == wikiurl && lastMention == tokens) {
        // A duplicate!
        
      } 
      else if (currentWikiUrl == wikiurl && lastMention != tokens) {
        // A non duplicate, cadrinality++
        ++uniques.back();
      }
      else {
        // This case does not happen
        log_err("Logic err... %u %u %u", m.docid, m.mentionidx, m.entityid);
      }
    }
    else {
      if (currentWikiUrl == wikiurl) {
        ++uniques.back();
      }
      else if (currentWikiUrl == "" || currentWikiUrl != wikiurl ) {
        uniques.push_back(1);
        currentWikiUrl = wikiurl;
      }
      else {
        log_err("Logic err... %u %u %u", m.docid, m.mentionidx, m.entityid);
      }
    

    }
    
    ++row_counter;
  }

  sqlite3_reset (stmt);
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  sqlite3_shutdown();


  Stats s;
  for (const auto &v: uniques) {
    s.add(v);
  }

  log_info("row_counter: %u", row_counter);
  log_info("sum: %lf", s.sum());
  log_info("std_err: %lf", s.std_err());
  log_info("variance: %lf", s.variance());
  log_info("N: %u", s.n);
  log_info("%s", s.to_string());
  

  // Count how many ones, twos, threes, fours and fives
  unsigned int ones = 0, twos = 0, threes = 0, fours = 0, fives = 0; 
  for (const auto &u: uniques) {
    if (u == 1) ++ones;
    else if (u == 2) ++twos;
    else if (u == 3) ++threes;
    else if (u == 4) ++fours;
    else if (u == 5) ++fives;
  }

  // Print out the histogram 
  log_info("ones %u, twos: %u, threes: %u, fours: %u, fives %u",
    ones, twos, threes, fours, fives);
  
  std::sort(uniques.begin(), uniques.end());

  // Create histograms
  for(const auto &u: uniques) {
    if (hist.find(u) == hist.end())
      hist[u] = 1;
    else
      ++hist[u];
  }

  std::cout << "Number,Frequency\n";
  for (const auto &h: hist) {
    std::cout << h.first << "," << h.second << "\n";
  }
  

}



void buildEntityStructures (const std::string& dbfile) {

  std::hash<std::string> hash_fn;

  std::vector<dsr::Mention> mentions;
  std::vector<dsr::Entity*> entities; 

  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  std::string sql;

  sqlite3_initialize();
  rc = sqlite3_open_v2(dbfile.c_str(), &db, SQLITE_OPEN_READONLY, NULL); 
  //rc = sqlite3_open(dbfile.c_str(), &db); 
  if (rc) {
    log_err("Cannon open the database: %s, %s", sqlite3_errmsg(db), dbfile.c_str());
    exit(-1);
  }
  else {
    log_info("Database opened at %s", dbfile.c_str());
  }

  sql = "SELECT docid, mention, mentionidx, wikiurl FROM wikilink_mention;";
  //sql = "SELECT docid, mention, mentionidx, wikiurl FROM wikilink ORDER BY mention LIMIT 100000;";

  sqlite3_stmt* stmt;
  sqlite3_exec(db, "PRAGMA cache_size = 200000;", NULL, NULL, &zErrMsg);
  sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) {log_err(" Query error"); exit(-1); }
  log_info("prepared query...");

  dsr::Entity *e = nullptr;

  std::cout << "mentions,entities,mention (bytes),entity (bytes)\n";
  std::cout << "data = []\n";
  unsigned int row_counter = 0;
  std::deque<std::string> temp_tokens;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    dsr::Mention m;
    m.docid = sqlite3_column_int(stmt, 0);
    m.mentionidx = sqlite3_column_int64(stmt, 2);
    m.entityid = hash_fn((const char*)sqlite3_column_text(stmt, 3));
    
    std::string tokens = (char *) sqlite3_column_text(stmt, 1);
    boost::trim_if(tokens, boost::is_any_of("\t "));
    boost::split(temp_tokens, tokens, boost::is_any_of("\t "), boost::token_compress_on);
    for(auto& t: temp_tokens) m.tokens.push_back(hash_fn(t));
    temp_tokens.clear();

    // Add this to the mentions list
    auto midx = mentions.size();
    mentions.push_back(m);

    if (e == nullptr || mentions.size() > 1
      && !std::equal(mentions.back().tokens.begin(), mentions.back().tokens.end(), mentions[mentions.size()-2].tokens.begin()) ) {
      e = new dsr::Entity();
      e->add(midx);
      entities.push_back(e);

    }
    else {
      e->add(midx);
    }
    
    if (row_counter++ == 0
        || row_counter == 10
        || row_counter == 100
        || row_counter == 1000
        || row_counter == 10000
        || row_counter == 100000
        || row_counter == 1000000 
        || row_counter == 10000000) {

      std::cout << "data += [";
      std::cout << mentions.size() << "," << entities.size() << ",";
      
      unsigned int mcounter = 0L;
      for (auto& m: mentions) mcounter += m.bytes();
      std::cout << mcounter << ",";
      unsigned int ecounter = 0L;  
      for (auto& e: entities) ecounter += e->bytes();
      std::cout << ecounter << "]" << std::endl;
      log_info("Check the time.");
    }

    if (row_counter % 100000 == 0) log_info("%u", row_counter);


  }

  sqlite3_reset (stmt);
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  sqlite3_shutdown();

/*
  log_info("counting mentions...");
  unsigned int mcounter = 0L;
  for (auto& m: mentions) mcounter += m.bytes();

  log_info("counting entities...");
  unsigned int ecounter = 0L;  
  for (auto& e: entities) ecounter += e->bytes();

  std::cerr << "Mention size: " << mentions.size() << "\n";
  std::cerr << "Entitie size: " << entities.size() << "\n";
  std::cerr << "Mention bytes: " << mcounter << "\n";
  std::cerr << "Entitie bytes: " << ecounter << "\n";
  std::cerr << "unsigned long: " << sizeof(unsigned long) << "\n";
  std::cerr << "unsigned int: " << sizeof(unsigned int) << "\n";
  std::cerr << "dsr::Mention: " << sizeof (dsr::Mention) << "\n";
  std::cerr << "dsr::Entity: " << sizeof (dsr::Entity) << "\n";
  //std::cerr << "unsigned long" << << "\n";
*/
      
  std::cout << "data = []\n";
  std::cout << "data += [";
  std::cout << mentions.size() << "," << entities.size() << ",";
  unsigned int mcounter = 0L;
  for (auto& m: mentions) mcounter += m.bytes();
  std::cout << mcounter << ",";
  unsigned int ecounter = 0L;  
  for (auto& e: entities) ecounter += e->bytes();
  std::cout << ecounter << "]\n";

}


void vectorizeMentions (const std::string& dbfile) {

  std::hash<std::string> hash_fn;
  std::hash<const char*> hash_fn2;

  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  std::string sql;

  sqlite3_initialize();
  sqlite3_open_v2(dbfile.c_str(), &db, SQLITE_OPEN_READONLY, NULL); 
  //rc = sqlite3_open(dbfile.c_str(), &db); 
  if (rc) {
    log_err("Cannon open the database: %s", sqlite3_errmsg(db));
  }
  else {
    log_info("Database opened at %s", dbfile.c_str());
  }

  sql = "SELECT * FROM wikilink ORDER BY mention LIMIT 100000;";

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) {log_err(" Query error"); exit(-1); }

  std::deque<std::string> temp_tokens;
  unsigned int thecounter = 0;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    dsr::Mention m;
    m.docid = sqlite3_column_int(stmt, 0);
    m.mentionidx = sqlite3_column_int64(stmt, 2);
    m.entityid = hash_fn((const char*)sqlite3_column_text(stmt, 3));
    
    std::string tokens = (char *) sqlite3_column_text(stmt, 1);
    boost::trim_if(tokens, boost::is_any_of("\t "));
    boost::split(temp_tokens, tokens, boost::is_any_of("\t "), boost::token_compress_on);
    for(auto& t: temp_tokens) m.tokens.push_back(hash_fn(t));

    std::cerr << "Row("
      << m.docid << " | "
      << m.mentionidx << " | "
      << m.entityid << " | "
      << "[ ";
        for(auto&t:m.tokens) std::cerr << t << " ";
      std::cerr << "]"
      <<  sqlite3_column_text(stmt, 1)
      << ")\n\n";

    temp_tokens.clear();
    if(++thecounter % 100000 == 0) std::cerr << ".";
  
  }

  sqlite3_reset (stmt);
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  sqlite3_shutdown();
}

void selectMentions (const std::string& dbfile) {

  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  std::string sql;

  sqlite3_initialize();
  sqlite3_open_v2(dbfile.c_str(), &db, SQLITE_OPEN_READONLY, NULL); 
  //rc = sqlite3_open(dbfile.c_str(), &db); 
  if (rc) {
    log_err("Cannon open the database: %s", sqlite3_errmsg(db));
  }
  else {
    log_info("Database opened at %s", dbfile.c_str());
  }

  sql = "SELECT * FROM wikilink ORDER BY mention LIMIT 10;";
    sqlite3_exec(db, "PRAGMA cache_size = 200000", NULL, NULL, &zErrMsg);

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) {log_err(" Query error"); exit(-1); }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    std::cerr << "Row("
      << sqlite3_column_int64(stmt, 0)
      << sqlite3_column_text(stmt, 1)
      << sqlite3_column_int64(stmt, 2)
      << sqlite3_column_text(stmt, 3)
      << sqlite3_column_text(stmt, 4)
      << sqlite3_column_text(stmt, 5)
      << ")\n";

  }

  sqlite3_reset (stmt);
  sqlite3_finalize(stmt);
  sqlite3_close(db);
  sqlite3_shutdown();
}



// NOTE rowid's are 1 indexed not zero indexed
static const char * doCompareQuery = "SELECT mention FROM wikilink WHERE rowid = ?;";


void ER::mcmc (long unsigned int iterations) {

  sqlite3 *db;
  char *zErrMsg;
  int rc = sqlite3_open_v2("wikilinks.db", &db, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE , NULL); 
  //int rc = sqlite3_open_v2("wikilinks.db", &db, SQLITE_OPEN_READONLY | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_PRIVATECACHE, NULL); 
  if (rc) log_err("Cannot open the database: %s", sqlite3_errmsg(db));
  //sqlite3_exec(db, "PRAGMA cache_size = 1000000;", NULL, NULL, &zErrMsg); 
  sqlite3_exec(db, "PRAGMA cache_size = 1000000;", NULL, NULL, &zErrMsg); 
  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(db, doCompareQuery, -1, &stmt, NULL);



  long unsigned int e_src, e_dst, m_src;
  while (iterations-- > 0) {

    // Get source mention
    e_src = RandInt() % entities->size();
    if (entities->operator[](e_src).size() == 0) {
      iterations++;
      continue;
    }
    m_src = entities->operator[](e_src).rand();

    // destination 
    do {
      e_dst = RandInt() % entities->size();
    } while (e_dst == e_src);

    // Score the two options
    auto srcscore = entities->operator[](e_src).score(m_src, false, stmt, db);
    auto dstscore = entities->operator[](e_dst).score(m_src, true, stmt, db);

    double doMove = srcscore.second + dstscore.first;
    double dontMove = srcscore.first + dstscore.second; 

    // percentage I will accept and do the merge anyways
    if (dontMove < doMove) {
      entities->operator[](e_src).remove(m_src);
      entities->operator[](e_dst).add(m_src);

      log_info("+ %lu:%lu --> %lu", e_src, m_src, e_dst);
    }
    else { 
      // Still do the merge with a small probability
      if (RandDouble() < (1.0 / (1.0 + std::exp(dontMove - doMove)))) {
        entities->operator[](e_src).remove(m_src);
        entities->operator[](e_dst).add(m_src);
        log_info("-+%lu:%lu --> %lu", e_src, m_src, e_dst);
      }
    }
    assert(m_src != 0);
  }

  sqlite3_finalize(stmt);
  sqlite3_close_v2(db);
}


int mainOld (int argc, char **argv) {
  //selectMentions("/data/wikilinks/context-only/063.db");
  //vectorizeMentions("/data/wikilinks/context-only/063.db");
  //buildEntityStructures("/data/wikilinks/context-only/wikilinks.db");
  //buildEntityStructures("wikilinks.db");
  //computeUniqueMention("wikilinks.db"); // Default histogram (mention_uniques.log)
  computeUniqueMention("wikilinks.db", true); // True histogramp (wikiurl_uniques.log)

  return 0;
}
