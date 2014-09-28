
#include "Util.h"
#include "WikiLinkFile.h"

#include <fstream>
#include <future>
#include <unordered_map>

#include <boost/program_options.hpp>

#include <stdio.h>
#include <sqlite3.h> 


void compute_histogram() {
  char file[50];
  std::unordered_map<std::string, int> hist;

  const char* filePath = "/data/wikilinks/context-only/%03d.gz";
  
  Stats total_mentions;
  Stats total_wikilinkitems;
  
  for (int i = 1; i < 110; ++i) {
    snprintf (file, 50, filePath, i);
    log_info("Reading the file: %s", file);

    WikiLinkFile wlf(file);
    while (wlf.hasNext()) {

      total_wikilinkitems.inc();
      auto wli = wlf.next();

      for (auto& m : wli.mentions) {
        total_mentions.inc();

        if (hist.find(m.wiki_url) == hist.end()) {
          hist[m.wiki_url] = 1;
        }
        else {
          hist[m.wiki_url] += 1;
        }
      }
    }
  }

  // Output stats
  log_info("Mentions stats: %s", total_mentions.to_string());
  log_info("WikiLinkItems stats: %s", total_mentions.to_string());
  log_info("Total hist size: %ld", hist.size());

  // Write histogram to a log
  std::ofstream o;
  o.open ("/data/wikilinks/context-only/hist.tsv");
  
  o << "# Mentions stats: " << total_mentions.to_string() << "\n";
  o << "# WikiLinkItems stats: " << total_mentions.to_string() << "\n";
  o << "# Total hist size: " << hist.size() << "\n";
  o << "# Entity\tCount\n";
  for (auto& h: hist) {
    o << h.first << "\t" << h.second << "\n";
  }
  o.flush();
  o.close();

}

void hello_world () {
  //WikiLinkFile wlf("/data/wikilinks/001.gz");
  //WikiLinkFile wlf("/data/wikilinks/tmp/001", false);
  WikiLinkFile wlf("/data/wikilinks/context-only/001.gz");

  int counter = 0;
  while(wlf.hasNext()) {
    auto wli = wlf.next();
    if (counter == 3 || counter == 4 || counter == 5 || counter == 6) std::cerr << wli << "\n";
    //std::cout << wli << std::endl << std::endl;
    if(++counter % 100 == 0) std::cerr << ".";
    //if (counter > 0) break;
  }
  log_info("\n");
  log_info("The number of items: %d", counter);

}

void createdb(std::string dbfile) {

  // Creates the sqlite datebase file
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  std::string sql;

  rc = sqlite3_open(dbfile.c_str(), &db); 
  if (rc) {
    log_err("Cannon open the database: %s", sqlite3_errmsg(db));
  }
  else {
    log_info("Database opened at %s", dbfile.c_str());
  }

  sql = "CREATE TABLE wikilink (" \
        "docid INT, " \
        "mention char(100)," \
        "mentionidx INT," \
        "wikiurl char(100));";

  rc = sqlite3_exec(db, sql.c_str(), 
    [&] (void *params, int argc, char**Argv, char **AzColName ) -> int {
      return 0;
    }, 
    0, &zErrMsg);
  if (rc != SQLITE_OK) {
    sqlite3_free(zErrMsg);
  }
  else {
    log_info("[wikilink] Table created successfully at %s", dbfile.c_str());
  }

  sql = "CREATE TABLE wikilink_filemap (" \
        "docid INT, " \
        "file char(7));";

  rc = sqlite3_exec(db, sql.c_str(), 
    [&] (void *params, int argc, char**Argv, char **AzColName ) -> int {
      return 0;
    }, 
    0, &zErrMsg);
  if (rc != SQLITE_OK) {
    sqlite3_free(zErrMsg);
  }
  else {
    log_info("[wikilink_filemap] Table created successfully at %s", dbfile.c_str());
  }



  sqlite3_close(db);
}


/**
  * Takes the zip file, dbname, and the new name
  */
void loaddb_file (std::string dbfile, std::string dbName, std::string newDBFile) {
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  std::string sql, sql2;

  rc = sqlite3_open(newDBFile.c_str(), &db); 
  if (rc) {
    log_err("Cannon open the database: %s", sqlite3_errmsg(db));
  }
  else {
    log_info("Database opened at %s", newDBFile.c_str());
  }

  sql = "INSERT INTO wikilink VALUES (?1, ?2, ?3, ?4)";
  sql2 = "INSERT INTO wikilink_filemap VALUES (?1, ?2)";

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
  sqlite3_stmt* stmt2;
  sqlite3_prepare_v2(db, sql2.c_str(), -1, &stmt2, NULL);
  
  log_info("Reading the file: %s", dbfile.c_str());

  WikiLinkFile wlf(dbfile.c_str(), true);
  while (wlf.hasNext()) {
    auto wli = wlf.next();

    // Insert file map
    sqlite3_bind_int(stmt2,1,wli.doc_id);
    sqlite3_bind_text(stmt2,2,dbName.c_str(),-1,SQLITE_TRANSIENT); 
    if (sqlite3_step(stmt2) != SQLITE_DONE) {
      log_err("Error inserting to wikilink_filemap: (%d, %s)", wli.doc_id, dbName.c_str());
      sqlite3_reset(stmt2);
      sqlite3_close(db);
      return;
    }
    sqlite3_reset(stmt2);

    int mcounter = 0; // The mention index
    for (auto& m : wli.mentions) {

      // Insert the mentions
      sqlite3_bind_int(stmt,1,wli.doc_id);
      sqlite3_bind_text(stmt,2,m.anchor_text.c_str(),-1,SQLITE_TRANSIENT); 
      sqlite3_bind_int(stmt,3,mcounter);
      sqlite3_bind_text(stmt,4,m.wiki_url.c_str(),-1,SQLITE_TRANSIENT);

      if (sqlite3_step(stmt) != SQLITE_DONE) {
        log_err("Error executing the prepared statement");
        sqlite3_reset(stmt);
        sqlite3_close(db);
        return;
      }

      ++mcounter;
      sqlite3_reset(stmt);
    }
  }
  sqlite3_close(db);
}


void loaddb (std::string dbfile) {
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  std::string sql, sql2;

  rc = sqlite3_open(dbfile.c_str(), &db); 
  if (rc) {
    log_err("Cannon open the database: %s", sqlite3_errmsg(db));
  }
  else {
    log_info("Database opened at %s", dbfile.c_str());
  }

  sql = "INSERT INTO wikilink VALUES (?1, ?2, ?3, ?4)";
  sql2 = "INSERT INTO wikilink_filemap VALUES (?1, ?2)";

  sqlite3_stmt* stmt;
  sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
  sqlite3_stmt* stmt2;
  sqlite3_prepare_v2(db, sql2.c_str(), -1, &stmt2, NULL);
  
  // Test

  char file[50], fName[8];
  const char* filePath = "/data/wikilinks/context-only/%03d.gz";
  const char* fileName = "%03d.gz";
  
  for (int i = 1; i < 110; ++i) {
    snprintf (file, 50, filePath, i);
    snprintf (fName, 8, fileName, i);
    log_info("Reading the file: %s", file);

    WikiLinkFile wlf(file, true);
    while (wlf.hasNext()) {
      auto wli = wlf.next();

      // Insert file map
      sqlite3_bind_int(stmt2,1,wli.doc_id);
      sqlite3_bind_text(stmt2,2,fName,-1,SQLITE_TRANSIENT); 
      if (sqlite3_step(stmt2) != SQLITE_DONE) {
        log_err("Error inserting to wikilink_filemap: (%d, %s)", wli.doc_id, fName);
        sqlite3_reset(stmt2);
        sqlite3_close(db);
        return;
      }
      sqlite3_reset(stmt2);

      int mcounter = 0; // The mention index
      for (auto& m : wli.mentions) {

        // Insert the mentions
        sqlite3_bind_int(stmt,1,wli.doc_id);
        sqlite3_bind_text(stmt,2,m.anchor_text.c_str(),-1,SQLITE_TRANSIENT); 
        sqlite3_bind_int(stmt,3,mcounter);
        sqlite3_bind_text(stmt,4,m.wiki_url.c_str(),-1,SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
          log_err("Error executing the prepared statement");
          sqlite3_reset(stmt);
          sqlite3_close(db);
          return;
        }

        ++mcounter;
        sqlite3_reset(stmt);
      }
    }
  }
  sqlite3_close(db);
}

void dropdb(std::string dbfile) {

  // Creates the sqllite datebase file
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;
  std::string sql;

  rc = sqlite3_open(dbfile.c_str(), &db); 
  if (rc) {
    log_err("Cannon open the database: %s", sqlite3_errmsg(db));
  }
  else {
    log_info("Database opened at %s", dbfile.c_str());
  }

  sql = "DROP TABLE wikilink;";

  rc = sqlite3_exec(db, sql.c_str(),
    [&] (void *params, int argc, char**Argv, char **AzColName ) -> int {
      return 0;
    }, 
    0, &zErrMsg);
  if (rc != SQLITE_OK) {
    log_err("Table drop at %s was unsuccessful", dbfile.c_str());
    sqlite3_free(zErrMsg);
  }
  else {
    log_info("Table %s dropped successfully", dbfile.c_str());
  }

  sqlite3_close(db);
}



void parallel_loaddb() {

  auto create_and_load = [] (const std::string& dbfile, const std::string& dbName, const std::string& newDBFile) {
    createdb (newDBFile); // Create the sqllite file
    loaddb_file (dbfile, dbName, newDBFile);

  };

  // Iterate over all the files and add a parallel call to create the database file


  std::vector<std::future<void>> futures;

  // TODO fix the loading files

  char file[50], fName[8],dbfName[50];
  const char* filePath = "/data/wikilinks/context-only/%03d.gz";
  const char* fileName = "%03d.gz";
  const char* dbFileName = "/data/wikilinks/context-only/%03d.db";
  
  for (int i = 1; i < 110; ++i) {
    snprintf (file, 50, filePath, i);
    snprintf (fName, 8, fileName, i);
    snprintf (dbfName, 50, dbFileName, i);

    futures.push_back(
      std::async (std::launch::async|std::launch::deferred,
                  create_and_load, file, fName, dbfName));
  }

  int counter = 0;
  for (auto &f: futures) {
    f.get();
    log_info("Running number: %d", counter++);
  }

  log_info("Finished all %d!!", counter);

}



////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////

int main (int argc, char** argv) {
  
  using namespace boost::program_options;
  
  // Programs
  bool opt_hello_world = false;
  bool opt_compute_histogram = false;
  bool opt_createdb = false;
  bool opt_loaddb = false;
  bool opt_dropdb = false;
  bool opt_parallel_load = false;
  std::string dbfile = "/data/wikilinks/wikilinks.db";
  
  boost::program_options::options_description desc("Using the wikilink data set.");
  desc.add_options()
    ("help,h", "Print a helpul help message")
    ("helloworld,w", value<bool>(&opt_hello_world)->default_value(false),
      "The hello world program to get if we can we the wikilinks data set")
    ("histogram,h", value<bool>(&opt_compute_histogram)->default_value(false),
      "Compute a histogram of the wikilink data set")
    ("createdb", value<bool>(&opt_createdb)->zero_tokens(),
      "Create am sqllite3 database file (-f) that contains the wikilink data set.")
    ("loaddb", value<bool>(&opt_loaddb)->zero_tokens(),
      "Loads wikilink data into the database file (-f).")
    ("dropdb", value<bool>(&opt_dropdb)->zero_tokens(),
      "Drops the wikilink database file (-f). It is performed first so it can be used with other commands.")
    ("parload,p", value<bool>(&opt_parallel_load)->zero_tokens(),
      "Loads the db files in separate files in parallel.")
    ("dbfile,f", value<std::string>(&dbfile)->default_value("/data/wikilinks/wikilinks.db"),
      "The file of the wikilink database.");
  
  variables_map vm;
  try {
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);
    if (vm.count("help") ) {
      logInfo(desc);
      return 0;
    }

  }
  catch (boost::program_options::error &e) {
    log_err("Bad parameters");
    logInfo(desc);
    return 1;
  }


  if (opt_dropdb)
    dropdb(dbfile);

  if (opt_hello_world)
    hello_world();

  if (opt_compute_histogram)
    compute_histogram();

  if (opt_createdb)
    createdb(dbfile);

  if (opt_loaddb)
    loaddb(dbfile);

  if (opt_parallel_load)
    parallel_loaddb();


  return 0;
}




