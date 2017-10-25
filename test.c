#include <math.h>
#include <stdio.h>
#include <stdlib.h>
int get_col_from_index(int nRow, int nCol, int i);
int get_row_from_index(int nRow, int nCol, int i);
int main(){
	int i,j;
	double **ans=(double**)malloc(sizeof(double*)*3);	
	double *ans2;
	for(i=0;i<3;i++)
		ans[i]=(double*)malloc(sizeof(double)*3);
	ans2=&ans[0][0];
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			*ans2[i][j]=(double)i;

	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			printf("%f\n",*ans2[i][j]);

}

// int get_col_from_index(int nRow, int nCol, int i){
// 	float ii = (float)i;
// 	return(int) (ii)%nCol;
// }

// int get_row_from_index(int nRow, int nCol, int i){
// 	float ii = (float)i;
// 	return (int)floor(((int)ii)%nRow);
// }
int get_row_from_index(int rows, int columns, int index){
    for(int i=0; i<rows; i++){
        //check if the index parameter is in the row
        if(index < (columns * i) + columns && index >= columns * i){
            //return x, y
            return  i;
        }
    }
}
int get_col_from_index(int rows, int columns, int index){
    for(int i=0; i<rows; i++){
        //check if the index parameter is in the row
        if(index < (columns * i) + columns && index >= columns * i){
            //return x, y
            return index - columns * i;
        }
    }
}