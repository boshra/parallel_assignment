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

int main(int argc, char *argv[])
{
	int ierr, procid, numprocs;
	ierr = MPI_Init(&argc, &argv);
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &procid);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	
	//set matrix sizes
	int n = 512;

	//Generate random matrix of size n
	if(procid == 0)
	{
		srand(time(NULL));
		float random [n][n];
		int i,j;
		for(i = 0; i < n; i++)
		{
			for(j = 0; j < n; j++)
			{
				random[i][j] = (float) (rand() % 255);
			}
		}
	}
	float square [n][n];
	float square2 [n][n];
	//Generate square of matrix	
	Matrix_Multiply(random, random, square, n, n, n);
	Matrix_Multiply(random, random, random, n, n, n);

	printf("Equals is %d\n", IsEqual(square, square2, n, n));


	printf("Hello world! I am process %d out of %d!\n", procid,
				numprocs);
	ierr = MPI_Finalize();
}
