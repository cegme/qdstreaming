
#include <iostream>
#include <string>
#include <vector>

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TFDTransport.h>
#include <thrift/transport/TFileTransport.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TZlibTransport.h>


#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include <fcntl.h>

#include "gen-cpp/wikilink_constants.h"
#include "gen-cpp/wikilink_types.h"

#include "Util.h"




std::vector<WikiLinkItem> 
FileToWikiLinkItem(std::string filePath) {

  namespace att = apache::thrift::transport;
  namespace atp = apache::thrift::protocol;

  std::vector<WikiLinkItem> wlis;

  int fd = open(filePath.c_str(), O_RDONLY);
  if (fd == -1) { log_err("Error opening file %s", filePath.c_str()); return wlis; }

  boost::shared_ptr<att::TFDTransport> transportInput(new att::TFDTransport(fd, att::TFDTransport::ClosePolicy::CLOSE_ON_DESTROY));
  boost::shared_ptr<att::TBufferedTransport> buffTransportInput(new att::TBufferedTransport(transportInput));
  boost::shared_ptr<atp::TBinaryProtocol> protocolInput(new atp::TBinaryProtocol(transportInput));
  //boost::shared_ptr<att::TZlibTransport> z_trans(new att::TZlibTransport(transportInput));
  //boost::shared_ptr<atp::TBinaryProtocol> protocolInput(new atp::TBinaryProtocol(z_trans));

  while (true) {
    try {

      WikiLinkItem wikilink_item;
      wikilink_item.read(protocolInput.get()); 

      wlis.push_back(wikilink_item);
    }
    catch (att::TTransportException &e) {
      bool eof = false;
      switch(e.getType()) {
        case att::TTransportException::END_OF_FILE:
          eof = true;
          break;
        case att::TTransportException::UNKNOWN: log_err("TTransportException: Unknown transport exception"); return wlis;
        case att::TTransportException::NOT_OPEN: log_err("TTransportException: Transport not open"); return wlis;
        case att::TTransportException::TIMED_OUT: log_err("TTransportException: Timed out"); return wlis;
        case att::TTransportException::INTERRUPTED: log_err("TTransportException: Interrupted"); return wlis;
        case att::TTransportException::BAD_ARGS: log_err("TTransportException: Invalid arguments"); return wlis;
        case att::TTransportException::CORRUPTED_DATA: log_err("TTransportException: Corrupted Data"); return wlis;
        case att::TTransportException::INTERNAL_ERROR: log_err("TTransportException: Internal error"); return wlis;
        default: log_err("TTransportException: (Invalid exception type)"); break;
      }
      if(eof) break;
    }
    catch (atp::TProtocolException &e) {
      log_err("Protocol has a negative size");
      break;
    }
  }

  return wlis;
}



int main (int argc, char** argv) {
  //std::cout << "Hello world with thrift";

  auto wlis = FileToWikiLinkItem("/data/wikilinks/002.gz");

  for (auto& w : wlis) {
    std::cout << "|" << w.doc_id << ", " << w.url << "\n";

  }
  


  return 0;
}




