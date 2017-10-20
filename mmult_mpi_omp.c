#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#define min(x, y) ((x)<(y)?(x):(y))

double* gen_matrix(int n, int m);
int mmult(double *c, double *a, int aRows, int aCols, double *b, int bRows, int bCols);
void compare_matrix(double *a, double *b, int nRows, int nCols);

/** 
    Program to multiply a matrix times a matrix using both
    mpi to distribute the computation among nodes and omp
    to distribute the computation among threads.
*/

int main(int argc, char* argv[])
{
	int nrows, ncols;
  	double *a;	/* the A matrix */
  	double *b;	/* the B matrix */
	double *c1;	/* A x B computed using the omp-mpi code you write */
	double *c2;	/* A x B computed using the conventional algorithm */
	int myid, numprocs;
	double starttime, endtime;
	MPI_Status status;
	/* insert other global variables here */


	/*   								  */
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	if (argc > 1) {
		//command line params
		nrows = atoi(argv[1]);
		ncols = atoi(argv[2]);
		if (myid == 0) {
			// Master Code goes here
			a = gen_matrix(nrows, ncols);
			b = gen_matrix(ncols, nrows);
			c1 = malloc(sizeof(double) * nrows * nrows); 
			starttime = MPI_Wtime();
			/* Insert your master code here to store the product into c1 */
			
			
			
			/*END MASTER CODE */

			endtime = MPI_Wtime();
			printf("%f\n",(endtime - starttime));
			c2  = malloc(sizeof(double) * nrows * nrows);
			mmult(c2, a, nrows, ncols, b, ncols, nrows);
			compare_matrices(c2, c1, nrows, nrows);
		}//end inner if
		else {
			  // Slave Code goes here
			  

    	}//end inner else
	}//end outer if 
	else {
   		fprintf(stderr, "Usage matrix_times_vector <size>\n");
  	}//end outer else
	MPI_Finalize();
	return 0;
}
