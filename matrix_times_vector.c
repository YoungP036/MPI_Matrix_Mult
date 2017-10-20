#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define min(x, y) ((x) < (y) ? (x) : (y))
// typedef struct _MPI_Status {
// 	int count;
// 	int cancelled;
// 	int MPI_SOURCE;
// 	int MPI_TAG;
// 	int MPI_ERROR;
//   } MPI_Status, *PMPI_Status;
int main(int argc, char *argv[])
{
	int nrows, ncols;
	double *a, *b, *c;
	double *buffer, ans;
	double *times;
	double total_times;
	int run_index;
	int nruns;
	int myid, master, numprocs;
	double starttime, endtime;
	MPI_Status status;
	int i, j, numsent, sender;
	int anstype, row;
	srand(time(0));
	MPI_Init(&argc, &argv); //init MPI env
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	if (argc > 1)
	{
		nrows = atoi(argv[1]);
		ncols = nrows;
		a = (double *)malloc(sizeof(double) * nrows * ncols);
		b = (double *)malloc(sizeof(double) * ncols * nrows);
		c = (double *)malloc(sizeof(double) * nrows * ncols);
		// slice = (double*)malloc(sizeof(double)*ncols)
		buffer = (double *)malloc(sizeof(double) * ncols);
		master = 0;
		if (myid == master)
		{
			// Master Code goes here

			//populate matrices a and b
			printf("**A**\n");
			for (i = 0; i < nrows; i++)
			{
				for (j = 0; j < ncols; j++)
				{
					a[i * ncols + j] = (double)rand() / RAND_MAX;
					printf(" %f", a[i * ncols + j]);
				}
				printf("\n");
			}
			printf("**B**\n");
			for (i = 0; i < ncols; i++)
			{
				for (j = 0; j < nrows; j++)
				{
					b[i * nrows + j] = (double)rand() / RAND_MAX;
					printf(" %f", b[i * nrows + j]);
				}
				printf("\n");
			}

			starttime = MPI_Wtime();
			numsent = 0;

			//(buffer,bufferentrycount,datatype,rank of broadcast root, communicator/handler)
			MPI_Bcast(b, ncols * nrows, MPI_DOUBLE, master, MPI_COMM_WORLD);
			MPI_Bcast(a, ncols * nrows, MPI_DOUBLE, master, MPI_COMM_WORLD);
			//for each process
			for (i = 0; i < min(numprocs - 1, nrows); i++)
			{
				//get a row per process
				for (j = 0; j < ncols; j++)
					buffer[j] = a[i * ncols + j];
				//send row
				MPI_Send(buffer, ncols, MPI_DOUBLE, i + 1, i + 1, MPI_COMM_WORLD);
				numsent++;
			} //end for
			printf("numsent=%d\n", numsent);
			for (i = 0; i < nrows; i++)
			{
				MPI_Recv(&ans, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG,
						 MPI_COMM_WORLD, &status);
				sender = status.MPI_SOURCE;
				anstype = status.MPI_TAG;
				c[anstype - 1] = ans;
				if (numsent < nrows)
				{
					for (j = 0; j < ncols; j++)
						buffer[j] = a[numsent * ncols + j];
					MPI_Send(buffer, ncols, MPI_DOUBLE, sender, numsent + 1, MPI_COMM_WORLD);
					numsent++;
				}
				else
					MPI_Send(MPI_BOTTOM, 0, MPI_DOUBLE, sender, 0, MPI_COMM_WORLD);
			}
			endtime = MPI_Wtime();
			printf("%f\n", (endtime - starttime));
		}
		//slave code here
		else
		{
			MPI_Bcast(b, ncols * nrows, MPI_DOUBLE, master, MPI_COMM_WORLD);
			MPI_Bcast(a, ncols * nrows, MPI_DOUBLE, master, MPI_COMM_WORLD);
			for (i = 0; i < sizeof(b); i++)
				printf("from %d, b[%d]=%f\n", myid, i, b[i]);
			for (i = 0; i < sizeof(a); i++)
				printf("from %d, a[%d]=%f\n", myid, i, a[i]);
			if (myid <= nrows)
			{
				while (1)
				{
					MPI_Recv(buffer, ncols, MPI_DOUBLE, master, MPI_ANY_TAG,
							 MPI_COMM_WORLD, &status);
					if (status.MPI_TAG == 0)
						break;

					row = status.MPI_TAG;
					ans = 0.0;
					for (j = 0; j < ncols; j++)
						ans += buffer[j] * b[j];

					MPI_Send(&ans, 1, MPI_DOUBLE, master, row, MPI_COMM_WORLD);
				}
			} //end if myid<=nrows
		}//end else for ifmyid==ma ster
	}//end if argc>1
	else
		fprintf(stderr, "Usage matrix_times_vector <size>\n");

	MPI_Finalize();
	return 0;
} //end main
