
package edu.ufl.cise.dsr.query

//import org.apache.spark.SparkConf
//import org.apache.spark.SparkContext
//import org.apache.spark.SparkContext._

case class QueryNode(targetid:String,
                     group:String,
                     entity_type:String, 
                     alias:String)

// TODO add function that can quickly do string matches



object QueryNode {

  def GetAllQueryNodes:Array[QueryNode] = {
    // Read the file alias_list
    null
  }

  def main(args: Array[String]) {
    // Testing SparkConf too
/*    val conf = new SparkConf()
                .setMaster("local")
                .setAppName("QueryNode")
                .setSparkHome("/home/cgrant/projects/spark/")
                .setJars(List("target/scala-2.10/qdstreaming_2.10-0.01.jar"))
                .set("spark.executor.memory", "5g")
                .set("spark.logConf", "true")
                .set("akka.loggers.0", "akka.event.slf4j.Slf4jLogger")
                .set("event-handlers.0", "akka.event.slf4j.Slf4jEventHandler")

    val t = conf.getOption("spark.logConf")
    println("-"*40) 
    println(s">>> $t") 
    println("-"*40) 
    val sc = new SparkContext(conf)

// /home/cgrant/projects/spark/examples/target/scala-2.10/spark-examples_2.10-1.0.0-SNAPSHOT.jar
    val logFile = "/home/cgrant/projects/qdstreaming/code/resourecs/topics/trec-kba-ccr-and-ssf-query-topics-2013-06-11.json"
    //val logFile = "topics/trec-kba-ccr-and-ssf-query-topics-2013-06-11.json"
    val logData = sc.textFile(logFile)
    val lineCount = logData.count()
    println(s"Line count: $lineCount")
    */
  }

}
