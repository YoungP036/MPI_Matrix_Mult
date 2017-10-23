#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#define min(x, y) ((x) < (y) ? (x) : (y))

double *gen_mmatrix(int nrows, char *input);
int mmult(double *c, double *a, int aRows, int aCols, double *b, int bRows, int bCols);
void compare_matrix(double *a, double *b, int nRows, int nCols);
int get_nrows(char *input);
int get_ncols(char *input);
double *get_row(int, int, char *input);
void get_col(int nrows, int ncols,int col, char *file, double *ret);

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
	int nrowsA, ncolsA, nrowsB, ncolsB;
	double **c;
	double *times;
	double *curr_row, *curr_col;
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
		nrowsB = get_nrows(argv[2]);
		ncolsB=get_ncols(argv[2]);
		nrowsA=get_nrows(argv[1]);
		ncolsA = get_ncols(argv[1]);

		if (nrowsB != ncolsA)
		{
			printf("Invalid Matrix Parameters.");
			return -1;
		}

		double *my_ans = malloc(sizeof(double) * 1);
		int i,j;
		c=(double**)malloc(nrowsA*sizeof(double*));
		for(i=0;i<nrowsA;i++)
			for(j=0;j<ncolsB;j++)
				c[i]=(double*)malloc(ncolsB*sizeof(double));

		if (myid == 0)
		{
			// Master Code goes here
			curr_row = malloc(sizeof(double) *ncolsA);
			curr_col = malloc(sizeof(double) * nrowsB);
			int master_row=1;
			int master_col=1;
			double curr_ans;
			/* Insert your master code here to store the product into cc1 */
			starttime = MPI_Wtime();
			i=0;
			while(i<nrowsA*ncolsB)
				for(j=0;j<numprocs&&i<nrowsA*ncolsB;j++){
					get_col(nrowsB,ncolsB,curr_col,argv[2],curr_col);
					get_row(ncolsA,curr_row,argv[1]);

					//total of nrowsA*ncolsB sub-answers to compute
					//TODO send a row
					//TODO send a col
					//TODO indicate sub-answers position in C, tags?
					i++;
				}
				int nrows = 3;
				int ncols = 3;
			endtime = MPI_Wtime();
			//print answer
			for(i=0;i<nrowsA;i++){
				printf("\n");
				for(j=0;j<ncolsB;j++)
					printf(" %f",c[i][j]);
			}
			printf("%f\n", (endtime - starttime));


		}
		else
		{
			//note ncols=nrows in mat 2
			// Slave Code goes here
			double *my_row = malloc(sizeof(double) * ncolsA);
			double *my_col = malloc(sizeof(double)*nrowsB);
			int i;
			while(1)
			{
				//TODO recv a row into my_row
				//TODO recv a col into my_col

				//check for termination sentinal
				if (status.MPI_TAG == 0)
					break;			

				rown = status.MPI_TAG;

				for (j = 0; j < ncolsA; j++)
					*my_ans += my_row[i] * my_col[i];

				//TODO send ans with position
			}//end inf while
		}//end slave
	}
	else
		fprintf(stderr, "Usage matrix_times_vector <size>\n");
	
	MPI_Finalize();
	return 0;
}

int get_nrows(char *input)
{
	FILE *fp;
	if(fp=fopen(input,"r+")==NULL){
		printf("No file\n");
		return -1;
	}
	int row_count = 0;
	int c;

	while ((c = fgetc(fp)) != EOF)
		if (c == '\n')
			row_count++;
	
	fclose(fp);
	return row_count;
}

int get_ncols(char *input)
{
	FILE *fp;
	if((fp=fopen(input,"r"))==NULL){
		printf("No file\n");
		return -1;
	}
	int col_count = 1;
	int c;

	while ((c = fgetc(fp)) != '\n')
		if (c == ' ')
			col_count++;

	fclose(fp);
	return col_count;
}

void get_col(int nrows, int ncols,int col, char *file, double *ret)
{
	FILE *fp = fopen(file, "r");

	if (fp == NULL)
	{
		printf("file not found\n");
		return -1;
	}

	double chr;
	int curr_col = 1;

	//iterate to col
	while (curr_col != col)
	{
		fscanf(fp, "%lf", &chr);
		curr_col++;
	}

	int i, j;
	i = 0;
	j = 0;
	while (i != nrows)
	{
		fscanf(fp, "%lf", &chr);
		if (j % ncols == 0)
		{
			*ret = chr;
			ret++;
			i++;
		}
		j++;
	}
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