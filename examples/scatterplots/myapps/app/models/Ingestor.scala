package models

trait Ingestor[T,S] extends Iterator[T] { 

  val streamIterator:S

  def hasNext:Boolean;
  
  def next: T


}
