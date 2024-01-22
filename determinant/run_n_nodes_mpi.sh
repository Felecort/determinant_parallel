#!/bin/bash
#SBATCH -J c_mpi            # Job name  
#SBATCH -N 3                # Количество узлов
#SBATCH -p tornado          # Тип узлов
#SBATCH -o ./stdout.txt     # !!!stdout!!!
#SBATCH -e ./stederr.txt    # stderr
#SBATCH -t 00:01:00         # Ограничение на время выполнения
module load mpi/openmpi/4.1.3/gcc/11
mpicc det_mpi.c -lm -o out  # compile
mpirun -n 1 ./out           # & run
mpirun -n 2 ./out           # & run
mpirun -n 4 ./out           # & run
mpirun -n 8 ./out           # & run
mpirun -n 15 ./out           # & run
mpirun -n 20 ./out           # & run
mpirun -n 28 ./out           # & run
mpirun -n 36 ./out           # & run
mpirun -n 53 ./out           # & run
mpirun -n 72 ./out           # & run
