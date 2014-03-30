package controllers

import play.api._
import play.api.mvc._
import play.api.libs.json.Json
import views.html.plots
import models.Kmeans
import scala.collection.mutable.ListBuffer
import play.api.libs.json.JsValue
import scala.Array

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
    var test  = new Kmeans
    var testone = test.KMeans.ingestor.streamIterator
 //   var linesdata = testone.mkString
    println(linesdata);
    Ok(Json.toJson(linesdata));
  }

}