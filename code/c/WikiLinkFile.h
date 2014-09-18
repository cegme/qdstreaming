#ifndef WIKILINK_FILE_H
#define WIKILINK_FILE_H

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TFDTransport.h>
#include <thrift/transport/TBufferTransports.h>

#include <boost/shared_ptr.hpp>

#include "gen-cpp/wikilink_constants.h"
#include "gen-cpp/wikilink_types.h"


class WikiLinkFile {

  public:
    WikiLinkFile(std::string filename): filename(filename), thetmpname(""), started(false), finished(false) {
      init();
    }

    bool empty() const { return finished; }
    bool hasNext() const { return !empty(); }
    WikiLinkItem next();

  private:
    void init();
    void loadNext();
  
  private:
    std::string filename;
    std::string thetmpname;
    WikiLinkItem current;
    WikiLinkItem nextItem;
    bool started;
    bool finished;
    boost::shared_ptr<apache::thrift::transport::TFDTransport> transportInput;
    boost::shared_ptr<apache::thrift::transport::TBufferedTransport> buffTransportInput;
    boost::shared_ptr<apache::thrift::protocol::TBinaryProtocol> protocolInput;

};

#endif
