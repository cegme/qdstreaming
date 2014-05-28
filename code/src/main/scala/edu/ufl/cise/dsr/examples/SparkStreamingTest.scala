
package edu.ufl.cise.dsr.examples

import akka.actor.ActorSystem
//import akka.actor.Props

import org.apache.spark.rdd.RDD

import org.apache.spark.SparkConf
import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._

import org.apache.spark.streaming._
import org.apache.spark.streaming.StreamingContext._
import org.apache.spark.streaming.Duration._
//import org.apache.spark.streaming.receiver.ActorReceiver
//import org.apache.spark.streaming.receiver.Receiver

import scala.collection.JavaConversions._

import edu.ufl.cise.dsr.ingestor.KBAIngestor
import edu.ufl.cise.dsr.MyLogging

import streamcorpus.StreamItem

object SparkStreamingTest extends MyLogging {
//object SparkStreamingTest extends  org.apache.spark.Logging {
  
  def logInfo(msg:String) { println(msg) }
  def logDebug(msg:String) { println(msg) }
  def logError(msg:String) { println(msg) }

      def main(args:Array[String]) {
      //val system = ActorSystem("spark")
      //val master = "spark://localhost:7077"
      val master = "local[16]"

      logInfo(s"Starting at the master: $master")
      val YOUR_SPARK_HOME= "/home/cgrant/projects/spark/"
      val conf = new SparkConf()
                  .setMaster(master)
                  .setAppName("SSTest")
                  .set("spark.executor.memory", "5g")
                  .setSparkHome(s"$YOUR_SPARK_HOME")
                  .set("akka.version", "2.2.3")
                  //.set("spark.locality.wait", "10000")
                  //.set("spark.akka.failure-detector.threshold", "3000.0")
                  //.set("spark.akka.heartbeat.interval", "5000")
                  //.set("spark.akka.heartbeat.pauses", "3000")
      val sc = new SparkContext(conf)
      /*val sc = new SparkContext(master, "SSTest", 
                                s"$YOUR_SPARK_HOME",
                                Seq("target/scala-2.10/qdstreaming_2.10-0.01.jar"))
    */

      val ssc = new StreamingContext(conf, Seconds(1))
     //                                "KBAStreamSource",
      //                                Seconds(1))
      //val rdd = sc.textFile("/home/cgrant/data/maddendb.sql", 8)
      //val lines = rdd.count()
      //logInfo(s"Total lines: $lines")



    // Word count
    val kba = new KBAIngestor()

    val nullRDD:RDD[StreamItem] = sc.makeRDD( Seq[StreamItem]() ) 

    /*val kbaRDD:RDD[StreamItem] = kba.take(2000)
      //.grouped(100)
      .fold(nullRDD)
        {(x:RDD[StreamItem],y:StreamItem) => x ++ sc.makeRDD(Seq(y))}
*/

    val sis = kba.take(200).toSeq
    logInfo(":::Created the sis file")

    val kbaRDD = sc.makeRDD(sis)
    logInfo("::Made an RDD out of the sis file")
    
    val total = kbaRDD.countApprox(5000, .4)

    logInfo(s"Should be 2000: $total")

  }
}



