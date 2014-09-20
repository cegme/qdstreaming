
#include "Util.h"
#include "WikiLinkFile.h"

#include <fstream>
#include <unordered_map>

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


int main (int argc, char** argv) {
  
  //hello_world();

  compute_histogram();


  return 0;
}




