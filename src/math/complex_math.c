#include "math_ops.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Complex number operations */
Complex complex_add(Complex a, Complex b) {
    Complex result;
    result.real = a.real + b.real;
    result.imag = a.imag + b.imag;
    return result;
}

Complex complex_multiply(Complex a, Complex b) {
    Complex result;
    result.real = a.real * b.real - a.imag * b.imag;
    result.imag = a.real * b.imag + a.imag * b.real;
    return result;
}

Complex complex_divide(Complex a, Complex b) {
    Complex result;
    double denominator = b.real * b.real + b.imag * b.imag;
    
    if (denominator == 0) {
        result.real = INFINITY;
        result.imag = INFINITY;
        return result;
    }
    
    result.real = (a.real * b.real + a.imag * b.imag) / denominator;
    result.imag = (a.imag * b.real - a.real * b.imag) / denominator;
    return result;
}

double complex_magnitude(Complex z) {
    return fast_sqrt(z.real * z.real + z.imag * z.imag);
}

Complex complex_power(Complex z, int n) {
    if (n == 0) {
        Complex result = {1.0, 0.0};
        return result;
    }
    
    if (n == 1) return z;
    
    Complex result = {1.0, 0.0};
    Complex base = z;
    int exp = abs(n);
    
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = complex_multiply(result, base);
        }
        base = complex_multiply(base, base);
        exp /= 2;
    }
    
    if (n < 0) {
        Complex one = {1.0, 0.0};
        result = complex_divide(one, result);
    }
    
    return result;
}

/* Vector operations */
Vector* vector_create(size_t size) {
    Vector *vec = malloc(sizeof(Vector));
    if (!vec) return NULL;
    
    vec->data = calloc(size, sizeof(double));
    if (!vec->data) {
        free(vec);
        return NULL;
    }
    
    vec->size = size;
    return vec;
}

void vector_destroy(Vector *vec) {
    if (vec) {
        free(vec->data);
        free(vec);
    }
}

double vector_dot_product(const Vector *a, const Vector *b) {
    if (!a || !b || a->size != b->size) return 0.0;
    
    double result = 0.0;
    
#ifdef __riscv
    // RISC-V optimized implementation
    #ifdef __riscv_vector
        // Vector extension implementation would go here
        // For now, use scalar optimization
    #endif
    
    // Manual loop unrolling for RISC-V
    size_t i = 0;
    for (; i + 3 < a->size; i += 4) {
        result += a->data[i] * b->data[i];
        result += a->data[i + 1] * b->data[i + 1];
        result += a->data[i + 2] * b->data[i + 2];
        result += a->data[i + 3] * b->data[i + 3];
    }
    
    // Handle remaining elements
    for (; i < a->size; i++) {
        result += a->data[i] * b->data[i];
    }
#else
    // Standard implementation
    for (size_t i = 0; i < a->size; i++) {
        result += a->data[i] * b->data[i];
    }
#endif
    
    return result;
}

Vector* vector_cross_product(const Vector *a, const Vector *b) {
    if (!a || !b || a->size != 3 || b->size != 3) return NULL;
    
    Vector *result = vector_create(3);
    if (!result) return NULL;
    
    result->data[0] = a->data[1] * b->data[2] - a->data[2] * b->data[1];
    result->data[1] = a->data[2] * b->data[0] - a->data[0] * b->data[2];
    result->data[2] = a->data[0] * b->data[1] - a->data[1] * b->data[0];
    
    return result;
}

double vector_magnitude(const Vector *vec) {
    if (!vec) return 0.0;
    
    double sum_squares = 0.0;
    for (size_t i = 0; i < vec->size; i++) {
        sum_squares += vec->data[i] * vec->data[i];
    }
    
    return fast_sqrt(sum_squares);
}

void vector_normalize(Vector *vec) {
    if (!vec) return;
    
    double magnitude = vector_magnitude(vec);
    if (magnitude == 0.0) return;
    
    for (size_t i = 0; i < vec->size; i++) {
        vec->data[i] /= magnitude;
    }
}

/* Solve linear system Ax = b using Gaussian elimination */
double* solve_linear_system(double **A, double *b, int n) {
    if (!A || !b || n <= 0) return NULL;
    
    // Create augmented matrix
    double **augmented = malloc(n * sizeof(double*));
    if (!augmented) return NULL;
    
    for (int i = 0; i < n; i++) {
        augmented[i] = malloc((n + 1) * sizeof(double));
        if (!augmented[i]) {
            // Cleanup on error
            for (int j = 0; j < i; j++) {
                free(augmented[j]);
            }
            free(augmented);
            return NULL;
        }
        
        for (int j = 0; j < n; j++) {
            augmented[i][j] = A[i][j];
        }
        augmented[i][n] = b[i];
    }
    
    // Forward elimination
    for (int i = 0; i < n; i++) {
        // Find pivot
        int max_row = i;
        for (int k = i + 1; k < n; k++) {
            if (fabs(augmented[k][i]) > fabs(augmented[max_row][i])) {
                max_row = k;
            }
        }
        
        // Swap rows
        if (max_row != i) {
            double *temp = augmented[i];
            augmented[i] = augmented[max_row];
            augmented[max_row] = temp;
        }
        
        // Check for singular matrix
        if (fabs(augmented[i][i]) < 1e-10) {
            for (int j = 0; j < n; j++) {
                free(augmented[j]);
            }
            free(augmented);
            return NULL;
        }
        
        // Eliminate column
        for (int k = i + 1; k < n; k++) {
            double factor = augmented[k][i] / augmented[i][i];
            for (int j = i; j <= n; j++) {
                augmented[k][j] -= factor * augmented[i][j];
            }
        }
    }
    
    // Back substitution
    double *x = malloc(n * sizeof(double));
    if (!x) {
        for (int i = 0; i < n; i++) {
            free(augmented[i]);
        }
        free(augmented);
        return NULL;
    }
    
    for (int i = n - 1; i >= 0; i--) {
        x[i] = augmented[i][n];
        for (int j = i + 1; j < n; j++) {
            x[i] -= augmented[i][j] * x[j];
        }
        x[i] /= augmented[i][i];
    }
    
    // Cleanup
    for (int i = 0; i < n; i++) {
        free(augmented[i]);
    }
    free(augmented);
    
    return x;
}
