#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#define min(x, y) ((x) < (y) ? (x) : (y))

void get_row(int ncols, int row, char *input,double *ret);
int get_ncols(char *input);
int get_nrows(char *input);

int main(int argc, char *argv[]){
	double starttime, endtime;
	MPI_Status status;
	int myid, numprocs;
	int nrowsA, ncolsA, nrowsB, ncolsB;
	int source,dest,i,j,slices_needed;
	double **matA,**matB,**matC;
	double *ret_row,*curr_rowA, *curr_rowB;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	//master
	if(myid==0){
		starttime = MPI_Wtime();
		char *m1="mat1.txt";
		char *m2="mat2.txt";
		nrowsA=get_nrows(m1);
		ncolsA=get_ncols(m1);
		nrowsB=get_nrows(m2);
		ncolsB=get_ncols(m2);
		slices_needed=nrowsA;
		ret_row=(double*)malloc(sizeof(double)*ncolsB);
		curr_rowA=(double*)malloc(sizeof(double)*ncolsA);
		curr_rowB=(double*)malloc(sizeof(double)*ncolsB);
		matA=(double**)malloc(sizeof(double*)*nrowsA);
		matB=(double**)malloc(sizeof(double*)*nrowsB);
		matC=(double**)malloc(sizeof(double*)*nrowsA);
		ret_row=(double*)malloc(sizeof(double)*ncolsB);
		for(i=0;i<nrowsA;i++)
			matA[i]=(double*)malloc(sizeof(double)*ncolsA);
		for(i=0;i<nrowsB;i++)
			matB[i]=(double*)malloc(sizeof(double)*ncolsB);
		for(i=0;i<nrowsA;i++)
			matC[i]=(double*)malloc(sizeof(double)*ncolsB);
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
		
		for(i=0;i<nrowsB;i++)
			MPI_Bcast(&matB[i][0], ncolsB, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		
		dest=1;
		for(i=0;i<nrowsA;i++){
			MPI_Send(&matA[i][0],ncolsA,MPI_DOUBLE,dest,i+1,MPI_COMM_WORLD);
			if(dest==numprocs-1)
				dest=1;
			else
				dest++;
		}

		while(slices_needed!=0){
			MPI_Recv(&ret_row[0],ncolsB,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
			for(i=0;i<ncolsB;i++)
				matC[status.MPI_TAG-1][i]=ret_row[i];
			slices_needed--;
		}
		//all rows sent, terminate slaves with sentinal as tag=0
		for(dest=1;dest<numprocs;dest++)
			MPI_Send(&matA[0][0],ncolsA,MPI_DOUBLE,dest,0,MPI_COMM_WORLD);
		endtime = MPI_Wtime();
		printf("***FINAL ANSWER***\n");
		for(i=0;i<nrowsA;i++){
			printf("\nP%d",myid);
			for(j=0;j<ncolsB;j++)
				printf("  %f", matC[i][j]);
		}
		printf("\n");
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
				MPI_Send(&ret_row[0],ncolsB,MPI_DOUBLE,0,status.MPI_TAG,MPI_COMM_WORLD);

//				printf("P%d row %d:",myid,status.MPI_TAG-1);
//				for(i=0;i<ncolsB;i++)
//					printf("  %f",ret_row[i]);
//				printf("\n");
			}//end inf while
		}//end active slave block
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
