
package edu.ufl.cise.dsr.filter

import streamcorpus.StreamItem
import edu.ufl.cise.dsr.query.QueryNode

object CandidateFilter {

  /**
    * Takes a streamitem and cheks to see if any of the 
    * candidate entities are presentent in some form in
    * the streamitem.
    * This returns a list of ids identifying the possible
    * candidates present
    */

  def check(qns:Seq[QueryNode], si: StreamItem):(Seq[QueryNode],StreamItem) = {

    // TODO check for the existance of this query node

    (qns.take(2), si)
  }
  
}
