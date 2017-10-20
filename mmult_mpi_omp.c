#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#define min(x, y) ((x) < (y) ? (x) : (y))

double *gen_matrix(int n, int m);
int mmult(double *c, double *a, int aRows, int aCols, double *b, int bRows, int bCols);
void compare_matrix(double *a, double *b, int nRows, int nCols);

int get_nrows(char *);
int get_ncols(char *);
double *get_matrix(int, char *);
double *get_row(int, int, char *);

/**
    Program to multiply a matrix times a matrix using both
    mpi to distribute the computation among nodes and omp
    to distribute the computation among threads.
*/

int main(int argc, char *argv[])
{
	double *aa;  /* the A matrix */
	double *bb;  /* the B matrix */
	double *cc1; /* A x B computed using the omp-mpi code you write */
	double *cc2; /* A x B computed using the conventional algorithm */
	int myid, numprocs;
	double starttime, endtime;
	MPI_Status status;
	/* insert other global variables here */
	int nrows, ncols;
	double *b, *c;
	double *times;
	double total_times;
	int run_index;
	int nruns;
	int anstype, rown;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	if (argc > 1)
	{
		nrows = get_nrows(argv[2]);
		ncols = get_ncols(argv[1]);

		if (nrows != ncols)
		{
			printf("Incompatible matrix dimensions");
			return -1;
		}

		b = (double *)malloc(sizeof(double)*nrows);
		b = get_matrix(nrows, argv[2]);

		double *ans = malloc(sizeof(double) * 1);

		c = (double *)malloc(sizeof(double) * nrows);

		if (myid == 0)
		{
			// Master Code goes here
			aa = malloc(sizeof(double) * nrows * ncols);
			bb = malloc(sizeof(double) * nrows);
			cc1 = malloc(sizeof(double) * nrows);

			/* Insert your master code here to store the product into cc1 */
			starttime = MPI_Wtime();
			MPI_Bcast(b, ncols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

			int i;
			for (i = 1; i < nrows + 1; i++)
			{
				double *row = get_row(nrows, i, argv[1]);
				double *test = row;

				int t;
				for (t = 0; t < nrows; t++)
				{
					//printf("%lf\n", *test);
					test++;
				}

				status.MPI_TAG = i;

				int rank = i % 3;
				if (rank == 0)
					rank = 3;

				MPI_Send(row, ncols, MPI_DOUBLE, rank, i, MPI_COMM_WORLD);
			}

			for (i = 0; i < nrows; i++)
			{
				MPI_Recv(ans, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				anstype = status.MPI_TAG;
				cc1[anstype - 1] = *ans;
			}

			endtime = MPI_Wtime();

			printf("%f\n", (endtime - starttime));

			int x;
			for (x = 0; x < nrows; x++)
			{
				printf("%lf\n", *cc1);
				cc1++;
			}
		}
		else
		{
			// Slave Code goes here
			MPI_Bcast(b, nrows, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			double *buf = malloc(sizeof(double) * nrows);

			int i;
			for (i = 0; i < nrows; i++)
			{
				MPI_Recv(buf, nrows, MPI_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

				double *buffer = buf;
				int t;
				for (t = 0; t < nrows; t++)
				{
					//printf("%lf\n", *buffer);
					buffer++;
				}

				rown = status.MPI_TAG;

				int j;
				for (j = 0; j < nrows; j++)
				{
					*ans += b[j] * buf[j];
				}

				//				printf("%lf\n", *ans);
				//				printf("***********");

				MPI_Send(ans, 1, MPI_DOUBLE, 0, rown, MPI_COMM_WORLD);
			}
		}
	}
	else
	{
		fprintf(stderr, "Usage matrix_times_vector <size>\n");
	}

	MPI_Finalize();

	return 0;
}

//count number of newlines between file.start and file.end
int get_nrows(char *input)
{
	FILE *fp;
	if (fp=fopen(input,"r") == NULL)
	{
		printf("File input error");
		return -1;
	}
	int row_count = 0;
	int cols = 0;
	int c;

	while ((c = fgetc(fp)) != EOF)
		if (c == '\n')
			row_count++;
	fclose(fp);
	return row_count;
}

//count number of values between line.start and line.end
int get_ncols(char *input)
{
	FILE *fp;
	if (fp=fopen(input,"r") == NULL)
	{
		printf("File input error");
		return -1;
	}
	int col_count=1;
	int c;
	while ((c = fgetc(fp)) != '\n')	
		if (c == ' ')		
			col_count++;		
	fclose(fp);
	return col_count;
}


double* get_matrix(int nrows, char *input)
{
	//guard against file and malloc failures
	FILE *fp;
	if (fp=fopen(input,"r") == NULL)
	{
		printf("File input error");
		return -1;
	}
	double *ret_mat;
	if((ret_mat = malloc(sizeof(double) * nrows))==NULL)
		return -1;	
	
	double *working_mat = ret_mat;
	double c;
	
	int i;
	//read doubles into c, then store in the "matrix" we're pointing to
	for (i = 0; i < nrows; i++)
	{
		fscanf(fp, "%lf", &c);
		*working_mat = c;
		working_mat++;
	}	

	fclose(fp);
	return ret_mat;
}

double* get_row(int ncols, int row, char *input)
{
	//guard against file and malloc failures
	FILE *fp;
	if (fp=fopen(input,"r") == NULL)
	{
		printf("File input error");
		return -1;
	}
	double *ret_row;
	if((ret_row = malloc(sizeof(double) * ncols)) == NULL)
		return -1;

	double *working_row = ret_row;
	int curr_row = 1;

	//iterate to row we want using newlines as indicators
	double value;
	while (curr_row != row)
	{
		if ((value = fgetc(fp)) == '\n')		
			curr_row++;		
	}

	int i;
	for (i = 0; i < ncols; i++)
	{
		fscanf(fp, "%lf", &value);
		*working_row = value;
		working_row++;
	}
	fclose(fp);
	return ret_row;
}
