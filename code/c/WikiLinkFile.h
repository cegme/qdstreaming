#ifndef WIKILINK_FILE_H
#define WIKILINK_FILE_H

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TFDTransport.h>
#include <thrift/transport/TBufferTransports.h>

#include <boost/shared_ptr.hpp>

#include "gen-cpp/wikilink_constants.h"
#include "gen-cpp/wikilink_types.h"


/**
  * This class takes a wikilink zip file, extracts it and allows to a temporary
  *location and allows iteration over the items inside. 
  */
class WikiLinkFile {

  public:
    WikiLinkFile(std::string filename): WikiLinkFile(filename, true) { }
    WikiLinkFile(std::string _filename, bool _compressed): filename(_filename), thetmpname(""), started(false), finished(false), compressed(_compressed) {
      init();
    }

    bool empty() const { return finished; }
    bool hasNext() const { return !empty(); }
    WikiLinkItem next();

    // Looks forward for the next item with this docid
    WikiLinkItem* nextDocID(int docid);

    // Looks forward for this docid and the indexed Mentions
    Mention* nextDocIDMentionIndex(int docid, int mention_idx);

  private:
    // Performs the file copy and initializes the pointer
    void init();
    void loadNext();
  
  private:
    // Is the file compressed
    bool compressed;
    // The main file that was passed in
    std::string filename;
    // The temporary location the file the passed to
    std::string thetmpname;
    // The current wikilkink item
    WikiLinkItem current;
    // The next wikilink item. The next one is always held if available
    WikiLinkItem nextItem;
    bool started; // Starts the init() call
    bool finished;
    boost::shared_ptr<apache::thrift::transport::TFDTransport> transportInput;
    boost::shared_ptr<apache::thrift::transport::TBufferedTransport> buffTransportInput;
    boost::shared_ptr<apache::thrift::protocol::TBinaryProtocol> protocolInput;

};

#endif
