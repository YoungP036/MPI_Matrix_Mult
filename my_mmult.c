#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
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
	char* m1 = "mat1.txt";
	char* m2 = "mat2.txt";
	int nrowsA=get_nrows(m1);
	int nrowsB=get_nrows(m2);
	int ncolsA=get_ncols(m1);
	int ncolsB=get_ncols(m2);
	int i,j,k;
	int myid, numprocs;
	MPI_Status status;
	int master=0;
	double starttime, endtime;
	int m_tag,s_tag,s2_tag;
	int col_from_index;
	int row_from_index;
	int current_proc;
	double* s_ans =(double*)malloc(sizeof(double));
	double* curr_row=(double*)malloc(sizeof(double)*ncolsB);
	double* curr_col=(double*)malloc(sizeof(double)*nrowsA);
	double **ans=(double**)malloc(sizeof(double*)*nrowsA);	
	double* s_row = (double*)malloc(sizeof(double)*ncolsB);
	double* s_col=(double*)malloc(sizeof(double)*nrowsA);
	for(i=0;i<nrowsA;i++)
		ans[i]=(double*)malloc(sizeof(double)*ncolsB);
	for(i=0;i<nrowsA;i++)
		for(j=0;j<ncolsB;j++)
			ans[i][j]=0.0;


	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	current_proc=0;
	//master
	if(myid==0){
		starttime = MPI_Wtime();

		for(i=0;i<nrowsA;i++){
			for(j=0;j<ncolsB;j++){
				get_row(ncolsA, i+1, m1, curr_row);
				get_col(nrowsB,ncolsB,j+1,m2,curr_col);
				m_tag=get_linear_index_from_mIndex(i,j,nrowsA,ncolsB);
				//TODO SEND
				MPI_Send(&curr_row, ncolsB, MPI_DOUBLE, current_proc, m_tag, MPI_COMM_WORLD);
				MPI_Send(&curr_col, 1, MPI_DOUBLE, current_proc, m_tag, MPI_COMM_WORLD);
				//
				//TODO recv
				//get row/col from tag
				//set ans accordingl
				//reset if we're at final proc
				current_proc= (current_proc==numprocs) ? 0 : current_proc+1;
			}
		}


		//print final answer
		for(i=0;i<nrowsA;i++){
			printf("\n");
			for(j=0;j<ncolsB;j++)
				printf(" %f",ans[i][j]);
		}
		printf("\n");
		endtime = MPI_Wtime();
	}
//slave
	else{
		//TODO recv
		while(1){
			MPI_Recv(&s_row, ncolsA, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			if(status.MPI_TAG==0)
				break;
			s_tag=status.MPI_TAG;
			MPI_Recv(&s_col, nrowsB, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG,MPI_COMM_WORLD, &status);
			if(status.MPI_TAG==0)
				break;
			s2_tag=status.MPI_TAG;
			if(s_tag!=s2_tag){
				printf("Reciever tags not equal, ERROR\n");
				return -1;
			}
			
			for(k=0;k<ncolsB+1;k++)
				*s_ans+=curr_row[k]*curr_col[k];
			//SEND to master
			MPI_Send(ans,ncolsA,MPI_DOUBLE,master,s_tag,MPI_COMM_WORLD);
		}			
	}
	MPI_Finalize();
	return 0;
}//end main
int get_nrows(char *input)
{
	FILE *fp;
	fp=fopen(input, "r+");
	if(fp==NULL){
		printf("No file\n");
		return -1;
	}
	// printf("in get_nrows: file opened\n");
	int row_count = 0;
	int c;
	// printf("in get_nrows: starting loops\n");
	
	while ((c = fgetc(fp)) != EOF)
		if (c == '\n')
			row_count++;
	// printf("in get_nrows: done looping \n");
			
	fclose(fp);
	return row_count;
}

int get_ncols(char *input)
{
	// printf("in get_ncols ");
	FILE *fp;
	fp=fopen(input,"r+");
	if(fp==NULL){
		printf("No file\n");
		return -1;
	}
	// printf("in get_ncols: file opened");
	int col_count = 1;
	int c;

	while ((c = fgetc(fp)) != '\n')
		if (c == ' ')
			col_count++;
	// printf("in get_ncols: done looping");			
	fclose(fp);
	return col_count;
}
void get_col(int nrows, int ncols,int col, char *file, double *ret)
{
	FILE *fp = fopen(file, "r");
	// printf("in get_col\n");
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
		// printf("loop iter : %d\n",curr_col);
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
double *get_row2(int ncols, int row, char *input)
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
void get_row(int ncols, int row, char *input,double *ret)
{
	// printf("in get row\n");
	FILE *fp;
	fp = fopen(input, "r");
	if (fp == NULL)
	{
		printf("No File Found");
		return -1;
	}
	
	double *m=ret;

	double c;
	int r = 1;

	while (r != row){
		if ((c = fgetc(fp)) == '\n')
			r++;
		// printf("looping\n");
	}
	int i;
	for (i = 0; i < ncols; i++)
	{
		fscanf(fp, "%lf", &c);
		// printf("row[%d]=%f\n",i,c);
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
//case 0,3 index+=3=3
//case 1,0 index=1*4=4
//case 1,3 index=4, index = 3+4=7
int get_linear_index_from_mIndex(int row, int col, int nrows, int ncols){
	int index=0;
	index+=col;
	index+=row*ncols;
	return index;
}
