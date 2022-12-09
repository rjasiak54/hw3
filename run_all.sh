LU_PATH="LU"


make_run_cmd() {
    echo "using LU$1.txt with $2 processors"
    mpirun --np $2 ./main LU/LU$1.txt >> results/$1.$2.out
}

rm -r results
mkdir results


make_run_cmd 100 1
make_run_cmd 100 2
make_run_cmd 100 4
make_run_cmd 100 10

make_run_cmd 200 1
make_run_cmd 200 2
make_run_cmd 200 4
make_run_cmd 200 10

make_run_cmd 500 1
make_run_cmd 500 2
make_run_cmd 500 4
make_run_cmd 500 10

make_run_cmd 1000 1
make_run_cmd 1000 2
make_run_cmd 1000 4
make_run_cmd 1000 10

make_run_cmd 2000 1
make_run_cmd 2000 2
make_run_cmd 2000 4
make_run_cmd 2000 10

# for file in $LU_PATH/*
# do
#     make_run_cmd $file 1
#     make_run_cmd $file 2
#     make_run_cmd $file 4
#     make_run_cmd $file 5
#     make_run_cmd $file 10
# done