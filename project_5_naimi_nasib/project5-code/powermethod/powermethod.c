/****************************************************************
 *                                                              *
 * This file has been written as a sample solution to an        *
 * exercise in a course given at the CSCS-USI Summer School.    *
 * It is made freely available with the understanding that      *
 * every copy of this file must include this header and that    *
 * CSCS/USI take no responsibility for the use of the enclosed  *
 * teaching material.                                           *
 *                                                              *
 * Purpose: : Parallel matrix-vector multiplication and the     *
 *            and power method                                  *
 *                                                              *
 * Contents: C-Source                                           *
 *                                                              *
 ****************************************************************/


#include <stdio.h>
#include "hpc-power.h"
#include <mpi.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#define SEED 42
#define MAX 10000
#define MIN -10000


// prints the matrix
void printMatrix(double* A, int n, int numrows){
    for(int i = 0; i < numrows; i++){
        for(int j = 0;  j < n; j++){
            printf("%f ", A[i*n+j]);
        }
        printf("\n");
    }
}

// prints the vector
void printVec(double* x, int n) 
{
    for(int i = 0; i < n; i++){
        printf("%f ", x[i]);
    }
    printf("\n");
}

// random double generator
double pseudorand(double max, double min)
{   
    srand(SEED);
    double range = (max - min); 
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

// generates a radom matrix with aij in the range [min,max]
double* generateRandomMat(int n, int startrow, int numrows, double max, double min){
    double* A;
    int i, j;
    A = (double*)calloc(n * numrows, sizeof(double));
    for(i = 0; i < numrows; i++){
        for(j = 0; j < n; j++){
            A[i * n + j] = pseudorand(max,min);
        }
    }
    return A;
}

// generates the identity matrix
double* generateMatID(int n, int startrow, int numrows){
    double* A;
	int i;
	int diag;

	A = (double*)calloc(n * numrows, sizeof(double));

	for (i = 0; i < numrows; i++) {
		diag = startrow + i;

		A[i * n + diag] = 1;
	}
	return A;
}

// generates a matrix of ones
double* generateMatOnes(int n, int startrow, int numrows){
    double* A;
    int i;
    A = (double*)calloc(n * numrows, sizeof(double));
    for(i = 0; i < numrows*n; i++){
        A[i]=1;
    }
    return A;
}

// computes y=Ax
double* matVec(double* A, double* x, int n, int numrows) {
    double* y = (double*)calloc(numrows, sizeof(double));
    double yi;
    for(int i = 0; i < numrows; i++){
        yi = 0;
        for(int j  = 0; j < n; j++){
            yi += A[i * n + j] * x[j];
        }
        y[i]=yi;
    }
    return y;
}

// computes norm(x)
double norm(double* x, int n) {
    double normsqrd = 0;
    for(int i = 0; i < n; i++){
        normsqrd += x[i] * x[i];
    }
    return sqrt(normsqrd);
}

// powermethod
double* powermethod(double* A, double* x, int n, int numrows) {
    double normx = norm(x,n);
    for(int i = 0; i < n; i++){
        x[i] /= normx;
    }
    // printf("x/norm(x): \n");
    // printVec(x, n);
    return matVec(A, x, n, numrows);
}


int main (int argc, char *argv[])
{
    // Read flags
    if (argc !=3)
    {
        printf("Usage: main N\n");
        printf("  N        matrix dimension\n");
        printf("  NUM_ITER number of powermethod iterations");
        exit(1);
    }
    int N = atoi(argv[1]);
    if(N < 1){
        fprintf(stderr, "N must be a positive integer\n");
        exit(-1);
    }
    int NUM_ITER = atoi(argv[2]);
    if(NUM_ITER < 1){
        fprintf(stderr, "NUM_ITER must be a positive integer\n");
        exit(-1);
    }
    

    int my_rank, size;
    int snd_buf, rcv_buf;
    int right, left;
    int sum, i;

    MPI_Status  status;
    MPI_Request request;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* This subproject is about to write a parallel program to
       multiply a matrix A by a vector x, and to use this routine in
       an implementation of the power method to find the absolute
       value of the largest eigenvalue of the matrix. Your code will
       call routines that we supply to generate matrices, record
       timings, and validate the answer.
    */

    srand((unsigned) time(NULL));
    int startrow = N / size * my_rank; // assumption that N is divisible by size
    int numrows =  (my_rank == size - 1) ? N / size + N % size : N / size;
    int lambda;
    double* A;
    double* x;
    double* localx;
    double normx = 0;
    double localNormx;
    localx = (double*)calloc(numrows, sizeof(double));
    x = (double*)calloc(N, sizeof(double));
    
    // matrix 1024x1024
    // MPI_Bcast MPI_Gather MPI_Scatter

    A = generateRandomMat(N, startrow, numrows, MAX, MIN);

    double total_time=0;
    double time_start=hpc_timer();
    
    if(my_rank==0){
        x = generateRandomMat(N, 0, 1, MAX, MIN);
    }
    for(int iter = 0; iter < NUM_ITER; iter++){
        MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        localx = powermethod(A, x, N, numrows);
        MPI_Gather(localx, numrows, MPI_DOUBLE, x, numrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }

    if(my_rank == 0){
        lambda = norm(x, N);
        total_time = hpc_timer()-time_start;
        for(int i=0; i < N; i++){
            if(x[i] < 0)
                x[i]=-x[i];
        }
        printf("version,processes, N, NIter, time, result\n");
        printf("gather,%2d, %4d, %4d, %6f, %10f\n", size, N, NUM_ITER, total_time,lambda);
    }
    
    MPI_Finalize();
    return 0;
}
