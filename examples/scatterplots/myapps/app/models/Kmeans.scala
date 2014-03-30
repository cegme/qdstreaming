package models
/**
 *     
     Concept : 
     Step 1 : get the points in the list  -- essentialy this is what TwoDIngestor does
     Step 2 : from the list obtained get the total number of points 
     Step 3 : initialze the number of canopies i.e the  "clusters"
     Step 4 : Calcualte the centroid ( starting with random distribtuion
    
     Stage 2
     Update centroid and canopies with each point/entity added.
     
     In this implementation , I am not using TwoDIngestor and Ingestor implementations ; instead we will be rewriting our own implementation to get a better understanding of language
     Please note : TwoDpoints and Points definitions are still maintained.
 */
class Kmeans {

//Things to NOTE : lets use ListBuffer instead of list to maintain TwoDPoint collection.
// reason being List in scala are immutable by default , which will have severe performance issues when we
//modify the canopy lables of the TwoDPoints as one will have to copy everything to a new list for everychange.
//moreover transition from ListBuffer to List is constant time irrespective of size.
  
 //also for some reason Float doesnt seem to get along with array. i.e 
  //scala> var newvar : Array[Float] = Array(0.25,2)
//<console>:8: error: type mismatch;
// found   : Double(0.25)
//required: Float
//var newvar : Array[Float] = Array(0.25,2)
//error might be due to a simple point i am missing , for now lets go ahead with double.

object Canopy { 
  
  /** Centroid:  essentially collects the center of all the TwoDPoints I have. Now to do that I merely do a fold-left i.e start with a initial value and a function
   *  and recursively apply that function to all the list members left -> right.
   */
  def centroid(items:List[TwoDPoint]):TwoDPoint = {
    items.foldLeft ((0.000, 0.000, 0)) {
      (t:(Double,Double,Int), p:TwoDPoint) => (t._1 + p.x, p.y + t._2, t._3 + 1)
    } match { 	
      case (x, y, c) => new TwoDPoint((x/c).toDouble, (y/c).toDouble, -1)
      case _ => throw new Exception("huh??")
    }

  }

  /**
   * defining labels on the points 
   */
  def centroidWithLabel(items:(List[TwoDPoint], Int)):TwoDPoint = {
    centroid(items._1)
  }

  /**
   * eucleadian distance    ( (x1-x2)^2 - (y1-y2)^2 )^1/2 
   */
  def distance(p1:TwoDPoint, p2:TwoDPoint):Float = {
    scala.math.sqrt(scala.math.pow(p1.x - p2.x, 2) + scala.math.pow(p1.y - p2.y, 2)).asInstanceOf[Float]
  }
}


object KMeans {
		
			var renderData  : List[TwoDPoint] = List[TwoDPoint]();
			var ingestor = new TwoDIngestor("../../../datasets/joensuu_datasets/Aggregation.txt");
			var items : List[Array[Double]] =  List[Array[Double]]();
			var counter = 0
			for ( index <-  ingestor.streamIterator){
									var strarray : Array[String] = index.split("\t")
									var myDoubles : Array[Double] = Array[Double]()	
									for ( value <- strarray){
											myDoubles = value.toDouble +: myDoubles
										}
									items = myDoubles ::items
							
				

			}
			
			
			
	def getFileData() : Iterator[List[Array[Double]]] = {
				return Iterator[List[Array[Double]]](items);
	}
	
		
	}

}
