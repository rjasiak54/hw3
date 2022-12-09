#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int UTILS_rank = 0;
int log_rank = 3;

int LOG = 0;

void logg(char *context, char *message) {
    if (LOG && (UTILS_rank == log_rank) ) printf(" LOGG -- [ pid %d ].%-20s -- %s\n",UTILS_rank,context,message);
}
void logg_int(char *context, char *message, int i) {
    if (LOG && (UTILS_rank == log_rank)) printf(" LOGG -- [ pid %d ].%-20s -- %s, %d\n",UTILS_rank,context,message,i);
}
void logg_err(char *context, char *message) {
    printf(" ERROR -- [ pid %d ].%-2s -- %s\n",UTILS_rank,context,message);
}

int **alloc_2d_int(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);

    return array;
}


void read_matrix_from_file(char *filename, int ***matrix_ptr, int *size_ptr) {
    logg("read_matrix_from_file", "reading matrix from file...");
    FILE *file = fopen(filename, "r");
    // logg("read_matrix_from_file", "file opened");
    if (!file) {
        printf("File '%s' failed to open...\n",filename);
        exit(1);
    }
    
    // logg("read_matrix_from_file", "scanning for int...");
    fscanf(file, "%d\t", size_ptr);
    // logg("read_matrix_from_file", "int scanned");
    int n = *size_ptr;
    *matrix_ptr = malloc(n * sizeof(int*));
    
    
    int k;
    for (int i = 0; i < n; i++) {
        (*matrix_ptr)[i] = malloc(sizeof(int) * n);
        for (int j = 0; j < n; j++) {
            fscanf(file, "%d\t", &k);
            (*matrix_ptr)[i][j] = k;
        }
    }
    fclose(file);
    logg("read_matrix_from_file","done reading file");
}

void print_mat(int **a, int nrows, int ncols, char *s) {
    // logg_int("print_mat","printing mat", nrows);
    // logg_int("print_mat","printing mat", ncols);
    printf("%s\n", s);
    for (int row = 0; row < nrows; row++) {
        for (int col = 0; col < ncols; col++) {
            printf("%5d", a[row][col]);
        }
        printf("\n");
    }
    printf("\n");
}


void destroy_mat(int **m, int n) {
    // free(m[0]);
    // free(m);
    for (int i = 0; i < n; i++) {
        // (*matrix_ptr)[i] = malloc(sizeof(int) * n);
        free(m[i]);
    }
    free(m);
}


void print_row(int *row, int n, int which_col) {
    logg_int("print_row","printing row",which_col);
    for (int i =0;i < n; i++) {
        printf("%5d", row[i]);
    }
    printf("\n");
} 


void copy_row(int *from, int *to, int n) {
    for (int i = 0; i < n; i++) {
        to[i] = from[i];
    }
}