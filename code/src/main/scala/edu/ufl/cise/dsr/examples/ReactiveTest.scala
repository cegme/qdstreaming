
package edu.ufl.cise.dsr.examples

import akka.actor.ActorSystem
import akka.stream.FlowMaterializer
import akka.stream.MaterializerSettings
import akka.stream.scaladsl.Flow

import scala.collection.JavaConversions._

import edu.ufl.cise.dsr.ingestor.KBAIngestor


object ReactiveTest {

  implicit val system = ActorSystem("gatordsr")
  val matsettings = MaterializerSettings(8, 32, 8, 32)
  val materializer = FlowMaterializer(matsettings)

    def main(args:Array[String]) {
    // Word count
    val kba = new KBAIngestor()
    Flow(kba).
      take(2000). // First 20 Streamitems
      map(si => si.body.sentences.values().withFilter(_!=null)
        .flatMap{x => x}.withFilter(_!=null)
        .flatMap{s => s.tokens}).
      //foreach { token => val sz = token.size; println(s"The size is: $sz") }.

      //toProducer(materializer).
      onComplete(materializer) { _ => system.shutdown()}
  }
}


