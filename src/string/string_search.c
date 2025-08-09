#include "string_ops.h"
#include <stdlib.h>

/* Basic string search (naive algorithm) */
int string_find(const char *haystack, const char *needle) {
    if (!haystack || !needle) return -1;
    
    int haystack_len = string_length(haystack);
    int needle_len = string_length(needle);
    
    if (needle_len == 0) return 0;
    if (needle_len > haystack_len) return -1;
    
    for (int i = 0; i <= haystack_len - needle_len; i++) {
        int match = 1;
        for (int j = 0; j < needle_len; j++) {
            if (haystack[i + j] != needle[j]) {
                match = 0;
                break;
            }
        }
        if (match) return i;
    }
    
    return -1;
}

/* Optimized string search with early termination */
int string_find_optimized(const char *haystack, const char *needle) {
    if (!haystack || !needle) return -1;
    
    int haystack_len = string_length(haystack);
    int needle_len = string_length(needle);
    
    if (needle_len == 0) return 0;
    if (needle_len > haystack_len) return -1;
    
    // Quick check for first character
    char first_char = needle[0];
    
    for (int i = 0; i <= haystack_len - needle_len; i++) {
        // Skip positions where first character doesn't match
        if (haystack[i] != first_char) continue;
        
        // Check remaining characters
        int match = 1;
        for (int j = 1; j < needle_len; j++) {
            if (haystack[i + j] != needle[j]) {
                match = 0;
                break;
            }
        }
        if (match) return i;
    }
    
    return -1;
}

/* KMP (Knuth-Morris-Pratt) string search algorithm */
static void compute_lps_array(const char *pattern, int *lps, int pattern_len) {
    int len = 0;
    lps[0] = 0;
    int i = 1;
    
    while (i < pattern_len) {
        if (pattern[i] == pattern[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

int kmp_search(const char *text, const char *pattern) {
    if (!text || !pattern) return -1;
    
    int text_len = string_length(text);
    int pattern_len = string_length(pattern);
    
    if (pattern_len == 0) return 0;
    if (pattern_len > text_len) return -1;
    
    // Create LPS array
    int *lps = malloc(pattern_len * sizeof(int));
    if (!lps) return -1;
    
    compute_lps_array(pattern, lps, pattern_len);
    
    int i = 0; // text index
    int j = 0; // pattern index
    
    while (i < text_len) {
        if (pattern[j] == text[i]) {
            j++;
            i++;
        }
        
        if (j == pattern_len) {
            free(lps);
            return i - j;
        } else if (i < text_len && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }
    
    free(lps);
    return -1;
}

/* Boyer-Moore string search algorithm (simplified) */
int boyer_moore_search(const char *text, const char *pattern) {
    if (!text || !pattern) return -1;
    
    int text_len = string_length(text);
    int pattern_len = string_length(pattern);
    
    if (pattern_len == 0) return 0;
    if (pattern_len > text_len) return -1;
    
    // Create bad character table
    int bad_char[256];
    for (int i = 0; i < 256; i++) {
        bad_char[i] = -1;
    }
    
    for (int i = 0; i < pattern_len; i++) {
        bad_char[(unsigned char)pattern[i]] = i;
    }
    
    int shift = 0;
    while (shift <= text_len - pattern_len) {
        int j = pattern_len - 1;
        
        while (j >= 0 && pattern[j] == text[shift + j]) {
            j--;
        }
        
        if (j < 0) {
            return shift;
        } else {
            shift += (j - bad_char[(unsigned char)text[shift + j]] > 1) ? 
                     j - bad_char[(unsigned char)text[shift + j]] : 1;
        }
    }
    
    return -1;
}

/* Rabin-Karp string search algorithm */
#define PRIME 101

int rabin_karp_search(const char *text, const char *pattern) {
    if (!text || !pattern) return -1;
    
    int text_len = string_length(text);
    int pattern_len = string_length(pattern);
    
    if (pattern_len == 0) return 0;
    if (pattern_len > text_len) return -1;
    
    int hash_pattern = 0;
    int hash_text = 0;
    int h = 1;
    
    // Calculate h = pow(256, pattern_len-1) % PRIME
    for (int i = 0; i < pattern_len - 1; i++) {
        h = (h * 256) % PRIME;
    }
    
    // Calculate hash value of pattern and first window of text
    for (int i = 0; i < pattern_len; i++) {
        hash_pattern = (256 * hash_pattern + pattern[i]) % PRIME;
        hash_text = (256 * hash_text + text[i]) % PRIME;
    }
    
    // Slide the pattern over text one by one
    for (int i = 0; i <= text_len - pattern_len; i++) {
        // Check if hash values match
        if (hash_pattern == hash_text) {
            // Check characters one by one
            int match = 1;
            for (int j = 0; j < pattern_len; j++) {
                if (text[i + j] != pattern[j]) {
                    match = 0;
                    break;
                }
            }
            if (match) return i;
        }
        
        // Calculate hash value for next window
        if (i < text_len - pattern_len) {
            hash_text = (256 * (hash_text - text[i] * h) + text[i + pattern_len]) % PRIME;
            
            // Handle negative hash values
            if (hash_text < 0) {
                hash_text += PRIME;
            }
        }
    }
    
    return -1;
}
