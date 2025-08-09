#include "math_ops.h"
#include "benchmark.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Fast square root using Newton's method */
double fast_sqrt(double x) {
    if (x <= 0) return 0;
    
    double guess = x / 2.0;
    double prev_guess;
    
    do {
        prev_guess = guess;
        guess = (guess + x / guess) / 2.0;
    } while (fabs(guess - prev_guess) > 1e-10);
    
    return guess;
}

/* Fast sine approximation using Taylor series */
double fast_sin(double x) {
    // Normalize x to [-π, π]
    while (x > M_PI) x -= 2 * M_PI;
    while (x < -M_PI) x += 2 * M_PI;
    
    double x2 = x * x;
    double result = x;
    double term = x;
    
    // Taylor series: sin(x) = x - x³/3! + x⁵/5! - x⁷/7! + ...
    for (int i = 1; i <= 10; i++) {
        term *= -x2 / ((2 * i) * (2 * i + 1));
        result += term;
    }
    
    return result;
}

/* Fast cosine approximation using Taylor series */
double fast_cos(double x) {
    // Normalize x to [-π, π]
    while (x > M_PI) x -= 2 * M_PI;
    while (x < -M_PI) x += 2 * M_PI;
    
    double x2 = x * x;
    double result = 1.0;
    double term = 1.0;
    
    // Taylor series: cos(x) = 1 - x²/2! + x⁴/4! - x⁶/6! + ...
    for (int i = 1; i <= 10; i++) {
        term *= -x2 / ((2 * i - 1) * (2 * i));
        result += term;
    }
    
    return result;
}

/* Fast exponential approximation using Taylor series */
double fast_exp(double x) {
    if (x > 700) return INFINITY; // Prevent overflow
    if (x < -700) return 0;       // Prevent underflow
    
    double result = 1.0;
    double term = 1.0;
    
    // Taylor series: e^x = 1 + x + x²/2! + x³/3! + ...
    for (int i = 1; i <= 20; i++) {
        term *= x / i;
        result += term;
        if (fabs(term) < 1e-15) break; // Early termination
    }
    
    return result;
}

/* Fast natural logarithm approximation */
double fast_log(double x) {
    if (x <= 0) return -INFINITY;
    if (x == 1) return 0;
    
    // Use the identity: ln(x) = 2 * artanh((x-1)/(x+1))
    double y = (x - 1) / (x + 1);
    double y2 = y * y;
    double result = y;
    double term = y;
    
    // Series expansion of artanh
    for (int i = 1; i <= 20; i++) {
        term *= y2;
        result += term / (2 * i + 1);
    }
    
    return 2 * result;
}

/* Calculate factorial */
double factorial(int n) {
    if (n < 0) return 0;
    if (n == 0 || n == 1) return 1;
    
    double result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

/* Naive Fibonacci calculation */
double fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

/* Optimized Fibonacci using iterative approach */
double fibonacci_optimized(int n) {
    if (n <= 1) return n;
    
    double a = 0, b = 1, c;
    for (int i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

/* Check if number is prime */
int is_prime(long long n) {
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    
    for (long long i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return 0;
        }
    }
    return 1;
}

/* Greatest Common Divisor using Euclidean algorithm */
long long gcd(long long a, long long b) {
    while (b != 0) {
        long long temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

/* Least Common Multiple */
long long lcm(long long a, long long b) {
    return (a / gcd(a, b)) * b;
}

/* Simpson's rule for numerical integration */
double integrate_simpson(double (*func)(double), double a, double b, int n) {
    if (n % 2 != 0) n++; // Ensure n is even
    
    double h = (b - a) / n;
    double sum = func(a) + func(b);
    
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        if (i % 2 == 0) {
            sum += 2 * func(x);
        } else {
            sum += 4 * func(x);
        }
    }
    
    return sum * h / 3.0;
}

/* Calculate determinant of n×n matrix */
double determinant(double **matrix, int n) {
    if (n == 1) return matrix[0][0];
    if (n == 2) return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
    
    double det = 0;
    for (int i = 0; i < n; i++) {
        // Create submatrix
        double **submatrix = malloc((n - 1) * sizeof(double*));
        for (int j = 0; j < n - 1; j++) {
            submatrix[j] = malloc((n - 1) * sizeof(double));
        }
        
        // Fill submatrix
        for (int j = 1; j < n; j++) {
            int col = 0;
            for (int k = 0; k < n; k++) {
                if (k != i) {
                    submatrix[j - 1][col++] = matrix[j][k];
                }
            }
        }
        
        // Recursive call
        double subdet = determinant(submatrix, n - 1);
        det += (i % 2 == 0 ? 1 : -1) * matrix[0][i] * subdet;
        
        // Free submatrix
        for (int j = 0; j < n - 1; j++) {
            free(submatrix[j]);
        }
        free(submatrix);
    }
    
    return det;
}

/* Benchmark mathematical operations */
double benchmark_math_operations(int iterations) {
    Timer timer;
    timer_start(&timer);
    
    volatile double result; // Prevent optimization
    
    for (int i = 0; i < iterations; i++) {
        result = fast_sqrt(i + 1.0);
        result = fast_sin(i * 0.01);
        result = fast_cos(i * 0.01);
        result = fast_exp(i * 0.001);
        result = fast_log(i + 1.0);
        result = fibonacci_optimized(i % 30);
        (void)is_prime(i + 1000);
    }
    
    timer_stop(&timer);
    (void)result; // Suppress unused variable warning
    
    return timer_elapsed_ms(&timer) / iterations;
}

/* Compare different mathematical algorithms */
void compare_math_algorithms(void) {
    printf("Mathematical Algorithm Comparison\n");
    printf("================================\n");
    
    Timer timer;
    
    // Fibonacci comparison
    printf("\nFibonacci(35):\n");
    timer_start(&timer);
    double fib_naive = fibonacci(35);
    timer_stop(&timer);
    printf("Naive:     %.0f (%.6f ms)\n", fib_naive, timer_elapsed_ms(&timer));
    
    timer_start(&timer);
    double fib_opt = fibonacci_optimized(35);
    timer_stop(&timer);
    printf("Optimized: %.0f (%.6f ms)\n", fib_opt, timer_elapsed_ms(&timer));
    
    // Mathematical function accuracy
    printf("\nFunction Accuracy Test (x = 1.5):\n");
    double x = 1.5;
    printf("sqrt: %.6f vs %.6f (stdlib)\n", fast_sqrt(x), sqrt(x));
    printf("sin:  %.6f vs %.6f (stdlib)\n", fast_sin(x), sin(x));
    printf("cos:  %.6f vs %.6f (stdlib)\n", fast_cos(x), cos(x));
    printf("exp:  %.6f vs %.6f (stdlib)\n", fast_exp(x), exp(x));
    printf("log:  %.6f vs %.6f (stdlib)\n", fast_log(x), log(x));
}
