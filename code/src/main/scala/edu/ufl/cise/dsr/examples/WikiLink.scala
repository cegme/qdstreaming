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

import org.apache.spark.sql.SQLContext
import org.apache.spark.sql._

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

import java.nio.ByteBuffer



import MyWikiLinkItem._
object MyWikiLinkItem {
  import scala.language.implicitConversions
  type WLI = edu.umass.cs.iesl.wikilink.expanded.data.WikiLinkItem
  implicit def optionStringtoString(s: Option[String]) = s match {
    case Some(s) => s
    case None => ""
   }
  //implicit def bytebuffer2Option(b:java.nio.ByteBuffer) = Some(b)
  implicit def context2Option(c:Context) = Some(c)
  def smallTuple(w:edu.umass.cs.iesl.wikilink.expanded.data.WikiLinkItem ) = {
    w.docId 
  }
  def apply(w:edu.umass.cs.iesl.wikilink.expanded.data.WikiLinkItem ) = 
    new MyWikiLinkItem(
      w.docId, 
      w.url/*, 
      PageContentItem(w.content.raw match {
                        case Some(b) => b.array()
                        case None    => Array[Byte]()},
                      w.content.fullText,
                      w.content.articleText,
                      w.content.dom),
      w.rareWords.map{ r =>
        RareWord(r.word, r.offset) },
      w.mentions.map{ m =>
        Mention(m.wikiUrl,
                m.anchorText,
                m.rawTextOffset,
                m.context match {
                  case Some(c) => Context(c.left, c.right, c.middle)
                  case None => Context("", "", "")},
                m.freebaseId) }
    */)
}

case class MyWikiLinkItem(
  val docId: Int,
  val url: String//,
  //val content: PageContentItem,
  //val rareWords: Seq[RareWord] = Seq[RareWord](),
  //val mentions: Seq[Mention] = Seq[Mention]()
  )
case class Mention(
  val wikiUrl: String,
  val anchorText: String,
  val rawTextOffset: Int,
  val context: Context,
  val freebaseId: String)
case class RareWord(
  val word: String,
  val offset: Int)
case class Context(
  val left: String,
  val right: String,
  val middle: String)
case class PageContentItem(
  val raw: Array[Byte],
  val fullText: String,
  val articleText: String,
  val dom: String)





object WikiLink extends MyLogging {

  // TODO: Try Parque thttps://groups.google.com/forum/#!topic/parquet-dev/8Ei4IVKXgoc

  def MakeRDD(sc:SparkContext, file:String = "/data/d04/wikilinks/content-only/001.gz") = {

    val (stream, protocol) = ThriftSerializerFactory
      .getReader(new java.io.File(file))

    val theItems = 
      Iterator.continually(getWikiItem(protocol))
        .takeWhile(_ match { 
          case Some(x) => true
          case None    => {stream.close; false}
        })
        .withFilter(_!=None)
        .flatMap {wli => wli}
        .map { wli => MyWikiLinkItem(wli) }
        //.map { wli => MyWikiLinkItem.smallTuple(wli.get) }
        .sliding(9,9) // Group the items
        .toSeq
        .take(11111)

    //var rdd = sc.emptyRDD[WikiLinkItem]
    //var rdd = sc.makeRDD(Seq(theItems.next))
    //var rdd = sc.makeRDD(theItems.next.toSeq)
    //rdd.persist(StorageLevel.MEMORY_AND_DISK)
    logInfo("The items group size %d".format(theItems.size))


    val rdd = {
      import scala.language.implicitConversions
      implicit def wikiitem2rdd(x:MyWikiLinkItem) = sc.makeRDD(Seq(x))
      implicit def wikiseq2rdd(x:Seq[MyWikiLinkItem]) = sc.makeRDD(x)
      val rdd = sc.makeRDD(theItems.take(11110).reduce(_ ++ _) )
      logInfo("RDD Type: %s".format(rdd.toString))
      rdd.foreach(x => println("First: %s".format(x)))
      rdd
    }
    /*for (itemGroup <- theItems) {
      rdd = rdd.union(sc.makeRDD(Seq(itemGroup)))
      rdd.cache
    }*/
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
    import MySpark.sqlsc._

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
    itemsRDD.cache
    //itemsRDD.saveAsParquetFile("wikilinks.parquet")

    logInfo("Counting the total items...")
    val count1 = itemsRDD.count()
    logInfo(s"Total item count is $count1")

    //logInfo("Reading ParquetFile")
    //val parquetFile = MySpark.sqlsc.parquetFile("wikilinks.parquet")
    //parquetFile.registerAsTable("parquetFile")
    itemsRDD.registerAsTable("itemsRDD")
    val count = sql("SELECT * from itemsRDD LIMIT 10")
      count.collect().foreach(println)


   logInfo("The itemsRDD count: %d".format(itemsRDD.count))

    /*val edgeRdd =
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
    */
    
    }


  }
