#include <math.h>
#include <stdio.h>
#include <stdlib.h>
int get_col_from_linear_index(int index, int ncols);
int get_row_from_linear_index(int index, int ncols);
int get_linear_index_from_mIndex(int row, int col, int nrows, int ncols);
int main()
{
    int i;
    for (i = 0; i < 9; i++)
    {
        printf("row=%d\tcol=%d\n", get_row_from_linear_index(i, 3), get_col_from_linear_index(i, 3));
    }
    char *B = "mat1.txt";
    FILE *fp = fopen("mat1.txt", "r+");
    if (fp == NULL)
        return -1;
    int nrowsB = get_nrows(B);
    int ncolsB = get_ncols(B);
    printf("nrowsB=%d\tncolsB=%d\n", nrowsB, ncolsB);
    double val, curr_row, curr_col;
    double *matB = (double *)malloc(sizeof(double) * nrowsB * ncolsB);
    int j, index;
    for (i = 0; i < nrowsB; i++)
        for (j = 0; j < ncolsB; j++)
        {
            // printf("looping file\n");
            fscanf(fp, "%lf", &val);
            matB[get_linear_index_from_mIndex(i, j, nrowsB, ncolsB)] = val;
            printf("[%d][%d]=%f @ linear index %d\n",i,j,matB[get_linear_index_from_mIndex(i,j,nrowsB,ncolsB)],get_linear_index_from_mIndex(i,j,nrowsB,ncolsB));
        }

    for(i=0;i<nrowsB*ncolsB;i++)
            printf("[%d][%d]=%f\n",get_row_from_linear_index(i,ncolsB),get_col_from_linear_index(i,ncolsB),matB[i]);
}
int get_nrows(char *input)
{
    FILE *fp;
    fp = fopen(input, "r+");
    if (fp == NULL)
    {
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
    fp = fopen(input, "r+");
    if (fp == NULL)
    {
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
void get_col(int nrows, int ncols, int col, char *file, double *ret)
{
    //file setup
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

void get_row(int ncols, int row, char *input, double *ret)
{
    //file setup
    FILE *fp;
    fp = fopen(input, "r");
    if (fp == NULL)
    {
        printf("No File Found");
        return -1;
    }

    double *m = ret;
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

int get_row_from_linear_index(int index, int ncols)
{
    int row = 0;
    while (index >= ncols)
    {
        index -= ncols;
        row++;
    }
    return row;
}

int get_col_from_linear_index(int index, int ncols)
{
    return index % ncols;
}

int get_linear_index_from_mIndex(int row, int col, int nrows, int ncols)
{
    int index = 0;
    index += col;
    index += row * ncols;
    return index;
}