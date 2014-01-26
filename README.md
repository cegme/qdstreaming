qdstreaming
===========

Query-driven entity resolution over a streaming data set 

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

  1) SCRUB/UNLOAD to the doubling algorithm
    - SCRUB --- Get rid of non-query items, or "weird" items
    - UNLOAD --- Create a summary of for ER use.
  2) Self-managing Canopies
  3) Query-distribution, The number of query nodes vs the number of clusters/canopies
    3a) If cannopies become large, create more sub-entities
    3b) Swaps at the sub-entity level
  4) Coreference models
    - Query-Driven within a canopy
    - Lifted Inference (Percy Liang)
    - Heirchical merge proposals (wick12hierarchical.pdf)


## Experiments

  1) Successful merges/second for increased number of query nodes
    - This should increase with more query nodes
    - If not query-driven is pointless
  2) ([Merges/Second] and F1-Score) vs Scrub rate
    - Find the optimal scrub rate
  3) [Document Arrival Rate] vs [Merges/Second]
    - As the arrival rate increases the merges/second should not decrease
    - Constant or increase means the system is able to handle increase
  4) [Entity Chains Seen] v [# Clusters produced] v [# query nodes]
    - Clusters produced should increase with entity chains seen
    - Clusters produced should increase with number of query nodes
  5) [Coref Chains processed] v [Time] v [Query nodes]
    - Im not sure 

     
## Data Sets
  - Use bibtex http://www.iesl.cs.umass.edu/data/bibtex
  - 


## Visualization
  - Large Scale Real-time visualization using Superconductor [http://superconductor.github.io/superconductor/]
