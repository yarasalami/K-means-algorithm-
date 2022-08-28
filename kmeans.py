import math
import sys

# toDelete
# 1.
#     x = '\n'.join([' '.join(['{:4}'.format(item) for item in row]) for row in inputMat])
#     print(x)

# constants
EPSILON = 0.001


# d := number of column of an input file = len(inputMat[0])
# n := number of line of an input file = number of vectors = len(inputMat)


#  max_itr := the maximum number of iterations of the K-means algorithm, the default value is 200.
#  k := the number of clusters required. = len(clusters)
#  input_filename := *.txt file that contains data-points separated by commas.
#  output_filename := *.txt file that contains the centroids separated by commas.
def execute(k, max_itr, input_filename, output_filename):
    # inputMat := input-matrix
    inputMat = []
    # open file
    input_ifp = open(input_filename, "r")
    if input_ifp is None:
        print("Invalid Input! \n")
        return
    output_ifp = open(output_filename, "w")
    if output_ifp is None:
        print("Invalid Input! \n")
        return

    # step 1: store all the input from  input_filename into a 2-dimensional array
    for line in input_ifp:
        temp_array = line.split(",")
        inputMat.append(temp_array)

    # bug
    if k > len(inputMat):
        print("Invalid Input! \n")
        return

    # converting type inputMat[i][j] from str to float
    for i in range(len(inputMat)):
        for j in range(len(inputMat[i])):
            inputMat[i][j] = float(inputMat[i][j])

    # step 2: Algorithm
    # clusters := centroids µ1, µ2, ... , µK as first k data-points x1, x2, ... , xK
    clusters = [([0] * len(inputMat[0])) for i in range(k)]
    for i in range(k):
        for j in range(len(inputMat[i])):
            clusters[i][j] = inputMat[i][j]

    algorithm(clusters, inputMat, max_itr)

    # step 3 : output
    for i in range(k):
        for j in range(len(inputMat[0])):
            output_ifp.write(format(float(clusters[i][j]), '.4f'))
            if j != (len(inputMat[0]) - 1):
                output_ifp.write(',')
        output_ifp.write("\n")

    # close file
    input_ifp.close()
    output_ifp.close()


def algorithm(clusters, inputMat, max_itr):
    for iteration in range(max_itr):
        if norm_mat(clusters):
            break
        # groupOfClusters := group of clusters by S1,...,SK, each cluster Sj is represented by its
        #    centroid which is the mean µj ∈ Rd of the cluster’s members.
        groupOfClusters = [([0] * len(inputMat)) for i in range(len(clusters))]

        #  for xi, 0 < i ≤ N:
        #  Assign xi to the closest cluster Sj : argmin_Sj(xi − µj )^2 , ∀j 1 ≤ j ≤ K
        #  0 < index ≤ K
        for x_i in range(len(inputMat)):
            index = min_index(clusters, inputMat[x_i])
            groupOfClusters[index][x_i] = 10

        #  for µk, 0 < i ≤ K:
        #  we want to change clusters[i] , 0< i <= k
        for m_i in range(len(clusters)):
            update(clusters[m_i], groupOfClusters[m_i], inputMat)

    return None


def norm_mat(matrix):
    for i in range(len(matrix)):
        if calculate_norm(matrix[i]) > EPSILON:
            return False
    return True


def calculate_norm(vector):
    sum_norm = 0.0
    for i in range(len(vector)):
        sum_norm += math.pow(float(vector[i]), 2.0)
    return math.sqrt(sum_norm)


def min_index(clusters, vector):
    m_index = 0
    min_distance = float('inf')
    for i in range(len(clusters)):
        temp_min_distance = distance(vector, clusters[i])
        if temp_min_distance < min_distance:
            m_index = i
            min_distance = temp_min_distance
    return m_index


def distance(vector1, vector2):
    sum_distance = 0.0
    # len(vector2) = len(vector1)
    for i in range(len(vector2)):
        # argmin_Sj(xi − µj)^2
        sum_distance += math.pow(float(vector1[i]) - float(vector2[i]), 2.0)

    return sum_distance


def update(toChange, groupOfClusters, inputMat):
    cnt = 0
    # fill tochange with 0.0
    for i in range(len(toChange)):
        toChange[i] = 0.0
    for j in range(len(inputMat)):
        if groupOfClusters[j] > 5:
            cnt += 1
            sumVectors(toChange, inputMat[j])
    avgVectors(toChange, cnt)
    return None


def sumVectors(vector1, vector2):
    # len(vector2)=len(vector1)
    for i in range(len(vector1)):
        vector1[i] += vector2[i]
    return None


def avgVectors(vector1, counter):
    if counter == 0:
        return None

    # len(vector2)=len(vector1)
    for i in range(len(vector1)):
        vector1[i] /= counter
    return None


# main
# sys.argv is the list of command-line arguments.
# len(sys.argv) is the number of command-line arguments.
# sys.argv[0] is the program i.e. the script name.
input_argv = sys.argv
input_argc = len(sys.argv)
# todoo we need to valid(input_args)
if input_argc == 5:
    # update max_itr if needed
    execute(int(input_argv[1]), int(input_argv[2]), input_argv[3], input_argv[4])
else:
    execute(int(input_argv[1]), 200, input_argv[2], input_argv[3])
