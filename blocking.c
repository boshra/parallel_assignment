#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

void Matrix_Multiply(float *A, float *B, float *C, int m, int n, int p)
{
	int i, j, k;
	for (i = 0; i < m; i++)
		for (j = 0; j < p; j++)
		{
			C[i*p + j] = 0;
			for (k = 0; k < n; k++)
				C[i*p + j] += A[i*n + k] * B[k*p + j];
		}
}

int IsEqual(float *A, float *B, int m, int n)
{
	int i, j;
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
		{	
			if(A[i*n + j] != B[i*n + j])
				return 0;
		}
	return 1;
}

void displayMatrix(float *matrix, int n, int m)
{
	int i,j;
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < m; j++)
		{
			printf("%.4f ", matrix[i*n + j]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[])
{
	int ierr, procid, numprocs;
	ierr = MPI_Init(&argc, &argv);
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &procid);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Status status;
	
	//Define sizes and matrices
	int n = 4;
	int m = 4;
	int row_per = n/numprocs;
	float matrix_1 [n][m];
	float matrix_2 [m][n];
	float result [n][n];
	int row, column;


	//Generate random matrix of size n
	if(procid == 0)
	{
		srand(time(NULL));
		int i,j;
		for(i = 0; i < n; i++)
		{
			for(j = 0; j < m; j++)
			{
				matrix_1[i][j] = (float) rand() / (float) RAND_MAX;
				matrix_2[j][i] = (float) rand() / (float) RAND_MAX;
			}
		}

		printf("Created random matrices\n");

		displayMatrix(matrix_1, n, m);
		printf("-------------------\n");
		displayMatrix(matrix_2, m, n);
		printf("-------------------\n");

		displayMatrix(result, n, n);
		printf("-------------------\n");

		//Send both matrices to each subprocess
		MPI_Send(&matrix_1[row_per*m], row_per*m, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
		MPI_Send(matrix_2, m*n, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);	

		MPI_Send(&matrix_1[row_per*m*2], row_per*m, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
		MPI_Send(matrix_2, m*n, MPI_FLOAT, 2, 0, MPI_COMM_WORLD);
		
		MPI_Send(&matrix_1[row_per*m*3], row_per*m, MPI_FLOAT, 3, 0, MPI_COMM_WORLD);
		MPI_Send(matrix_2, m*n, MPI_FLOAT, 3, 0, MPI_COMM_WORLD);


		//Calculate a part of the result matrix
		//TODO
		

		//Receive Data from other processes
		MPI_Recv(&result[row_per*n], row_per*n, MPI_FLOAT, 1,0, MPI_COMM_WORLD, &status);

		displayMatrix(result, n, n);
		printf("-------------------\n");

		MPI_Recv(&result[row_per*n*2], row_per*n, MPI_FLOAT, 2,0, MPI_COMM_WORLD, &status);
		MPI_Recv(&result[row_per*n*3], row_per*n, MPI_FLOAT, 3,0, MPI_COMM_WORLD, &status);

		displayMatrix(matrix_1, n, m);
		printf("-------------------\n");
		displayMatrix(matrix_2, m, n);
		printf("-------------------\n");

		displayMatrix(result, n, n);
		
		Matrix_Multiply(matrix_1, matrix_2, result, n, m, n);
	
		printf("-------------------\n");
		displayMatrix(result, m, n);

	}
	else	
	{
		ierr = MPI_Recv(matrix_1, row_per*m, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
		ierr = MPI_Recv(matrix_2, m*n, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
		
		Matrix_Multiply(matrix_1, matrix_2, result, row_per, m, row_per);	
		
		MPI_Send(result, row_per*row_per, MPI_FLOAT, 0,0,MPI_COMM_WORLD);
		
	/*	
	float square [n][n];
	float square2 [n][n];
	//Generate square of matrix	
	Matrix_Multiply(random, random, square, n, n, n);
	Matrix_Multiply(random, random, random, n, n, n);
	*/
		//printf("Equals is %d\n", IsEqual(matrix_1, matrix_2, n, n));
	}

	//printf("Hello world! I am process %d out of %d!\n", procid,
	//			numprocs);
	ierr = MPI_Finalize();
}
