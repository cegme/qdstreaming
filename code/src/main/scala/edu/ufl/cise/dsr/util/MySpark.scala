
package edu.ufl.cise.dsr.util

import org.apache.spark.SparkConf
import org.apache.spark.SparkContext
import org.apache.spark.streaming._
import org.apache.spark.streaming.Duration._
import org.apache.spark.streaming.StreamingContext._

import edu.ufl.cise.dsr.MyLogging

object MySpark extends MyLogging {

  lazy val conf:SparkConf = {
    val master = "local[16]"
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
    conf
  }

  lazy val sc:SparkContext = {
    new SparkContext(conf)
  }

  lazy val ssc:StreamingContext = {
    new StreamingContext(conf, Seconds(1))
  }

}


