
package edu.ufl.cise.dsr.point


class TwoDPoint(val x:Float, val y:Float, val label:Integer = null)  extends Point {

  def dimentions[Float]():Iterator[Float] = {
    //Iterator[Float](this.x.asInstanceOf[Float],this.y.asInstanceOf[Float])
    Iterator[Float](x.asInstanceOf[Float],y.asInstanceOf[Float])
  }

}
