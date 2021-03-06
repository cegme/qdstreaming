package models

import scala.collection.mutable.ListBuffer
import scala.collection.mutable.ListBuffer
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
  

  def distance(p1:TwoDPoint, p2:TwoDPoint):Float = {
    scala.math.sqrt(scala.math.pow(p1.x - p2.x, 2) + scala.math.pow(p1.y - p2.y, 2)).asInstanceOf[Float]
  }
}


object KMeans {
		
			//data  model for the server side manipulations..
var renderData  : ListBuffer[TwoDPoint] = ListBuffer[TwoDPoint]();
var token : Array[String]=  new Array[String](3);
var datapoints : Array[Double] = new Array[Double](3);
var dataset  : ListBuffer[TwoDPoint] = ListBuffer[TwoDPoint]();

def begin() : Unit = {
		var ingestor = new TwoDIngestor("../../../datasets/joensuu_datasets/Aggregation.txt");
		while ( ingestor.streamIterator.hasNext){
			token =  ingestor.streamIterator.next.split("\t");
			var datapoints = token map(_.toDouble);
			var twoDpoint = new TwoDPoint(datapoints(0) , datapoints(1), datapoints(2));
			dataset += twoDpoint;
		}
}
def  updatePointList(point : TwoDPoint) : ListBuffer[TwoDPoint] = {
		dataset += point;	
}		
def  updateLables(items : ListBuffer[TwoDPoint]) : Unit = {
		//TODO : finsih this.
}
def formatClientData(items : ListBuffer[TwoDPoint]) : String = {
		return "";
}
def fetchClientData() : String  = {
			formatClientData(dataset);
}	

	}

}
