#include <math.h>
#include <stdio.h>
#include <stdlib.h>
int get_col_from_index(int nRow, int nCol, int i);
int get_row_from_index(int nRow, int nCol, int i);
int main(){
	int r=2;
	int c=3;
	int i=5;
	int i2=4;
	printf("index %d maps to row=%d\n",0,get_row_from_index(r,c,0));
	printf("index %d maps to col=%d\n",0,get_col_from_index(r,c,0));
	printf("index %d maps to row=%d\n",1,get_row_from_index(r,c,1));
	printf("index %d maps to col=%d\n",1,get_col_from_index(r,c,1));
	printf("index %d maps to row=%d\n",2,get_row_from_index(r,c,2));
	printf("index %d maps to col=%d\n",2,get_col_from_index(r,c,2));
	printf("index %d maps to row=%d\n",3,get_row_from_index(r,c,3));
	printf("index %d maps to col=%d\n",3,get_col_from_index(r,c,3));
	printf("index %d maps to row=%d\n",4,get_row_from_index(r,c,4));
	printf("index %d maps to col=%d\n",4,get_col_from_index(r,c,4));
	printf("index %d maps to row=%d\n",5,get_row_from_index(r,c,5));
	printf("index %d maps to col=%d\n",5,get_col_from_index(r,c,5));

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