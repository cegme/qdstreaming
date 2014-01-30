
package edu.ufl.cise.dsr.ingestor

import edu.ufl.cise.dsr.point.Point

trait Ingestor[T] extends Iterator[T] { 

  def hasNext:Boolean

  def next():T

  def vectorize(t: T): Point

}

