
package edu.ufl.cise.dsr.point

import java.lang.{Integer => JInt, Long => JLong, Double => JDouble}
import java.util.concurrent.Callable

import edu.umass.cs.iesl.wikilink.expanded.data.WikiLinkItem

import edu.stanford.nlp.util.EditDistance

import com.google.common.cache.Cache
import com.google.common.cache.CacheBuilder
import com.google.common.cache.CacheLoader

import scala.collection.JavaConversions._
import scala.collection.mutable.ArrayBuffer


case class WikiLinkMention (
  val docId:Int, // From WikiLinkItem
  val url:String, // From WikiLinkItem
  val token:String, // From Mention
  val contextLeft:String, // From Mention
  val contextRight:String, // From Mention
  val contextMiddle:String, // From Mention
  val wikiUrl:String, // The truth value (from Mention)
  //TODO not yet supported 
  //var canopy:ArrayBuffer[Int] = ArrayBuffer[Int]()
  var canopy:String = ""
  ) extends Point {

  def dimentions[String]():Iterator[String] = {
    Iterator[String](/*docId,
      url,
      token,
      contextLeft,
      contextRight,
      contextMiddle*/)
  }
  //override def dimentions[Int] = (0 to 5).toIterator[Int]

}

object WikiLinkMention {
  private val ed = new EditDistance()

  // This cache can be persisted to disk http://java.dzone.com/articles/extending-guava-caches
  private val myResultCache:Cache[(JLong, JLong), JDouble] = {
    CacheBuilder.newBuilder()
    .maximumSize(10000000)
    .build()
  }

  private val myFeatureCache:Cache[(JLong, JLong), Array[JInt]] = {
    CacheBuilder.newBuilder()
    .maximumSize(10000000)
    .build()
  }

  val featureCount = 6
  def defaultFeatureArray = Array.ofDim[Int](WikiLinkMention.featureCount).map( t => 0)

  def similarity(m1:WikiLinkMention, m2:WikiLinkMention, weights:Array[Double]):Double = {
    myResultCache.get((m1.docId,m2.docId), new Callable[JDouble] {
      def call = {
          (weights zip featureSimilarity(m1,m2,weights)).map(t => t._1 * t._2).sum
      }
    })
  }

  def featureSimilarity(m1:WikiLinkMention, m2:WikiLinkMention, weights:Array[Double]): Array[JInt] = {
    myFeatureCache.get((m1.docId,m2.docId), new Callable[Array[JInt]] {
      def call:Array[JInt] = {

        val minLength:Int = scala.math.min(m1.token.length, m2.token.length)
        val maxScore:Int = 6 

        val score:Array[JInt] = Array[JInt](
          // Exact match 
          (if (m1.token.equalsIgnoreCase(m2.token)) 1 else -1).toInt,
          // Edit Distance 
          // If the difference is greater than half the smaller word
          if (Math.abs(minLength/2.0 - ed.score(m1.token, m2.token)) > 0) 1 else -1,
          // Token overlap
          (m1.token.split("\\s+") intersect m2.token.split("\\s+")).isEmpty match {
            case false => 1
            case _     => -1
          },
          // Context overlap
          (m1.contextLeft.split("\\s+") intersect m2.contextLeft.split("\\s+")).isEmpty match {
            case false => 1
            case _     => -1
          } ,
          (m1.contextRight.split("\\s+") intersect m2.contextRight.split("\\s+")).isEmpty match {
            case false => 1
            case _     => -1
          } 
        ) // End array

        //score / maxScore
        score // Return the score array
      }
    })
  }

  /**
    * Use this to extract mentions from a document.
    */
  def getMentions(wi:WikiLinkItem):Seq[WikiLinkMention] = {
    wi.mentions.map{ m => 
      val (left, right, middle) = m.context match {
                                    case Some(z) => (z.left, z.right, z.middle)
                                    case _       => ("", "", "")
                                  }
      new WikiLinkMention(wi.docId,
                      wi.url,
                      m.anchorText,
                      left,
                      right,
                      middle,
                      m.wikiUrl)
    }
    //.toVector
  }

}
