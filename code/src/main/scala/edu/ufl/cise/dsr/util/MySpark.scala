
package edu.ufl.cise.dsr.util

import org.apache.spark.SparkConf
import org.apache.spark.SparkContext
import org.apache.spark.streaming._
import org.apache.spark.streaming.Duration._
import org.apache.spark.streaming.StreamingContext._

import edu.ufl.cise.dsr.MyLogging

object MySpark extends MyLogging {

  lazy val conf:SparkConf = {
    val master = "local"
    val YOUR_SPARK_HOME= "/home/cgrant/projects/spark/"
    // https://github.com/apache/spark/blob/master/docs/configuration.md
    val conf = new SparkConf()
      .set("spark.logConf", "true")
      .setMaster(master)
      .setAppName("SSTest")
      .setSparkHome(s"$YOUR_SPARK_HOME")
      .set("akka.version", "2.2.3")
      .set("spark.deploy.recoveryMode", "FILESYSTEM")
      .set("spark.deploy.recoveryDirectory", "/tmp")
      .set("spark.shuffle.consolidateFiles", "true")
      .set("spark.executor.memory", "5g")
      .set("spark.serializer", "org.apache.spark.serializer.KryoSerializer")
      .set("spark.kryo.registrator", "org.apache.spark.graphx.GraphKryoRegistrator")
      .set("spark.kryo.referenceTracking", "false")
      .set("spark.kryoserializer.buffer.mb", "16")
      .set("spark.rdd.compress", "true")
      .set("spark.io.compression.codec", "org.apache.spark.io.SnappyCompressionCodec")
      //.set("spark.io.compression.codec", "org.apache.spark.io.LZFCompressionCodec")
      .set("spark.storage.memoryMapThreshold", "32768")
    conf
  }

  lazy val sc:SparkContext = {
    new SparkContext(conf)
  }

  lazy val ssc:StreamingContext = {
    new StreamingContext(conf, Seconds(1))
  }

}


