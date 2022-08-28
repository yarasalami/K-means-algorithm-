# K-means-algorithm
Given a set of N datapoints x1, x2, . . . , xN ∈ R^d , the goal is to group the data into K ∈ N clusters, each datapoint is assigned to exactly one cluster and the number of clusters K is such that 1 &lt; K &lt; N. We will denote the group of clusters by S1, S2, . . . , SK, each cluster Sj is represented by it’s centroid which is the mean µj ∈ R^d of the cluster’s members.

The program receives the input: K, filename and optional(max iter):
  (a) K – the number of clusters required.
  (b) input filename - *.txt file that contains datapoints separated by commas.
  (c) output filename - *.txt file that contains the centroids separated by commas.
  (d) max iter – the maximum number of iterations of the K-means algorithm, if not provided
                  the default value is 200.
                  
Outputs must be formatted to 4 decimal places (use: ’%.4f’) in both languages c and python.
