// gcc -o out -lpthread -lm det_pthread.c && ./out 8

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <bits/pthreadtypes.h>


// #define PRINT_MATRIX
#define WRITE_MATRIX_TO_FILE
pthread_barrier_t barrier;


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


typedef struct{
	int shape;
    double** matrix;
    int num_threads;
    int rank_id;
} MatrixData;

// make column with zeros
void recalc_row(double** matrix, int shape, int start_row, int lines, int main_arr_index){
    double coef;
    for (int row = start_row; row < lines + start_row; row++){
        coef = -1 * matrix[row][main_arr_index] / matrix[main_arr_index][main_arr_index];
        for (int col = 0; col < shape; col++){
            matrix[row][col] += coef * matrix[main_arr_index][col];
        }
    }
}


void* triangalization(void* matrix_data){
    MatrixData *data = (MatrixData*)matrix_data;
    double** matrix = data->matrix;
    double coef;
    int shape = data->shape;
    int num_threads = data->num_threads;
    int rank_id = data->rank_id;
    int div_, mod_, inner_lines, start_row;

    for (int main_arr_index = 0; main_arr_index < shape - 1; main_arr_index++){
        div_ = (shape - main_arr_index - 1) / num_threads;
        mod_ = (shape - main_arr_index - 1) % num_threads;
            
        if (rank_id < mod_){
            inner_lines = div_ + 1;
        } else {
            inner_lines = div_;
        }
        // Each thread process own lines
        if (rank_id < mod_ && inner_lines != 0){
            start_row = main_arr_index + 1 + rank_id * inner_lines;
            recalc_row(matrix, shape, start_row, inner_lines, main_arr_index);
        } else if (inner_lines != 0) {
            start_row = main_arr_index + 1 + rank_id * inner_lines + mod_;
            recalc_row(matrix, shape, start_row, inner_lines, main_arr_index);
        } 
        // sync
        pthread_barrier_wait(&barrier);
    }
    return NULL;

}


int main(int argc, char* argv[]){
    long start, end;
    struct timeval tval_before, tval_after, tval_result;
    int shape, rank, num_threads, div_, mod_, inner_lines, start_row, running_threads, status;
    double coef, res = 1;
    double **matrix;
    // Preparations...
    if (argc == 1){
        printf("use: ./out <num of threads>\n");
        return 1;
    }
    num_threads = atoi(argv[1]);
    if (num_threads < 1) {
        printf("num threads nust be positive\n");
        return 2;
    }
    // Create a matrix
    printf("Enter a matrix shape: ");
    fflush(stdout);
    scanf("%d", &shape);
    if (shape < 1){
        printf("Shape must be > 1\n");
        return 3;
    }
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
    printf("\n");
    

    gettimeofday(&tval_before, NULL);
    
    pthread_t* threads = (pthread_t*) malloc(num_threads * sizeof(pthread_t));
    MatrixData* matrix_data = (MatrixData*)malloc(num_threads * sizeof(MatrixData));

    pthread_barrier_init(&barrier, NULL, num_threads);
    for (int rank_id = 0; rank_id < num_threads; rank_id++) {
        // Create struct with function parametrs
        matrix_data[rank_id].matrix = matrix;
        matrix_data[rank_id].shape = shape;
        matrix_data[rank_id].rank_id = rank_id;
        matrix_data[rank_id].num_threads = num_threads;
        pthread_create(&threads[rank_id], NULL, triangalization, (void*) &(matrix_data[rank_id]));
    }
    // pthread_barrier_destroy(&barrier);

    for(int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
#ifdef PRINT_MATRIX
    print_matrix(matrix, shape);
#endif
    // multiplication of elements on the main diagonal
    for (int mid = 0; mid < shape; mid++){
        res *= matrix[mid][mid];
    }
    gettimeofday(&tval_after, NULL);
    timersub(&tval_after, &tval_before, &tval_result);

    printf("DETERMINANT: %lf\n", res);
    printf("Time: %ld.%06ld\n", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
    FILE* f = fopen("result.txt", "w");
    fprintf(f, "%.5lf", res);
    fclose(f);
    free(threads);
    free(matrix_data);
    for (int row = 0; row < shape; row++){
        free(matrix[row]);
    }
    free(matrix);
}
