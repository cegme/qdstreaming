package models

trait Ingestor[T,S] extends Iterator[T] { 

  val streamIterator:S

  def hasNext:Boolean;
  
  def next: T
  
 // def vectorize[T <: AnyVal] : Iterator[T]


}
