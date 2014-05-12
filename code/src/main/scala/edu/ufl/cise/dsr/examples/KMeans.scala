
package edu.ufl.cise.dsr.examples

import edu.ufl.cise.dsr.point.{Point,TwoDPoint}


//case class Canopy(val items:List[TwoDPoint]) {
object Canopy { 
  
  def centroid(items:List[TwoDPoint]):TwoDPoint = {
    items.foldLeft ((0.0F, 0.0F, 0)) {
      (t:(Float,Float,Int), p:TwoDPoint) => (t._1 + p.x, p.y + t._2, t._3 + 1)
    } match {
      case (x, y, c) => new TwoDPoint(x/c, y/c, -1)
      case _ => throw new Exception("huh??")
    }

  }

  def centroidWithLabel(items:(List[TwoDPoint], Int)):TwoDPoint = {
    centroid(items._1)
  }

  def distance(p1:TwoDPoint, p2:TwoDPoint):Float = {
    scala.math.sqrt(scala.math.pow(p1.x - p2.x, 2) + scala.math.pow(p1.y - p2.y, 2)).asInstanceOf[Float]
  }
}


object KMeans {

  def main(args:Array[String]) {
  
    val ingestor = new edu.ufl.cise.dsr.ingestor.TwoDIngestor("../datasets/joensuu_datasets/Aggregation.txt") 

    // Get the set of points from the ingestor
    val points = ingestor.toList
    val totalPoints = points.size
    
    val K = 7
    // Initialize canopy
    // todo change with points.grouped{...}
    var canopies = points.grouped(points.size/K).toList.zipWithIndex

    for (i <- Range(0,7)) {
      // get the centroids
      // List of centroids
      val centroids = canopies.map{ Canopy.centroidWithLabel(_)}

      // Returns a: (List of points, Label)      
      val newCanopy =  points.map{ p => (p, 
                    centroids.map{c => Canopy.distance(c, p)}
                        .zipWithIndex
                        .sortBy{_._1}
                        .head._2)
                  }
                  .groupBy{ _._2 }
                  .toIterator
                  .map{e => (e._2.map{_._1}, e._1)}
                  .toList

      canopies = newCanopy
    }

    canopies.foreach { p =>
      println("%s --- %s".format(p._1, p._2))
    }
    canopies.foreach { p =>
      println("Canopy %d size: %d".format(p._2, p._1.size))
    }
  }

}
