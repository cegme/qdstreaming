name := "qdstreaming"

version := "0.01"

scalaVersion := "2.10.3"

scalacOptions ++= Seq("-unchecked", "-deprecation")

resolvers += "Sonatype snapshots" at "http://oss.sonatype.org/content/repositories/snapshots/"

resolvers += "spray repo" at "http://repo.spray.io"

resolvers += "Typesafe Repository" at "http://repo.typesafe.com/typesafe/releases"

libraryDependencies += "com.typesafe.akka" % "akka-actor_2.10" % "2.2.3"

libraryDependencies += "com.typesafe.akka" % "akka-remote_2.10" % "2.2.3"

libraryDependencies += "org.tukaani" % "xz" % "1.2"

libraryDependencies += "org.slf4j" % "slf4j-api" % "1.7.5"
