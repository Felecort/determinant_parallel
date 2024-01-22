folder := ../determinant
processes := 5
threads := 10

mpi_c:
	mpicc $(folder)/det_mpi.c -lm -o $(folder)/out
	mpirun -np $(processes) $(folder)/out
	python $(folder)/real_value.py

mpi_py:
	mpiexec -n $(processes) python $(folder)/det_mpi.py
	python $(folder)/real_value.py

omp:
	gcc -o $(folder)/out -fopenmp $(folder)/det_omp.c
	$(folder)/out $(threads)
	python $(folder)/real_value.py

pthread:
	gcc -o $(folder)/out -lpthread -lm $(folder)/det_pthread.c
	$(folder)/out $(threads)
	python $(folder)/real_value.py

clear:
	rm $(folder)/out ./result.txt  ./generated_matrix.txt 