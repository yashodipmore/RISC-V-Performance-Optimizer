#ifndef MATH_OPS_H
#define MATH_OPS_H

#include <complex.h>

/* Basic mathematical operations */
double fast_sqrt(double x);
double fast_sin(double x);
double fast_cos(double x);
double fast_exp(double x);
double fast_log(double x);

/* Advanced mathematical functions */
double factorial(int n);
double fibonacci(int n);
double fibonacci_optimized(int n);
int is_prime(long long n);
long long gcd(long long a, long long b);
long long lcm(long long a, long long b);

/* Complex number operations */
typedef struct {
    double real;
    double imag;
} Complex;

Complex complex_add(Complex a, Complex b);
Complex complex_multiply(Complex a, Complex b);
Complex complex_divide(Complex a, Complex b);
double complex_magnitude(Complex z);
Complex complex_power(Complex z, int n);

/* Numerical algorithms */
double integrate_simpson(double (*func)(double), double a, double b, int n);
double* solve_linear_system(double **A, double *b, int n);
double determinant(double **matrix, int n);

/* Vector operations */
typedef struct {
    double *data;
    size_t size;
} Vector;

Vector* vector_create(size_t size);
void vector_destroy(Vector *vec);
double vector_dot_product(const Vector *a, const Vector *b);
Vector* vector_cross_product(const Vector *a, const Vector *b);
double vector_magnitude(const Vector *vec);
void vector_normalize(Vector *vec);

/* Performance benchmarks */
double benchmark_math_operations(int iterations);
void compare_math_algorithms(void);

#endif /* MATH_OPS_H */
