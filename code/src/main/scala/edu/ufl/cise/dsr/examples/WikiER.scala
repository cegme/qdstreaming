
package edu.ufl.cise.dsr.examples

import java.lang.{Integer => JInt, Long => JLong, Double => JDouble}

import org.roaringbitmap.RoaringBitmap

import edu.ufl.cise.dsr.MyLogging
import edu.ufl.cise.dsr.ingestor.WikiLinkIngestor
import edu.ufl.cise.dsr.point.WikiLinkMention

import scala.collection.JavaConversions._


object WikiER extends MyLogging {

  /**
    * Keep a sketch of the size of each entity
    */
  val emptyEntityMap = new RoaringBitmap
  
  def main(args: Array[String]) {

    // Get ingestor stream
    val wikiStream = new WikiLinkIngestor()
    
    // Get training data
    var clusters =
      wikiStream.take(500)
        .toVector
        .flatMap(WikiLinkMention.getMentions)
        .groupBy(_.token)
        .map(_._2)
        .toVector

    val sampleRank = new edu.ufl.cise.dsr.util.SampleRank()
    sampleRank.run(clusters)

    //Inference------------------------
    clusters = wikiStream
      .take(2000) // TODO how big can we get??
      .toVector
      //.view // Don't create intermediate data structures
      .flatMap(WikiLinkMention.getMentions) // Generate a mentions from each Document
      .groupBy(_.token) // Assign each cluster as its own entity
      .map(_._2)
      .toVector

    logInfo("-"*200)
    logInfo("Cluster Count: %d".format(clusters.size))
    logInfo("Mention Count: %d".format(clusters.map{_.size}.sum))
    logInfo("Input cluster: %s".format(clusterInfo(clusters).toString))
    logInfo("Truth cluster: %s".format(clusterInfo(createTruthVector(clusters)).toString))
 
    val rand = new scala.util.Random(System.nanoTime)

    // Algorithm
    var iterations = 10000001
    while (iterations > 0) {

      val e1 = rand.nextInt(clusters.size+1)
      val e2 = rand.nextInt(clusters.size+1)

      if (e1 >= clusters.size || e2 >= clusters.size) {
        // Increase the cluster size to make room for an extra entity
        // (e1 cannot actually be an empty vector)
        clusters = clusters :+ Vector()
        emptyEntityMap.add(clusters.size) // Add the new entity to the empty list
      }

      if (clusters(e1).size > 0 && e1 != e2) {
        val m1 = rand.nextInt(clusters(e1).size)
        
        if (score(clusters(e1), clusters(e2), clusters(e1)(m1)) ) {
          // Add m1 to the new cluster
          clusters =
            clusters updated (e2, clusters(e2) :+ clusters(e1)(m1))
          // Remove m1 from the old cluster
          clusters =
            clusters updated (e1, clusters(e1) patch (m1, Vector(), 1))
         }

        iterations -= 1
        if (iterations % 2000000 == 0) {
          logDebug(s"calculating stats... [iterations]")
          val (tp, fp, tn, fn, prec, rec, f1) = allStats(clusters)
          logInfo("f1score: %f | (p,r): (%f, %f)".format(f1, prec, rec))
        }
      }
    }

    // Print some results
    clusters.withFilter(_.size == 5).foreach{x => x.foreach(z => logInfo(z.toString))}
    val (tp, fp, tn, fn, prec, rec, f1) = allStats(clusters)
    logInfo("f1score: %f | (p,r): (%f, %f)".format(f1, prec, rec))
  }

  def score(a:Seq[WikiLinkMention], b:Seq[WikiLinkMention], m:WikiLinkMention): Boolean = {

    val mInA = a.foldLeft(0.0) {(sum,ai) =>
      sum + WikiLinkMention.similarity(m, ai, Array[Double]())
    } / a.size

    val mInB = b.foldLeft(WikiLinkMention.similarity(m,m,Array[Double]())) {(sum, bi) =>
      sum + WikiLinkMention.similarity(m, bi, Array[Double]())
    } / (b.size + 1)

    mInA < mInB
  }


  /**
    * Takes the source entity (a) and destination entity (b) and the 
    * mention in question (m) and it return the decision to make the 
    * move or not. It also returns the combined amount of times each
    * feature was aactivated (+1) or unactivated (-1). It also returns
    * the amount of correct matches in the new entity.
    *
    */
  def scoreFeature(a:Vector[WikiLinkMention], b:Vector[WikiLinkMention], m:WikiLinkMention, weights:Array[Double]):
    (Boolean, Array[JInt], (Double, Double)) = {

    //logInfo("a: %s".format(a))
    //logInfo("b: %s".format(b))

    val (mInA, aArray) = 
      a.foldLeft (0.0, WikiLinkMention.defaultFeatureArray) {
        case ((accSum, accFeats), ai) => //println("accFeats: %s".format(accFeats.mkString("{",",","}")))
          ((accSum + WikiLinkMention.similarity(m, ai, weights)),
            ((accFeats zip WikiLinkMention.featureSimilarity(m, ai, weights))
              .map{ t => t._1 + t._2 }
              //.map{JInt.valueOf}  
          ) )
    } // / a.size


    val (mInB, bArray) = 
      b.foldLeft (WikiLinkMention.similarity(m,m,Array[Double]()), WikiLinkMention.featureSimilarity(m, m, Array[Double]()) ) {
        case ((accSum, accFeats), bi) => 
          ((accSum + WikiLinkMention.similarity(m, bi, weights)),
            ((accFeats zip WikiLinkMention.featureSimilarity(m, bi, weights))
              .map{ t => t._1 + t._2 }
              .map{JInt.valueOf}
             ))
    } // / (b.size + 1)

    // Sum up the feature with the most weights
    (1.0*mInA/a.size < mInB/(b.size+1.0),
      (
        if (1.0*mInA/a.size < mInB/(b.size+1.0)) 
          (bArray zip aArray).map{t => t._1 - t._2}
        else 
          (aArray zip bArray).map{t => t._1 - t._2}
      ).map{JInt.valueOf},
      (
        b.count(x => x != m && x.wikiUrl.equalsIgnoreCase(m.wikiUrl))/(b.size+1.0),
        a.count(x => x != m && x.wikiUrl.equalsIgnoreCase(m.wikiUrl))*1.0/a.size
      ) 
    )
  }



  /**
    * Use the truth data to create our absolute truth
    */
  def createTruthMap(clusters:Vector[Vector[WikiLinkMention]]): Map[String,Vector[WikiLinkMention]] = {
    clusters
      .flatten // Make one lone list of entitiy mentions
      .groupBy(_.wikiUrl) // Group By the wikiUrl which is the truth value
      //.map(_._2) // Only keep the mentions not the group labels
      //.toVector // We still want a vector of vectors
  }
  def createTruthVector(clusters:Vector[Vector[WikiLinkMention]]): Vector[Vector[WikiLinkMention]] = {
    clusters
      .flatten // Make one lone list of entitiy mentions
      .groupBy(_.wikiUrl) // Group By the wikiUrl which is the truth value
      .map(_._2) // Only keep the mentions not the group labels
      .toVector // We still want a vector of vectors
  }
  /**
    * Returns the count, sum, mean, standard dev, max, min
    */
  def clusterInfo (clusters:Vector[Vector[WikiLinkMention]]):
    (Double, Double, Double, Double, Double, Double) = {

    val s = new org.apache.commons.math3.stat.descriptive.SummaryStatistics
    ////val dist = createTruthVector(clusters).flatMap(_._2.size)
    //val dist:Vector[Int] = clusters.flatMap{v => v.size}
    //val sum:Double = dist.reduce(_ + _)
    //val mean:Double = sum / dist.size
    //val devs:Vector[Double] = dist.map{v => (v - mean) * (v - mean)}
    //val stddev:Double = Math.sqrt(devs.reduce(_ + _) / devs.size)

    //clusters
      //.foreach{x => s.addValue(x.size)} // One pass add all values
    var i = 0;
    while ( i < clusters.size) {
      s.addValue(clusters(i).size)
      i += 1
    }

    val thesum:Double = s.getSum()
    val mean:Double = s.getMean()
    val stddev:Double = s.getStandardDeviation()

    //logInfo(s"sum: $thesum")
    //logInfo("Sum: %f".format(s.getSum()))
    //logInfo("Mean: %f".format(s.getMean()))
    //logInfo("StandardDeviation: %f".format(s.getStandardDeviation()))
    logInfo(s"sum: $thesum, mean: $mean, stddev: $stddev")

    (clusters.size.toDouble, thesum, mean, stddev, s.getMax, s.getMin)
  }



  /** 
    * Compute the stats, returns a tuple with count of true positive, false positive, 
    * true negative, and false negative.
    */
  def computeStats(clusters:Vector[Vector[WikiLinkMention]]): (Double,Double,Double,Double) = {
    // Pairwise combile
    val pairWise = clusters
      //par.view
      .zipWithIndex // Keep an id so we know where each the first entity came from
      .flatMap{ case (e1, eid) =>
        e1.flatMap{ a1 => 
          clusters.flatMap{ t =>
            t.flatMap { a2 => a1.wikiUrl.hashCode <= a2.wikiUrl.hashCode match {
              case true  => Some(a1, a2, eid)
              case false => None
            } }
      } } }

    // Computing tp, fpp, tn, fp
    val (tp,fp,tn,fn) = pairWise
      .foldLeft((0,0,0,0)) { case ((tp,fp,tn,fn), (a1, a2, e)) =>
        val p:Boolean = a1.wikiUrl == a2.wikiUrl // Supposed to be together?
        val t:Boolean = clusters(e).contains(a2) // Actuall together?
        (p,t) match {
          case (true,true)    => (tp+1, fp, tn, fn) // True positive
          case (false, true)  => (tp, fp+1, tn, fn) // false negative
          case (true, false)  => (tp, fp, tn+1, fn) // true negative
          //case (fals, fals) => (tp, fp, tn, fn+1)
          case _              => (tp, fp, tn, fn+1) // false positive
        }

      }

    val d = Double.MinPositiveValue
    (tp+d, fp+d, tn+d, fn+d)
  }

  // @See http://www.cs.cmu.edu/afs/cs.cmu.edu/project/listen/NLP/Parsers/Stanford/stanford-parser-2005-07-21/MCALL/lingpipe-3.6.0/docs/api/com/aliasi/classify/PrecisionRecallEvaluation.html
  def accuracy(clusters:Vector[Vector[WikiLinkMention]]):Double = {
    val (tp,fp,tn,fn)  = computeStats(clusters)
    (tp+tn) / (tp + fp + tn + fn)
  }
  def recall(clusters:Vector[Vector[WikiLinkMention]]):Double = {
    val (tp,fp,tn,fn)  = computeStats(clusters)
    tp / (tp+fn)
  }
  def precision(clusters:Vector[Vector[WikiLinkMention]]):Double = {
    val (tp,fp,tn,fn)  = computeStats(clusters)
    tp / (tp+fp)
  }
  def f1score(clusters:Vector[Vector[WikiLinkMention]]):Double = {
    val (tp,fp,tn,fn)  = computeStats(clusters)
    val prec = tp / (tp+fp)
    val rec = tp / (tp+fn)
    2.0 * prec * rec/(prec + rec) 
  }
  def rejectionRecall(clusters:Vector[Vector[WikiLinkMention]]):Double = {
    val (tp,fp,tn,fn)  = computeStats(clusters)
    tn / (fp+tn)
  }
  def rejectionPrecision(clusters:Vector[Vector[WikiLinkMention]]):Double = {
    val (tp,fp,tn,fn)  = computeStats(clusters)
    tn / (fn+tn)
  }
  def allStats(clusters:Vector[Vector[WikiLinkMention]]):
  (Double, Double, Double, Double, Double, Double, Double) = {
    val (tp,fp,tn,fn)  = computeStats(clusters)
    val prec = tp / (tp+fp)
    val rec = tp / (tp+fn)
    val f1 = 2.0 * prec * rec/(prec + rec)
    (tp, fp, tn, fn, prec, rec, f1)
  }


  /**
    * Use the absolute truth to compute the corret
    */
  /*def bCubed(truth:Vector[Vector[WikiLinkMention]])(clusters:Vector[Vector[WikiLinkMention]]): Double = {
    
    val truthMap = truth
                    .map{e => e(0).wikiUrl -> e} // Take the 1st item use it to label entity
                    .toMap // Make it a map


    // TODO fix this!!
    // Calculate b^3
    clusters
      .map { e => 
        val precision = e.map{ m =>
          truthMap.get(m.wikiUrl) match {
            case Some(z) => 1
            case None    => 0
          }
        }
      }
      //. pr =>
    0.0
  }*/


  // TODO https://gist.github.com/azymnis/7940080
  //def doMinHash 


}
