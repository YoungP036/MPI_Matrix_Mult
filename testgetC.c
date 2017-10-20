#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
int main(){
		FILE* fp = fopen("mat1.txt","r");
		int nrows=3;
		int ncols=3;
		int col=3;
	if (fp==NULL){
		printf("file not found\n");
		return -1;
	}
	double* m;
	m = malloc(sizeof(double) * nrows);
	double *mm=m;
	if(m==NULL)
		return -1;

	double chr;
	int curr_col=1;

	//iterate to col
	while(curr_col!=col){
		fscanf(fp,"%lf",&chr);
		curr_col++;
		// printf("curr_col=%d\n",curr_col);
	}
	
	int i,j;
	i=0;
	j=0;
	// double c;
	while(i!=nrows){
		fscanf(fp, "%lf", &chr);
		// printf("scanning %f\n",chr);
		if(j%ncols==0){
			*mm = chr;
			mm++;
			i++;
			// printf("adding %f\n",chr);
		}
		j++;
	}
	
	for(i=0;i<nrows;i++)
		printf("%f\n",m[i]);
}