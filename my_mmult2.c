#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#define min(x, y) ((x) < (y) ? (x) : (y))

void get_row(int ncols, int row, char *input,double *ret);
void get_col(int nrows, int ncols,int col, char *file, double *ret);
int get_ncols(char *input);
int get_nrows(char *input);
int get_row_from_linear_index(int index, int ncols);
int get_col_from_linear_index(int index, int ncols);
int get_linear_index_from_mIndex(int row, int col, int ncols, int nrows);

int main(int argc, char *argv[]){
	double starttime, endtime;
	MPI_Status status;
	int myid, numprocs;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	int nrowsA, ncolsA, nrowsB, ncolsB;
	int source,dest,i,j;
	double **matA;
	double **matB;
	double *ret_row;
	double *curr_rowA;
	double *curr_rowB;
	//master
	if(myid==0){
		starttime = MPI_Wtime();
		char *m1="mat1.txt";
		char *m2="mat2.txt";
		nrowsA=get_nrows(m1);
		ncolsA=get_ncols(m1);
		nrowsB=get_nrows(m2);
		ncolsB=get_ncols(m2);
		ret_row=(double*)malloc(sizeof(double)*nrowsA);
		curr_rowA=(double*)malloc(sizeof(double)*ncolsA);
		curr_rowB=(double*)malloc(sizeof(double)*ncolsB);
		matA=(double**)malloc(sizeof(double*)*nrowsA);
		matB=(double**)malloc(sizeof(double*)*nrowsB);
		for(i=0;i<nrowsA;i++)
			matA[i]=(double*)malloc(sizeof(double)*ncolsA);
		for(i=0;i<nrowsB;i++)
			matB[i]=(double*)malloc(sizeof(double)*ncolsB);
		for(i=0;i<nrowsA;i++)
			get_row(ncolsA,i+1,m1,matA[i]);
		for(i=0;i<nrowsB;i++)
			get_row(ncolsB,i+1,m2,matB[i]);

		for(dest=1;dest<numprocs;dest++){
			MPI_Send(&nrowsA,1, MPI_INT,dest,1,MPI_COMM_WORLD);
			MPI_Send(&ncolsA,1, MPI_INT,dest,1,MPI_COMM_WORLD);
			MPI_Send(&nrowsB,1, MPI_INT,dest,1,MPI_COMM_WORLD);
			MPI_Send(&ncolsB,1, MPI_INT,dest,1,MPI_COMM_WORLD);
		}
		for(i=0;i<nrowsB;i++){
			printf("master bcast %d\n",i);
			MPI_Bcast(&matB[i][0], ncolsB, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		}

		dest=1;
		for(i=0;i<nrowsA;i++){
			MPI_Send(&matA[i][0],ncolsA,MPI_DOUBLE,dest,i,MPI_COMM_WORLD);
			if(dest==numprocs)
				dest==1;
			else
				dest++;
		}
		endtime = MPI_Wtime();		
	}
	//slave
	else{
		source=0;
		MPI_Recv(&nrowsA,1,MPI_INT,source,1,MPI_COMM_WORLD, &status);
		MPI_Recv(&ncolsA,1,MPI_INT,source,1,MPI_COMM_WORLD, &status);
		MPI_Recv(&nrowsB,1,MPI_INT,source,1,MPI_COMM_WORLD, &status);
		MPI_Recv(&ncolsB,1,MPI_INT,source,1,MPI_COMM_WORLD, &status);
		printf("\nP%d: A=%dx%d\tB=%dx%d\n",myid, nrowsA,ncolsA,nrowsB,ncolsB);
		curr_rowA=(double*)malloc(sizeof
		ret_row=(double*)malloc(sizeof(double)*nrowsA);
//		printf("ret_row malooc\n");
		matB=(double**)malloc(sizeof(double*)*nrowsB);
//		printf("slave done single malloc\n");
		for(i=0;i<nrowsB;i++)
			matB[i]=(double*)malloc(sizeof(double*)*ncolsB);
//		printf("slave done malloc loop\n");
		for(i=0;i<nrowsB;i++){
			printf("slave %d bcast\n",myid);
			MPI_Bcast(&matB[i][0], ncolsB, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		}

		while(1){
			

			if(status.MPI_TAG==0)
				break;
		}
//		for(i=0;i<nrowsB;i++)
//			for(j=0;j<ncolsB;j++)
//				printf("P%d: [%d][%d]=\n",myid,i,j);
//				printf("P%d: [%d][%d]=%f\n",myid,i,j,matB[i][j]);


		printf("slave end\n");
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

void get_row(int ncols, int row, char *input,double *ret)
{
//	printf("in get row\n");
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
	while (r != row){
//		printf("in get row  search iterator\n");
		if ((c = fgetc(fp)) == '\n')
			r++;
	}
	//capture row
	for (i = 0; i < ncols; i++)
	{
//		printf("capturing row\n");
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
