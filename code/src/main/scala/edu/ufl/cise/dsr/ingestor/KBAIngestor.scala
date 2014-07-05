
package edu.ufl.cise.dsr.ingestor

import java.io.ByteArrayInputStream
import java.io.File
import java.lang.ClassLoader
import java.nio.file.Files
import java.util.Date

import edu.ufl.cise.dsr.MyLogging
import edu.ufl.cise.dsr.point.KBAEntityChain
import edu.ufl.cise.dsr.util.Util

import org.tukaani.xz.XZInputStream
//import org.apache.commons.compress.compressors.xz.XZCompressorInputStream
import org.apache.thrift.protocol.TBinaryProtocol
import org.apache.thrift.transport.TIOStreamTransport
import org.apache.thrift.transport.TTransportException
import org.apache.thrift.transport.TFileTransport
import org.apache.thrift.transport.TStandardFile
import org.apache.thrift.protocol.TCompactProtocol

import scala.io.Source
import scala.sys.process.stringToProcess
import scala.sys.process.ProcessLogger
import scala.sys.process._

import streamcorpus.StreamItem


/**
  * This KBAIngestor is really an iterator that grabs data from disk.
  * You may use this as an Iterator to retrieve all the streamItems.
  * 
  * e.g.
  *
  *   for (si in new KBAIngestor()) process(si)
  *
  *
  * TODO: startDate needs to be integrated to fast forward the iterator
  *
  */
class KBAIngestor(startDate:Date = null) extends Ingestor[streamcorpus.StreamItem,KBAFolders] with MyLogging {
  

  /** TODO Implement a fast forwarding startdate */
  def withStartDate(startDate:Date): KBAIngestor = {
    new KBAIngestor(startDate)
  }

  override lazy val streamIterator = new KBAFolders

  def hasNext = streamIterator.streamItems.hasNext

  def next:streamcorpus.StreamItem =  streamIterator.streamItems.next

  def vectorize[KBAEntityChain](si: streamcorpus.StreamItem): Iterator[KBAEntityChain] = {
    // TODO Extract the mentions from the StreamItem
    // NOTE: Each streamitem may have multiple entities 
    null
  }
}


/**
  * A class to access the StreamItem data. Assumes the GPG key has been loaded
  * and it assumes a file structure as seen in the variable gpgFilePathTemplate.
  */
class KBAFolders extends MyLogging {
  val gpgFilePathTemplate = "/data/%s/kba2013/aws-publicdatasets/trec/kba/kba-streamcorpus-2013-v0_2_0-english-and-unknown-language/%s/"

  // FIXME My temp fix until logging is fixed
  /*def logInfo(msg:String) { println(msg) }
  def logDebug(msg:String) { println(msg) }
  def logError(msg:String) { println(msg) }*/

  /** An iterator for streamitems */
  lazy val streamItems:Iterator[streamcorpus.StreamItem] = {
    
    gpgFiles.map { gpgFile =>

      // Decrypt the file and put it in a the baos
      val baos = new java.io.ByteArrayOutputStream(100 * 1024 * 1024)
      val xzGPG = Files.readAllBytes(gpgFile.toPath) 
      ( gpgFile #> 
        "gpg --no-permission-warning --trust-model always --output - --decrypt -" #>
        baos) ! ProcessLogger(line => ())
 
      // Uncompress the decrypted stream
      val is = new ByteArrayInputStream(baos.toByteArray)
      //val bais = new XZCompressorInputStream(is)
      val bais = new XZInputStream(is)

      // Open the transport so we can read the thrift objects
      val transport = new TIOStreamTransport(bais)
      transport.open
      val protocol = new TBinaryProtocol(transport)

      // Iterate over the stream item objects  
      Iterator.continually(mkStreamItem(protocol))
        .takeWhile(_ match { case None => transport.close; is.reset; false; case _ => true })
        .map { _.get }
    }
    .flatten // Combine all the iterators
    .toIterator
  }

  /** Iterate through the gpg files in a folder. */
  lazy val gpgFiles:Iterator[File] = {
    dateFolders.map { dateString =>
      logInfo("dateString: %s".format(dateString))
      val (folder, datehour) = dateString
      val gpgFolderPath = gpgFilePathTemplate.format(folder,datehour)
      logInfo("gpgFolderPath: %s".format(gpgFolderPath))
      new File(gpgFolderPath).listFiles
    }.flatten.toIterator
  }

  /** Use the indexes to find all the date folders locations. 
    * Returns [(folder, hour), ... ].
    */
  lazy val dateFolders:Iterator[(String,String)] = {
    (Iterator.continually("d01") zip Source.fromURL(this.getClass.getResource("/kba2013/d01")).getLines) ++
      (Iterator.continually("d02") zip Source.fromURL(this.getClass.getResource("/kba2013/d02")).getLines) ++
      (Iterator.continually("d03") zip Source.fromURL(this.getClass.getResource("/kba2013/d03")).getLines) ++
      (Iterator.continually("d04") zip Source.fromURL(this.getClass.getResource("/kba2013/d04")).getLines) ++
      (Iterator.continually("d05") zip Source.fromURL(this.getClass.getResource("/kba2013/d05")).getLines) ++
      (Iterator.continually("d06") zip Source.fromURL(this.getClass.getResource("/kba2013/d06")).getLines) ++
      (Iterator.continually("d07") zip Source.fromURL(this.getClass.getResource("/kba2013/d07")).getLines) ++
      (Iterator.continually("d08") zip Source.fromURL(this.getClass.getResource("/kba2013/d08")).getLines)
  }

  /**
   * Creates a StreamItem from a protocol. return an Option[StramItem] just in case
   * for some of them we don't have data we are safe.
   */
  def mkStreamItem(protocol: TBinaryProtocol/*, s:StreamItem = new StreamItem*/): Option[StreamItem] = {
    var s:StreamItem = null
    var successful = false
    try {
      s = StreamItem.decode(protocol) 
      //s.read(protocol)
      successful = true
    } catch {
      case e:java.lang.OutOfMemoryError => logError("OOM Error: %s".format(e.getStackTrace.mkString("\n"))); None
      case e:TTransportException => e.getType match { 
        case TTransportException.END_OF_FILE => /*logDebug("mkstream Finished.");*/ None
        case TTransportException.ALREADY_OPEN => logError("mkstream already opened."); None
        case TTransportException.NOT_OPEN => logError("mkstream not open."); None
        case TTransportException.TIMED_OUT => logError("mkstream timed out."); None
        case TTransportException.UNKNOWN => logError("mkstream unknown."); None
        case e => logError("Error in mkStreamItem: %s".format(e.toString)); None
      }
      case e: Exception => logDebug("Error in mkStreamItem"); None
    }
    if (successful) Some(s) else None
  } 

}


