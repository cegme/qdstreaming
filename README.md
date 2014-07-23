qdstreaming
===========

Query-driven entity resolution over a streaming data set 

## Compile 
  You need to have spark install and running somewhere

    sbt compile
    sbt -mem 8000 assembly
  
  This creates an assembly jar that has all the qdstreaming package.
  To launch a class `edu.ufl.cise.dsr.examples.WikiLink` use the follwing command.
    
    ~/projects/spark/bin/spark-submit\
     --class "edu.ufl.cise.dsr.examples.WikiLink"\
     --master "local[4]"\
     /home/cgrant/projects/qdstreaming/code/target/scala-2.10/qdstreaming-assembly-0.01.jar

  Even better, to run on the sm321 server use the following command.
  
    time ~/projects/spark/bin/spark-submit\
     --class "edu.ufl.cise.dsr.examples.DistributedER"\
     --supervise --driver-cores 1 --total-executor-cores 32 --executor-memory 2G\ 
     --driver-memory 6G -v --master spark://sm321-01.cise.ufl.edu:7077\
     /home/cgrant/projects/qdstreaming/code/target/scala-2.10/qdstreaming-assembly-0.01.jar
    
  To launch a process in the *REPL* use the following command.
    
    ~/projects/spark/bin/spark-shell\
     --master local[8]\
     --jars /home/cgrant/projects/qdstreaming/code/target/scala-2.10/qdstreaming-assembly-0.01.jar 


## Algorithm

We apply the doubling algorithm in minibatches
accepting the stream of document and monitoring a set
of queries, keeping the clusters about the same size.


1) IngestorActor
  - Pulls documents from a data stream
  - Pushes documents to a classifier actor

2) ClassifierActor
  - Vectorizes the documents
    - Extracts entity chains and contexts
  - Accepts documents
  - discards bad documents
    - If new doc is farther than the difference of the individual cluster centers
  - pushes docs to the CorefActor

3) CorefActor
  - Accept vectorized documents
  - Knows the centroid of all CanopyActors
  - Run NewDoublingAlgorithm
  - Default in update mode
  - When Canopy Actors complain
    - Sends a signal to the MergeActor

4) CanopyActor
  - Store local data structures
    - Pairwise similarity
    - term-document frequencies (ambiquity)
  - Accepts vectorized documents
  - Represents a cluster
  - Keeps track of its containing entity nodes
  - Performs incremental entity resolution 
  - if size is large
    - Perform a SCRUB step 
    - If SCRUB doesnt help
      - Complain to CorefActor
  + Possibly have several duplicate canopies doing random coreference and perform periodic merges for consensus

5) MergeActor
  - Performs MERGE on Canopy Actors
  - Call on Canopy actors UNLOAD
  


## Technical Merit

  - SCRUB/UNLOAD to the doubling algorithm
    - SCRUB --- Get rid of non-query items, or "weird" items
    - UNLOAD --- Create a summary of for ER use.
  - Self-managing Canopies
    - We can also have clone canopies, these canopies communicate to make merge decisions.
      - (1) Clone canopies only keep mentions that appear in at least one of the query nodes. 
    - (2) Canopies also throw our any instances of duplicate informaiton. Obviouse duplicates (redundant information) is not needed to make decisions.
  - Query-distribution, The number of query nodes vs the number of clusters/canopies
    - If cannopies become large, create more sub-entities
    - Swaps at the sub-entity level
  - Coreference models
    - Query-Driven within a canopy
    - Lifted Inference (Percy Liang)
    - Heirchical merge proposals (wick12hierarchical.pdf)


## Experiments

  - Successful merges/second for increased number of query nodes
    - This should increase with more query nodes
    - If not query-driven is pointless
  - ([Merges/Second] and F1-Score) vs Scrub rate
    - Find the optimal scrub rate
  - [Document Arrival Rate] vs [Merges/Second]
    - As the arrival rate increases the merges/second should not decrease
    - Constant or increase means the system is able to handle increase
  - [Entity Chains Seen] v [# Clusters produced] v [# query nodes]
    - Clusters produced should increase with entity chains seen
    - Clusters produced should increase with number of query nodes
  - [Coref Chains processed] v [Time] v [Query nodes]
    - Im not sure 

     
## Data Sets
  - Use bibtex http://www.iesl.cs.umass.edu/data/bibtex


## Visualization
  - Large Scale Real-time visualization using Superconductor [http://superconductor.github.io/superconductor/]
