#include <stdio.h>
#include <stdlib.h> 
#include "mpi.h"

#include "utils.h"

double start_t,end_t;
int n               = 0;
int rank            = 0;
int num_procs       = 0;
int rows_per_proc   = 0;



void leader_proc(char *filename);
void other_procs();
void forward_row(int *row, int start_col);

int main(int argc, char **argv) {
	MPI_Init(&argc, &argv);
    if (argc < 2) {
        printf("Expecting nums filename in arguments!!\n");
        return 1;
    }
    start_t = MPI_Wtime();
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    UTILS_rank = rank;
    if (rank == 0) {
        leader_proc(argv[1]);
    }
    else {
        other_procs();
    }
    MPI_Finalize();
    return 0;
}



void leader_proc(char *filename) {
    logg("leader_proc", "starting leader...");
    int **mat = NULL;
    read_matrix_from_file(filename, &mat, &n);
    // print_mat(mat, n, n, "Original Matrix:");
    rows_per_proc = n / num_procs;
    if (rows_per_proc*num_procs != n) {
        logg_err("leader_proc", "n must be divisible by number of processes!!!");
        exit(1);
    }
    // printf("rows_per_proc: %d\n",rows_per_proc);
    //
    // Send 'n' to each proc, and it's designated row:
    start_t = MPI_Wtime();
    for (int proc = 1; proc < num_procs; proc++) {
        logg_int("leader_proc","sending \'n\' to",proc);
        MPI_Send(&n, 1, MPI_INT, proc, 0, MPI_COMM_WORLD);
        
        for (int row = proc*rows_per_proc; row < rows_per_proc*proc+rows_per_proc; row++) {
            logg_int("leader_proc","sending designated row to",proc);
            MPI_Send(&mat[row][0], n, MPI_INT, proc, 0, MPI_COMM_WORLD);
        }
    }

    int product = 1;
    int sum = 0;
    //
    // Forward first row(s) to each process:
    //
    //  Each process may own multiple rows. Need to forward all rows. 
    int *prev_row;
    for (int col = 0; col < rows_per_proc-1; col++) {
        logg_int("leader_proc","forwarding row",col);
        // MPI_Barrier(MPI_COMM_WORLD);
        product *= mat[col][col];
        sum += mat[col][col];
        forward_row(mat[col],col);
        // printf("HERERERERERER\n");
        prev_row = mat[col];
        if (rank == log_rank) {
            // print_mat(mat, rows_per_proc, n, "before mat loop");
            // print_row(prev_row,n,col);
        }
        
        //
        // For each row designated to this process:
        for (int cur_row = col+1; cur_row < rows_per_proc; cur_row++) {
            //
            // Computation:
            int L_r_c = mat[cur_row][col] / prev_row[col];
            if (rank == log_rank)logg_int("leader_proc","L_r_c", L_r_c);
            for (int j = col; j < n; j++) {
                mat[cur_row][j] -= (L_r_c * prev_row[j]);
            }
            if (rank == log_rank) {
                // print_mat(mat, rows_per_proc, n, "mat loop");
                // printf("col,cur_row %d,%d\n",col,cur_row);
            }
        }
    }
    if (rank == log_rank) logg_int("leader_proc","forwarding row",rows_per_proc-1);
    product *= mat[rows_per_proc-1][rows_per_proc-1];
    sum += mat[rows_per_proc-1][rows_per_proc-1];
    forward_row(mat[rows_per_proc-1],rows_per_proc-1);

    int prod = 1;
    int s = 0;
    for (int proc = 1; proc < num_procs; proc++) {
        logg_int("leader_prod","Waiting to recieve product from node",proc);
        MPI_Recv(&prod, 1, MPI_INT, proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&s, 1, MPI_INT, proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        logg_int("leader_prod","Product recieved!",prod);
        product *= prod;
        sum += s;
    }
    // MPI_Barrier(MPI_COMM_WORLD);
    end_t = MPI_Wtime();
    printf("%f\n",end_t-start_t);
    // printf("determinant: %d\n",product);
    // printf("diag sum: %d\n",sum);
    destroy_mat(mat,n);
}



void other_procs() {
    logg("other_procs","starting proc...");
    MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if (rank == log_rank) logg_int("other_procs","recieved \'n\'",n);
    rows_per_proc = n / num_procs;
    logg_int("other_procs","rows_per_proc",rows_per_proc);
    int **my_rows = malloc(sizeof(int*) * rows_per_proc);
    //
    // Recieve designated rows:
    for (int row = 0; row < rows_per_proc; row++) {
        my_rows[row] = malloc(sizeof(int) * n);
        MPI_Recv(&my_rows[row][0],n, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }
    if (rank == log_rank) {
        // print_mat(my_rows, rows_per_proc, n, "mat");
    }
    int *recv_row = malloc(sizeof(int) * n);
    int *prev_row= recv_row;
    int src_proc = 0;
    int row_start = 0;
    int product = 1;
    int sum = 0;
    //
    // For each column reduction (make column zeros):
    for (int col = 0; col < (rank+1)*rows_per_proc-1; col++) {
        //
        // Recieve source row:
        if (src_proc < rank) {
            logg("other_procs","BRUHHHHHH\n");
            if (rank == log_rank) logg_int("other_procs","recieving row ",src_proc);
            // MPI_Barrier(MPI_COMM_WORLD);
            MPI_Recv(recv_row, n, MPI_INT, src_proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // if (rank==log_rank) print_row(recv_row, n, -999);
            prev_row = recv_row;
            // if (rank == log_rank) print_row(prev_row, n, -1);
        }
        else {
            // if (rank== log_rank) logg("HERERERERERER","HERERRERE");
            prev_row = my_rows[col%rows_per_proc];
            
            row_start++;
            // printf("NEW PROD: %d *= prev_row[%d] -> %d *= %d, row_start: %d\n",product, col, product, prev_row[col], row_start);
            product *= prev_row[col];
            sum += prev_row[col];
            if (rank < num_procs-1) 
                forward_row(prev_row,0);
            // copy_row(my_rows[col%rows_per_proc], recv_row,n);
            // if (rank== log_rank) print_row(prev_row,n,col);
            // if (rank== log_rank) printf("src_proc, rank %d, %d\n",src_proc, rank);
            
        }
        //
        // For each row designated to this process:
        for (int cur_row = row_start; cur_row < rows_per_proc; cur_row++) {
            if (rank == log_rank) {
                // print_row(prev_row, n, col);
                // print_mat(my_rows, rows_per_proc, n, "mat loop");
                // printf("src_proc,col,cur_row %d,%d,%d\n",src_proc,col,cur_row);
            }
            //
            // Computation:
            int L_r_c = my_rows[cur_row][col] / prev_row[col];
            logg_int("other_procs","L_r_c", L_r_c);
            for (int j = col; j < n; j++) {
                my_rows[cur_row][j] -= (L_r_c * prev_row[j]);
            }

        }

        if (col % rows_per_proc == rows_per_proc-1) {
            src_proc++;
        }
        
    }
    product *= my_rows[row_start][rows_per_proc*rank+rows_per_proc-1];
    sum += my_rows[row_start][rows_per_proc*rank+rows_per_proc-1];
    logg_int("other_procs","diag product",product);
    
    if (rank < num_procs-1) forward_row(my_rows[row_start],0);
    MPI_Send(&product, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Send(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

    logg("other_procs","FINISHED");
    // MPI_Barrier(MPI_COMM_WORLD);
}



void forward_row(int *row, int start_col) {
    logg_int("forward_row","distributing row",start_col);
    for (int proc = rank+1; proc < num_procs; proc++) {
        MPI_Send(row, n, MPI_INT, proc, 0, MPI_COMM_WORLD);
    }
}