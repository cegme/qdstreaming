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
import edu.ufl.cise.dsr.ingestor.WikiLinkIngestor
import edu.ufl.cise.dsr.MyLogging
import edu.ufl.cise.dsr.point.WikiLinkMention
import edu.ufl.cise.dsr.util.MySpark

import streamcorpus.StreamItem

import org.apache.thrift.protocol.TProtocol
import org.apache.thrift.protocol.TBinaryProtocol

import edu.umass.cs.iesl.wikilink.expanded.data.WikiLinkItem
import edu.umass.cs.iesl.wikilink.expanded.process.ThriftSerializerFactory

import java.nio.ByteBuffer

object WikiLink extends MyLogging with App {

  // TODO: Try Parque thttps://groups.google.com/forum/#!topic/parquet-dev/8Ei4IVKXgoc

  //def main(args: Array[String]) {

    val sc = MySpark.sc
    import MySpark.sqlsc._

    def titleHash(s:String) = s match {
      case s:String => s.toLowerCase.hashCode
      case _ => 0
    }

    // Create RDD for Edges
    logInfo("Retrieving items to add to the RDD")
    //val stuff = theItems.toArray
    //val itemsRDD = sc.parallelize(stuff)
    // val itemsRDD = sc.parallelize(theItems.take(10000).toSeq)
    val itemsRDD = MySpark.sc.makeRDD(
      (new WikiLinkIngestor).take(10).flatMap(WikiLinkMention.getMentions).toSeq
    )
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
    
    MySpark.sc.stop()
    //}


  }
