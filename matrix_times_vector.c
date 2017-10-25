#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define min(x, y) ((x) < (y) ? (x) : (y))
void get_row(int ncols, int row, char *input,double *ret);
double *get_row2(int ncols, int row, char *input);
void get_col(int nrows, int ncols,int col, char *file, double *ret);
int get_ncols(char *input);
int get_nrows(char *input);
int get_row_from_linear_index(int index, int ncols);
int get_col_from_linear_index(int index, int ncols);
int get_linear_index_from_mIndex(int row, int col, int ncols, int nrows);
int main(int argc, char *argv[])
{
	int nrowsA,nrowsB, ncolsA,ncolsB;
	double *aa,*b, *c;
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
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	if (argc > 4)
	{
		nrowsA = atoi(argv[1]);
		ncolsA = atoi(argv[2]);
		nrowsB=atoi(argv[3]);
		ncolsB=atoi(argv[4]);

		if(ncolsA!=nrowsB){
			printf("Dimension mismatch\n");
			return -1;
		}
		aa = (double *)malloc(sizeof(double) * nrowsA * ncolsA);
		b = (double *)malloc(sizeof(double) * nrowsB* ncolsB);
		c = (double *)malloc(sizeof(double) * nrows);
		buffer = (double *)malloc(sizeof(double) * ncols);
		master = 0;
		if (myid == master)
		{
			// Master Code goes here
			for (i = 0; i < nrowsA; i++)
				for (j = 0; j < ncolsA; j++)
					aa[i * ncolsA + j] = (double)rand() / RAND_MAX;
			for(i=0;i<nrowsB;i++)
				for(j=0;j<ncolsB;j++)
				b[i+ncolsB+j]=(double)rand()/ RAND_MAX;

			starttime = MPI_Wtime();
			numsent = 0;
			MPI_Bcast(b, ncols, MPI_DOUBLE, master, MPI_COMM_WORLD);
			for (i = 0; i < min(numprocs - 1, nrows); i++)
			{
				for (j = 0; j < ncols; j++)
				{
					buffer[j] = aa[i * ncolsA + j];
				}
				MPI_Send(buffer, ncolsA, MPI_DOUBLE, i + 1, i + 1, MPI_COMM_WORLD);
				numsent++;
			}
			for (i = 0; i < nrows; i++)
			{
				MPI_Recv(&ans, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG,
						 MPI_COMM_WORLD, &status);
				sender = status.MPI_SOURCE;
				anstype = status.MPI_TAG;
				c[anstype - 1] = ans;
				if (numsent < nrowsA)
				{
					for (j = 0; j < ncolsA; j++)
					{
						buffer[j] = aa[numsent * ncolsA + j];
					}
					MPI_Send(buffer, ncolsA, MPI_DOUBLE, sender, numsent + 1,
							 MPI_COMM_WORLD);
					numsent++;
				}
				else
				{
					MPI_Send(MPI_BOTTOM, 0, MPI_DOUBLE, sender, 0, MPI_COMM_WORLD);
				}
			}
			endtime = MPI_Wtime();
			for(i=0;i<nrows;i++)
				printf("%f\n",c[i]);
			printf("%f\n", (endtime - starttime));
		}
		else
		{
			// Slave Code goes here
			MPI_Bcast(b, ncolsB, MPI_DOUBLE, master, MPI_COMM_WORLD);
			if (myid <= nrowsA)
			{
				while (1)
				{
					MPI_Recv(buffer, ncolsA, MPI_DOUBLE, master, MPI_ANY_TAG,
							 MPI_COMM_WORLD, &status);
					if (status.MPI_TAG == 0)
					{
						break;
					}
					row = status.MPI_TAG;
					ans = 0.0;
					for (j = 0; j < ncols; j++)
					{
						ans += buffer[j] * b[j];
						printf("id=%d, buf=%f, b=%f, i=%f\n",myid,buffer[j],b[j],ans);
					}
					MPI_Send(&ans, 1, MPI_DOUBLE, master, row, MPI_COMM_WORLD);
				}
			}
		}
	}
	else
	{
		fprintf(stderr, "Enter dimension params, rowsA, colsA, rowsB, colsB\n");
	}
	MPI_Finalize();
	return 0;
}
int get_nrows(char *input)
{
	FILE *fp;
	fp=fopen(input, "r+");
	if(fp==NULL){
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
	fp=fopen(input,"r+");
	if(fp==NULL){
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
	//file setup
	FILE *fp = fopen(file, "r");
	if (fp == NULL){
		printf("file not found\n");
		return -1;
	}

	double chr;
	int curr_col = 1;

	//iterate to col
	while (curr_col != col)
	{
		// printf("loop iter : %d\n",curr_col);
		fscanf(fp, "%lf", &chr);
		curr_col++;
	}

	int i, j;
	i = 0;
	j = 0;
	//capture col
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
// double *get_row2(int ncols, int row, char *input)
// {
// 	//file setup
// 	FILE *fp;
// 	fp = fopen(input, "r");
// 	if (fp == NULL)
// 	{
// 		printf("No File Found");
// 		return -1;
// 	}

// 	//heap setup
// 	double *m;
// 	m = malloc(sizeof(double) * ncols);
// 	double *mm = m;
// 	if (m == NULL)	
// 		return -1;
	
// 	//stack setup
// 	double c;
// 	int i;
// 	int r = 1;

// 	//get to row
// 	while (r != row)
// 		if ((c = fgetc(fp)) == '\n')
// 			r++;
// 	//capture row
// 	for (i = 0; i < ncols; i++)
// 	{
// 		fscanf(fp, "%lf", &c);
// 		*mm = c;
// 		mm++;
// 	}
	

// 	fclose(fp);
// 	return m;
// }
void get_row(int ncols, int row, char *input,double *ret)
{
	//file setup
	FILE *fp;
	fp = fopen(input, "r");
	if (fp == NULL){
		printf("No File Found");
		return -1;
	}
	
	double *m=ret;
	double c;
	int r = 1;
	int i;
	
	//iterate to correct row
	while (r != row)
		if ((c = fgetc(fp)) == '\n')
			r++;

	//capture row
	for (i = 0; i < ncols; i++)
	{
		fscanf(fp, "%lf", &c);
		*m = c;
		m++;
	}
	fclose(fp);
}

int get_row_from_linear_index(int index, int ncols){
	int row=0;
	while(index>=ncols){
		index-=ncols;
		row++;
	}
	return row;
}

int get_col_from_linear_index(int index, int ncols){
	return index%ncols;
}

int get_linear_index_from_mIndex(int row, int col, int nrows, int ncols){
	int index=0;
	index+=col;
	index+=row*ncols;
	return index;
}
