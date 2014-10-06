
#include "sqlite3.h"
#include <boost/algorithm/string.hpp>



#include <iostream>

#include "ER.h"
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

  sql = "SELECT * FROM wikilink ORDER BY mention LIMIT 1000;";

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
  if (rc != SQLITE_OK) {log_err(" Query error"); exit(-1); }

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


int main (int argc, char **argv) {
  //selectMentions("/data/wikilinks/context-only/063.db");
  vectorizeMentions("/data/wikilinks/context-only/063.db");
  return 0;
}
