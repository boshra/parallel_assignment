#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <mpi.h>

/*
void Matrix_serial(float *A, float *B, float *C, int m, int n, int p)
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
*/

int main(int argc, char *argv[])
{
	int ierr, procid, numprocs;
	ierr = MPI_Init(&argc, &argv);
	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &procid);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Status status;
	
	//Define sizes and matrices
	int n;
	if(argc == 0)
		n = 1000;
	else
		n = atoi(argv[1]);
	int m = 32;
	int row_per = n/numprocs;
	int rem = n%numprocs;
	float matrix_1 [n][m];
	float matrix_2 [m][n];
	float result [n][n];
	float C_serial[n][n];

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
		/*
		printf("Created random matrices\n");

		displayMatrix(matrix_1, n, m);
		printf("-------------------\n");
		displayMatrix(matrix_2, m, n);
		printf("-------------------\n");

		displayMatrix(result, n, n);
		printf("-------------------\n");
		*/
		time_t start, end;
		MPI_Request request[6];
		start = clock();

		//Send both matrices to each subprocess
		MPI_Isend(&matrix_1[row_per][0], row_per*m, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &request[0]);
		MPI_Isend(&matrix_2, m*n, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &request[1]);	

		//MPI_Waitall(6, request, MPI_STATUSES_IGNORE);

		MPI_Isend(&matrix_1[row_per*2][0], row_per*m, MPI_FLOAT, 2, 0, MPI_COMM_WORLD, &request[2]);
		MPI_Isend(&matrix_2, m*n, MPI_FLOAT, 2, 0, MPI_COMM_WORLD, &request[3]);
		
		//MPI_Waitall(6, request, MPI_STATUSES_IGNORE);

		MPI_Isend(&matrix_1[row_per*3][0], (row_per+rem)*m, MPI_FLOAT, 3, 0, MPI_COMM_WORLD, &request[4]);
		MPI_Isend(&matrix_2, m*n, MPI_FLOAT, 3, 0, MPI_COMM_WORLD, &request[5]);


		//Calculate a part of the result matrix
		int k;
		for (i = 0; i < row_per; i++)
			for (j = 0; j < n; j++)
			{
				result[i][j] = 0;
				for (k = 0; k < m; k++)
					result[i][j] += matrix_1[i][k] * matrix_2[k][j];
			}
		
		
		MPI_Waitall(6, &request, MPI_STATUSES_IGNORE);

		//Receive Data from other processes
		MPI_Irecv(&result[row_per], row_per*n, MPI_FLOAT, 1,0, MPI_COMM_WORLD, &request[0]);
		MPI_Irecv(&result[row_per*2], row_per*n, MPI_FLOAT, 2,0, MPI_COMM_WORLD, &request[1]);
		MPI_Irecv(&result[row_per*3], (row_per+rem)*n, MPI_FLOAT, 3,0, MPI_COMM_WORLD, &request[2]);

		MPI_Waitall(3, &request, MPI_STATUSES_IGNORE);

		end = clock();
			
		/*
		displayMatrix(result, n, n);
		printf("-------------------\n");
		*/

		
		printf("nonbl,%d,%.1f\n", n,(1000*(double)end-start)/CLOCKS_PER_SEC);
		/*
		start = clock();

		/*
		displayMatrix(matrix_1, n, m);
		printf("-------------------\n");
		displayMatrix(matrix_2, m, n);
		printf("-------------------\n");
		

		displayMatrix(result, n, n);
		printf("-------------------\n");
		



		Matrix_serial((float *) matrix_1, (float *)matrix_2, (float *)C_serial, n, m, n);
	
		end = clock();
		
		printf("%.1f\n", (1000*(double)end-start)/CLOCKS_PER_SEC);
		/*
		printf("-------------------\n");
		displayMatrix(C_serial, n, n);
		

		//If the computation isn't done correctly exit with an error code
		if(IsEqual((float *)result, (float *)C_serial, n, n) != 1)
		{
			printf("ERROR, MATRICES NOT EQUAL\n");
			exit(-1);
		}
		*/
	}
	else	
	{
		if(procid == 3)
			row_per = row_per + rem;

		MPI_Request request[2];

		ierr = MPI_Irecv(&matrix_1, row_per*m, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &request[0]);
		ierr = MPI_Irecv(&matrix_2, m*n, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &request[1]);
		
		//displayMatrix(matrix_1, row_per, m);
		//printf("-------------------\n");
		
		MPI_Waitall(2, request, MPI_STATUSES_IGNORE);
		
		int i, j, k;
		for (i = 0; i < row_per; i++)
			for (j = 0; j < n; j++)
			{
				result[i][j] = 0;
				for (k = 0; k < m; k++)
					result[i][j] += matrix_1[i][k] * matrix_2[k][j];
			}


		MPI_Isend(&result, row_per*n, MPI_FLOAT, 0,0,MPI_COMM_WORLD, &request[0]);
		MPI_Wait(&request[0], MPI_STATUS_IGNORE);
		
	}

	ierr = MPI_Finalize();
}
