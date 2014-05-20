
package edu.ufl.cise.dsr.server

/*
import akka.actor.Actor
import akka.actor.{ActorSystem, Props}
import akka.actor.Actor._
//import akka.camel._
import org.apache.camel.Message
//import org.apache.camel.Consumer
//import akka.camel.Message
//import org.apache.camel.CamelServiceManager
import org.apache.camel._


import akka.camel.CamelExtension
import akka.camel.{ CamelMessage, Consumer }

import  edu.ufl.cise.dsr.MyLogging

import edu.ufl.cise.dsr.ingestor.KBAIngestor

class MyCamel extends Actor with Consumer {
  def endpointUri = "jetty:http://0.0.0.0:32608/kba2013"

  val ingestor = new KBAIngestor()

  def receive = {
    // URL Format: http://0.0.0.0:32608/kba2013?<number>
    case CamelMessage(body, headers) => sender ! process(headers)
    case _ => sender ! "No message."

  }

  def process(headers: Map[String, Any]): String = {

    val sb = new StringBuilder

    var thecount = Integer.parseInt(headers("CamelHttpQuery").toString, 10)
    for(i <- 0 until thecount) {
      sb ++= ingestor.next.toString
    }

    sb.toString
  }

}



object MyCamel extends MyLogging {
  def main(args: Array[String]) {
    val system = ActorSystem("CamelSystem")
    val camel = CamelExtension(system)
    val mina = system.actorOf(Props[MyCamel])


    ////camel.context.addRoutes(MyCamel)
    //val service = CamelServiceManager.startCamelService
    //service.awaitEndpointActivationn(1) {
      //Actor.actorOf[Camel].start

    //}
  }
}*/


