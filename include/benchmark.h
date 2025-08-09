#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <time.h>
#include <sys/time.h>

/* Timing utilities */
typedef struct {
    struct timeval start;
    struct timeval end;
} Timer;

void timer_start(Timer *timer);
void timer_stop(Timer *timer);
double timer_elapsed_ms(const Timer *timer);
double timer_elapsed_us(const Timer *timer);

/* CPU performance counters (Linux-specific) */
typedef struct {
    long long instructions;
    long long cycles;
    long long cache_misses;
    long long branch_misses;
} PerfCounters;

void perf_counters_start(PerfCounters *counters);
void perf_counters_stop(PerfCounters *counters);
void perf_counters_print(const PerfCounters *counters);

/* Benchmark suite */
typedef struct {
    const char *name;
    void (*benchmark_func)(void);
    double execution_time;
    PerfCounters counters;
} Benchmark;

void run_benchmark_suite(Benchmark *benchmarks, int count);
void print_benchmark_results(const Benchmark *benchmarks, int count);

/* Memory usage tracking */
typedef struct {
    long peak_memory_kb;
    long current_memory_kb;
} MemoryStats;

void memory_stats_start(MemoryStats *stats);
void memory_stats_update(MemoryStats *stats);
void memory_stats_print(const MemoryStats *stats);

/* Cross-platform performance utilities */
double get_cpu_frequency_ghz(void);
int get_cpu_core_count(void);
const char* get_cpu_architecture(void);

#endif /* BENCHMARK_H */
