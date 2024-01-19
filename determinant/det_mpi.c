// mpicc det_mpi.c -lm -o out && mpirun -n 3 out

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define MASTER 0            /* rank of first task */
#define FROM_MASTER 1            /* setting a message type */
#define FROM_WORKER 2

// #define PRINT_MATRIX
#define WRITE_MATRIX_TO_FILE


void fill_matrix(double *matrix, int shape)
{
    double value;
    for (int row = 0; row < shape; row++){
        for (int col = 0; col < shape; col++){
            value = ((double)rand() / RAND_MAX) / 5;
            matrix[row * shape + col] = value;
        }
    }
#ifdef WRITE_MATRIX_TO_FILE
    FILE *f = fopen("generated_matrix.txt", "w");
    for (int row = 0; row < shape; row++){
        for (int col = 0; col < shape; col++){
            value = matrix[row * shape + col];
            fprintf(f, "%.20lf, ", value);
        }
        fprintf(f, "\n");
    }
    fclose(f);
#endif
}


void print_matrix(double *matrix, int shape)
{
    for (int row = 0; row < shape; row++){
        for (int col = 0; col < shape; col++){
            printf("%lf ", matrix[row * shape + col]);
        }
        printf("\n");
    }
}


void get_main_line(double* matrix, double* main_line, int main_line_index, int shape){
    for (int i = 0; i < shape - main_line_index; i++){
        main_line[i] = matrix[main_line_index * shape + i + main_line_index];
    }
}


void triangalization(double *main_arr, double*  buffer, int shape, int main_arr_index, int lines){
    for (int row = 0; row < lines; row++){
        double coef = -1 * buffer[row * shape + main_arr_index] / main_arr[main_arr_index];
        for (int col = 0; col < shape; col++){
            buffer[row * shape + col] = coef * main_arr[col] + buffer[row * shape + col];
        }
    }
}


int main(int argc, char *argv[])
{
    int rc, rank, size, shape, main_arr_index, lines, div_, mod_, inner_lines;
    int tag = 1;
    double *matrix, *main_arr, *buffer;
    double start_time, stop_time;
    MPI_Status status;

    if (rc = MPI_Init(&argc, &argv)){
        printf("Launch error\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
        return -1;
    }
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0){
        printf("Enter a matrix shape: ");
        fflush(stdout);
        scanf("%d", &shape);
        printf("Allocation...\n");
        matrix = (double *)malloc(shape * shape * sizeof(double));
        fill_matrix(matrix, shape);
        printf("Matrix created!\n");
#ifdef PRINT_MATRIX
        print_matrix(matrix, shape);
#endif
        printf("\n");
        start_time = MPI_Wtime();
    }
    MPI_Bcast(&shape, 1, MPI_INT, 0, MPI_COMM_WORLD);

    for (main_arr_index = 0; main_arr_index < shape - 1; main_arr_index++){
        div_ = (shape - main_arr_index - 1) / size;
        mod_ = (shape - main_arr_index - 1) % size;
        main_arr = (double *)malloc(shape * sizeof(double));
        if (rank < mod_){
            lines = div_ + 1;
        } else {
            lines = div_;
        }
        buffer = (double *)malloc(shape * lines * sizeof(double));


        if (rank == 0){
            memcpy(main_arr, matrix + main_arr_index * shape, shape * sizeof(double));
            for (int rank_id = 1; rank_id < size; rank_id++){

                if (rank_id < mod_){
                    inner_lines = div_ + 1;
                } else {
                    inner_lines = div_;
                }

                if (rank_id < mod_ && inner_lines != 0){
                    MPI_Send(matrix + shape * (main_arr_index + 1 + rank_id * inner_lines), shape * inner_lines, MPI_DOUBLE, rank_id, tag, MPI_COMM_WORLD);
                } else if (inner_lines != 0) {
                    MPI_Send(matrix + shape * (main_arr_index + 1 + rank_id * inner_lines + mod_), shape * inner_lines, MPI_DOUBLE, rank_id, tag, MPI_COMM_WORLD);
                }
            }
            memcpy(buffer, matrix + shape * (main_arr_index + 1), shape * lines * sizeof(double));
        }

        if (lines != 0 && rank != 0){
            MPI_Recv(buffer, shape * lines, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);

        }
        // MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(main_arr, shape, MPI_DOUBLE, 0, MPI_COMM_WORLD);


        triangalization(main_arr, buffer, shape, main_arr_index, lines);
    

        if (lines != 0 && rank != 0){
            MPI_Send(buffer, shape * lines, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
        }
        if (rank == 0){
            memcpy(matrix + (main_arr_index + 1) * shape, buffer, lines * shape * sizeof(double));

            for (int rank_id = 1; rank_id < size; rank_id++){

                if (rank_id < mod_){
                    inner_lines = div_ + 1;
                } else {
                    inner_lines = div_;
                }

                if (rank_id < mod_ && inner_lines != 0){
                    MPI_Recv(matrix + shape * (main_arr_index + 1 + rank_id * inner_lines), shape * inner_lines, MPI_DOUBLE, rank_id, tag, MPI_COMM_WORLD, &status);
                } else if (inner_lines != 0) {
                    MPI_Recv(matrix + shape * (main_arr_index + 1 + rank_id * inner_lines + mod_), shape * inner_lines, MPI_DOUBLE, rank_id, tag, MPI_COMM_WORLD, &status);
                }
            }
        }
        free(main_arr);
        free(buffer);
    }

    if (rank == 0){
        double res;
#ifdef PRINT_MATRIX
        printf("\nresult:\n");
        print_matrix(matrix, shape);
        printf("\n");
#endif
        res = matrix[0];
        for (int mid = 1; mid < shape; mid++){
            res *= matrix[mid * shape + mid];
        }
        stop_time = MPI_Wtime();
        printf("\nDETERMINANT: %lf\n", res);
        printf("Time: %lf\n", stop_time - start_time);
    }

    if (rank == 0){
        free(matrix);
    }
    MPI_Finalize();
}