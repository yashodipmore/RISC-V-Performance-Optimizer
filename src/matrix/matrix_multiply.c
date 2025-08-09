#include "matrix_ops.h"
#include <stdlib.h>
#include <string.h>

/* Naive matrix multiplication - O(n^3) */
Matrix* matrix_multiply_naive(const Matrix *a, const Matrix *b) {
    if (!a || !b || a->cols != b->rows) return NULL;
    
    Matrix *result = matrix_create(a->rows, b->cols);
    if (!result) return NULL;
    
    for (size_t i = 0; i < a->rows; i++) {
        for (size_t j = 0; j < b->cols; j++) {
            double sum = 0.0;
            for (size_t k = 0; k < a->cols; k++) {
                sum += a->data[i * a->cols + k] * b->data[k * b->cols + j];
            }
            result->data[i * result->cols + j] = sum;
        }
    }
    
    return result;
}

/* Cache-optimized matrix multiplication with loop tiling */
Matrix* matrix_multiply_optimized(const Matrix *a, const Matrix *b) {
    if (!a || !b || a->cols != b->rows) return NULL;
    
    Matrix *result = matrix_create(a->rows, b->cols);
    if (!result) return NULL;
    
    // Initialize result matrix to zero
    memset(result->data, 0, result->rows * result->cols * sizeof(double));
    
    const size_t BLOCK_SIZE = 64; // Cache-friendly block size
    
    // Blocked matrix multiplication for better cache locality
    for (size_t i = 0; i < a->rows; i += BLOCK_SIZE) {
        for (size_t j = 0; j < b->cols; j += BLOCK_SIZE) {
            for (size_t k = 0; k < a->cols; k += BLOCK_SIZE) {
                
                // Process block
                size_t i_end = (i + BLOCK_SIZE < a->rows) ? i + BLOCK_SIZE : a->rows;
                size_t j_end = (j + BLOCK_SIZE < b->cols) ? j + BLOCK_SIZE : b->cols;
                size_t k_end = (k + BLOCK_SIZE < a->cols) ? k + BLOCK_SIZE : a->cols;
                
                for (size_t ii = i; ii < i_end; ii++) {
                    for (size_t jj = j; jj < j_end; jj++) {
                        double sum = result->data[ii * result->cols + jj];
                        for (size_t kk = k; kk < k_end; kk++) {
                            sum += a->data[ii * a->cols + kk] * b->data[kk * b->cols + jj];
                        }
                        result->data[ii * result->cols + jj] = sum;
                    }
                }
            }
        }
    }
    
    return result;
}

/* RISC-V specific optimized matrix multiplication */
Matrix* matrix_multiply_riscv_optimized(const Matrix *a, const Matrix *b) {
    if (!a || !b || a->cols != b->rows) return NULL;
    
    Matrix *result = matrix_create(a->rows, b->cols);
    if (!result) return NULL;
    
    // Initialize result matrix
    memset(result->data, 0, result->rows * result->cols * sizeof(double));
    
    const size_t BLOCK_SIZE = 32; // Optimized for RISC-V cache hierarchy
    
#ifdef __riscv
    // RISC-V specific optimizations
    // Use vector extensions if available (RVV)
    #ifdef __riscv_vector
        // Vector-optimized implementation would go here
        // For now, fall back to scalar optimization
    #endif
    
    // RISC-V specific loop unrolling and memory access patterns
    for (size_t i = 0; i < a->rows; i += BLOCK_SIZE) {
        for (size_t j = 0; j < b->cols; j += BLOCK_SIZE) {
            for (size_t k = 0; k < a->cols; k += BLOCK_SIZE) {
                
                size_t i_end = (i + BLOCK_SIZE < a->rows) ? i + BLOCK_SIZE : a->rows;
                size_t j_end = (j + BLOCK_SIZE < b->cols) ? j + BLOCK_SIZE : b->cols;
                size_t k_end = (k + BLOCK_SIZE < a->cols) ? k + BLOCK_SIZE : a->cols;
                
                // Unrolled inner loops for RISC-V pipeline optimization
                for (size_t ii = i; ii < i_end; ii++) {
                    double *result_row = &result->data[ii * result->cols];
                    const double *a_row = &a->data[ii * a->cols];
                    
                    for (size_t kk = k; kk < k_end; kk++) {
                        double a_val = a_row[kk];
                        const double *b_row = &b->data[kk * b->cols];
                        
                        // Manual loop unrolling (4x)
                        size_t jj = j;
                        for (; jj + 3 < j_end; jj += 4) {
                            result_row[jj] += a_val * b_row[jj];
                            result_row[jj + 1] += a_val * b_row[jj + 1];
                            result_row[jj + 2] += a_val * b_row[jj + 2];
                            result_row[jj + 3] += a_val * b_row[jj + 3];
                        }
                        
                        // Handle remaining elements
                        for (; jj < j_end; jj++) {
                            result_row[jj] += a_val * b_row[jj];
                        }
                    }
                }
            }
        }
    }
#else
    // Fallback to regular optimized version for non-RISC-V architectures
    Matrix *temp = matrix_multiply_optimized(a, b);
    if (temp) {
        memcpy(result->data, temp->data, result->rows * result->cols * sizeof(double));
        matrix_destroy(temp);
    }
#endif
    
    return result;
}
