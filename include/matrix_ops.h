#ifndef MATRIX_OPS_H
#define MATRIX_OPS_H

#include <stddef.h>

/* Matrix structure definition */
typedef struct {
    double *data;
    size_t rows;
    size_t cols;
} Matrix;

/* Matrix operation function prototypes */
Matrix* matrix_create(size_t rows, size_t cols);
void matrix_destroy(Matrix *matrix);
void matrix_fill_random(Matrix *matrix);
void matrix_fill_identity(Matrix *matrix);
Matrix* matrix_multiply_naive(const Matrix *a, const Matrix *b);
Matrix* matrix_multiply_optimized(const Matrix *a, const Matrix *b);
Matrix* matrix_multiply_riscv_optimized(const Matrix *a, const Matrix *b);
void matrix_print(const Matrix *matrix);
double matrix_sum(const Matrix *matrix);
void matrix_transpose(Matrix *matrix);

/* Performance measurement utilities */
double benchmark_matrix_multiply(size_t size, int iterations);
void compare_matrix_algorithms(size_t size);

#endif /* MATRIX_OPS_H */
