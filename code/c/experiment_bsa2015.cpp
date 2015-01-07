
#include "Entity.h"
#include "ER.h"
#include "Random.h"
#include "Util.h"
#include "WikiLinkFile.h"
#include "WikiLinkUtil.hpp"

#include <boost/program_options.hpp>

void InitializeEntity() {
  // TODO pass in parameters to initialize 

  /*WikiLinkFile wlf("/data/wikilinks/context-only/001.gz", true);
  while (wlf.hasNext()) {

    auto wli = wlf.next();

  }*/

}

bool experiment1 () {

  log_info("ReadEntityFile");
  auto entities = ReadEntityFile("WikiLinkStart.data.bin");
  //auto entities2 = ReadEntityFile("WikiLinkStart.data.bin", true);
  log_info("ComputeStats");
  MyStats startStats;
  startStats.ComputeStats(entities, "WikiLinkStart.data.bin");
  log_info("[Start] %s", startStats.tostring().c_str());
  log_info("[Start] Precision: %f, Recall: %f, F1: %f", startStats.pairPrecision(), startStats.pairRecall(), startStats.pairF1());
  log_info("[Start] %s", startStats.tostring().c_str());

  auto er = dsr::ER(&entities);

  log_info("Start mcmc 10 iterations");
  er.mcmc(10);
  log_info("Start mcmc 100 iterations");
  er.mcmc(100);
  log_info("Start mcmc 1000 iterations");
  er.mcmc(1000);
  /*log_info("Start mcmc 10000 iterations");
  er.mcmc(10000);
  log_info("Start mcmc 100000 iterations");
  er.mcmc(100000);
  log_info("Start mcmc 1000000 iterations");
  er.mcmc(1000000);
  log_info("Start mcmc 10000000 iterations");
  er.mcmc(100000000);
  log_info("Start mcmc 100000000 iterations");
  er.mcmc(1000000000);
  log_info("Start mcmc 1000000000 iterations");
  er.mcmc(10000000000);*/

  log_info("ComputeStats");
  startStats.ComputeStats(entities, "WikiLinkStart.data.bin");
  log_info("[Start] %s", startStats.tostring().c_str());
  log_info("[Start] Precision: %f, Recall: %f, F1: %f", startStats.pairPrecision(), startStats.pairRecall(), startStats.pairF1());
  log_info("[Start] %s", startStats.tostring().c_str());

  return true;
}

bool experiment2 () {
  return true;
}

bool experiment3 () {
  return true;
}


int main (int argc, char** argv) {

  namespace po = boost::program_options;

  // Create the Ground Truth
  //CreateGroundTruthFile ("WikiLinkTruth.data.bin"); // From WikiLinkUtil.hpp
  //CreateStartFile ("WikiLinkStart.data.bin"); // From WikiLinkUtil.hpp
 
  if(false){
    log_info("ReadEntityFile");
    auto entities1 = ReadEntityFile("WikiLinkTruth.data.bin");
    //auto entities1 = ReadEntityFile("WikiLinkTruth.data.bin", true);
    log_info("ComputeStats");
    MyStats trueStats;
    trueStats.ComputeStats(entities1, "WikiLinkTruth.data.bin");
    log_info("[True] %s", trueStats.tostring().c_str());
    log_info("[True] Precision: %f, Recall: %f, F1: %f", trueStats.pairPrecision(), trueStats.pairRecall(), trueStats.pairF1());
    log_info("[True] %s", trueStats.tostring().c_str());
  }

  if(false){
    log_info("ReadEntityFile");
    auto entities2 = ReadEntityFile("WikiLinkStart.data.bin");
    //auto entities2 = ReadEntityFile("WikiLinkStart.data.bin", true);
    log_info("ComputeStats");
    MyStats startStats;
    startStats.ComputeStats(entities2, "WikiLinkStart.data.bin");
    log_info("[Start] %s", startStats.tostring().c_str());
    log_info("[Start] Precision: %f, Recall: %f, F1: %f", startStats.pairPrecision(), startStats.pairRecall(), startStats.pairF1());
    log_info("[Start] %s", startStats.tostring().c_str());
  }


  // Experiment 1 --- 
  // -- Regular sampling
  // -- Sampling with the best early stopping and best compression
  experiment1();


  // Micro benchmarks ==============

  // Experiment 2 --- Sampling with different compression ratios
  experiment2();


  // Experiment 3 --- ER with different early stopping ratios
  experiment3();


  return 0;
}

