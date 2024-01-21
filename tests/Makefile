mpi_c:
	mpicc ../determinant/det_mpi_optimized.c -lm -o ../determinant/out
	mpirun -np 3 ../determinant/out
	python ../determinant/real_value.py