
package edu.ufl.cise.dsr.examples
/*
import akka.actor.ActorSystem
import akka.actor.Props
import akka.stream.FlowMaterializer
import akka.stream.MaterializerSettings
import akka.stream.scaladsl.Flow

import scala.collection.JavaConversions._

import edu.ufl.cise.dsr.ingestor.KBAIngestor
import edu.ufl.cise.dsr.MyLogging


object ReactiveTest extends MyLogging {

  implicit val system = ActorSystem("gatordsr")
  val matsettings = MaterializerSettings(16, 64, 16, 64)
  val materializer = FlowMaterializer(matsettings)

  def main(args:Array[String]) {
    // Word count
    val kba = new KBAIngestor()
    //val kba = system.actorOf(Props[KBAIngestor], name ="kba")

    Flow(kba)
      .take(2000) // First 20 Streamitems
      .fold(0L)((x,y) => x + 1)
      //map(si => si.body.sentences.values().withFilter(_!=null)
       // .flatMap{x => x}.withFilter(_!=null)
        //.flatMap{s => s.tokens}).
      //foreach { token => val sz = token.size; println(s"The size is: $sz") }.
      .foreach { thecount => logInfo(s"count $thecount ...") }.

      //toProducer(materializer).
      onComplete(materializer) { _ => system.shutdown()}
  }
}

*/
