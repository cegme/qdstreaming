package controllers

import play.api._
import play.api.mvc._
import play.api.libs.json.Json
import views.html.plots

object Application extends Controller {

  def index = Action {
    Ok(views.html.index("Your new application is ready."))
  }
  
  val sourcedata = scala.io.Source.fromFile("test.txt")
  val linesdata = sourcedata.mkString
  sourcedata.close()
  
  def plots = Action {
	 Ok(views.html.plots("yoyo"))
  }
  def jsondata = Action {
	  Ok(Json.toJson(linesdata))
  }

}