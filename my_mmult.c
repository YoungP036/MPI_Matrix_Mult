#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#define min(x, y) ((x) < (y) ? (x) : (y))

void get_row(int ncols, int row, char *input,double *ret);
// void get_col(int nrows, int ncols,int col, char *file, double *ret);
int get_ncols(char *input);
int get_nrows(char *input);
// int get_row_from_linear_index(int index, int ncols);
// int get_col_from_linear_index(int index, int ncols);
// int get_linear_index_from_mIndex(int row, int col, int ncols, int nrows);

int main(int argc, char *argv[]){
	double starttime, endtime;
	MPI_Status status;
	int myid, numprocs;
	int nrowsA, ncolsA, nrowsB, ncolsB;
	int source,dest,i,j;
	double **matA;
	double **matB;
	double *ret_row;
	double *curr_rowA;
	double *curr_rowB;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	//master
	if(myid==0){
		printf("MASTER ID = %d\n",myid);
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
////////////////////////////////////////////////////////////////////////////
		dest=1;
		for(i=0;i<nrowsA;i++){
			MPI_Send(&matA[i][0],ncolsA,MPI_DOUBLE,dest,i+1,MPI_COMM_WORLD);
			if(dest==numprocs)
				dest=1;
			else
				dest++;
		}


		// printf("all rows sent\n");
		//all rows sent, terminate slaves with sentinal as tag=0
		for(dest=1;dest<numprocs;dest++)
			MPI_Send(&matA[0][0],ncolsA,MPI_DOUBLE,dest,0,MPI_COMM_WORLD);
		endtime = MPI_Wtime();
	}
	//slave
	else{
		source=0;
		//get dims
		MPI_Recv(&nrowsA,1,MPI_INT,source,1,MPI_COMM_WORLD, &status);
		MPI_Recv(&ncolsA,1,MPI_INT,source,1,MPI_COMM_WORLD, &status);
		MPI_Recv(&nrowsB,1,MPI_INT,source,1,MPI_COMM_WORLD, &status);
		MPI_Recv(&ncolsB,1,MPI_INT,source,1,MPI_COMM_WORLD, &status);
		//malloc accordingly
		curr_rowA=(double*)malloc(sizeof(double)*ncolsA);
		ret_row=(double*)malloc(sizeof(double)*ncolsB);
		matB=(double**)malloc(sizeof(double*)*nrowsB);
		for(i=0;i<nrowsB;i++)
			matB[i]=(double*)malloc(sizeof(double*)*ncolsB);
		
		//get matB
		for(i=0;i<nrowsB;i++)
			MPI_Bcast(&matB[i][0], ncolsB, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		if(myid<=nrowsA){
			while(1){
				//get a row, break if all done
				MPI_Recv(&curr_rowA[0],ncolsA,MPI_DOUBLE,source,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
				if(status.MPI_TAG==0)
					break;

				for(i=0;i<ncolsA;i++){
					ret_row[i]=0.0;
					for(j=0;j<nrowsB;j++)
						ret_row[i]+=curr_rowA[j]*matB[j][i];
				}
				//report slice of answer
				printf("P%d row %d:",myid,status.MPI_TAG-1);
				for(i=0;i<ncolsB;i++)
					printf("  %f",ret_row[i]);
				printf("\n");
			}//end inf while
		}//end active slave block
		printf("slave end\n");
	}//end slaves

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
// void get_col(int nrows, int ncols,int col, char *file, double *ret)
// {
// 	//file setup
// 	FILE *fp = fopen(file, "r");
// 	if (fp == NULL){
// 		printf("file not found\n");
// 		return -1;
// 	}

// 	double chr;
// 	int curr_col = 1;

// 	//iterate to col
// 	while (curr_col != col)
// 	{
// 		// printf("loop iter : %d\n",curr_col);
// 		fscanf(fp, "%lf", &chr);
// 		curr_col++;
// 	}

// 	int i, j;
// 	i = 0;
// 	j = 0;
// 	//capture col
// 	while (i != nrows)
// 	{
// 		fscanf(fp, "%lf", &chr);
// 		if (j % ncols == 0)
// 		{
// 			*ret = chr;
// 			ret++;
// 			i++;
// 		}
// 		j++;
// 	}
// }

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

// int get_row_from_linear_index(int index, int ncols){
// 	int row=0;
// 	while(index>=ncols){
// 		index-=ncols;
// 		row++;
// 	}
// 	return row;
// }

// int get_col_from_linear_index(int index, int ncols){
// 	return index%ncols;
// }

// int get_linear_index_from_mIndex(int row, int col, int nrows, int ncols){
// 	int index=0;
// 	index+=col;
// 	index+=row*ncols;
// 	return index;
// } 
