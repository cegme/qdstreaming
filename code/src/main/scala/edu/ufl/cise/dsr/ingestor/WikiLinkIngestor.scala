
package edu.ufl.cise.dsr.ingestor

import java.util.zip.GZIPInputStream
import java.io.BufferedInputStream
import java.io.ByteArrayInputStream
import java.io.File
import java.io.FileInputStream
import java.io.FilenameFilter
import java.lang.ClassLoader
import java.nio.file.Files

import org.tukaani.xz.XZInputStream
//import org.apache.commons.compress.compressors.xz.XZCompressorInputStream
import org.apache.thrift.protocol.TBinaryProtocol
import org.apache.thrift.transport.TIOStreamTransport
import org.apache.thrift.transport.TTransportException
import org.apache.thrift.transport.TFileTransport
import org.apache.thrift.transport.TStandardFile
import org.apache.thrift.protocol.TCompactProtocol
import org.apache.thrift.protocol.TProtocol
import org.apache.thrift.protocol.TBinaryProtocol

import edu.ufl.cise.dsr.MyLogging

import edu.umass.cs.iesl.wikilink.expanded.data.WikiLinkItem
//import edu.umass.cs.iesl.wikilink.expanded.process.ThriftSerializerFactory

class WikiLinkIngestor extends Ingestor[WikiLinkItem,WikiLinkFolders] with MyLogging {

  override lazy val streamIterator = new WikiLinkFolders

  def hasNext = streamIterator.streamItems.hasNext

  def next:WikiLinkItem = streamIterator.streamItems.next

  // TODO add vectorize

}



class WikiLinkFolders extends MyLogging {

  // This is the small version
  val compressedFiles = "/data/d04/wikilinks/content-only/"

  lazy val streamItems:Iterator[WikiLinkItem] = {
    gzFiles.flatMap{ gzFile => 
      logDebug("Extracting Wikilinks from file: %s".format(gzFile.getName)) 


      

      //val (stream, protocol) = ThriftSerializerFactory
        //.getReader(gzFile)

      val stream = new BufferedInputStream(new GZIPInputStream(new FileInputStream(gzFile)), 2048)
      val protocol = new TBinaryProtocol(new TIOStreamTransport(stream))
            (stream, protocol)


      Iterator.continually(mkWikiItem(protocol))
        .takeWhile(_ match { 
          case Some(x) => true
          case None    => {stream.close; false}
        })
    }
    .flatten
    .toIterator
  }

  lazy val gzFiles:Iterator[File] = {
    new File(compressedFiles).listFiles( new FilenameFilter() {
      def accept(dir:File, name:String):Boolean = name.endsWith(".gz")
    })
    .sortBy(f => f.getName)
    .toIterator
  }
    
  def mkWikiItem(protocol:TProtocol):Option[WikiLinkItem] = {

    var w:WikiLinkItem = null
    var successful = false
    try {
      w = WikiLinkItem.decode(protocol)
      //w.read(protocol)
      successful = true
    } catch {
      case e:java.lang.OutOfMemoryError => logError("OOM Error: %s".format(e.getStackTrace.mkString("\n"))); None
      case e:TTransportException => e.getType match { 
        case TTransportException.END_OF_FILE => /*logDebug("wikiLinkStream Finished.");*/ None
        case TTransportException.ALREADY_OPEN => logError("wikiLinkStream already opened."); None
        case TTransportException.NOT_OPEN => logError("wikiLinkStream not open."); None
        case TTransportException.TIMED_OUT => logError("wikiLinkStream timed out."); None
        case TTransportException.UNKNOWN => logError("wikiLinkStream unknown."); None
        case e => logError("Error in wikiLinkStreamItem: %s".format(e.toString)); None
      }
      case e: Exception => logDebug("Error in wikiLinkStreamItem"); None
    }
    if (successful) Some(w) else None

  }

}



