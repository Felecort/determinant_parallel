// export OMP_NUM_THREADS=8 && gcc -o out -fopenmp det_omp.c && ./out
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
 

void fill_matrix(double **matrix, int shape)
{
    int i = 0;
    double value;
    FILE *f = fopen("generated_matrix.txt", "w");
    for (int row = 0; row < shape; row++){
        for (int col = 0; col < shape; col++){
            value = ((double)rand() / RAND_MAX) / 5;
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
    print_matrix(matrix, shape);


 
    // Begin of parallel region
    #pragma omp parallel private(rank) shared(size, shape, matrix)
    {
        size = omp_get_num_threads();
        rank = omp_get_thread_num();
        for (int main_arr_index = 0; main_arr_index < shape - 1; main_arr_index++){
            // create common cycle and try to hang pragma for it

            #pragma omp barrier
        }
        
    }


    for (int row = 0; row < shape; row++){
        free(matrix[row]);
    }
    free(matrix);
}