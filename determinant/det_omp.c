// gcc -o out -fopenmp det_omp.c && ./out 12


#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// #define PRINT_MATRIX
#define WRITE_MATRIX_TO_FILE


void fill_matrix(double **matrix, int shape)
{
    srand(time(NULL));
    int i = 0;
    double value;
    for (int row = 0; row < shape; row++){
        for (int col = 0; col < shape; col++){
            value = ((double)rand() / RAND_MAX) / 5;
            matrix[row][col] = value;
            i++;
        }
    }
#ifdef WRITE_MATRIX_TO_FILE
    FILE *f = fopen("generated_matrix.txt", "w");
    for (int row = 0; row < shape; row++){
        for (int col = 0; col < shape; col++){
            value = matrix[row][col];
            fprintf(f, "%.20lf, ", value);
        }
        fprintf(f, "\n");
    }
    fclose(f);
#endif    
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
    int shape, num_threads;
    double start_time, stop_time, coef, res = 1;
    double **matrix;
    
    num_threads = atoi(argv[1]);
    omp_set_num_threads(num_threads);

#ifndef TEST
    printf("Threads: %d\n", omp_get_max_threads());
    printf("Enter a matrix shape: ");
    fflush(stdout);
    scanf("%d", &shape);
    printf("Allocation...\n");

    matrix = (double **)malloc(shape * sizeof(double *));
    for (int i = 0; i < shape; i++){
        matrix[i] = (double *)malloc(shape * sizeof(double));
    }
    
    fill_matrix(matrix, shape);
    
    printf("Matrix created!\n\n");
#else
    shape = atoi(argv[2]);
    matrix = (double **)malloc(shape * sizeof(double *));
    for (int i = 0; i < shape; i++){
        matrix[i] = (double *)malloc(shape * sizeof(double));
    }
    fill_matrix(matrix, shape);
#endif

#ifdef PRINT_MATRIX
    print_matrix(matrix, shape);
#endif

    start_time = omp_get_wtime();

    #pragma omp parallel private(coef) shared(shape, matrix)
    {
        for (int main_arr_index = 0; main_arr_index < shape - 1; main_arr_index++){
            #pragma omp for
            for (int row = main_arr_index + 1; row < shape; row++){
                coef = -1 * matrix[row][main_arr_index] / matrix[main_arr_index][main_arr_index];
                for (int col = main_arr_index; col < shape; col++){
                    matrix[row][col] += coef * matrix[main_arr_index][col];
                }
            }            
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

#ifndef TEST
    printf("DETERMINANT: %lf\n", res);
#endif
    printf("Time: %lf\n", stop_time - start_time);
    FILE* f = fopen("result.txt", "w");
    fprintf(f, "%.20lf", res);
    fclose(f);
    // Free memory
    for (int row = 0; row < shape; row++){
        free(matrix[row]);
    }
    free(matrix);
}