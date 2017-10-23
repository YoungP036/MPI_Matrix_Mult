#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
void get_row(int ncols, int row, char *input,double *ret);
double *get_row2(int ncols, int row, char *input);
void get_col(int nrows, int ncols,int col, char *file, double *ret);
int get_ncols(char *input);
int get_nrows(char *input);
int main()
{
	char* m1 = "mat1.txt";
	char* m2 = "mat2.txt";
	// printf("getting rows\n");
	int nrowsA=get_nrows(m1);
	// printf("got nrowsA\n");
	int nrowsB=get_nrows(m2);
	// printf("got nrowsB, getting cols\n");
	int ncolsA=get_ncols(m1);
	// printf("got ncolsA\n");
	int ncolsB=get_ncols(m2);
	// printf("DBG got ncolsB\n");
	double* curr_row=(double*)malloc(sizeof(double)*ncolsB);
	// double* curr_row;
	double* curr_col=(double*)malloc(sizeof(double)*nrowsA);
	int i,j,k;
	
	// printf("nrowsA=%d\tncolsA=%d\n",nrowsA,ncolsA);
	// printf("nrowsB=%d\tncolsB=%d\n",nrowsB,ncolsB);
	double **ans=(double**)malloc(sizeof(double*)*nrowsA);	
	for(i=0;i<nrowsA;i++)
		ans[i]=(double*)malloc(sizeof(double)*ncolsB);
	// printf("malloced ans\n");
	for(i=0;i<nrowsA;i++)
		for(j=0;j<ncolsB;j++)
			ans[i][j]=0.0;

	for(i=0;i<nrowsA;i++){
		for(j=0;j<ncolsB;j++){
			get_row(ncolsA, i+1, m1, curr_row);
			get_col(nrowsB,ncolsB,j+1,m2,curr_col);

			for(k=0;k<ncolsB+1;k++){
				ans[i][j]+=curr_row[k]*curr_col[k];
			}
		}
	}

	// printf("main loops done\n");

	for(i=0;i<nrowsA;i++){
		printf("\n");
		for(j=0;j<ncolsB;j++)
			printf(" %f",ans[i][j]);
	}
	printf("\n");
}
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
	// m = malloc(sizeof(double) * ncols);
	// double *mm = m;

	// if (m == NULL)
	// {
	// 	return -1;
	// }

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