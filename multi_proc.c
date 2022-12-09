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
    if (argc < 2) {
        printf("Expecting nums filename in arguments!!\n");
        return 1;
    }
    NumsData ND;
    double start_t,end_t;
    start_t = MPI_Wtime();
    int rank, size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int K = atoi(argv[2])-1;
    if (K==0) {
        ND = NumsData_create(1,size);
    }
    // printf("HERE1 %d %d %s\n",size,rank, argv[1]);
    for (int k = 0; k < K; k++) {
        if (rank == 0) {
            if (k == 0) ND = read_nums(argv[1], size);
            int dest = 1;
            for (int x = 0; x < ND.sub_xy; x++) {
                for (int y = 0; y < ND.sub_xy; y++) {
                    // printf("sending to %d with starting x,y: %d,%d\n",dest,x,y);
                    if (x == 0 && y == 0) y++;
                    int nxy[3] = {ND.n, x, y}; 
                    MPI_Send(&nxy, 3, MPI_INT, dest, 0, MPI_COMM_WORLD);
                    if (k == 0)
                        MPI_Send(&(ND.data1[0][0]), ND.n*ND.n, MPI_INT, dest, 0, MPI_COMM_WORLD);
                    else {
                        
                        MPI_Send(&(ND.data2[0][0]), ND.n*ND.n, MPI_INT, dest, 0, MPI_COMM_WORLD);
                    }
                    dest++;
                }
            }
            
            compute_sub_mm(ND,0,0);
            
            dest = 1;
            int **temp_arr = alloc_2d_int(ND.n, ND.n);
            for (int x = 0; x < ND.sub_xy; x++) {
                for (int y = 0; y < ND.sub_xy; y++) {
                    if (x == 0 && y == 0) y++;
                    MPI_Recv(&(temp_arr[0][0]), ND.n*ND.n, MPI_INT, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    copy_sub_matrix(ND.prod, temp_arr, x, y, ND.sub_n);
                    dest++;
                }
            }
            copy_sub_matrix(ND.data2, ND.prod, 0, 0, ND.n);
            // NumsData_print(ND);
            // print_matrix(ND.prod,ND.n);
            free(temp_arr[0]);
            free(temp_arr);
            // NumsData_destroy(ND);
        }

        else {
            int nxy[3];
            MPI_Recv(&nxy, 3, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int n = nxy[0],
                x = nxy[1],
                y = nxy[2];
            if (k == 0)
                ND = NumsData_create(n,size);
            if (k == 0) {
                MPI_Recv(&(ND.data1[0][0]), n*n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                copy_sub_matrix(ND.data2, ND.data1, 0, 0, ND.n);
            }
            else {
                MPI_Recv(&(ND.data2[0][0]), n*n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            compute_sub_mm(ND, x, y);
            MPI_Send(&(ND.prod[0][0]), n*n, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }
    if (rank == 0) {
        // print_matrix(ND.prod,ND.n);
        end_t = MPI_Wtime();
        printf("Time taken: %f\n",end_t-start_t);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    //
    // Clean-up and get outta here!
    //
    NumsData_destroy(ND);
    MPI_Finalize();
    return 0;
}