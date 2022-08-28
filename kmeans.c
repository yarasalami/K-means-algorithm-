#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "math.h"
#include <float.h>

#define LINESIZE 1000

void resetMat(int row,int col,double** mat);
double** createMat(int col, int row);
int calculateRows(char* fileName);
int calculateCol(char* fileName);
void fillMat(char* fileName,double** inputMat);
void freeMemory(double** matrix ,int n);
void update(double* toChage,double* GroupOfClusters,double** inputMat);
void sumVectors(double* vector1,double* vector2);
void avgVectors(double* vector1,int cnt);
void algorithm(double** clusters, double ** inputMat, double ** GroupOfClusters);
int minIndex(double** clusters, double* victor);
double distance(double* vector1 , double* vector2);
double calculateNorm(double* vector);
int normMat(double** matrix);

/*
 *  max_itr := the maximum number of iterations of the K-means algorithm, the default value is 200.
 *  k := the number of clusters required.
 *  d := number of column of an input file.
 *  n := number of line of an input file , = number of vectors.
 */
int max_itr = 200;
int k;
int d;
int n;
const double epsilon = 0.001;

/*
 * argv := ["...",k,max_itr,input_filename,output_filename]
 * argv := ["...",k,input_filename,output_filename]
 * argv[0] is the name of the program.
 */
int main(int argc, char** argv) {
    /*
     *   input_filename := *.txt file that contains data-points separated by commas.
     *   output_filename := *.txt file that contains the centroids separated by commas.
     */
    char* input_filename;
    char* output_filename;
    FILE* input_ifp;
    FILE* output_ifp;
    int i,j;
    char* useless;
    /*
     *  clusters := centroids µ1, µ2, ... , µK as first k data-points x1, x2, ... , xK
     */
    double** clusters = NULL;
    /*
     *  groupOfClusters := group of clusters by S1,...,SK, each cluster Sj is represented by it’s
     *    centroid  which is the mean µj ∈ Rd of the cluster’s members.
     */
    double** groupOfClusters = NULL;
    /*
     *  inputMat := input-matrix
     */
    double** inputMat = NULL;

    assert(argc > 0);

    k = strtol(argv[1], &useless, 10);
    /*
    * update max_itr if needed
    */
    if(argc >= 5){
        max_itr = strtol(argv[2], &useless, 10);
        input_filename = argv[3];
        output_filename = argv[4];
    }else{
        input_filename = argv[2];
        output_filename = argv[3];
    }
    assert(k > 0);
    assert(max_itr > 0);
    /*
     * open file
     */
    input_ifp = fopen(input_filename,"r");
    if(input_ifp == NULL){
        printf("Invalid Input! \n");
        return 0;
    }
    output_ifp = fopen(output_filename,"w+");
    if(output_ifp == NULL){
        printf("Invalid Input! \n");
        return 0;
    }

    /*
     * step 1: store all the input from  input_filename into a 2-dimensional array
     *
     *  input_filename : 2.3,7.0,4.7       x_1
     *                   66.7,8.9.11.2     x_2
     *                   3.4,88.7,5.7      x_3
     *                   1.2,3.4,5.6       x_4
     *                   4.0,78.9,8.1      x_5
     *
     *  inputMat :=  [[2.3,7.0,4.7]
     *               ,[66.7,8.9.11.2]
     *               ,[3.4,88.7,5.7]
     *               ,[1.2,3.4,5.6]
     *              ,[4.0,78.9,8.1]]
     *   N = 5
     *   d = 3
    */
    /*
     * calculating the number of n in the input file ,:= N
     */
    n = calculateRows(input_filename);
    /*
     * bug
     */
    if(k>n){
        printf("Invalid Input! \n");
        return 0;
    }

    /*
     * calculating the number of d in the input file ,:= d
     */
    d = calculateCol(input_filename);


    inputMat = createMat(n, d);

    assert(inputMat!=NULL);
    fillMat(input_filename,inputMat);

    /*
     *  step 2: Algorithm
     *  initialize centroids µ1, µ2, ... , µK as first k data-points x1, x2, ... , xK
     *  k=2
     *  inputMat :=  [[2.3,7.0,4.7]
     *               ,[66.7,8.9.11.2]
     *               ,[3.4,88.7,5.7]
     *               ,[1.2,3.4,5.6]
     *               ,[4.0,78.9,8.1]]
     *
     *  clusters =  [[2.3,7.0,4.7]         µ1 = x_1
     *              ,[66.7,8.9.11.2]]      µ2 = x_2
    */
    clusters = createMat(k, d);
    assert(clusters != NULL);
    for(i=0;i<k; i++){
        for(j=0;j<d;j++){
            clusters[i][j] = inputMat[i][j];
        }
    }
    /*
     *  groupOfClusters := [[0.0,0.0,0.0,0,0,0.0]
     *                      ,[0.0,0.0,0.0,0,0,0.0]]
    */
    groupOfClusters = createMat(k, n);
    assert(groupOfClusters != NULL);

    for(i=0; i<k; i++){
        for(j=0;j<n;j++){
            groupOfClusters[i][j] = 0.0;
        }
    }
    algorithm(clusters,inputMat,groupOfClusters);
    /*
     *  step 3 : output
     */
    for(i=0;i<k;i++){
        for(j=0;j<d;j++){
            fprintf(output_ifp,"%.4f",clusters[i][j]);
            if(j!=(d-1)){
                fprintf(output_ifp,"%s",",");
            }
        }
        fprintf(output_ifp, "\n");
    }
    /*
     * close file
     */
    fclose(input_ifp);
    fclose(output_ifp);

    /*
     * step 4: freeing memory
     */
    freeMemory(inputMat, n);
    freeMemory(groupOfClusters, k);
    freeMemory(clusters,k);

    return 0;
}

void algorithm(double** clusters, double** inputMat, double** GroupOfClusters){
    int numOfItr=0;
    int x_i;
    int m_i;
    int index;
    while (numOfItr < max_itr){
        if(normMat(clusters)==1){
            printf("break\n");
            break;
        }
        resetMat(k,n,GroupOfClusters);
        /*
         * for xi, 0 < i ≤ N:
         *  Assign xi to the closest cluster Sj : argmin_Sj(xi − µj )^2 , ∀j 1 ≤ j ≤ K
         *  0 < index ≤ K
         */
        for(x_i=0;x_i<n;x_i++){
            index = minIndex(clusters, inputMat[x_i]);
            GroupOfClusters[index][x_i] = 10;
        }
        /*
         * for µk, 0 < i ≤ K:
         *  we want to change clusters[i] , 0< i <= k
         */
        for(m_i=0;m_i<k;m_i++){
            update(clusters[m_i], GroupOfClusters[m_i], inputMat);
        }
        numOfItr++;
    }

}
int normMat(double** matrix){
    int i;
    for(i=0;i<k;i++){
        if(calculateNorm(matrix[i]) > epsilon){
            return 0;
        }
    }
    return 1;
}
double calculateNorm(double* vector){
    int i;
    double sum=0.0;
    double value;
    for(i=0;i<d;i++){
        sum = sum + pow(vector[i],2.0);
    }
    value = sqrt(sum);
    return value;
}


void resetMat(int row,int col,double** mat){
    int i,j;
    for(i=0;i<row;i++){
        for(j=0;j<col;j++){
            mat[i][j] = 0.0;
        }
    }
}

void sumVectors(double* vector1,double* vector2){
    int i;
    for(i=0;i<d;i++){
        vector1[i] = vector1[i] + vector2[i];
    }
}
void avgVectors(double* vector1,int cnt){
    int i;
    if(cnt == 0){
        return;
    }
    for(i=0;i<d;i++){
        vector1[i] = vector1[i]/cnt;
    }
}
void update(double* toChage,double* GroupOfClusters,double** inputMat){
    int i;
    int cnt=0;
    /*
     * fill tochange with 0.0
     */
    for(i=0;i<d;i++){
        toChage[i]= 0.0;
    }
    for(i=0;i<n;i++){
        if(GroupOfClusters[i] > 5){
            cnt++;
            sumVectors(toChage,inputMat[i]);
        }
    }
    avgVectors(toChage,cnt);
}

int minIndex(double** clusters, double* victor){
    int minIndex=0;
    double minDistance=DBL_MAX;
    double tempMinDistance;
    int i;

    for(i=0;i<k;i++){
        tempMinDistance = distance(victor, clusters[i]);
        if(tempMinDistance<minDistance) {
            minIndex = i;
            minDistance = tempMinDistance;
        }
    }
    return minIndex;
}
double distance(double* vector1 , double* vector2){
    int i;
    double sum=0.0;
    for(i=0; i < d; i++){
        /*
         * argmin_Sj(xi − µj)^2
         */
        sum = sum + pow((vector1[i] - vector2[i]), 2.0);
    }
    return sum;
}

/*
 *  freeing 2-dimensional arrays
 */
void freeMemory(double** matrix ,int len){
    int i;
    if(matrix == NULL){
        return;
    }
    for(i = 0; i < len ; i++){
        if(matrix[i] == NULL){
            continue;
        }
        free(matrix[i]);
    }
    free(matrix);
}

/*
 *  create 2-dimensional arrays
 */
double** createMat(int col, int row){
    int i;
    double ** matrix = (double**)malloc(col* sizeof(double *));
    assert(matrix != NULL);
    for(i=0;i<col;i++){
        matrix[i]= (double*)malloc(row* sizeof(double ));
        assert(matrix[i] != NULL);
    }
    return matrix;
}
/*
 * calculation the number of col in fileName
 */
int calculateCol(char* fileName){
    int cnt = 0;
    FILE* ifp;
    char* token;
    const char breaks[] = ",";
    char line[LINESIZE];
    /*
     * open file
     */
    ifp = fopen(fileName,"r");
    if(ifp == NULL) {
        printf("Invalid Input! \n");
        return -1;
    }
    /*
     * calculating the number of col
     */
    fgets(line, 1000, ifp);
    token = strtok(line, breaks);
    while(token != NULL)
    {
        token = strtok(NULL, breaks);
        ++cnt;
    }

    /*
     * close file
     */
    fclose(ifp);
    return cnt;

}
/*
 * calculate the number of rows in fileName
 */
int calculateRows(char* fileName){
    int cnt = 0;
    char line[LINESIZE];
    FILE* ifp;

    /*
     * open file
     */
    ifp = fopen(fileName,"r");
    if(ifp ==NULL) {
        printf("Invalid Input! \n");
        return 0;
    }
    /*
     * calculating the number of rows
     */
    while (fgets(line, LINESIZE, ifp) != NULL){
        cnt++;
    }
    /*
     * close file
     */
    fclose(ifp);
    return cnt ;
}

/*
 * fill 2-dimensional arrays
 */
void fillMat(char* fileName,double** inputMat){
    FILE* ifp;
    char* token;
    const char breaks[] = ",";
    char line[LINESIZE];
    int row=0,col=0;
    char* useless=NULL;

    /*
     * open file
     */
    ifp = fopen(fileName,"r");
    if(ifp == NULL) {
        printf("Invalid Input! \n");
        return;
    }
    /*
     * filling the inputMat
     */
    while (fgets(line,LINESIZE,ifp) != NULL){
        /*
         * get the first token from each line
         */
        token = strtok(line,breaks);
        while (token != NULL){
            /*
             * walk through other tokens
             * insert by using strtod
             */
            inputMat[row][col] = strtod(token, &useless);
            token = strtok(NULL,breaks);
            /*
             * next column
             */
            col++;
        }
        col=0;
        /*
         * next line
         */
        row++;
    }
    /*
     * close the  file
     */
    fclose(ifp);
}