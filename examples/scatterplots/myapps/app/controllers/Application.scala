package controllers

import play.api._
import play.api.mvc._
import play.api.libs.json.Json
import views.html.plots
import models.Kmeans
import scala.collection.mutable.ListBuffer
import play.api.libs.json.JsValue
import scala.Array
import models.TwoDIngestor
import models.TwoDPoint

object Application extends Controller {

  def index = Action {
    Ok(views.html.index("Your new application is ready."))
  }
  
  val sourcedata = scala.io.Source.fromFile("Aggregation.txt")
  val linesdata = sourcedata.mkString
  sourcedata.close()
  	
  def plots = Action {
	 Ok(views.html.plots("yoyo"))
  }
  def jsondata = Action {
//    var test  = new Kmeans
//    var testone = test.KMeans.ingestor.streamIterator;
//    println(linesdata);
			var ingestor = new TwoDIngestor("../../../datasets/joensuu_datasets/Aggregation.txt");
			var token : Array[String]=  new Array[String](3);
			var datapoints : Array[Double] = new Array[Double](3);
			var dataset  : ListBuffer[TwoDPoint] = ListBuffer[TwoDPoint]();
			while ( ingestor.streamIterator.hasNext){
				token =  ingestor.streamIterator.next.split("\t");
				var datapoints = token map(_.toDouble);
				var twoDpoint = new TwoDPoint(datapoints(0) , datapoints(1), datapoints(2));
				dataset += twoDpoint;
			}
			for ( eachval <- dataset){
				println(eachval);
			}
    Ok(Json.toJson(linesdata));
  }

}