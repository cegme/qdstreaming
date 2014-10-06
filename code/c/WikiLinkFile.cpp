#include <fcntl.h>


#include <cstdlib>

#include "Util.h"
#include "WikiLinkFile.h"

void WikiLinkFile::init() {
  using namespace apache::thrift::transport;
  using namespace apache::thrift::protocol;
  using namespace boost;

  char cmd[512+L_tmpnam+L_tmpnam]; // The command to decrypt 
  char fname [512+L_tmpnam]; // the tmp file name
  char fullpath [512+L_tmpnam]; // The full path of the file name 
  int fd, good;

  if (compressed) {  

    snprintf(fname, 512+L_tmpnam, "%s_tmp_XXXXXX", filename.c_str());
    fd = mkstemp(fname);

    snprintf(cmd, L_tmpnam+512, "gunzip --stdout %s > %s", filename.c_str(), fname);
    thetmpname = std::string(fname);
    log_info("Performing command: %s", cmd);
    
    good = system(cmd);
    if(good == -1) { log_err("Error copying the command: '%s'", cmd); throw "Could not copy command"; }
    log_info("Command complete! [%s]", filename.c_str());
  }
  else {

    log_info("No compression, opening the file %s", filename.c_str());
    
    fd = open(filename.c_str(), O_RDONLY);
  }
    
  transportInput = shared_ptr<TFDTransport>(new TFDTransport(fd, TFDTransport::ClosePolicy::CLOSE_ON_DESTROY));
  buffTransportInput = shared_ptr<TBufferedTransport>(new TBufferedTransport(transportInput));
  protocolInput = shared_ptr<TBinaryProtocol>(new TBinaryProtocol(buffTransportInput));

  // Load the first Item
  started = true;
  current.read(protocolInput.get());
  // Put the next item in the chamber
  loadNext();

  
}

WikiLinkItem WikiLinkFile::next() { 
  if (empty()) throw "Calling next on empty WikiLinkFile";
  current = nextItem; 
  loadNext();
  return current;

}

void WikiLinkFile::loadNext() {
  // Warning Erases the nextItem
  // The nextItem should have already been send to current
  using namespace apache::thrift::transport;
  using namespace apache::thrift::protocol;
  started = true;
  try{

    nextItem.read(protocolInput.get()); 

  }
  catch (TTransportException &e) {
    bool eof = false;
    switch(e.getType()) {
      case TTransportException::END_OF_FILE:
        finished = true;
        break;
      case TTransportException::UNKNOWN: log_err("TTransportException: Unknown transport exception"); finished = true; break;
      case TTransportException::NOT_OPEN: log_err("TTransportException: Transport not open"); finished = true; break;
      case TTransportException::TIMED_OUT: log_err("TTransportException: Timed out"); finished = true; break;
      case TTransportException::INTERRUPTED: log_err("TTransportException: Interrupted"); finished = true; break;
      case TTransportException::BAD_ARGS: log_err("TTransportException: Invalid arguments"); finished = true; break;
      case TTransportException::CORRUPTED_DATA: log_err("TTransportException: Corrupted Data"); finished = true; break;
      case TTransportException::INTERNAL_ERROR: log_err("TTransportException: Internal error"); finished = true; break;
      default: log_err("TTransportException: (Invalid exception type)"); break;
    }
  }
  catch (TProtocolException &e) {
    log_err("Protocol has a negative size");
    finished = true;
  }
  if (finished == true && compressed)  {
    // Delete the file
    char cmd[512+L_tmpnam+L_tmpnam];
    snprintf(cmd, 512+L_tmpnam+L_tmpnam, "rm %s &", thetmpname.c_str());
    system(cmd);
  }

}


WikiLinkItem* WikiLinkFile::nextDocID(int docid) {

  do {
    if (current.doc_id == docid) 
      return &current;
    else 
      next();
  } while (hasNext());

  return (WikiLinkItem*)-1;
}


Mention* WikiLinkFile::nextDocIDMentionIndex(int docid, int mention_idx) {
  WikiLinkItem* w = nextDocID(docid);

  if (w == (WikiLinkItem*)-1) {
    return (Mention*)-1;
  }
  else {
    return &w->mentions[mention_idx];
  }

}








