


package edu.ufl.cise.dsr.util

import java.util.Date
import java.text.SimpleDateFormat

import scala.collection.JavaConversions._

object Util {

  val sdf = new SimpleDateFormat("yyyy-MM-dd");

  def toString(date:Date):String = 
    sdf.format(date)
  
  def toString(date:Date, fmat:String):String = 
    new SimpleDateFormat(fmat).format(date)
  
  def toDate(date:String):Date = sdf.parse(date)
  
  def toDate(date:String, format:String = "yyyy-MM-dd"):Date = 
    new SimpleDateFormat(format).parse(date)
  

  
}
