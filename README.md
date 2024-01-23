# Сalculation determinant of the matrix using the modified Gauss method 

## Method idea  
Bringing the matrix to an upper-triangular appearance  
![Alt text](./images/alg.png)  
## Parallelization idea  
Send to each process or thread respective rows  
![Alt text](./images/paral.png)  
# Build and run  
You can chenge number of process and threads in Makefile  
- MPI C  
`make mpi_c`
- MPi Python  
`make mpi_py`  
- OMP  
`make omp`  
- PThreads  
`make pthreads`    
- clear  
`make clear`  
After run you should enter matrix shape in stdin  

# Results  
The results are obtained for a 1000x1000 matrix for programs written in C and for a 350x350 matrix for MPI on Python  
- MPI C with different number of proccess and nodes  
![Alt text](images/MPI_C_res.png) 
- MPI Python   
![Alt text](images/mpi_py.png)  
- OpenMP  
![Alt text](images/omp.png)  
- PThread  
![Alt text](images/pthread.png)