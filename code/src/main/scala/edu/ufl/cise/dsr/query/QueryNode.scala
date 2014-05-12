
package edu.ufl.cise.dsr.query

case class QueryNode(targetid:String,
                     group:String,
                     entity_type:String, 
                     alias:String)

// TODO add function that can quickly do string matches



object QueryNode {

  def GetAllQueryNodes:Array[QueryNode] = {
    // Read the file alias_list
  }


}
