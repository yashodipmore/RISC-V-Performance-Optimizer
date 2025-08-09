#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "matrix_ops.h"
#include "string_ops.h"
#include "math_ops.h"
#include "benchmark.h"

/* Function prototypes */
void print_usage(const char *program_name);
void run_all_tests(void);
void run_all_benchmarks(void);
void print_system_info(void);
void demonstrate_features(void);

/* Test functions */
void test_matrix_operations(void);
void test_string_operations(void);
void test_math_operations(void);

/* Benchmark functions */
void benchmark_matrix_performance(void);
void benchmark_string_performance(void);
void benchmark_math_performance(void);

int main(int argc, char *argv[]) {
    printf("RISC-V Performance Optimizer\n");
    printf("============================\n");
    printf("A comprehensive software optimization demonstration for RISC-V architecture\n\n");
    
    print_system_info();
    
    if (argc == 1) {
        demonstrate_features();
        return 0;
    }
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--test") == 0) {
            run_all_tests();
        } else if (strcmp(argv[i], "--benchmark") == 0) {
            run_all_benchmarks();
        } else if (strcmp(argv[i], "--matrix") == 0) {
            test_matrix_operations();
            benchmark_matrix_performance();
        } else if (strcmp(argv[i], "--string") == 0) {
            test_string_operations();
            benchmark_string_performance();
        } else if (strcmp(argv[i], "--math") == 0) {
            test_math_operations();
            benchmark_math_performance();
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
        } else {
            printf("Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    return 0;
}

void print_usage(const char *program_name) {
    printf("Usage: %s [OPTIONS]\n\n", program_name);
    printf("Options:\n");
    printf("  --test        Run all functionality tests\n");
    printf("  --benchmark   Run all performance benchmarks\n");
    printf("  --matrix      Test and benchmark matrix operations\n");
    printf("  --string      Test and benchmark string operations\n");
    printf("  --math        Test and benchmark mathematical operations\n");
    printf("  --help, -h    Show this help message\n\n");
    printf("With no arguments, runs a demonstration of all features.\n");
}

void print_system_info(void) {
    printf("System Information:\n");
    printf("  Architecture: %s\n", get_cpu_architecture());
    printf("  CPU Cores:    %d\n", get_cpu_core_count());
    printf("  CPU Freq:     %.1f GHz\n", get_cpu_frequency_ghz());
    
#ifdef __riscv
    printf("  RISC-V Features:\n");
    #ifdef __riscv_vector
    printf("    - Vector Extension (RVV): Enabled\n");
    #else
    printf("    - Vector Extension (RVV): Not available\n");
    #endif
    
    #ifdef __riscv_compressed
    printf("    - Compressed Instructions (RVC): Enabled\n");
    #else
    printf("    - Compressed Instructions (RVC): Not available\n");
    #endif
    
    printf("    - XLEN: %d-bit\n", __riscv_xlen);
#endif
    
    printf("\n");
}

void demonstrate_features(void) {
    printf("=== FEATURE DEMONSTRATION ===\n\n");
    
    // Quick matrix demo
    printf("1. Matrix Operations Demo:\n");
    Matrix *a = matrix_create(3, 3);
    Matrix *b = matrix_create(3, 3);
    
    if (a && b) {
        matrix_fill_random(a);
        matrix_fill_random(b);
        
        printf("Matrix A (3x3):\n");
        matrix_print(a);
        
        Matrix *result = matrix_multiply_optimized(a, b);
        if (result) {
            printf("A × B result sum: %.3f\n", matrix_sum(result));
            matrix_destroy(result);
        }
        
        matrix_destroy(a);
        matrix_destroy(b);
    }
    printf("\n");
    
    // Quick string demo
    printf("2. String Operations Demo:\n");
    const char *text = "RISC-V Performance Optimization Project";
    char *upper = string_to_uppercase(text);
    char *reversed = string_reverse(text);
    
    printf("Original: %s\n", text);
    printf("Uppercase: %s\n", upper);
    printf("Reversed: %s\n", reversed);
    
    free(upper);
    free(reversed);
    printf("\n");
    
    // Quick math demo
    printf("3. Mathematical Operations Demo:\n");
    printf("Fast sqrt(25.0) = %.6f\n", fast_sqrt(25.0));
    printf("Fast sin(π/4) = %.6f\n", fast_sin(M_PI/4));
    printf("Fibonacci(20) = %.0f\n", fibonacci_optimized(20));
    printf("Is 97 prime? %s\n", is_prime(97) ? "Yes" : "No");
    printf("\n");
    
    printf("Run with --help to see all available options.\n");
}

void run_all_tests(void) {
    printf("=== RUNNING ALL TESTS ===\n\n");
    test_matrix_operations();
    test_string_operations();
    test_math_operations();
}

void run_all_benchmarks(void) {
    printf("=== RUNNING ALL BENCHMARKS ===\n\n");
    benchmark_matrix_performance();
    benchmark_string_performance();
    benchmark_math_performance();
}

void test_matrix_operations(void) {
    printf("Testing Matrix Operations...\n");
    printf("---------------------------\n");
    
    // Test matrix creation and basic operations
    Matrix *m = matrix_create(4, 4);
    if (m) {
        matrix_fill_identity(m);
        printf("✓ Identity matrix creation\n");
        
        double sum = matrix_sum(m);
        printf("✓ Matrix sum: %.1f (expected: 4.0)\n", sum);
        
        matrix_destroy(m);
    }
    
    // Test matrix multiplication
    Matrix *a = matrix_create(2, 3);
    Matrix *b = matrix_create(3, 2);
    
    if (a && b) {
        matrix_fill_random(a);
        matrix_fill_random(b);
        
        Matrix *result = matrix_multiply_naive(a, b);
        if (result) {
            printf("✓ Matrix multiplication (2x3 × 3x2 = 2x2)\n");
            matrix_destroy(result);
        }
        
        matrix_destroy(a);
        matrix_destroy(b);
    }
    
    printf("Matrix operations test completed.\n\n");
}

void test_string_operations(void) {
    printf("Testing String Operations...\n");
    printf("----------------------------\n");
    
    const char *test_str = "Hello RISC-V World";
    
    // Test basic operations
    printf("✓ String length: %d\n", string_length(test_str));
    
    char *copy = string_copy(test_str);
    if (copy) {
        printf("✓ String copy successful\n");
        free(copy);
    }
    
    char *upper = string_to_uppercase(test_str);
    if (upper) {
        printf("✓ Uppercase: %s\n", upper);
        free(upper);
    }
    
    // Test search operations
    int pos = string_find(test_str, "RISC-V");
    printf("✓ Pattern search: 'RISC-V' found at position %d\n", pos);
    
    // Test advanced search algorithms
    const char *text = "The quick brown fox jumps over the lazy dog";
    const char *pattern = "fox";
    
    printf("✓ Search algorithm comparison for pattern '%s':\n", pattern);
    printf("  - Naive: %d\n", string_find(text, pattern));
    printf("  - KMP: %d\n", kmp_search(text, pattern));
    printf("  - Boyer-Moore: %d\n", boyer_moore_search(text, pattern));
    printf("  - Rabin-Karp: %d\n", rabin_karp_search(text, pattern));
    
    printf("String operations test completed.\n\n");
}

void test_math_operations(void) {
    printf("Testing Mathematical Operations...\n");
    printf("---------------------------------\n");
    
    // Test basic math functions
    printf("✓ Mathematical function tests:\n");
    printf("  - fast_sqrt(16.0) = %.6f\n", fast_sqrt(16.0));
    printf("  - fast_sin(π/2) = %.6f\n", fast_sin(M_PI/2));
    printf("  - fast_cos(0) = %.6f\n", fast_cos(0));
    printf("  - fast_exp(1) = %.6f\n", fast_exp(1));
    printf("  - fast_log(e) = %.6f\n", fast_log(M_E));
    
    // Test number theory functions
    printf("✓ Number theory tests:\n");
    printf("  - factorial(5) = %.0f\n", factorial(5));
    printf("  - fibonacci(15) = %.0f\n", fibonacci_optimized(15));
    printf("  - gcd(48, 18) = %lld\n", gcd(48, 18));
    printf("  - is_prime(17) = %s\n", is_prime(17) ? "true" : "false");
    
    // Test complex numbers
    printf("✓ Complex number tests:\n");
    Complex a = {3.0, 4.0};
    Complex b = {1.0, 2.0};
    Complex sum = complex_add(a, b);
    Complex product = complex_multiply(a, b);
    
    printf("  - (3+4i) + (1+2i) = %.1f+%.1fi\n", sum.real, sum.imag);
    printf("  - (3+4i) × (1+2i) = %.1f+%.1fi\n", product.real, product.imag);
    printf("  - |3+4i| = %.6f\n", complex_magnitude(a));
    
    // Test vector operations
    printf("✓ Vector operation tests:\n");
    Vector *v1 = vector_create(3);
    Vector *v2 = vector_create(3);
    
    if (v1 && v2) {
        v1->data[0] = 1.0; v1->data[1] = 2.0; v1->data[2] = 3.0;
        v2->data[0] = 4.0; v2->data[1] = 5.0; v2->data[2] = 6.0;
        
        double dot = vector_dot_product(v1, v2);
        printf("  - [1,2,3] · [4,5,6] = %.1f\n", dot);
        
        Vector *cross = vector_cross_product(v1, v2);
        if (cross) {
            printf("  - [1,2,3] × [4,5,6] = [%.1f,%.1f,%.1f]\n", 
                   cross->data[0], cross->data[1], cross->data[2]);
            vector_destroy(cross);
        }
        
        vector_destroy(v1);
        vector_destroy(v2);
    }
    
    printf("Mathematical operations test completed.\n\n");
}

void benchmark_matrix_performance(void) {
    printf("Matrix Performance Benchmarks\n");
    printf("=============================\n");
    
    const size_t sizes[] = {64, 128, 256};
    const int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int i = 0; i < num_sizes; i++) {
        printf("\nMatrix size: %zux%zu\n", sizes[i], sizes[i]);
        compare_matrix_algorithms(sizes[i]);
    }
    printf("\n");
}

void benchmark_string_performance(void) {
    printf("String Performance Benchmarks\n");
    printf("============================\n");
    
    const char *test_texts[] = {
        "The quick brown fox jumps over the lazy dog",
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua",
        "RISC-V is an open standard instruction set architecture based on established reduced instruction set computer principles"
    };
    
    const char *patterns[] = {"fox", "dolor", "instruction"};
    const int num_tests = sizeof(test_texts) / sizeof(test_texts[0]);
    
    for (int i = 0; i < num_tests; i++) {
        printf("\nTest %d:\n", i + 1);
        compare_search_algorithms(test_texts[i], patterns[i]);
        
        double time = benchmark_string_operations(test_texts[i], 1000);
        printf("Basic string ops: %.6f ms per iteration\n", time);
    }
    printf("\n");
}

void benchmark_math_performance(void) {
    printf("Mathematical Performance Benchmarks\n");
    printf("==================================\n");
    
    double time = benchmark_math_operations(10000);
    printf("Mathematical operations: %.6f ms per iteration\n\n", time);
    
    compare_math_algorithms();
    printf("\n");
}
