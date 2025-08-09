#ifndef STRING_OPS_H
#define STRING_OPS_H

#include <stddef.h>

/* String operation function prototypes */
int string_length(const char *str);
char* string_copy(const char *src);
char* string_concatenate(const char *str1, const char *str2);
int string_compare(const char *str1, const char *str2);
char* string_reverse(const char *str);
char* string_to_uppercase(const char *str);
char* string_to_lowercase(const char *str);

/* Advanced string operations */
int string_find(const char *haystack, const char *needle);
int string_find_optimized(const char *haystack, const char *needle);
int string_count_occurrences(const char *text, const char *pattern);
char** string_split(const char *str, char delimiter, int *count);
void string_array_free(char **array, int count);

/* Pattern matching algorithms */
int kmp_search(const char *text, const char *pattern);
int boyer_moore_search(const char *text, const char *pattern);
int rabin_karp_search(const char *text, const char *pattern);

/* Performance benchmarks */
double benchmark_string_operations(const char *text, int iterations);
void compare_search_algorithms(const char *text, const char *pattern);

#endif /* STRING_OPS_H */
