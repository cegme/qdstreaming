
package edu.ufl.cise.dsr.util

import edu.ufl.cise.dsr.examples.WikiER
import edu.ufl.cise.dsr.point.WikiLinkMention

import edu.ufl.cise.dsr.MyLogging

object SampleRank {



}




class SampleRank(var weights:Array[Double]=Array[Double](1,1,1,1,1,1), eta:Double = .01 ) extends MyLogging{

  def run(_clusters:Vector[Vector[WikiLinkMention]], iterations:Int = 100) {

    var clusters = _clusters

    val rand = new scala.util.Random
    var iterations = 10000001
    while (iterations > 0) {
      // 1) Sample e1, e2, m1
      val e1 = rand.nextInt(clusters.size+1)
      val e2 = rand.nextInt(clusters.size+1)

      if (e1 >= clusters.size || e2 >= clusters.size) {
        // Increase the cluster size to make room for an extra entity
        // (e1 cannot actually be an empty vector)
        clusters = clusters :+ Vector()
      }

      if (clusters(e1).size > 0 && e1 != e2) {
        val m1 = rand.nextInt(clusters(e1).size)
        //logInfo("%d-[%d]->%d".format(e1, m1, e2))
        
        // 2) Compare the m1 to all mention in e1 and e2
        val (accept, weightChange, (e1Correct, e2Correct)) =
          WikiER.scoreFeature(clusters(e1), clusters(e2), clusters(e1)(m1), weights)

        if (accept) { // did
          logInfo(s"acccpted! $e1Correct | $e2Correct | %s".format(weightChange.mkString("{",",","}"))) 
          clusters =
            clusters updated (e2, clusters(e2) :+ clusters(e1)(m1))
          clusters =
            clusters updated (e1, clusters(e1) patch (m1, Vector(), 1))
          if (e1Correct > e2Correct) { // Should not have
            // Subtract feature weights bc they are too aggressive
            logInfo("1Weight before: %s %s".format(iterations, weights.mkString("{",",","}")))
            logInfo("1weightChange: %s".format(weightChange.mkString("{",",","}")))
            weights =
              (weights zip weightChange).map{t => t._1 - t._2} 
            logInfo("1Weight after: %s %s".format(iterations, weights.mkString("{",",","}")))
          }
        }
        else { // did not
          if (e1Correct < e2Correct) { // Should have
            logInfo(s"notacccpted! $e1Correct | $e2Correct | %s".format(weightChange.mkString("{",",","}"))) 
            // Increase weights because they did not trigger a change
            logInfo("2Weight before: %s %s".format(iterations, weights.mkString("{",",","}")))
            logInfo("2weightChange: %s".format(weightChange.mkString("{",",","}")))
            weights =
              (weights zip weightChange).map{t => t._1 + t._2} 
            logInfo("2Weight after: %s %s".format(iterations, weights.mkString("{",",","}")))
          }
        }
        iterations -= 1
        if (iterations % 1000000 == 0) logInfo("Iteration %d, Weight: %s".format(iterations, weights.mkString("{",",","}")))
      }
    }

  }

  // Calculate how many exact entities match a mention
  // Give a value of +1 for each match
  // Give a value of -1 for each match
  // (The sum is the confidence score)
  // The result feature vectors tell which features are turned on and off
  // feature = (current features) + (confidence)(computed weight)


}
