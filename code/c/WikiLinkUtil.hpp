

#ifndef WIKILINKUTIL_H
#define WIKILINKUTIL_H

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>


#include "Entity.h"
#include "Random.h"
#include "Util.h"
#include "WikiLinkFile.h"

#include <boost/program_options.hpp>

#include <stdio.h>
#include <sqlite3.h> 

/**
  * Writes a file with the ground truth for the 
  * wikilink data set.
  */
void CreateGroundTruthFile (std::string fileName) {

  std::unordered_map<std::string, std::vector<int>> hist;

  // Open the database file
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  std::string sql;
  rc = sqlite3_open_v2("wikilinks.db", &db, SQLITE_OPEN_READONLY, NULL); 
    if (rc) {
    log_err("Cannot open the database: %s", sqlite3_errmsg(db));
  }
  else {
    log_info("Database opened at wikilinks.db");
  }

  sql = "SELECT rowid, wikiurl from wikilink;";
  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

  log_info("Reading the file.");
  sqlite3_exec(db, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg); // Improve speed #YOLO
  sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);

  rc = sqlite3_step(stmt);
  while (rc == SQLITE_ROW) {
    unsigned int rowid =  sqlite3_column_int(stmt, 0);
    std::string wikiurl = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

    if (hist.find(wikiurl) == hist.end()) {
      std::vector<int> elements;
      elements.push_back(rowid);
      hist[wikiurl] = elements;
    }
    else {
      hist[wikiurl].push_back(rowid);
    }
    rc = sqlite3_step(stmt);
  }

  // Close the database file
  sqlite3_finalize(stmt);

  sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &zErrMsg);
  sqlite3_close_v2(db);


  // Write the results to a file
  log_info("Writing the results to a binary file. %s.bin", fileName.c_str());
  FILE* pFile;
  std::ofstream o;
  //o.open(fileName, ios::out | ios::binary);
  pFile = fopen("WikiLinkTruth.data.bin", "wb");
  unsigned int total_count = hist.size();
  fwrite(&total_count, sizeof(total_count), 1, pFile); // Number of Total entries
  for (auto& h: hist) {
    //o << h.first << ":";
    unsigned int mcount = h.second.size();
    fwrite(&mcount, sizeof(mcount), 1, pFile); // Number of entries
    
    for (auto& m: h.second) {
      fwrite(&m, sizeof(m), 1, pFile);
    }
  }
  fclose (pFile);

  log_info("Done!");
}


void CreateStartFile (std::string fileName) {

  std::unordered_map<std::string, std::vector<unsigned int>> hist;

  // Open the database file
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  std::string sql;
  rc = sqlite3_open_v2("wikilinks.db", &db, SQLITE_OPEN_READONLY, NULL); 
    if (rc) {
    log_err("Cannot open the database: %s", sqlite3_errmsg(db));
  }
  else {
    log_info("Database opened at wikilinks.db");
  }

  sql = "SELECT rowid, mention from wikilink;";
  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

  log_info("Reading the file.");
  sqlite3_exec(db, "PRAGMA synchronous = OFF", NULL, NULL, &zErrMsg); // Improve speed #YOLO
  sqlite3_exec(db, "BEGIN TRANSACTION", NULL, NULL, &zErrMsg);

  rc = sqlite3_step(stmt);
  while (rc == SQLITE_ROW) {
    unsigned int rowid =  sqlite3_column_int(stmt, 0);
    std::string mention = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

    if (hist.find(mention) == hist.end()) {
      std::vector<unsigned int> elements;
      elements.push_back(rowid);
      hist[mention] = elements;
    }
    else {
      hist[mention].push_back(rowid);
    }
    rc = sqlite3_step(stmt);
  }

  // Close the database file
  sqlite3_finalize(stmt);

  sqlite3_exec(db, "END TRANSACTION", NULL, NULL, &zErrMsg);
  sqlite3_close_v2(db);

  // Write the results to a file
  log_info("Writing the results to a file. %s", fileName.c_str());
  FILE* pFile;
  std::ofstream o;
  //o.open(fileName);
  pFile = fopen("WikiLinkStart.data.bin", "wb");  

  unsigned int total_count = hist.size();
  fwrite(&total_count, sizeof(total_count), 1, pFile); // Number of Total entries
  for (auto& h: hist) {
    //o << h.first << ":";
    unsigned int mcount = h.second.size();
    fwrite(&mcount, sizeof(mcount), 1, pFile); // Number of entries

    for (auto& m: h.second) {
      fwrite(&m, sizeof(m), 1, pFile);
    }
  }
  fclose (pFile);

  log_info("Done!");
}

std::vector<dsr::Entity> ReadEntityFile (std::string fileName, bool limit = false) {

  std::vector<dsr::Entity> entities;

  FILE* pFile;
  pFile = fopen(fileName.c_str(), "rb");
  if (pFile==NULL) {  
    log_err("Could not open the file %s", "WikiLinkTruth.data.bin");
  }
  unsigned int total_count = 0;

  fread(&total_count, sizeof(unsigned int), 1, pFile);

  log_info("Loading %d entities into an entity vector.", total_count);
  for (unsigned int i = 0; i < total_count && (!limit || i < 1000)  ; ++i) {

    unsigned int mention_size;
    fread(&mention_size, sizeof(unsigned int), 1, pFile);

    dsr::Entity e;
    for (unsigned int j = 0; j < mention_size; ++j) {
      unsigned int mention;
      fread(&mention, sizeof(unsigned int), 1, pFile);
      e.add(mention);
    }
    entities.push_back(e); 
    
    if (i == 100000 || i % 500000 == 0) {
      log_info("Read %d entities", i);
    }
  }
  fclose (pFile);

  log_info("Read all %d", total_count);
  return entities;
}



struct MyStats {

  long unsigned int tp;
  long unsigned int fp;
  long unsigned int tn;
  long unsigned int fn;

  long unsigned int total_pairs;
  static long unsigned int total_true_pairs;

  MyStats () : tp(0), fp(0), tn(0), fn(0), total_pairs(0) {
    if (total_true_pairs == 0 || total_true_pairs != MyStats::total_true_pairs) {
      init();
    }
    log_info("total_true_pairs: %u", MyStats::total_true_pairs);
  }

  // Thank you Menestrina, Whang, Garcia-Molina 2009
  // http://ilpubs.stanford.edu:8090/930/2/ERMetric.pdf

  double pairPrecision () const {
    return tp/total_pairs;
  }
  double pairRecall () const {
    return tp/total_true_pairs;
  }
  double pairF1 () const {
    double num = pairPrecision();
      num *= pairRecall();
    double dem = pairPrecision();
      dem += pairRecall();
    return 2.0 * num/dem;
  }

  static unsigned nChoosek( unsigned n, unsigned k ) {
    if (k > n) return 0;
    if (k * 2 > n) k = n-k;
    if (k == 0) return 1;

    int result = n;
    for( int i = 2; i <= k; ++i ) {
      result *= (n-i+1);
      result /= i;
    }
    return result;
  }

  // Initialize true count
  void init () {
    log_info("Initializing the total_true_pairs value from %u", total_true_pairs);
    unsigned int temp_true_pairs = 0;

    FILE* pFile;
    pFile = fopen("WikiLinkTruth.data.bin", "rb");
    if (pFile==NULL) {
      log_err("Could not open the file %s", "WikiLinkTruth.data.bin");
    }
    unsigned int total_count = 0;

    fread(&total_count, sizeof(unsigned int), 1, pFile);

    for (unsigned int i = 0; i < total_count; ++i) {

      unsigned int mention_size;
      fread(&mention_size, sizeof(unsigned int), 1, pFile);

      temp_true_pairs += MyStats::nChoosek(mention_size, 2);
      unsigned int mention_throwaway;
      for (unsigned int j = 0; j < mention_size; ++j) {
        fread(&mention_throwaway, sizeof(unsigned int), 1, pFile);
      }
    }
    fclose (pFile); 

    total_true_pairs = temp_true_pairs;
    log_info("Initialized! total_true_pairs value now  %u", total_true_pairs);
  }

};
//unsigned int MyStats::total_true_pairs = 0;
long unsigned int MyStats::total_true_pairs = 2616530327; 


MyStats ComputeStats (const std::vector<dsr::Entity>& entities, std::string trueFile) {
  MyStats s; 
  //s.init();

  // Compute the total number of pairs
  long unsigned int total_pairs = 0;
  for(unsigned i = 0; i < entities.size(); ++i) {
    total_pairs +=  MyStats::nChoosek(entities[i].size(), 2);
  }
  s.total_pairs = total_pairs;
  log_info("total_pairs: %u", total_pairs);

  // Open the database file
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  std::string sql;
  //rc = sqlite3_open("/data/wikilinks/context-only/wikilinks.db", &db); 
  rc = sqlite3_open_v2("wikilinks.db", &db, SQLITE_OPEN_READONLY, NULL); 
  //rc = sqlite3_open_v2("/data/wikilinks/context-only/wikilinks.db", &db, SQLITE_OPEN_READONLY, NULL); 
  //rc = sqlite3_open_v2("/data/wikilinks/context-only/wikilinks.db", &db, SQLITE_OPEN_READWRITE, NULL); 
  if (rc != SQLITE_OK) {
    log_err("Cannot open the database: %s", sqlite3_errmsg(db));
  }
  else {
    log_info("Database opened at wikilinks.db");
    //log_info("Database opened at /data/wikilinks/context-only/wikilinks.db");
  }

  sql = "SELECT wikiurl from wikilink_urlmap2 where rowid2 = ?;";
  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &stmt, NULL);

  for (unsigned int e = 0; e < entities.size(); ++e) {
    std::vector<std::string> truths;

    for (unsigned int m: entities[e].mentions) {

      sqlite3_bind_int(stmt, 1, m);

      rc = sqlite3_step(stmt);
      if (rc == SQLITE_ROW) {
        auto men = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        truths.push_back(men);
      }
      sqlite3_reset(stmt);
      //sqlite3_clear_bindings(stmt);
    }

    // Check each pairwise combination to see if they will be 
    auto sz = entities[e].mentions.size();

    for (auto i = 0; i < sz - 1; ++i) {
      for (auto j = i+1; j < sz; ++j) {
        if (truths[i] == truths[j]) s.tp += 1;
        //else if (truths[i] != truths[j]) s.fp += 1;
        else s.fp += 1;
      }
    }

  }

  sqlite3_close_v2(db);
  log_info("tp = %u, fp = %u", s.tp, s.fp);

  return s;

}





#endif  // ENTITY_H