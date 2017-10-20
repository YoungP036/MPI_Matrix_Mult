#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#define min(x, y) ((x) < (y) ? (x) : (y))

double *gen_mmatrix(int nrows, char *input);
int mmult(double *c, double *a, int aRows, int aCols, double *b, int bRows, int bCols);
void compare_matrix(double *a, double *b, int nRows, int nCols);
double *get_col(int nrows, int ncols, int col, char *input);
int get_nrows(char *input);
int get_ncols(char *input);
double *get_row(int, int, char *input);

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
		/* User Submitted Matrix Handling  */
		nrows = get_nrows(argv[2]);
		ncols = get_ncols(argv[1]);

		if (nrows != ncols)
		{
			printf("Invalid Matrix Parameters.");
			return -1;
		}

		b = (double *)malloc(sizeof(double) * nrows);
		b = gen_mmatrix(nrows, argv[2]);

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
				//get current row
				double *row = get_row(nrows, i, argv[1]);
				// double *test = row;

				// int t;
				// for (t = 0; t < nrows; t++)
				// {
				// 	//printf("%lf\n", *test);
				// 	test++;
				// }

				//how we communicate current row
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
			//note ncols=nrows in mat 2
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

int get_nrows(char *input)
{
	FILE *fp;
	int count = 0;
	int cols = 0;
	int c;

	fp = fopen(input, "r+");

	if (fp == NULL)
	{
		printf("No File");
		return -1;
	}
	else
	{
		while ((c = fgetc(fp)) != EOF)
		{
			if (c == '\n')
				count++;
		}
	}

	fclose(fp);
	return count;
}

int get_ncols(char *input)
{
	FILE *fp;
	int count = 1;
	int c;

	fp = fopen(input, "r");

	if (fp == NULL)
	{
		printf("No File");
		return -1;
	}
	else
	{
		while ((c = fgetc(fp)) != '\n')
		{
			if (c == ' ')
			{
				count++;
			}
		}
	}

	fclose(fp);
	return count;
}

double *gen_mmatrix(int nrows, char *input)
{
	FILE *fp;
	double *m;
	m = malloc(sizeof(double) * nrows);
	double *mm = m;

	if (m == NULL)
	{
		return -1;
	}

	double c;

	fp = fopen(input, "r");

	if (fp == NULL)
	{
		printf("No File Found");
		return -1;
	}
	else
	{
		int i;
		for (i = 0; i < nrows; i++)
		{
			fscanf(fp, "%lf", &c);
			*mm = c;
			mm++;
		}
	}

	fclose(fp);

	return m;
}
double *get_col(int nrows, int ncols, int col, char *input)
{
	FILE *fp = fopen("mat1.txt", "r");
	if (fp == NULL)
	{
		printf("file not found\n");
		return -1;
	}
	double *m;
	m = malloc(sizeof(double) * nrows);
	double *mm = m;
	if (m == NULL)
		return -1;

	double chr;
	int curr_col = 1;

	//iterate to col
	while (curr_col != col)
	{
		fscanf(fp, "%lf", &chr);
		curr_col++;
		// printf("curr_col=%d\n",curr_col);
	}

	int i, j;
	i = 0;
	j = 0;
	// double c;
	while (i != nrows)
	{
		fscanf(fp, "%lf", &chr);
		// printf("scanning %f\n",chr);
		if (j % ncols == 0)
		{
			*mm = chr;
			mm++;
			i++;
			// printf("adding %f\n",chr);
		}
		j++;
	}

	for (i = 0; i < nrows; i++)
		printf("%f\n", m[i]);
}
double *get_row(int ncols, int row, char *input)
{
	FILE *fp;
	double *m;

	m = malloc(sizeof(double) * ncols);
	double *mm = m;

	if (m == NULL)
	{
		return -1;
	}

	double c;
	int r = 1;

	fp = fopen(input, "r");

	if (fp == NULL)
	{
		printf("No File Found");
		return -1;
	}
	else
	{
		while (r != row)
		{
			if ((c = fgetc(fp)) == '\n')
			{
				r++;
			}
		}

		int i;
		for (i = 0; i < ncols; i++)
		{
			fscanf(fp, "%lf", &c);
			*mm = c;
			mm++;
		}
	}

	fclose(fp);

	return m;
}