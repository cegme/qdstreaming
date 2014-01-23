
package edu.ufl.cise.dsr

import akka.actor.Actor
import akka.actor.Props
import akka.actor.ActorLogging
import akka.actor.ActorSystem
import akka.actor.PoisonPill
import akka.event.Logging
import akka.pattern.gracefulStop
import akka.util.Timeout

import scala.concurrent.duration._
import scala.io.Source

class DocBuilder extends Actor with ActorLogging {

  var bag = collection.immutable.List[Word]()
  val docActor = context.actorOf(Props[DocStream], name = "docActor")

  def receive = {
    case w:Word => 
      bag = w :: bag
    case e:End => 
      log.info("Document {}", bag.size)
      docActor ! Document(bag)
      bag = collection.immutable.List[Word]()
    case Manager.Shutdown => 
      log.info("Goodbye!")
      context.system.shutdown()
    case _      => log.info("received unknown message")
  }
}


class DocStream extends Actor with ActorLogging {
  var docs = collection.immutable.List[Document]()

    def receive = {
      case d:Document =>
        docs = d :: docs
      case Manager.Shutdown => 
        log.info("Goodbye!")
        context.system.shutdown()
      case _ => log.info("huh??")
  }
}


object Manager {
  case object Shutdown
}

case class Document(val words:List[Word])
case class End()
case class Word(val docID:Int, val wordID:Int, val count:Int) 


object WordReader {

  def main(args:Array[String]): Unit = {
    val system = ActorSystem("mySystem")
    val myActor = system.actorOf(Props[DocBuilder], "myActor")

    val f = Source.fromFile("/home/cgrant/projects/qdstreaming/datasets/bag-of-words/docword.kos.txt")
    var last = -1
    for (line <- f.getLines.drop(3)) {
      // Go from three int to a word
      val t = line.split(" ").map{Integer.parseInt}
      val w = Word(t(0), t(1), t(2))

      if (last == w.docID)
        myActor ! w
      else {
        last = w.docID
        myActor ! End()
        myActor ! w

      }
    }

    myActor ! Manager.Shutdown
  }
}






