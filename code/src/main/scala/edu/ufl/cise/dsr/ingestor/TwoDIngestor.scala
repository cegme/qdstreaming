
package edu.ufl.cise.dsr.ingestor

import edu.ufl.cise.dsr.point.TwoDPoint
import edu.ufl.cise.dsr.MyLogging


/**
  * 
  * val ingestor = new TwoDIngestor("../datasets/joensuu_datasets/Aggregation.txt")
  */
class TwoDIngestor(dataFilePath:String = null) extends Ingestor[TwoDPoint,Iterator[String]] with MyLogging {

  def withFile(dataFilePath:String): TwoDIngestor = {
    new TwoDIngestor(dataFilePath)
  }

  override lazy val streamIterator = io.Source.fromFile(dataFilePath).getLines()

  def hasNext:Boolean = streamIterator.hasNext

  def next():TwoDPoint = {
    val PointRegex =  """(\S+)\s+(\S+)\s+(\S+)""".r
    val line:String = streamIterator.next
    line match {
      case PointRegex(x, y, label) => new TwoDPoint(x.toFloat, y.toFloat, label.toInt)
      case _ => throw new Exception("Malformed line read: %s".format(line))
    }
  }

  def vectorize[TwoDPoint](p: TwoDPoint): Iterator[TwoDPoint] = {
    Iterator[TwoDPoint](p)
  }

}
