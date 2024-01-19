// gcc -o out -std=c99 -lpthread det_pthread.c && ./out

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// #define PRINT_MATRIX
#define WRITE_MATRIX_TO_FILE


void fill_matrix(double **matrix, int shape)
{
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
	int shape; //номер обрабатываемой строки
	int main_arr_index;
    int lines;
    int start_row;
    double** matrix;
} pthrData;


void* triangalization(void* threads_dara_arr){

    double coef;
	pthrData *data = (pthrData*)threads_dara_arr;
 
    for (int row = data->start_row; row < data->lines + data->start_row; row++){
        coef = -1 * data->matrix[row][data->main_arr_index] / data->matrix[data->main_arr_index][data->main_arr_index];
        for (int col = 0; col < data->shape; col++){
            data->matrix[row][col] += coef * data->matrix[data->main_arr_index][col];
        }
    }

	return NULL;
}


int main(int argc, char* argv[]){
    int shape, rank, num_threads, div_, mod_, inner_lines, start_row, running_threads;
    double start_time, stop_time, coef, res = 1;
    double **matrix;
    
    num_threads = atoi(argv[1]);
    if (num_threads < 1) {
        printf("num threads nust be positive\n");
        return 1;
    }

    printf("Enter a matrix shape: ");
    fflush(stdout);
    scanf("%d", &shape);
    if (shape < 1){
        printf("Shape must be > 1\n");
        return 2;
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
    pthread_t* threads = (pthread_t*) malloc(num_threads * sizeof(pthread_t));
    pthrData* threads_dara_arr = (pthrData*) malloc(num_threads * sizeof(pthrData));

    for (int main_arr_index = 0; main_arr_index < shape - 1; main_arr_index++){
        div_ = (shape - main_arr_index - 1) / num_threads;
        mod_ = (shape - main_arr_index - 1) % num_threads;
        running_threads = 0;
        for (int rank_id = 0; rank_id < num_threads; rank_id++){
            threads_dara_arr[rank_id].shape = shape;
            threads_dara_arr[rank_id].main_arr_index = main_arr_index;
            threads_dara_arr[rank_id].matrix = matrix;
            if (rank_id < mod_){
                inner_lines = div_ + 1;
            } else {
                inner_lines = div_;
            }
            threads_dara_arr[rank_id].lines = inner_lines;

            if (rank_id < mod_ && inner_lines != 0){
                start_row = main_arr_index + 1 + rank_id * inner_lines;
                threads_dara_arr[rank_id].start_row = start_row;
                // printf("main_arr_index: %d, rank: %d, lines: %d start from: %d\n", main_arr_index, rank_id, inner_lines, start_row);
                pthread_create(&(threads[rank_id]), NULL, triangalization, &threads_dara_arr[rank_id]);
                running_threads++;
                // pthread_join(threads[rank_id], NULL);
                // print_matrix(matrix, shape);
                // return 0;

            } else if (inner_lines != 0) {
                start_row = main_arr_index + 1 + rank_id * inner_lines + mod_;
                threads_dara_arr[rank_id].start_row = start_row;
                // printf("main_arr_index: %d, rank: %d, lines: %d start from: %d\n", main_arr_index, rank_id, inner_lines, start_row);
                pthread_create(&(threads[rank_id]), NULL, triangalization, &threads_dara_arr[rank_id]);
                running_threads++;
            }
            // printf("rank: %d\n", rank_id);
        }
        for(int i = 0; i < running_threads; i++)
            pthread_join(threads[i], NULL);
        // print_matrix(matrix, shape);
        // printf("\n");
    }

#ifdef PRINT_MATRIX
    print_matrix(matrix, shape);
#endif
    for (int mid = 0; mid < shape; mid++){
        res *= matrix[mid][mid];
    }
    printf("DETERMINANT: %lf\n", res);


    free(threads);
	free(threads_dara_arr);
    for (int row = 0; row < shape; row++){
        free(matrix[row]);
    }
    free(matrix);
}
	// int shape; //номер обрабатываемой строки
	// int main_arr_index;
    // int lines;
    // int start_row;
    // double** matrix;