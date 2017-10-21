#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
double *get_row(int ncols, int row, char *input);
void get_col(int nrows, int ncols,int col, char *file, double *ret);
int main()
{
	int nrows = 3;
	int ncols = 3;
	int col = 3;
	char* file = "mat1.txt";
	double *m;
	int i;
	m = malloc(sizeof(double) * nrows);
	double *my_row;
	my_row=malloc(sizeof(double)*ncols);
	my_row=get_row(ncols,3,file);
	get_col(nrows, ncols, col,file,m);
	for (i = 0; i < nrows; i++)
		printf("col %f\n", m[i]);
	for(i=0;i<ncols;i++)
		printf(" %f",my_row[i]);
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