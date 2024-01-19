// export OMP_NUM_THREADS=8 && gcc -o out -fopenmp det_omp.c && ./out
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
 
// #define PRINT_MATRIX


void fill_matrix(double **matrix, int shape)
{
    int i = 0;
    double value;
    FILE *f = fopen("generated_matrix.txt", "w");
    for (int row = 0; row < shape; row++){
        for (int col = 0; col < shape; col++){
            value = ((double)rand() / RAND_MAX) / 2;
            matrix[row][col] = value;
            fprintf(f, "%lf, ", value);
            i++;
        }
        fprintf(f, "\n");
    }
    fclose(f);
}


void print_matrix(double** matrix, int shape){
    for (int row = 0; row < shape; row++){
        for (int col = 0; col < shape; col++){
            printf("%lf ", matrix[row][col]);
        }
        printf("\n");
    }
}


int main(int argc, char* argv[])
{
    int shape, size, rank;
    double start_time, stop_time, coef, res = 1;
    double **matrix;

    printf("Enter a matrix shape: ");
    fflush(stdout);
    scanf("%d", &shape);
    printf("Allocation...\n");
    matrix = (double **)malloc(shape * sizeof(double *));
    for (int i = 0; i < shape; i++){
        matrix[i] = (double *)malloc(shape * sizeof(double));
    }
    fill_matrix(matrix, shape);
    printf("\nMatrix created!\n");
#ifdef PRINT_MATRIX
    print_matrix(matrix, shape);
#endif
    printf("\n\n");

    start_time = omp_get_wtime();


    // Begin of parallel region
    #pragma omp parallel private(rank, coef) shared(size, shape, matrix)
    {
        // size = omp_get_num_threads();
        // rank = omp_get_thread_num();
        for (int main_arr_index = 0; main_arr_index < shape - 1; main_arr_index++){
            // create common cycle and try to hang pragma for it
            #pragma omp parallel for
            for (int row = main_arr_index + 1; row < shape; row++){
                coef = -1 * matrix[row][main_arr_index] / matrix[main_arr_index][main_arr_index];
                for (int col = main_arr_index; col < shape; col++){
                    matrix[row][col] += coef * matrix[main_arr_index][col];
                }
            }
            

            // #pragma omp barrier
        }

    }
    #pragma omp parallel for reduction(* : res)
    for (int mid = 0; mid < shape; mid++){
        res *= matrix[mid][mid];
    }
    stop_time = omp_get_wtime();

#ifdef PRINT_MATRIX
    print_matrix(matrix, shape);
#endif
    printf("DETERMINANT: %lf\n", res);
    printf("Time: %lf\n", stop_time - start_time);


    for (int row = 0; row < shape; row++){
        free(matrix[row]);
    }
    free(matrix);
}