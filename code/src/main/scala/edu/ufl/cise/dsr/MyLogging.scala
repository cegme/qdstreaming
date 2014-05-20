package edu.ufl.cise.dsr

import org.slf4j.Logger
import org.slf4j.LoggerFactory
import akka.actor.Actor
import akka.actor.ActorLogging
import akka.event.Logging
import akka.event.LoggingAdapter

/**
 * This trait class is was taken from the spark source code. 
 * core/src/main/scala/spark/Logging.scala
 *
 * Utility trait for classes that want to log data. Creates a SLF4J logger for the class and allows
 * logging messages at different levels using methods that only evaluate parameters lazily if the
 * log level is enabled.
 */
//trait MyLogging extends  org.apache.spark.Logging { }
trait MyLogging {

  // Make the log field transient so that objects with Logging can
  // be serialized and used on another machine
  @transient private var log_ : Logger = null

  // Method to get or create the logger for this object
  //protected  def log: Logger = {
  lazy val log: Logger = {
    if (log_ == null) {
      this match {
        case a:Actor => 
          //a.system.eventStream.setLogLevel(Logging.DebugLevel)
          log_ = LoggerFactory.getLogger(a.getClass)
        
        case _ =>
          //a.system.eventStream.setLogLevel(Logging.DebugLevel)
          var className = this.getClass.getName
          // Ignore trailing $'s in the class names for Scala objects
          if (className.endsWith("$")) {
            className = className.substring(0, className.length - 1)
          }
          log_ = LoggerFactory.getLogger(className)
      }
    }
    log_
  }

  // Log methods that take only a String
  protected def logInfo(msg: => String) {
    if (log.isInfoEnabled) log.info(msg)
  }

  protected def logDebug(msg: => String) {
    if (log.isDebugEnabled) log.debug(msg)
  }
  
  protected def logError(msg: => String) {
    if (log.isErrorEnabled) log.error(msg)
  }

  // Log methods that take Throwables (Exceptions/Errors) too
  protected def logInfo(msg: => String, throwable: Throwable) {
    if (log.isInfoEnabled) log.info(msg, throwable)
  }

  protected def logDebug(msg: => String, throwable: Throwable) {
    if (log.isDebugEnabled) log.debug(msg, throwable)
  }
  protected def logError(msg: => String, throwable: Throwable) {
    if (log.isErrorEnabled) log.error(msg, throwable)
  }
  protected def logTrace(msg: => String) {
    if (log.isTraceEnabled) log.trace(msg)
  }

  protected def logWarning(msg: => String) {
    if (log.isWarnEnabled) log.warn(msg)
  }

  protected def logTrace(msg: => String, throwable: Throwable) {
    if (log.isTraceEnabled) log.trace(msg, throwable)
  }

  protected def logWarning(msg: => String, throwable: Throwable) {
    if (log.isWarnEnabled) log.warn(msg, throwable)
  }


  // Method for ensuring that logging is initialized, to avoid having multiple
  // threads do it concurrently (as SLF4J initialization is not thread safe).
  protected def initLogging() { log }


}




/**
  * A trait just for Actor objects
  */
trait MyActorLogging extends Actor with ActorLogging {

  def receive = { case x => () }

  @transient private var log_ : LoggingAdapter = null

  // Method to get or create the logger for this object
  /*override  def log: LoggingAdapter = {
    if (log_ == null) {
      log_ = Logging(this.context.system, this)
      log_
    }
    else {
      log_
    }
      log_
  }*/
  /*def log2: LoggingAdapter = {
    if (log_ == null) {
      this match {
        case a:Actor => 
          //log_ = Logging.getLogger(context.system, this)
          //a.system.eventStream.setLogLevel(Logging.DebugLevel)
          log_ = Logging(a.context.system, a)
          //log_ = LoggingFactory.getLogger(a.context.system, a)
          //log_ = LoggerFactory.getLogger(a.getClass)
        
        case _ =>
          //a.system.eventStream.setLogLevel(Logging.DebugLevel)
          var className = this.getClass.getName
          // Ignore trailing $'s in the class names for Scala objects
          if (className.endsWith("$")) {
            className = className.substring(0, className.length - 1)
          }
           log_ = LoggerFactory.getLogger(className)
            //log_ = Logging(this) // because we extend actor
      }
    }
    log_
  }*/

  // Log methods that take only a String
  protected def logInfo(msg: => String) {
    if (log.isInfoEnabled) log.info(msg)
  }

  protected def logDebug(msg: => String) {
    if (log.isDebugEnabled) log.debug(msg)
  }
  
  protected def logError(msg: => String) {
    if (log.isErrorEnabled) log.error(msg)
  }

  // Log methods that take Throwables (Exceptions/Errors) too
  protected def logInfo(msg: => String, throwable: Throwable) {
    if (log.isInfoEnabled) log.info(msg, throwable)
  }

  protected def logDebug(msg: => String, throwable: Throwable) {
    if (log.isDebugEnabled) log.debug(msg, throwable)
  }
  protected def logError(msg: => String, throwable: Throwable) {
    if (log.isErrorEnabled) log.error(msg, throwable)
  }

  // Method for ensuring that logging is initialized, to avoid having multiple
  // threads do it concurrently (as SLF4J initialization is not thread safe).
  protected def initLogging() { log }


}


