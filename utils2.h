#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct {
    int n;
    int sub_n;
    int sub_xy;
    int **data1;
    int **data2;
    int **prod;
} NUMS_DATA;
typedef NUMS_DATA NumsData;


int **alloc_2d_int(int rows, int cols) {
    int *data = (int *)malloc(rows*cols*sizeof(int));
    int **array= (int **)malloc(rows*sizeof(int*));
    for (int i=0; i<rows; i++)
        array[i] = &(data[cols*i]);

    return array;
}


NumsData NumsData_create(int n, int num_procs) {
    NumsData d;
    d.data1 = alloc_2d_int(n,n);
    d.data2 = alloc_2d_int(n,n);
    d.prod  = alloc_2d_int(n,n);
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            d.prod[x][y] = 0;
        }
    }
    d.n = n;
    int sq = (int)sqrt(num_procs);
    if (sq*sq != num_procs) {
        printf("num_procs must be a square number!!\n");
        exit(1);
    }
    d.sub_xy = sq;
    d.sub_n  = n / sq;
    return d;
}

NumsData read_nums(char *filename1, int num_procs) {
    FILE *file1 = fopen(filename1, "r");
    if (!file1) {
        printf("File '%s' failed to open...\n",filename1);
        exit(1);
    }
    int n1;
    fscanf(file1, "%d\t", &n1);
    if (n1 % num_procs != 0) {
        printf("For the N*N matrix, N must be a multiple of the number of processes.\n\tGot %d processes, %d*%d matrix.\n\n",num_procs,n1,n1);
    }
    NumsData ND = NumsData_create(n1, num_procs);
    for (int i = 0; i < n1; i++) {
        for (int j = 0; j < n1; j++) {
            int k;
            fscanf(file1, "%d\t", &k);
            ND.data1[i][j] = k;
            ND.data2[i][j] = k;
        }
    }
    fclose(file1);
    return ND;
}

void NumsData_destroy(NumsData nd) {
    free(nd.data1[0]);
    free(nd.data1);
    free(nd.data2[0]);
    free(nd.data2);
    free(nd.prod[0]);
    free(nd.prod);
}

void NumsData_print(NumsData nd) {
    printf("NumsData.n: %d\n",nd.n);
    printf("NumsData.sub_n: %d\n",nd.sub_n);
    printf("NumsData.sub_xy: %d\n",nd.sub_xy);
    printf("NumsData.data1:\n");
    for (int i = 0; i < nd.n; i++) {
        printf("\t");
        for (int j = 0; j < nd.n; j++) {
            printf("%3d",nd.data1[i][j]);
        }
        printf("\n");
    }
    printf("NumsData.data2:\n");
    for (int i = 0; i < nd.n; i++) {
        printf("\t");
        for (int j = 0; j < nd.n; j++) {
            printf("%3d",nd.data2[i][j]);
        }
        printf("\n");
    }
    printf("NumsData.prod:\n");
    for (int i = 0; i < nd.n; i++) {
        printf("\t");
        for (int j = 0; j < nd.n; j++) {
            printf("%5d",nd.prod[i][j]);
        }
        printf("\n");
    }
}






 
// // Function to get cofactor of mat[p][q] in temp[][]. n is
// // current dimension of mat[][]
// void getCofactor(int **mat, int **temp, int p, int q, int n)
// {
//     int i = 0, j = 0;
 
//     // Looping for each element of the matrix
//     for (int row = 0; row < n; row++)
//     {
//         for (int col = 0; col < n; col++)
//         {
//             //  Copying into temporary matrix only those
//             //  element which are not in given row and
//             //  column
//             if (row != p && col != q)
//             {
//                 printf("temp: attempting: %d,%d\n",i,j);
//                 printf("mat:  attempting: %d,%d\n",row,col);
//                 temp[i][j++] = mat[row][col];

//                 // Row is filled, so increase row index and
//                 // reset col index
//                 if (j == n - 1)
//                 {
//                     j = 0;
//                     i++;
//                 }
//             }
//         }
//     }
// }
void print_matrix_(int **a, int n) {
    for (int i = 0; i < n; i++) {
        printf("bruh \t");
        for (int j = 0; j < n; j++) {
            printf("%5d",a[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
}
 
// /* Recursive function for finding determinant of matrix.
//    n is current dimension of mat[][]. */
// int determinantOfMatrix(int **mat, int n)
// {
//     // print_matrix_(mat,n);
//     int D = 0; // Initialize result
 
//     //  Base case : if matrix contains single element
//     if (n == 1)
//         return mat[0][0];

//     int temp[n][n]; // To store cofactors
 
//     int sign = 1; // To store sign multiplier
 
//     // Iterate for each element of first row
//     for (int f = 0; f < n; f++)
//     {
//         // Getting Cofactor of mat[0][f]
//         getCofactor(mat, temp, 0, f, n);
//         // printf("accessing: %d,%d\n",0,f);
//         D += sign * mat[0][f] * determinantOfMatrix(temp, n - 1);
 
//         // terms are to be added with alternate sign
//         sign = -sign;
//     }
 
//     return D;
// }