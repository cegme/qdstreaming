package edu.ufl.cise.dsr.examples


import org.apache.spark.rdd.RDD

import org.apache.spark.graphx._

import org.apache.spark.SparkConf
import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.storage.StorageLevel
import org.apache.spark.streaming._
import org.apache.spark.streaming.StreamingContext._
import org.apache.spark.streaming.Duration._


import org.apache.thrift.protocol.TBinaryProtocol
import org.apache.thrift.transport.TIOStreamTransport
import org.apache.thrift.transport.TTransportException
import org.apache.thrift.transport.TFileTransport
import org.apache.thrift.transport.TStandardFile
import org.apache.thrift.protocol.TCompactProtocol

import scala.collection.JavaConversions._

import edu.ufl.cise.dsr.ingestor.KBAIngestor
import edu.ufl.cise.dsr.MyLogging
import edu.ufl.cise.dsr.util.MySpark

import streamcorpus.StreamItem

import org.apache.thrift.protocol.TProtocol
import org.apache.thrift.protocol.TBinaryProtocol

import edu.umass.cs.iesl.wikilink.expanded.data.WikiLinkItem
import edu.umass.cs.iesl.wikilink.expanded.process.ThriftSerializerFactory

object WikiLink extends MyLogging {

  // TODO: Try Parque thttps://groups.google.com/forum/#!topic/parquet-dev/8Ei4IVKXgoc

  def MakeRDD(sc:SparkContext, file:String = "/data/d04/wikilinks/content-only/001.gz"):RDD[WikiLinkItem] = {

    val (stream, protocol) = ThriftSerializerFactory
      .getReader(new java.io.File(file))

    val theItems = 
      Iterator.continually(getWikiItem(protocol))
        .takeWhile(_ match { case None => stream.close; false; case _ => true})
        .map { _.get }
        .sliding(1000,1000) // Group the items

    //var rdd = sc.emptyRDD[WikiLinkItem]
    //var rdd = sc.makeRDD(Seq(theItems.next))
    var rdd:RDD[WikiLinkItem] = sc.makeRDD(theItems.next.toSeq)
    rdd.persist(StorageLevel.MEMORY_AND_DISK)

    for (itemGroup <- theItems) {
      rdd = rdd.union(sc.makeRDD(itemGroup))
    }

    rdd
  }

  def getWikiItem(protocol:TProtocol):Option[WikiLinkItem] = {

    var w:WikiLinkItem = null
    var successful = false
    try {
      w = WikiLinkItem.decode(protocol)
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

  def main(args: Array[String]) {

    val sc = MySpark.sc

    // Loop through all the WikiLinkItems
    /*val (stream, protocol) = ThriftSerializerFactory
      .getReader(new java.io.File("/data/d04/wikilinks/content-only/001.gz"))

    val theItems = 
      Iterator.continually(getWikiItem(protocol))
        .takeWhile(_ match { case None => stream.close; false; case _ => true})
        .map { _.get }
    */
    //logInfo("Total number of wikiitems: " + theItems.count(w => true))

    def titleHash(s:String) = s match {
      case s:String => s.toLowerCase.hashCode
      case _ => 0
    }

    // Create RDD for Edges
    logInfo("Retrieving items to add to the RDD")
    //val stuff = theItems.toArray
    //val itemsRDD = sc.parallelize(stuff)
   // val itemsRDD = sc.parallelize(theItems.take(10000).toSeq)
   val itemsRDD = MakeRDD(sc, "/data/d04/wikilinks/content-only/001.gz")


    //itemsRDD.persist(StorageLevel.MEMORY_AND_DISK)
    itemsRDD.persist(StorageLevel.MEMORY_ONLY_SER)
    //itemsRDD.persist(StorageLevel.MEMORY_AND_DISK_SER)
    //itemsRDD.persist(StorageLevel.DISK_ONLY)
    //itemsRDD.persist(StorageLevel.MEMORY_ONLY_2)
    //itemsRDD.persist(StorageLevel.MEMORY_AND_DISK_2)

    val edgeRdd =
      itemsRDD.flatMap{w => 
        w.mentions.flatMap{m => m.freebaseId match {
          case Some(fbid) => Some(Edge(titleHash(m.wikiUrl), titleHash(fbid), 1.0)) 
          case _ =>          None
        }
      }
    }

      val defaultVertex = "defaultVertex"

      logInfo("Loading the Graph...")
      val graph = Graph.fromEdges(edgeRdd, defaultVertex)
      logInfo("Graph has been loaded.")
      //logInfo("Total connected Components %d".format(graph.vertices.count))
      logInfo("-"*160)

      //def max(a: (VertexId, Int), b: (VertexId, Int)): (VertexId, Int) = { if (a._2 > b._2) a else b }
      //val maxDegrees = graph.degrees.reduce(max)
      //logInfo(s"The maxDegrees of the graph is $maxDegrees")

      //logInfo("The number of Vertices %d".format(graph.numVertices))
      //logInfo("The number of Edges %d".format(graph.numEdges))
      

    }


  }
