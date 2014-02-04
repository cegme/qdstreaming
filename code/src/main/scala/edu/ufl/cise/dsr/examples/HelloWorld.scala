package edu.ufl.cise.dsr.examples

import akka.actor.Actor
import akka.actor.ActorSystem
import akka.actor.Props

class HelloWorld extends Actor {

  override def preStart(): Unit = {
    // create the greeter actor
    val greeter = context.actorOf(Props[Greeter], "greeter")
      // tell it to perform the greeting
    greeter ! Greeter.Greet
  }

  def receive = {
    case Greeter.Done ⇒
      //context.stop(self)
      context.system.shutdown
  }


}

object HelloWorldDriver {
  def main(args: Array[String]) {
    val system = ActorSystem("Main")
    val ac = system.actorOf(Props[HelloWorld])
  }
}

object Greeter {
  case object Greet
  case object Done
}

class Greeter extends Actor {
  def receive = {
    case Greeter.Greet ⇒
    println("Hello World!")
    sender ! Greeter.Done
  }
}

