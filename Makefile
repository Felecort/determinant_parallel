folder := ./determinant
processes := 5
threads := 10

mpi_c:
	mpicc -Wall $(folder)/det_mpi.c -lm -o $(folder)/out
	mpirun -np $(processes) $(folder)/out

mpi_py:
	mpiexec -n $(processes) python $(folder)/det_mpi.py

omp:
	gcc -Wall -o $(folder)/out -fopenmp $(folder)/det_omp.c
	$(folder)/out $(threads)

pthread:
	gcc -Wall -o $(folder)/out -lpthread -lm $(folder)/det_pthread.c
	$(folder)/out $(threads)

clear:
	rm $(folder)/out ./result.txt  ./generated_matrix.txt 