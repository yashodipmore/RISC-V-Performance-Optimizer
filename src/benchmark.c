#include "benchmark.h"
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Timer implementation */
void timer_start(Timer *timer) {
    if (timer) {
        gettimeofday(&timer->start, NULL);
    }
}

void timer_stop(Timer *timer) {
    if (timer) {
        gettimeofday(&timer->end, NULL);
    }
}

double timer_elapsed_ms(const Timer *timer) {
    if (!timer) return 0.0;
    
    long seconds = timer->end.tv_sec - timer->start.tv_sec;
    long microseconds = timer->end.tv_usec - timer->start.tv_usec;
    
    return seconds * 1000.0 + microseconds / 1000.0;
}

double timer_elapsed_us(const Timer *timer) {
    if (!timer) return 0.0;
    
    long seconds = timer->end.tv_sec - timer->start.tv_sec;
    long microseconds = timer->end.tv_usec - timer->start.tv_usec;
    
    return seconds * 1000000.0 + microseconds;
}

/* Performance counters (simplified implementation) */
void perf_counters_start(PerfCounters *counters) {
    if (counters) {
        memset(counters, 0, sizeof(PerfCounters));
        // In a real implementation, this would setup hardware performance counters
        // For now, we'll just reset the counters
    }
}

void perf_counters_stop(PerfCounters *counters) {
    if (counters) {
        // In a real implementation, this would read hardware performance counters
        // For demonstration, we'll use placeholder values
        counters->instructions = 1000000;
        counters->cycles = 1500000;
        counters->cache_misses = 10000;
        counters->branch_misses = 5000;
    }
}

void perf_counters_print(const PerfCounters *counters) {
    if (!counters) return;
    
    printf("Performance Counters:\n");
    printf("  Instructions:  %lld\n", counters->instructions);
    printf("  Cycles:        %lld\n", counters->cycles);
    printf("  Cache Misses:  %lld\n", counters->cache_misses);
    printf("  Branch Misses: %lld\n", counters->branch_misses);
    
    if (counters->cycles > 0) {
        printf("  IPC:           %.3f\n", (double)counters->instructions / counters->cycles);
    }
}

/* Benchmark suite */
void run_benchmark_suite(Benchmark *benchmarks, int count) {
    if (!benchmarks || count <= 0) return;
    
    printf("Running Benchmark Suite (%d benchmarks)\n", count);
    printf("========================================\n");
    
    for (int i = 0; i < count; i++) {
        printf("Running: %s... ", benchmarks[i].name);
        fflush(stdout);
        
        Timer timer;
        timer_start(&timer);
        perf_counters_start(&benchmarks[i].counters);
        
        if (benchmarks[i].benchmark_func) {
            benchmarks[i].benchmark_func();
        }
        
        perf_counters_stop(&benchmarks[i].counters);
        timer_stop(&timer);
        
        benchmarks[i].execution_time = timer_elapsed_ms(&timer);
        printf("%.3f ms\n", benchmarks[i].execution_time);
    }
    
    printf("\n");
}

void print_benchmark_results(const Benchmark *benchmarks, int count) {
    if (!benchmarks || count <= 0) return;
    
    printf("Benchmark Results Summary\n");
    printf("========================\n");
    printf("%-20s %10s %12s %12s %12s %12s\n", 
           "Benchmark", "Time(ms)", "Instructions", "Cycles", "Cache Miss", "Branch Miss");
    printf("%-20s %10s %12s %12s %12s %12s\n", 
           "--------", "--------", "------------", "------", "----------", "-----------");
    
    for (int i = 0; i < count; i++) {
        printf("%-20s %10.3f %12lld %12lld %12lld %12lld\n",
               benchmarks[i].name,
               benchmarks[i].execution_time,
               benchmarks[i].counters.instructions,
               benchmarks[i].counters.cycles,
               benchmarks[i].counters.cache_misses,
               benchmarks[i].counters.branch_misses);
    }
    printf("\n");
}

/* Memory usage tracking (simplified) */
void memory_stats_start(MemoryStats *stats) {
    if (stats) {
        stats->peak_memory_kb = 0;
        stats->current_memory_kb = 0;
    }
}

void memory_stats_update(MemoryStats *stats) {
    if (!stats) return;
    
    // In a real implementation, this would read from /proc/self/status
    // For demonstration, we'll use placeholder values
    stats->current_memory_kb = 1024; // 1 MB
    if (stats->current_memory_kb > stats->peak_memory_kb) {
        stats->peak_memory_kb = stats->current_memory_kb;
    }
}

void memory_stats_print(const MemoryStats *stats) {
    if (!stats) return;
    
    printf("Memory Statistics:\n");
    printf("  Current: %ld KB\n", stats->current_memory_kb);
    printf("  Peak:    %ld KB\n", stats->peak_memory_kb);
}

/* Cross-platform utilities */
double get_cpu_frequency_ghz(void) {
    // Simplified implementation - in reality, this would read from
    // /proc/cpuinfo or use hardware-specific methods
    return 2.4; // 2.4 GHz placeholder
}

int get_cpu_core_count(void) {
    long cores = sysconf(_SC_NPROCESSORS_ONLN);
    return (cores > 0) ? (int)cores : 1;
}

const char* get_cpu_architecture(void) {
#ifdef __x86_64__
    return "x86_64";
#elif defined(__i386__)
    return "i386";
#elif defined(__riscv)
    #if __riscv_xlen == 64
        return "riscv64";
    #else
        return "riscv32";
    #endif
#elif defined(__aarch64__)
    return "aarch64";
#elif defined(__arm__)
    return "arm";
#else
    return "unknown";
#endif
}
