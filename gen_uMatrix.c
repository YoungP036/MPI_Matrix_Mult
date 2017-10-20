#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>

int main(){
        FILE *fp;
        double *m;
        m = malloc(sizeof(double) * 6);

        if (m == NULL) {
                return -1;
        }

        double c;

        fp = fopen("mar_B.txt", "r");

        if (fp == NULL) {
                printf("No File Found");
                return -1;
        } else {
                int i;
                for (i = 0; i < 6; i++) {
                        fscanf(fp, "%lf", &c);
                        *m = c;
                        m++;
                }
        }

        fclose(fp);

        return 0;
}
