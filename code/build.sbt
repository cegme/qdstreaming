
import AssemblyKeys._ 

assemblySettings

seq(assemblySettings: _*)

name := "qdstreaming"

version := "0.01"

scalaVersion := "2.10.3"

javacOptions ++= Seq("-Xlint:unchecked")

scalacOptions ++= Seq("-unchecked", "-deprecation", "-feature", "-Xmax-classfile-name", "254")

net.virtualvoid.sbt.graph.Plugin.graphSettings

com.twitter.scrooge.ScroogeSBT.newSettings

resolvers ++= Seq(
  "sonatype" at "https://oss.sonatype.org/content/groups/public",
  "Sonatype snapshots" at "http://oss.sonatype.org/content/repositories/snapshots/",
  "twitter-repo" at "http://maven.twttr.com",
  "spray repo" at "http://repo.spray.io",
  "IESL release repository" at "https://dev-iesl.cs.umass.edu/nexus/content/repositories/releases/",
  "sameersingh.releases" at "https://github.com/sameersingh/maven-repo/raw/master/releases",
  "sameersingh.snapshots" at "https://github.com/sameersingh/maven-repo/raw/master/snapshots"
  // -------------------------------
  //"IESL snapshot repository" at "https://dev-iesl.cs.umass.edu/nexus/content/repositories/snapshots/",
  //"akka" at "http://repo.akka.io/snapshots",
  //"Typesafe Repository" at "http://repo.typesafe.com/typesafe/releases",
  //"Akka Repository" at "http://repo.akka.io/releases/",
)


libraryDependencies ++= Seq ("com.twitter" % "parquet-format" % "2.1.0",
    "com.twitter" % "algebird_2.10" % "0.6.0",
    "com.twitter" % "algebird-core_2.10" % "0.6.0",
    "com.twitter" % "algebird-util_2.10" % "0.6.0",
    "com.twitter" % "finagle-core_2.10" % "6.17.0",
    "com.twitter" % "finagle-thrift_2.10" % "6.17.0",
    "com.twitter" % "scrooge-core_2.10" % "3.16.0",
    //"edu.umass.cs.iesl.wikilink" % "wikilink" % "0.1-SNAPSHOT" from "https://wiki-link.googlecode.com/files/wikilink-0.1-SNAPSHOT-jar-with-dependencies.jar",
    "edu.umass.cs.iesl.wikilink" % "wikilink" % "0.1-SNAPSHOT" from "https://wiki-link.googlecode.com/files/wikilink-0.1-SNAPSHOT-sources.jar",
    "de.l3s.boilerpipe" % "boilerpipe" % "1.1.0",
    "org.ccil.cowan.tagsoup" % "tagsoup" % "1.2.1",
    "org.jsoup" % "jsoup" % "1.7.3",
    "redis.clients" % "jedis" % "2.5.1",
    "org.sameersingh.utils" % "cmdopts" % "0.1.1",
    "org.sameersingh.utils" % "timing" % "0.1.1",
    "org.sameersingh.utils" % "coref" % "0.1.1",
    "org.sameersingh.utils" % "misc" % "0.1.1",
    "cc.factorie" % "factorie" % "1.0.0-RC1",
    "cc.factorie.app.nlp" % "all-models" % "1.0.0",
    "org.tukaani" % "xz" % "1.5",
    "org.apache.commons" % "commons-compress" % "1.8.1",
    "org.slf4j" % "slf4j-api" % "1.7.7",
    "org.slf4j" % "slf4j-log4j12" % "1.7.7",
    "ch.qos.logback" % "logback-classic" % "1.1.2" ,
    "ch.qos.logback" % "logback-core" % "1.1.2" ,
    "org.apache.thrift" % "libthrift" % "0.9.1",
    //"org.apache.commons" % "commons-compress" % "1.7",
    "org.apache.spark" %% "spark-core" % "1.0.1",
    "org.apache.spark" %% "spark-sql" % "1.0.1",
    "org.apache.spark" %% "spark-catalyst" % "1.0.1",
    "org.apache.spark" %% "spark-hive" % "1.0.1",
    "org.apache.spark" %% "spark-graphx" % "1.0.1",
    "org.apache.spark" %% "spark-mllib" % "1.0.1",
    "org.apache.spark" %% "spark-streaming" % "1.0.1",
    "org.apache.spark" %% "spark-tools" % "1.0.1",
    "org.apache.spark" %% "spark-repl" % "1.0.1",
    "org.apache.spark" %% "spark-examples" % "1.0.1",
    "org.apache.hadoop" % "hadoop-client" % "2.4.0",
    "org.apache.hadoop" % "hadoop-common" % "2.4.0",
    //"org.mortbay.jetty" % "servlet-api" % "3.0.20100224",
    "edu.stanford.nlp" % "stanford-corenlp" % "3.3.1",
    "edu.stanford.nlp" % "stanford-parser" % "3.3.1",
    "com.google.guava" % "guava" % "17.0",
    "io.spray" % "spray-caching" % "1.3.1",
    "com.googlecode.javaewah" % "JavaEWAH" % "0.8.6",
    "org.roaringbitmap" % "RoaringBitmap" % "0.3.6"
    )

//libraryDependencies ++= Seq(
//    "org.eclipse.jetty" % "jetty-webapp" % "8.1.7.v20120910" % "container,compile",
 //   "org.eclipse.jetty.orbit" % "javax.servlet" % "3.0.0.v201112011016" % "container,compile" artifacts Artifact("javax.servlet", "jar", "jar"))


//excludedJars in assembly <<= (fullClasspath in assembly) map { cp =>
// cp filter {x => x.data.getName.matches("sbt.*") || x.data.getName.matches(".*macros.*")}
//}


//mergeStrategy in assembly <<= (mergeStrategy in assembly) { (old) =>
//  {
//    case PathList("javax", "servlet", xs @ _*)         => MergeStrategy.first
//    case PathList(ps @ _*) if ps.last endsWith ".html" => MergeStrategy.first
//    case "application.conf" => MergeStrategy.concat
//    case "unwanted.txt"     => MergeStrategy.discard
//    case x => old(x)
//  }
//}

mergeStrategy in assembly <<= (mergeStrategy in assembly) { mergeStrategy => {
 case entry => {
   val strategy = mergeStrategy(entry)
   if (strategy == MergeStrategy.deduplicate) MergeStrategy.first
   else strategy
 }
}}

