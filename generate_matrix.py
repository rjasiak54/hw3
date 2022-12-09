import random
import os
import numpy

RANGE = 2

LU_PATH = 'LU/'
U_PATH = 'U/'
L_PATH = 'L/'
U_DET_PATH = 'U_DET/'


def generate_row_L(n, col) -> list:
    return [random.randrange(2)+1 for i in range(col)]+[1]+[0 for i in range(n-1-col)]


def generate_row_U(n,col) -> list:
    return [0 for i in range(col)]+[random.randrange(2)+1 for i in range(n-col)]


def print_m(M, name):
    print(name)
    for m in M:
        print(m)
    print('')
    

def generate_matrices_LU(n):
    L = [generate_row_L(n,i) for i in range(n)]
    U = [generate_row_U(n,i) for i in range(n)]
    return L,U


def write_mat_to_file(M,filename):
    s = '\t'.join(['\t'.join([str(k) for k in m]) for m in M])
    with open(filename,'w') as file: file.write(f'{n}\t{s}')


def det(M, n):
    sum = 0
    product = 1
    for i in range(n):
        product *= M[i][i]
        sum += M[i][i]
    return product,sum


if __name__ == '__main__':
    for n in [4, 8, 10, 20, 50, 100, 200, 500, 1000, 2000]:
        random.seed()
        L,U = generate_matrices_LU(n)
        U_det, U_diag_sum = det(U,n)
        LU = numpy.matmul(L,U)
        write_mat_to_file(LU,LU_PATH+f'LU{n}.txt')
        write_mat_to_file(L,L_PATH+f'L{n}.txt')
        write_mat_to_file(U,U_PATH+f'U{n}.txt')
        with open(U_DET_PATH+f"U{n}_det.txt",'w') as file:
            file.write(f'{U_det}\n{U_diag_sum}')
        print(f'{n} done')
    
