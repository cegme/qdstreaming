
#include "Entity.h"
#include "ER.h"
#include "Random.h"
#include "Util.h"
#include "WikiLinkFile.h"
#include "WikiLinkUtil.hpp"

#include <boost/program_options.hpp>
#include <functional>
#include <future>
#include <thread>

void InitializeEntity() {
  // TODO pass in parameters to initialize 

  /*WikiLinkFile wlf("/data/wikilinks/context-only/001.gz", true);
  while (wlf.hasNext()) {

    auto wli = wlf.next();

  }*/

}

bool experiment1 () {

  log_info("ReadEntityFile");
  //auto entities = ReadEntityFile("WikiLinkStart.data.bin");
  auto entities = ReadEntityFile("WikiLinkSingleton.data.bin");
  //auto entities2 = ReadEntityFile("WikiLinkStart.data.bin", true);

  auto er = dsr::ER(&entities);

  double sampletime = 0;
  clock_t toc, tic;

  tic = clock();
  log_info("Start mcmc 10 iterations");
  er.mcmc(10);
  toc = clock();
  sampletime = (toc - tic)/10;
  log_info(">>> Time: %f ", sampletime);

  tic = clock();
  log_info("Start mcmc 10000 iterations (parallel 100x100)");
  auto f = [&er](unsigned int i, unsigned int num) {
    std::this_thread::sleep_for (std::chrono::milliseconds(RandInt() % 1000));
    auto littletic = clock();
    er.mcmc(i);
    auto littletoc = clock();
    log_info("\t{%u, %u, %lu}", i, num, (littletoc-littletic)/i);
    //std::cerr << "Thread: " << std::this_thread::get_id() << std::endl;
  };

  std::vector<std::future<void>> pool;
  for (unsigned int t = 0; t < 100; ++t) {
    pool.push_back(std::async(std::launch::async, f, 100, t));
  }
  for (auto &t: pool){
    t.wait();
  }
  toc = clock();
  sampletime = (toc - tic)/10000;
  log_info(">>> Time: %f ", sampletime);

  tic = clock();
  log_info("Start mcmc 1000 iterations");
  er.mcmc(1000);
  toc = clock();
  sampletime = (toc - tic)/1000;
  log_info(">>> Time: %f ", sampletime);

  tic = clock();
  log_info("Start mcmc 10000 iterations");
  er.mcmc(10000);
  toc = clock();
  sampletime = (toc - tic)/10000;
  log_info(">>> Time: %f ", sampletime);
  /*log_info("Start mcmc 100000 iterations");
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
  MyStats startStats;
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
  CreateSingltonInitFile("WikiLinkSingleton.data.bin");


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

