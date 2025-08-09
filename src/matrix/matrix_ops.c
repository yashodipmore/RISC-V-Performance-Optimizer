#include "matrix_ops.h"
#include "benchmark.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* Create a new matrix with specified dimensions */
Matrix* matrix_create(size_t rows, size_t cols) {
    Matrix *matrix = malloc(sizeof(Matrix));
    if (!matrix) return NULL;
    
    matrix->data = calloc(rows * cols, sizeof(double));
    if (!matrix->data) {
        free(matrix);
        return NULL;
    }
    
    matrix->rows = rows;
    matrix->cols = cols;
    return matrix;
}

/* Free matrix memory */
void matrix_destroy(Matrix *matrix) {
    if (matrix) {
        free(matrix->data);
        free(matrix);
    }
}

/* Fill matrix with random values */
void matrix_fill_random(Matrix *matrix) {
    if (!matrix || !matrix->data) return;
    
    srand(time(NULL));
    for (size_t i = 0; i < matrix->rows * matrix->cols; i++) {
        matrix->data[i] = (double)rand() / RAND_MAX * 100.0;
    }
}

/* Fill matrix as identity matrix */
void matrix_fill_identity(Matrix *matrix) {
    if (!matrix || !matrix->data || matrix->rows != matrix->cols) return;
    
    memset(matrix->data, 0, matrix->rows * matrix->cols * sizeof(double));
    for (size_t i = 0; i < matrix->rows; i++) {
        matrix->data[i * matrix->cols + i] = 1.0;
    }
}

/* Print matrix contents */
void matrix_print(const Matrix *matrix) {
    if (!matrix || !matrix->data) return;
    
    printf("Matrix [%zu x %zu]:\n", matrix->rows, matrix->cols);
    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = 0; j < matrix->cols; j++) {
            printf("%8.3f ", matrix->data[i * matrix->cols + j]);
        }
        printf("\n");
    }
    printf("\n");
}

/* Calculate sum of all matrix elements */
double matrix_sum(const Matrix *matrix) {
    if (!matrix || !matrix->data) return 0.0;
    
    double sum = 0.0;
    for (size_t i = 0; i < matrix->rows * matrix->cols; i++) {
        sum += matrix->data[i];
    }
    return sum;
}

/* In-place matrix transpose (square matrices only) */
void matrix_transpose(Matrix *matrix) {
    if (!matrix || !matrix->data || matrix->rows != matrix->cols) return;
    
    for (size_t i = 0; i < matrix->rows; i++) {
        for (size_t j = i + 1; j < matrix->cols; j++) {
            double temp = matrix->data[i * matrix->cols + j];
            matrix->data[i * matrix->cols + j] = matrix->data[j * matrix->cols + i];
            matrix->data[j * matrix->cols + i] = temp;
        }
    }
}

/* Benchmark matrix multiplication performance */
double benchmark_matrix_multiply(size_t size, int iterations) {
    Matrix *a = matrix_create(size, size);
    Matrix *b = matrix_create(size, size);
    
    if (!a || !b) {
        matrix_destroy(a);
        matrix_destroy(b);
        return -1.0;
    }
    
    matrix_fill_random(a);
    matrix_fill_random(b);
    
    Timer timer;
    timer_start(&timer);
    
    for (int i = 0; i < iterations; i++) {
        Matrix *result = matrix_multiply_optimized(a, b);
        matrix_destroy(result);
    }
    
    timer_stop(&timer);
    double elapsed = timer_elapsed_ms(&timer);
    
    matrix_destroy(a);
    matrix_destroy(b);
    
    return elapsed / iterations;
}

/* Compare different matrix multiplication algorithms */
void compare_matrix_algorithms(size_t size) {
    printf("Matrix Algorithm Comparison [%zu x %zu]\n", size, size);
    printf("=========================================\n");
    
    Matrix *a = matrix_create(size, size);
    Matrix *b = matrix_create(size, size);
    
    if (!a || !b) {
        printf("Failed to allocate matrices\n");
        return;
    }
    
    matrix_fill_random(a);
    matrix_fill_random(b);
    
    Timer timer;
    
    // Naive algorithm
    timer_start(&timer);
    Matrix *result1 = matrix_multiply_naive(a, b);
    timer_stop(&timer);
    double time_naive = timer_elapsed_ms(&timer);
    
    // Optimized algorithm
    timer_start(&timer);
    Matrix *result2 = matrix_multiply_optimized(a, b);
    timer_stop(&timer);
    double time_optimized = timer_elapsed_ms(&timer);
    
    // RISC-V specific optimized algorithm
    timer_start(&timer);
    Matrix *result3 = matrix_multiply_riscv_optimized(a, b);
    timer_stop(&timer);
    double time_riscv = timer_elapsed_ms(&timer);
    
    printf("Naive Algorithm:     %.3f ms\n", time_naive);
    printf("Optimized Algorithm: %.3f ms (%.2fx speedup)\n", 
           time_optimized, time_naive / time_optimized);
    printf("RISC-V Optimized:    %.3f ms (%.2fx speedup)\n", 
           time_riscv, time_naive / time_riscv);
    
    // Verify results are consistent
    double sum1 = matrix_sum(result1);
    double sum2 = matrix_sum(result2);
    double sum3 = matrix_sum(result3);
    
    printf("Result verification: %.6f %.6f %.6f %s\n", 
           sum1, sum2, sum3, 
           (fabs(sum1 - sum2) < 1e-6 && fabs(sum2 - sum3) < 1e-6) ? "PASS" : "FAIL");
    
    matrix_destroy(a);
    matrix_destroy(b);
    matrix_destroy(result1);
    matrix_destroy(result2);
    matrix_destroy(result3);
}
