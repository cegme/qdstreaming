package edu.ufl.cise.dsr

import akka.actor.Actor
import akka.actor.Props
import akka.actor.ActorLogging
import akka.actor.ActorSystem
import akka.actor.PoisonPill
import akka.event.Logging
import akka.pattern.ask
import akka.pattern.gracefulStop
import akka.pattern.pipe
import akka.util.Timeout

import scala.concurrent.Await
import scala.concurrent.Future
import scala.concurrent.duration._
import scala.io.Source
import scala.util.Random
import scala.util.{Success, Failure}

import scala.concurrent.{ ExecutionContext, Promise }

import ExecutionContext.Implicits.global


class QuickSortActor extends Actor with ActorLogging {

  def receive = {
    case Seq(pivot:Int, rest @ _*) =>

      val requestor = context.sender
      val partition = Future { 

        val (left, right) = rest partition {
          case i:Int =>  i < pivot
        }

        (left, pivot, right, requestor)
      }
      
      partition onSuccess {
        case (left, pivot, right, requestor) => 
          val leftActor = context.actorOf(Props[QuickSortActor])
          val rightActor = context.actorOf(Props[QuickSortActor])

          val result = 
            for {
              leftSorted <- ((leftActor ? left)(Duration(120,SECONDS))).mapTo[Seq[Int]]
              rightSorted <- ((rightActor ? right)(Duration(120,SECONDS))).mapTo[Seq[Int]]
            } yield leftSorted ++ Seq(pivot) ++ rightSorted

          // Send the result back
          result onSuccess {
            case x =>
              //log.info(x.mkString("[",",","]"))
              result pipeTo requestor
          }
      }
      partition onFailure {
        case _ => log.error("Fail!")
      }

    case x :: Nil =>
      val currentSender = context.sender
      currentSender ! Seq(x)
    
    case Nil =>
      context.sender ! Seq()

    case _ =>
      log.info("huh??")

  }
}


object QuickSort  extends MyLogging { 

  def main(args:Array[String]) ={
    val system = ActorSystem("mySystem")
    val myActor = system.actorOf(Props[QuickSortActor], "main")
    
    //val seq = Seq(6,2,5,7,8,3,8,4)
    //val seq = for(i <- 0 until 100000) yield scala.util.Random.nextInt()
    val seq = Seq.fill(100000)(scala.util.Random.nextInt)
    val f:Future[Seq[Int]] = myActor.ask(seq)(Duration(120,SECONDS)).mapTo[Seq[Int]]
    f onSuccess {
      case x:Seq[Int] =>
        // Assume x is greater than zeero
        logInfo("Is sorted: " + x.sliding(2).forall(y => y(0) <= y(1)) )
        system.shutdown()
    }

    f onFailure {
      case e:Exception => 
        e.printStackTrace
        system.shutdown()
      case _ =>
        logError("Operation failed.")
        system.shutdown()
    }

  }
}

