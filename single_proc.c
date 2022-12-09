#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#include "utils.h"


void print_array(int *a, int n) {
    for (int i = 0; i < n; i++) {
        printf("%3d ",a[i]);
    }
    printf("\n\n");
}


void print_matrix(int **a, int n) {
    for (int i = 0; i < n; i++) {
        printf("\t");
        for (int j = 0; j < n; j++) {
            printf("%10d",a[i][j]);
        }
        printf("\n");
    }
}

int row_col_dot_product(int **a, int a_r, int **b, int b_c, int n) {
    int dp = 0;
    for (int i = 0; i < n; i++) {
        dp += a[a_r][i] * b[i][b_c];
    }
    return dp;
}


void compute_sub_mm(NumsData ND, int x, int y) {
    // printf("sub matrix starting at (x,y) -> (%d,%d)\n", x*ND.sub_n, y*ND.sub_n);
    for (int i = x*ND.sub_n; i < (x+1)*ND.sub_n; i++) {
        for (int j = y*ND.sub_n; j < (y+1)*ND.sub_n; j++) {
            ND.prod[i][j] = row_col_dot_product(ND.data1, i, ND.data2, j, ND.n);
        }
    }
}


void copy_sub_matrix(int **a, int **b, int x, int y, int n) {
    int start_i = x*n,
        start_j = y*n,
        end_i   = start_i+n,
        end_j   = start_j+n;
    
    for (int i = start_i; i < end_i; i++) {
        for (int j = start_j; j < end_j; j++) {
            a[i][j] = b[i][j];
        }
    }
}


int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    if (argc < 3) {
        printf("Expecting nums filename and k in arguments!!\n");
        return 1;
    }
    NumsData ND;
    int rank, size;

    ND = read_nums(argv[1], 1);
    int K = atoi(argv[2])-1;
    double start_t = MPI_Wtime();
    if (K == 0) {
        copy_sub_matrix(ND.prod, ND.data1, 0, 0, ND.n);
    }
    for (int k = 0; k < K; k++) {
        compute_sub_mm(ND,0,0);
        copy_sub_matrix(ND.data2, ND.prod, 0, 0, ND.n);
        // int **temp_arr = alloc_2d_int(ND.n, ND.n);
        // free(temp_arr[0]);
        // free(temp_arr);
        // if (k < K-1) {
        //     copy_sub_matrix(ND.prod, ND.)
        // }
    }
    // print_matrix(ND.prod, ND.n);
    double end_t = MPI_Wtime();
    printf("Time taken: %f\n",end_t-start_t);
    //
    // Clean-up and get outta here!
    //
    NumsData_destroy(ND);
    MPI_Finalize();
    return 0;
}