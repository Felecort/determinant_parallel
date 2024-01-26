# Сalculation determinant of the matrix using the modified Gauss method 
## Build and run  
You can chenge number of process and threads in Makefile. After run you should enter matrix shape in `stdin`.
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

## Testing  
For the program to work correctly, it is necessary:  
- Python==3.10
- numpy>=1.24.3
- pandas>=2.1.1  
Go to `./tests` directori and use the same commands as in point **Build and run**  

## Method idea  
Bringing the matrix to an upper-triangular appearance  
![Alt text](./images/alg.png)  
### Parallelization idea  
Send to each process or thread respective rows  
![Alt text](./images/paral.png)  


## Results  
The results are obtained for a 1000x1000 matrix for programs written in C and for a 350x350 matrix for MPI on Python  
- MPI C with different number of proccess and nodes  
![Alt text](images/MPI_C_res.png) 
- MPI Python   
![Alt text](images/mpi_py.png)  
- OpenMP  
![Alt text](images/omp.png)  
- PThread  
![Alt text](images/pthread.png)