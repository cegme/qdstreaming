
package edu.ufl.cise.dsr.ingestor

trait Ingestor[T] extends Iterator[T] { 

  def hasNext:Boolean

  def next():T

}

