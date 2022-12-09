# for (( i=2; i<=10; i+=2 )); do
#     echo $COUNTER
# done
# exit 0
mpicc main.c -o main

PBS_PATH="pbs"





make_pbs () {
name=LU$1_$2n_$3pp
rm -f $PBS_PATH/$name.pbs
echo "#PBS -l nodes=$2:ppn=$3,walltime=1:00
#PBS -N $name
#PBS -q edu_shared
#PBS -m abe
#PBS -M rjasia2@uic.edu
#PBS -e errs/$name.err
#PBS -o outs/$name.out
#PBS -d /home/rjasia2/hw2/hw3
module load OpenMPI/3.1.4-GCC-8.3.0
mpirun -machinefile \$PBS_NODEFILE -np \$PBS_NP ./main LU/LU$1.txt" >> $PBS_PATH/$name.pbs
}



make_pbs 100 1 1
make_pbs 100 1 2
make_pbs 100 1 4
make_pbs 100 1 10

make_pbs 200 1 1
make_pbs 200 1 2
make_pbs 200 1 4
make_pbs 200 1 10

make_pbs 500 1 1
make_pbs 500 1 2
make_pbs 500 1 4
make_pbs 500 1 10

make_pbs 1000 1 1
make_pbs 1000 1 2
make_pbs 1000 1 4
make_pbs 1000 1 10

make_pbs 2000 1 1
make_pbs 2000 1 2
make_pbs 2000 1 4
make_pbs 2000 1 10


for file in $PBS_PATH/*
do
    qsub $file
done