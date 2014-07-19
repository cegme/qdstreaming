

package edu.ufl.cise.dsr.examples

import org.apache.spark.sql.SQLContext
import org.apache.spark.storage.StorageLevel

import edu.ufl.cise.dsr.MyLogging
import edu.ufl.cise.dsr.ingestor.WikiLinkIngestor
import edu.ufl.cise.dsr.point.WikiLinkMention
import edu.ufl.cise.dsr.util.MySpark

import scala.collection.JavaConversions._


object DistributedER extends MyLogging {

  def main(args: Array[String]) {

    val sqlContext = new SQLContext(MySpark.sc)
    import sqlContext._

    // Read from the iterator
    val wikiStream = new WikiLinkIngestor()

    // Create a Parquet File
    var rdd = createParquetFile[WikiLinkMention](
        "/home/cgrant/data/wikilinksdata",
        true
      )
    rdd.registerAsTable("wikimentions")
    //rdd.persist(StorageLevel.MEMORY_AND_DISK_SER)
    //rdd.persist(StorageLevel.MEMORY_ONLY_SER)
    //rdd.unpersist(false)

    var totalMentions = 0L
    val subset = wikiStream//.take(100000)
    //for(g <- subset.grouped(200)) {
    for(g <- subset.grouped(100000)) {
      val mentions = g.toVector
                      .flatMap(WikiLinkMention.getMentions)
      val smallRDD = MySpark.sc.makeRDD[WikiLinkMention](mentions)
      smallRDD.insertInto("wikimentions", false)
      //smallRDD.unpersist(false)
      totalMentions += mentions.size
      logInfo("Mentions added: %d; Total; %d".format(mentions.size, totalMentions))

    }

      // Group By the mention token
        //.groupBy(_.token)
      // Sort by the token groups
        //.sortBy(_.token)
      //
        //.grouped(100)
    

    // Give each entitiy an extra columns, entity id
    // Add column CanopyId array

    // Canopy cluster ----------------------------
    // 1) Shuffdata into K partitions
    // 2) Choose M canopy senters inside each partition
    // 3) For each M center, find the points that are 
    // closest to it inside its random partition
    // 4) Update data points



  }

}
