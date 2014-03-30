package models

/**
  * Point is the representation of each object in the entity resolution space.
  * A point can be an author, a paper and a document in tradional er.
  */

trait Point {

  def dimentions[T <: AnyVal]():Iterator[T] 

}