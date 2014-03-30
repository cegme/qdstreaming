package models


/**
 * TwoDIngestor : functioning.
 */
class TwoDIngestor(dataFilePath:String = null) extends Ingestor[TwoDPoint,Iterator[String]]  {

	override lazy val streamIterator = io.Source.fromFile(dataFilePath).getLines()

	def hasNext:Boolean = streamIterator.hasNext
	
	
	def next : TwoDPoint = {return new TwoDPoint(0,0,0)}  //TODO fix type collisions..
}
