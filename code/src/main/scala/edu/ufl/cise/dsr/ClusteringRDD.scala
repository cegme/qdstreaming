
package edu.ufl.cise.dsr

import org.apache.spark.sql._
import org.apache.spark.sql.catalyst.plans.logical.LogicalPlan




class ClusteringRDD(
    override val sqlContext: SQLContext, 
    override val baseLogicalPlan: LogicalPlan) 
  extends SchemaRDD(sqlContext, baseLogicalPlan) {

  // Each partition is an entity

  // How to split large partitions

  // How to combine very small partitions

  // Inserts should be fast

  // Samples across (large) entities should be simple

  // Book keeping allow fast query driven sampling


}
