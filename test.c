#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

#define N 4

int n_count = 0;

char *_LOG = "** LOG -- $  ";
int LOG = 1;


void print_mat(int a[N][N], char *s) {
    printf("%s\n", s);
    for (int col = 0; col < N; col++) {
        for (int row = 0; row < N; row++) {
            printf("%5d", a[col][row]);
        }
        printf("\n");
    }
    printf("\n");
}


void fill_diagonal_ones(int a[N][N]) {
    for (int i = 0; i < N; i++)
        a[i][i] = 1;
}


void copy_row(int from[N][N], int to[N][N], int row) {
    for (int col = 0; col < N; col++)
        to[row][col] = from[row][col];
}


void copy(int from[N][N], int to[N][N]) {
    for (int i = 0, j = 0; j < N; i=i<N-1?++i:j++*0)
        to[i][j] = from[i][j];
}


void lu_decomp(int a[N][N], int l[N][N], int u[N][N]) {
    if (LOG) printf("%sstarting \'lu_decomp()\'...\n", _LOG);
    int b[N][N];
    copy_row(a, b, 0);
    if (LOG) printf("%sa copied to b\n", _LOG);
    for (int col = 0; col < N; col++) {
        for (int row = col+1; row < N; row++) {
            if (LOG) printf("%sat:  row,col  %d,%d\n", _LOG, row, col);
            if (LOG) printf("%sa[col][col] ==  %d\n", _LOG, a[col][col]);
            if (a[col][col] == 0) continue;
            l[row][col] = a[row][col] / a[col][col];
            for (int j = col; j < N; j++) {
                if (LOG) printf("%sat:  row,col,j  %d,%d,%d\n", _LOG, row, col,j);
                b[row][j] = a[row][j] - (l[row][col] * a[col][j]);
                n_count++;
            }
        }
        copy(b,a);
        if (LOG) print_mat(l, "L");
        if (LOG) print_mat(b, "U");
    }
    copy(b,u);
}

void fill_zero(int M[N][N]) {
    for (int i = 0, j = 0; j < N; i=i<N-1?++i:j++*0)
        M[i][j] = 0;
}

int main() {
    int M[N][N] = {{1,2,3,4},{5,6,7,8},{9,10,11,12},{13,14,15,16}};
    int a[N][N];
    copy(M,a);
    int U[N][N];
    int L[N][N];
    fill_zero(U);
    fill_zero(L);
    fill_diagonal_ones(L);
    lu_decomp(a, L, U);
    print_mat(L, "L");
    print_mat(U, "U");
    printf("n: %d\n", n_count);
}