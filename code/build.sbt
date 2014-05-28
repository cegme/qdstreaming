name := "qdstreaming"

version := "0.01"

scalaVersion := "2.10.3"

javacOptions ++= Seq("-Xlint:unchecked")

scalacOptions ++= Seq("-unchecked", "-deprecation", "-feature")

resolvers += "IESL release repository" at "https://dev-iesl.cs.umass.edu/nexus/content/repositories/releases/"

resolvers += "IESL snapshot repository" at "https://dev-iesl.cs.umass.edu/nexus/content/repositories/snapshots/"

resolvers += "Sonatype snapshots" at "http://oss.sonatype.org/content/repositories/snapshots/"

//resolvers += "akka" at "http://repo.akka.io/snapshots"

resolvers += "spray repo" at "http://repo.spray.io"

//resolvers += "Typesafe Repository" at "http://repo.typesafe.com/typesafe/releases"

//resolvers += "Akka Repository" at "http://repo.akka.io/releases/"

resolvers += "twitter-repo" at "http://maven.twttr.com"

libraryDependencies += "com.twitter" % "scrooge-core" % "3.15.0" from "http://repo1.maven.org/maven2/com/twitter/scrooge-core_2.10/3.15.0/scrooge-core_2.10-3.15.0.jar"

libraryDependencies += "edu.umass.cs.iesl.wikilink" % "wikilink" % "0.1-SNAPSHOT" from "https://wiki-link.googlecode.com/files/wikilink-0.1-SNAPSHOT-jar-with-dependencies.jar"

//libraryDependencies += "com.typesafe.akka" %% "akka-actor" % "2.2.3"

libraryDependencies += "com.typesafe.akka" %% "akka-remote" % "2.2.3"

//libraryDependencies += "com.typesafe.akka" %% "akka-agent" % "2.2.3"

libraryDependencies += "com.typesafe.akka" %% "akka-slf4j" % "2.2.3"

//libraryDependencies += "com.typesafe.akka" %% "akka-kernel" % "2.2.3"

//libraryDependencies += "org.apache.camel" % "camel-jetty" % "2.10.3"

//libraryDependencies += "com.typesafe.akka" %% "akka-camel" % "2.2.3"

//libraryDependencies += "org.apache.camel" % "camel-core" % "2.2.3"

libraryDependencies += "org.tukaani" % "xz" % "1.2"

//libraryDependencies += "org.slf4j" % "slf4j-api" % "1.7.7"

//libraryDependencies += "org.slf4j" % "slf4j-log4j12" % "1.7.7"

libraryDependencies += "ch.qos.logback" % "logback-classic" % "1.1.2" 

libraryDependencies += "ch.qos.logback" % "logback-core" % "1.1.2" 

libraryDependencies += "org.apache.thrift" % "libthrift" % "0.9.1"

//libraryDependencies += "org.apache.commons" % "commons-compress" % "1.7"

//libraryDependencies += "com.typesafe.akka" %% "akka-stream-experimental" % "0.2"

//libraryDependencies += "org.reactivestreams" % "reactive-streams-spi" % "0.3"

//libraryDependencies += "org.apache.spark" %% "spark-mllib" % "0.9.1"

libraryDependencies += "org.apache.spark" %% "spark-graphx" % "0.9.1"

libraryDependencies += "org.apache.spark" %% "spark-streaming" % "0.9.1"

libraryDependencies += "org.apache.spark" %% "spark-core" % "0.9.1"

 
