
package edu.ufl.cise.dsr.examples

import streamcorpus.StreamItem
import edu.ufl.cise.dsr.QueryNode

object CandidateFilter {

  /**
    * Takes a streamitem and cheks to see if any of the 
    * candidate entities are presentent in some form in
    * the streamitem.
    * This returns a list of ids identifying the possible
    * candidates present
    */

  def check(qns:Array[QueryNode], si: StreamItem):(Array[QueryNode],StreamItem) = {

    // TODO check for the existance of this query node

    (qns.take(2), si)
  }
  
}
