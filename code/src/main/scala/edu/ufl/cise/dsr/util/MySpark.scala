
package edu.ufl.cise.dsr.util

import org.apache.spark.SparkConf
import org.apache.spark.SparkContext
import org.apache.spark.streaming._
import org.apache.spark.streaming.Duration._
import org.apache.spark.streaming.StreamingContext._

import org.apache.spark.sql.SQLContext
import org.apache.spark.sql._

import com.esotericsoftware.kryo.Kryo
import org.apache.spark.serializer.KryoRegistrator

import edu.ufl.cise.dsr.MyLogging
import edu.ufl.cise.dsr.point.WikiLinkMention


object MySpark extends MyLogging {

  /**
    * Tuning Docs:
    * http://people.apache.org/~pwendell/catalyst-docs/tuning.html
    */
  lazy val conf:SparkConf = {
    val YOUR_SPARK_HOME= "/home/cgrant/projects/spark/"
    // https://github.com/apache/spark/blob/master/docs/configuration.md
    val conf = new SparkConf()
      .set("spark.logConf", "true")
      //.setMaster("local")
      //.setMaster("spark://sm457-08:7077")
      .setMaster("spark://128.227.176.46:7077")
      //.set("spark.driver.host", "128.227.176.46")
      .setAppName("qdstreaming")
      //.setSparkHome(s"$YOUR_SPARK_HOME")
      .set("akka.version", "2.2.3")
      .set("spark.deploy.recoveryMode", "FILESYSTEM")
      .set("spark.deploy.recoveryDirectory", "/tmp")
      .set("spark.shuffle.consolidateFiles", "true")
      .set("spark.eventLog.enabled", "true")
      .set("spark.executor.memory", "4g")
      .set("spark.storage.memoryFraction", "0.3")
      .set("spark.serializer", "org.apache.spark.serializer.KryoSerializer")
      //.set("spark.serializer", "org.apache.spark.serializer.JavaSerializer")
      //.set("spark.kryo.registrator", "org.apache.spark.graphx.GraphKryoRegistrator")
      .set("spark.kryo.referenceTracking", "false")
      .set("spark.kryoserializer.buffer.mb", "64")
      .set("spark.kryo.registrator", "edu.ufl.cise.dsr.util.MyRegistrator")
      .set("spark.rdd.compress", "true")
      .set("spark.io.compression.codec", "org.apache.spark.io.SnappyCompressionCodec")
      //.set("spark.io.compression.codec", "org.apache.spark.io.LZFCompressionCodec")
      //.set("spark.storage.memoryMapThreshold", "32768")
      .set("spark.task.maxFailures", "8")
      .set("spark.cleaner.referenceTracking", "true")
      .set("spark.cleaner.ttl", "%d".format(60*3))
      .set("spark.akka.frameSize", "512")

    logInfo("Akka Conf: %s".format(conf.getAkkaConf.toString))
    conf
  }

  lazy val sc:SparkContext = {
    new SparkContext(MySpark.conf)
  }

  lazy val ssc:StreamingContext = {
    new StreamingContext(MySpark.conf, Seconds(1))
  }

  lazy val sqlsc:SQLContext = {
    new SQLContext(MySpark.sc)
  }

}

class MyRegistrator extends KryoRegistrator {
  override def registerClasses(kryo: Kryo) {
    kryo.register(classOf[WikiLinkMention])
  }
}



