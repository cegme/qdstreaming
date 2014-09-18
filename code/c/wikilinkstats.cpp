
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <unistd.h>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/iostreams/filter/zlib.hpp>


#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TJSONProtocol.h>
#include <thrift/protocol/TDenseProtocol.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/transport/TFDTransport.h>
#include <thrift/transport/TFileTransport.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TZlibTransport.h>
#include <thrift/transport/TSimpleFileTransport.h>
#include <thrift/transport/TBufferTransports.h>


#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include <fcntl.h>
//#include "zlib.h"

#include "gen-cpp/wikilink_constants.h"
#include "gen-cpp/wikilink_types.h"

#include "Util.h"
#include "WikiLinkFile.h"


std::vector<WikiLinkItem> FileToWikiLinkItem(std::string filePath) {
  // FIXME this code explods because the vector grows so large, will remove soon


  using namespace apache::thrift::transport;
  using namespace apache::thrift::protocol;
  using namespace boost;

  std::vector<WikiLinkItem> wlis;

  /*int fd = open(filePath.c_str(), O_RDONLY);
  if (fd == -1) { log_err("Error opening file %s", filePath.c_str()); return wlis; }

  // Try this: http://www.boost.org/doc/libs/1_56_0/libs/iostreams/doc/classes/gzip.html
  // then use a binary input transport
  //std::ifstream file(filePath, std::ios_base::in | std::ios_base::binary);

  shared_ptr<TFDTransport> transportInput(new TFDTransport(fd, TFDTransport::ClosePolicy::CLOSE_ON_DESTROY));
  //shared_ptr<TBufferedTransport> buffTransportInput(new TBufferedTransport(transportInput));
  //shared_ptr<TZlibTransport> z_trans(new TZlibTransport(transportInput));
  //shared_ptr<TBufferedTransport> buffTransportInput(new TBufferedTransport(z_trans));
  shared_ptr<TBinaryProtocol> protocolInput(new TBinaryProtocol(transportInput));*/

  char cmd[512+L_tmpnam+L_tmpnam]; // The command to decrypt 
  char fname [512+L_tmpnam]; // the tmp file name
  char fullpath [512+L_tmpnam]; // The full path of the file name 
  
  // Get a temp file name
  snprintf(fname, 512+L_tmpnam, "%sXXXXXX", filePath.c_str());
  int fd = mkstemp(fname);
  //tmpnam(fname);

  // Copy commands to 'cmd' variable
  snprintf(cmd, L_tmpnam+512, "gunzip --stdout %s > %s", filePath.c_str(), fname);
  log_info("Performing command: %s", cmd);

  // Unzip the file to a tmp file
  auto good = system(cmd);
  if(good == -1) { log_err("Error copying the command: '%s'", cmd); return wlis; }

  
  //int fd = open("/data/wikilinks/001.gzCTrUr2", O_RDONLY);
  shared_ptr<TFDTransport> transportInput(new TFDTransport(fd, TFDTransport::ClosePolicy::CLOSE_ON_DESTROY));
  shared_ptr<TBufferedTransport> buffTransportInput(new TBufferedTransport(transportInput));
  shared_ptr<TBinaryProtocol> protocolInput(new TBinaryProtocol(buffTransportInput));
  //shared_ptr<TBinaryProtocol> protocolInput(new TBinaryProtocol(transportInput));

  while (true) {
    try {

      WikiLinkItem wikilink_item;
      wikilink_item.read(protocolInput.get()); 

      wlis.push_back(wikilink_item);
    }
    catch (TTransportException &e) {
      bool eof = false;
      switch(e.getType()) {
        case TTransportException::END_OF_FILE:
          eof = true;
          break;
        case TTransportException::UNKNOWN: log_err("TTransportException: Unknown transport exception"); return wlis;
        case TTransportException::NOT_OPEN: log_err("TTransportException: Transport not open"); return wlis;
        case TTransportException::TIMED_OUT: log_err("TTransportException: Timed out"); return wlis;
        case TTransportException::INTERRUPTED: log_err("TTransportException: Interrupted"); return wlis;
        case TTransportException::BAD_ARGS: log_err("TTransportException: Invalid arguments"); return wlis;
        case TTransportException::CORRUPTED_DATA: log_err("TTransportException: Corrupted Data"); return wlis;
        case TTransportException::INTERNAL_ERROR: log_err("TTransportException: Internal error"); return wlis;
        default: log_err("TTransportException: (Invalid exception type)"); break;
      }
      if(eof) break;
    }
    catch (TProtocolException &e) {
      log_err("Protocol has a negative size");
      break;
    }
  }

  // Copy commands to 'cmd' variable dont need to wait so ends in ampersand
  snprintf(cmd, L_tmpnam+25, "rm %s &", fname);
  // Delete the tmpfile
  //good = system(cmd);
  //if(good == -1) { log_err("Error deleting the file: '%s'", cmd); return wlis; }
  

  return wlis;
}



int main (int argc, char** argv) {
  //std::cout << "Hello world with thrift";
/*
  //auto wlis = FileToWikiLinkItem("/data/wikilinks/002.gz");
  auto wlis = FileToWikiLinkItem("/data/wikilinks/001.gz");
  //auto wlis = FileToWikiLinkItem("/data/wikilinks/tmp/001");

  for (auto& w : wlis) {
    std::cout << "|" << w.doc_id << ", " << w.url << "\n";

  }*/

  
  WikiLinkFile wlf("/data/wikilinks/001.gz");

  int counter = 0;
  while(wlf.hasNext()) {
    wlf.next();
    if(++counter % 10000 == 0) std::cerr << ".";
  }
  log_info("\n");
  log_info("The number of items: %d", counter);
    


  return 0;
}




