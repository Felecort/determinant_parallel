mpi_c:
	mpicc ../determinant/det_mpi_optimized.c -lm -o ../determinant/out
	mpirun -np 6 ../determinant/out
	python ../determinant/real_value.py

mpi_py:
	mpiexec -n 6 python ../determinant/det_mpi.py
	python ../determinant/real_value.py

