name := "qdstreaming"

version := "0.01"

scalaVersion := "2.10.3"

scalacOptions ++= Seq("-unchecked", "-deprecation", "-feature")

resolvers += "Sonatype snapshots" at "http://oss.sonatype.org/content/repositories/snapshots/"

resolvers += "akka" at "http://repo.akka.io/snapshots"

resolvers += "spray repo" at "http://repo.spray.io"

resolvers += "Typesafe Repository" at "http://repo.typesafe.com/typesafe/releases"


libraryDependencies += "com.typesafe.akka" % "akka-actor_2.10" % "2.3.0"

libraryDependencies += "com.typesafe.akka" % "akka-remote_2.10" % "2.3.0"

libraryDependencies += "com.typesafe.akka" % "akka-agent_2.10" % "2.3.0"

libraryDependencies += "com.typesafe.akka" % "akka-slf4j_2.10" % "2.3.0"


libraryDependencies += "org.tukaani" % "xz" % "1.2"

libraryDependencies += "org.slf4j" % "slf4j-api" % "1.7.5"

libraryDependencies += "ch.qos.logback" % "logback-classic" % "1.0.0" % "runtime"

libraryDependencies += "org.apache.thrift" % "libthrift" % "0.9.1"

libraryDependencies += "org.apache.commons" % "commons-compress" % "1.7"

libraryDependencies += "com.typesafe.akka" % "akka-kernel_2.10" % "2.3.0"

libraryDependencies += "com.typesafe.akka" % "akka-camel_2.10" % "2.3.0"

libraryDependencies += "org.apache.camel" % "camel-jetty" % "2.3.0"

libraryDependencies += "org.apache.camel" % "camel-core" % "2.3.0"

libraryDependencies += "org.reactivestreams" % "reactive-streams-spi" % "0.3"

libraryDependencies += "com.typesafe.akka" %% "akka-stream-experimental" % "0.2"

libraryDependencies += "org.apache.spark" % "spark-mllib_2.10" % "0.9.1"

libraryDependencies += "org.apache.spark" % "spark-streaming_2.10" % "0.9.1"
 
