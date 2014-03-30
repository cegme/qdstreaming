package models

case class TwoDPoint(val x:Double, val y:Double, val label:Integer = null)  extends Point {

  def dimentions[Double]():Iterator[Double] = {
    //Iterator[Float](this.x.asInstanceOf[Float],this.y.asInstanceOf[Float])
    Iterator[Double](x.asInstanceOf[Double],y.asInstanceOf[Double])
  }

}
