
package edu.ufl.cise.dsr.ingestor

import edu.ufl.cise.dsr.point.Point

trait Ingestor[T,S] extends Iterator[T] { 

  val streamIterator:S

  def hasNext:Boolean

  def next():T

  //def vectorize[P <: Point](t: T): Iterator[P] // FIXME

  lazy val iterator = {
    this.asInstanceOf[Iterator[T]]
  }

}

