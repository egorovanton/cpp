#include "stdio.h"
#include "stdlib.h"

int main(int argc, const char* argv[]) {
    int n;
    int x1, x2, y1, y2;
    int i, j;
    scanf("%d", &n);

    int **table = (int**) malloc(sizeof(int*)*n);
    table[0] = (int*) malloc(sizeof(int*)*n*n);
    
    for(i = 0; i<n; ++i) {
        table[i] = table[0] + i*n;
        for (j = 0; j<n; ++j) {
            table[i][j] = (i+1)*(j+1);
        }
    }

    while(1) {
        scanf("%d", &x1);
        if(x1 == 0) {
            break;
        }

        scanf("%d%d%d", &y1, &x2, &y2);
        for(i = x1-1; i<x2; ++i) {
            for (j = y1-1; j<y2; ++j) {
                printf("%d ", table[i][j]);
            }
            printf("\n");
        }
    }
    
    free(table[0]);
    free(table);
    return 0;
}