# mpiexec -n 4 python det_mpi.py

from mpi4py import MPI
import numpy as np
from random import randint

PRINT_MATRIX = 0
WRITE_MATRIX_TO_FILE = 1


def fill_matrix(matrix, shape):
    np.random.seed(randint(0, 1000))
    value = 0.0
    for row in range(shape):
        for col in range(shape):
            value = np.random.random() / 2
            matrix[row * shape + col] = value
    if WRITE_MATRIX_TO_FILE:
        with open("generated_matrix.txt", "w") as f:
            for row in range(shape):
                for col in range(shape):
                    value = matrix[row * shape + col]
                    f.write(f"{value}, ")
                f.write("\n")


def print_matrix(matrix, shape):
    for row in range(shape):
        for col in range(shape):
            print(round(matrix[row * shape + col], 4), end=" ")
        print()


def get_main_line(matrix, main_line, main_line_index, shape):
    for i in range(shape - main_line_index):
        main_line[i] = matrix[main_line_index * shape + i + main_line_index]


def triangalization(main_arr, buffer, shape, main_arr_index, lines):
    for row in range(lines):
        coef = -1 * buffer[row * shape + main_arr_index] / main_arr[main_arr_index]
        for col in range(main_arr_index, shape):
            buffer[row * shape + col] = coef * main_arr[col] + buffer[row * shape + col]


if __name__ == "__main__":
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    shape = None

    if rank == 0:
        shape = int(input('Enter a matrix shape: '))
        print("Allocation...")
        matrix = np.zeros(shape * shape, dtype=np.double)
        fill_matrix(matrix, shape)
        print("Matrix created!")
        if PRINT_MATRIX:
            print_matrix(matrix, shape)
        print("\n")
        start_time = MPI.Wtime()

    shape = comm.bcast(shape, root=0)
    for main_arr_index in range(shape - 1):
        div_ = (shape - main_arr_index - 1) // size
        mod_ = (shape - main_arr_index - 1) % size
        main_arr = np.zeros(shape, dtype=np.double)
        if rank < mod_:
            lines = div_ + 1
        else:
            lines = div_
        buffer = np.zeros(shape * lines, dtype=np.double)

        if rank == 0:
            main_arr = matrix[main_arr_index * shape:main_arr_index * shape + shape]
            for rank_id in range(1, size):
                
                if rank_id < mod_:
                    inner_lines = div_ + 1
                else:
                    inner_lines = div_
                
                if (rank_id < mod_) and (inner_lines != 0):
                    slice_ = slice(shape * (main_arr_index + 1 + rank_id * inner_lines), shape * (main_arr_index + 1 + rank_id * inner_lines) + shape *inner_lines)
                    comm.Send([matrix[slice_], MPI.DOUBLE], rank_id)
                elif inner_lines != 0:
                    slice_ = slice(shape * (main_arr_index + 1 + rank_id * inner_lines + mod_), shape * (main_arr_index + 1 + rank_id * inner_lines + mod_) + shape * inner_lines)
                    comm.Send([matrix[slice_], MPI.DOUBLE], rank_id)
            
            slice_ = slice(shape * (main_arr_index + 1), shape * (main_arr_index + 1) + shape * lines)
            buffer = matrix[slice_]
        
        if (lines != 0) and (rank != 0):
            comm.Recv([buffer, MPI.DOUBLE], 0, 0)
        comm.Bcast(main_arr, 0)

        triangalization(main_arr, buffer, shape, main_arr_index, lines)
        
        if (lines != 0) and (rank != 0):
            comm.Send([buffer, MPI.DOUBLE], 0, 0)
        
        if rank == 0:
            slice_ = slice((main_arr_index + 1) * shape, (main_arr_index + 1) * shape + shape * lines)
            matrix[slice_] = buffer
            
            for rank_id in range(1, size):
                if rank_id < mod_:
                    inner_lines = div_ + 1
                else:
                    inner_lines = div_

                recieve_buffer = np.zeros(shape * inner_lines, np.double)
                
                if (rank_id < mod_) and (inner_lines != 0):
                    comm.Recv([recieve_buffer, MPI.DOUBLE], rank_id, 0)
                    slice_ = slice(shape * (main_arr_index + 1 + rank_id * inner_lines), shape * (main_arr_index + 1 + rank_id * inner_lines) + shape * inner_lines)
                    matrix[slice_] = recieve_buffer
                elif (inner_lines != 0):
                    slice_ = slice(shape * (main_arr_index + 1 + rank_id * inner_lines + mod_), shape * (main_arr_index + 1 + rank_id * inner_lines + mod_) + shape * inner_lines)
                    comm.Recv([recieve_buffer, MPI.DOUBLE], rank_id, 0)
                    matrix[slice_] = recieve_buffer

    if rank == 0:
        res = matrix[0]
        for mid in range(1, shape):
            res *= matrix[shape * mid + mid]
        
        stop_time = MPI.Wtime()
        
        print("DETERMINANT: ", res)
        print(f"Time: {stop_time - start_time:.2f}")
        with open("result.txt", "w") as f:
            f.write(str(res))