#include "string_ops.h"
#include "benchmark.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

/* Calculate string length */
int string_length(const char *str) {
    if (!str) return 0;
    
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/* Copy string to new memory */
char* string_copy(const char *src) {
    if (!src) return NULL;
    
    int len = string_length(src);
    char *dest = malloc(len + 1);
    if (!dest) return NULL;
    
    for (int i = 0; i <= len; i++) {
        dest[i] = src[i];
    }
    
    return dest;
}

/* Concatenate two strings */
char* string_concatenate(const char *str1, const char *str2) {
    if (!str1 || !str2) return NULL;
    
    int len1 = string_length(str1);
    int len2 = string_length(str2);
    
    char *result = malloc(len1 + len2 + 1);
    if (!result) return NULL;
    
    int i = 0;
    for (int j = 0; j < len1; j++) {
        result[i++] = str1[j];
    }
    for (int j = 0; j < len2; j++) {
        result[i++] = str2[j];
    }
    result[i] = '\0';
    
    return result;
}

/* Compare two strings */
int string_compare(const char *str1, const char *str2) {
    if (!str1 || !str2) return -1;
    
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] < str2[i]) return -1;
        if (str1[i] > str2[i]) return 1;
        i++;
    }
    
    if (str1[i] == '\0' && str2[i] == '\0') return 0;
    return str1[i] == '\0' ? -1 : 1;
}

/* Reverse string */
char* string_reverse(const char *str) {
    if (!str) return NULL;
    
    int len = string_length(str);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    
    for (int i = 0; i < len; i++) {
        result[i] = str[len - 1 - i];
    }
    result[len] = '\0';
    
    return result;
}

/* Convert string to uppercase */
char* string_to_uppercase(const char *str) {
    if (!str) return NULL;
    
    int len = string_length(str);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    
    for (int i = 0; i <= len; i++) {
        result[i] = toupper(str[i]);
    }
    
    return result;
}

/* Convert string to lowercase */
char* string_to_lowercase(const char *str) {
    if (!str) return NULL;
    
    int len = string_length(str);
    char *result = malloc(len + 1);
    if (!result) return NULL;
    
    for (int i = 0; i <= len; i++) {
        result[i] = tolower(str[i]);
    }
    
    return result;
}

/* Count occurrences of pattern in text */
int string_count_occurrences(const char *text, const char *pattern) {
    if (!text || !pattern) return 0;
    
    int text_len = string_length(text);
    int pattern_len = string_length(pattern);
    int count = 0;
    
    if (pattern_len == 0) return 0;
    
    for (int i = 0; i <= text_len - pattern_len; i++) {
        int match = 1;
        for (int j = 0; j < pattern_len; j++) {
            if (text[i + j] != pattern[j]) {
                match = 0;
                break;
            }
        }
        if (match) count++;
    }
    
    return count;
}

/* Split string by delimiter */
char** string_split(const char *str, char delimiter, int *count) {
    if (!str || !count) return NULL;
    
    // Count delimiters
    int delim_count = 0;
    int len = string_length(str);
    for (int i = 0; i < len; i++) {
        if (str[i] == delimiter) delim_count++;
    }
    
    *count = delim_count + 1;
    char **result = malloc(*count * sizeof(char*));
    if (!result) return NULL;
    
    int start = 0;
    int part = 0;
    
    for (int i = 0; i <= len; i++) {
        if (str[i] == delimiter || str[i] == '\0') {
            int part_len = i - start;
            result[part] = malloc(part_len + 1);
            if (!result[part]) {
                // Cleanup on error
                for (int j = 0; j < part; j++) {
                    free(result[j]);
                }
                free(result);
                return NULL;
            }
            
            for (int j = 0; j < part_len; j++) {
                result[part][j] = str[start + j];
            }
            result[part][part_len] = '\0';
            
            part++;
            start = i + 1;
        }
    }
    
    return result;
}

/* Free string array */
void string_array_free(char **array, int count) {
    if (!array) return;
    
    for (int i = 0; i < count; i++) {
        free(array[i]);
    }
    free(array);
}

/* Benchmark string operations */
double benchmark_string_operations(const char *text, int iterations) {
    if (!text) return -1.0;
    
    Timer timer;
    timer_start(&timer);
    
    for (int i = 0; i < iterations; i++) {
        char *upper = string_to_uppercase(text);
        char *lower = string_to_lowercase(text);
        char *reversed = string_reverse(text);
        int len = string_length(text);
        
        free(upper);
        free(lower);
        free(reversed);
        (void)len; // Suppress unused variable warning
    }
    
    timer_stop(&timer);
    return timer_elapsed_ms(&timer) / iterations;
}

/* Compare different search algorithms */
void compare_search_algorithms(const char *text, const char *pattern) {
    printf("String Search Algorithm Comparison\n");
    printf("Text length: %d, Pattern: '%s'\n", string_length(text), pattern);
    printf("=====================================\n");
    
    Timer timer;
    int result;
    
    // Naive search
    timer_start(&timer);
    result = string_find(text, pattern);
    timer_stop(&timer);
    printf("Naive Search:      %.6f ms (found at %d)\n", timer_elapsed_ms(&timer), result);
    
    // Optimized search
    timer_start(&timer);
    result = string_find_optimized(text, pattern);
    timer_stop(&timer);
    printf("Optimized Search:  %.6f ms (found at %d)\n", timer_elapsed_ms(&timer), result);
    
    // KMP search
    timer_start(&timer);
    result = kmp_search(text, pattern);
    timer_stop(&timer);
    printf("KMP Search:        %.6f ms (found at %d)\n", timer_elapsed_ms(&timer), result);
    
    // Boyer-Moore search
    timer_start(&timer);
    result = boyer_moore_search(text, pattern);
    timer_stop(&timer);
    printf("Boyer-Moore:       %.6f ms (found at %d)\n", timer_elapsed_ms(&timer), result);
    
    // Rabin-Karp search
    timer_start(&timer);
    result = rabin_karp_search(text, pattern);
    timer_stop(&timer);
    printf("Rabin-Karp:        %.6f ms (found at %d)\n", timer_elapsed_ms(&timer), result);
}
