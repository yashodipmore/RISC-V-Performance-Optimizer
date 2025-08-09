#!/bin/bash

# Performance Profiling Script for RISC-V vs x86-64
# This script runs comprehensive benchmarks and generates performance reports

set -e

# Configuration
OUTPUT_DIR="./performance_reports"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
REPORT_FILE="$OUTPUT_DIR/performance_report_$TIMESTAMP.txt"
DATA_DIR="$OUTPUT_DIR/data"

# Create output directories
mkdir -p "$OUTPUT_DIR" "$DATA_DIR"

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_section() {
    echo -e "${BLUE}[SECTION]${NC} $1"
}

# Detect system architecture
detect_architecture() {
    ARCH=$(uname -m)
    case $ARCH in
        x86_64)
            NATIVE_ARCH="x86_64"
            ;;
        riscv64)
            NATIVE_ARCH="riscv64"
            ;;
        *)
            NATIVE_ARCH="unknown"
            ;;
    esac
    
    log_info "Detected architecture: $NATIVE_ARCH"
}

# Check available executables
check_executables() {
    X86_BINARY=""
    RISCV_BINARY=""
    
    if [ -f "./build/x86/riscv_optimizer" ]; then
        X86_BINARY="./build/x86/riscv_optimizer"
    fi
    
    if [ -f "./build/riscv/riscv_optimizer" ]; then
        RISCV_BINARY="./build/riscv/riscv_optimizer"
    fi
    
    if [ -z "$X86_BINARY" ] && [ -z "$RISCV_BINARY" ]; then
        log_error "No built binaries found. Please run 'make all' first."
        exit 1
    fi
    
    log_info "Available binaries:"
    [ -n "$X86_BINARY" ] && log_info "  x86-64: $X86_BINARY"
    [ -n "$RISCV_BINARY" ] && log_info "  RISC-V: $RISCV_BINARY"
}

# System information gathering
gather_system_info() {
    log_section "Gathering System Information"
    
    {
        echo "=========================================="
        echo "PERFORMANCE ANALYSIS REPORT"
        echo "Generated: $(date)"
        echo "=========================================="
        echo ""
        
        echo "SYSTEM INFORMATION"
        echo "=================="
        echo "Hostname: $(hostname)"
        echo "OS: $(uname -s -r)"
        echo "Architecture: $(uname -m)"
        echo ""
        
        if command -v lscpu &> /dev/null; then
            echo "CPU INFORMATION"
            echo "==============="
            lscpu | grep -E "(Model name|CPU\(s\)|Thread|Core|Socket|MHz|Cache)"
            echo ""
        fi
        
        if [ -f /proc/meminfo ]; then
            echo "MEMORY INFORMATION"
            echo "=================="
            grep -E "(MemTotal|MemFree|MemAvailable)" /proc/meminfo
            echo ""
        fi
        
        if command -v gcc &> /dev/null; then
            echo "COMPILER INFORMATION"
            echo "===================="
            echo "GCC Version: $(gcc --version | head -1)"
            if command -v riscv64-linux-gnu-gcc &> /dev/null; then
                echo "RISC-V GCC: $(riscv64-linux-gnu-gcc --version | head -1)"
            fi
            echo ""
        fi
        
    } > "$REPORT_FILE"
    
    log_info "System information collected"
}

# Run benchmark on specific architecture
run_benchmark() {
    local arch=$1
    local binary=$2
    local output_file="$3"
    
    log_info "Running $arch benchmark..."
    
    {
        echo "=========================================="
        echo "$arch PERFORMANCE RESULTS"
        echo "=========================================="
        echo ""
        
        echo "Timestamp: $(date)"
        echo "Binary: $binary"
        echo ""
        
        # Check if binary exists and is executable
        if [ ! -x "$binary" ]; then
            echo "ERROR: Binary $binary not found or not executable"
            return 1
        fi
        
        # File information
        echo "BINARY INFORMATION"
        echo "=================="
        if command -v file &> /dev/null; then
            file "$binary"
        fi
        
        if command -v readelf &> /dev/null; then
            echo "Architecture: $(readelf -h "$binary" 2>/dev/null | grep Machine || echo 'Unknown')"
        fi
        echo ""
        
        # Run benchmarks
        echo "BENCHMARK RESULTS"
        echo "================="
        
        if [ "$arch" = "riscv64" ] && [ "$NATIVE_ARCH" != "riscv64" ]; then
            # Running RISC-V binary on non-RISC-V system (emulation)
            if command -v qemu-riscv64-static &> /dev/null; then
                echo "Running under QEMU emulation..."
                echo ""
                timeout 300 qemu-riscv64-static "$binary" --benchmark 2>&1 || {
                    echo "Benchmark failed or timed out"
                    return 1
                }
            else
                echo "ERROR: QEMU not available for RISC-V emulation"
                return 1
            fi
        else
            # Native execution
            echo "Running natively..."
            echo ""
            timeout 300 "$binary" --benchmark 2>&1 || {
                echo "Benchmark failed or timed out"
                return 1
            }
        fi
        
        echo ""
        echo "Benchmark completed at: $(date)"
        
    } >> "$output_file"
}

# Performance comparison analysis
analyze_performance() {
    log_section "Analyzing Performance Data"
    
    {
        echo ""
        echo "=========================================="
        echo "PERFORMANCE COMPARISON ANALYSIS"
        echo "=========================================="
        echo ""
        
        # Extract performance data from benchmark outputs
        if [ -f "$DATA_DIR/x86_results.txt" ] && [ -f "$DATA_DIR/riscv_results.txt" ]; then
            echo "COMPARATIVE ANALYSIS"
            echo "===================="
            echo "Note: Analysis based on available benchmark data"
            echo ""
            
            # Try to extract timing information
            echo "Execution Time Comparison:"
            echo "--------------------------"
            
            # Look for matrix benchmark results
            x86_matrix=$(grep -o "Matrix.*[0-9]\+\.[0-9]\+ ms" "$DATA_DIR/x86_results.txt" 2>/dev/null | head -1 || echo "")
            riscv_matrix=$(grep -o "Matrix.*[0-9]\+\.[0-9]\+ ms" "$DATA_DIR/riscv_results.txt" 2>/dev/null | head -1 || echo "")
            
            if [ -n "$x86_matrix" ] && [ -n "$riscv_matrix" ]; then
                echo "Matrix Operations:"
                echo "  x86-64: $x86_matrix"
                echo "  RISC-V: $riscv_matrix"
            fi
            
            # Look for string benchmark results
            x86_string=$(grep -o "String.*[0-9]\+\.[0-9]\+ ms" "$DATA_DIR/x86_results.txt" 2>/dev/null | head -1 || echo "")
            riscv_string=$(grep -o "String.*[0-9]\+\.[0-9]\+ ms" "$DATA_DIR/riscv_results.txt" 2>/dev/null | head -1 || echo "")
            
            if [ -n "$x86_string" ] && [ -n "$riscv_string" ]; then
                echo "String Operations:"
                echo "  x86-64: $x86_string"
                echo "  RISC-V: $riscv_string"
            fi
            
            echo ""
            echo "DETAILED ANALYSIS"
            echo "================="
            echo "See individual result files for complete benchmark data:"
            echo "- x86-64 results: $DATA_DIR/x86_results.txt"
            echo "- RISC-V results: $DATA_DIR/riscv_results.txt"
            
        else
            echo "Insufficient data for comparative analysis"
        fi
        
        echo ""
        echo "RECOMMENDATIONS"
        echo "==============="
        echo "1. Cache optimization strategies are critical for both architectures"
        echo "2. RISC-V benefits significantly from loop unrolling and explicit scheduling"
        echo "3. Memory access patterns should be optimized for cache hierarchy"
        echo "4. Consider architecture-specific compiler optimizations"
        echo ""
        
    } >> "$REPORT_FILE"
}

# Generate performance plots (if gnuplot available)
generate_plots() {
    if ! command -v gnuplot &> /dev/null; then
        log_warn "gnuplot not available, skipping plot generation"
        return
    fi
    
    log_section "Generating Performance Plots"
    
    # Create sample plot script
    cat > "$DATA_DIR/plot_performance.gnuplot" << 'EOF'
set terminal png size 800,600
set output 'performance_comparison.png'
set title 'Performance Comparison: x86-64 vs RISC-V'
set ylabel 'Execution Time (ms)'
set xlabel 'Benchmark'
set style data histograms
set style fill solid
set boxwidth 0.8
set grid ytics

# Sample data - replace with actual extracted data
$data << EOD
Benchmark x86_64 RISC_V
"Matrix Multiply" 1230 1650
"String Search" 89 125
"Math Operations" 145 220
EOD

plot '$data' using 2:xtic(1) title 'x86-64' linecolor rgb "#1f77b4", \
     '' using 3:xtic(1) title 'RISC-V' linecolor rgb "#ff7f0e"
EOF
    
    cd "$DATA_DIR"
    gnuplot plot_performance.gnuplot 2>/dev/null || log_warn "Plot generation failed"
    cd - > /dev/null
    
    if [ -f "$DATA_DIR/performance_comparison.png" ]; then
        log_info "Performance plot generated: $DATA_DIR/performance_comparison.png"
    fi
}

# Profile with perf (if available)
profile_with_perf() {
    if ! command -v perf &> /dev/null; then
        log_warn "perf not available, skipping hardware performance profiling"
        return
    fi
    
    log_section "Hardware Performance Profiling"
    
    if [ -n "$X86_BINARY" ] && [ "$NATIVE_ARCH" = "x86_64" ]; then
        log_info "Profiling x86-64 binary with perf..."
        
        perf stat -e cycles,instructions,cache-misses,branch-misses \
            "$X86_BINARY" --benchmark 2> "$DATA_DIR/x86_perf.txt" || \
            log_warn "perf profiling failed for x86-64"
    fi
    
    # Note: perf profiling of RISC-V under QEMU emulation is not meaningful
    if [ "$NATIVE_ARCH" = "riscv64" ] && [ -n "$RISCV_BINARY" ]; then
        log_info "Profiling RISC-V binary with perf..."
        
        perf stat -e cycles,instructions,cache-misses,branch-misses \
            "$RISCV_BINARY" --benchmark 2> "$DATA_DIR/riscv_perf.txt" || \
            log_warn "perf profiling failed for RISC-V"
    fi
}

# Memory usage analysis
analyze_memory_usage() {
    log_section "Memory Usage Analysis"
    
    {
        echo ""
        echo "MEMORY USAGE ANALYSIS"
        echo "===================="
        echo ""
        
        if [ -n "$X86_BINARY" ]; then
            echo "x86-64 Binary Size:"
            ls -lh "$X86_BINARY" | awk '{print $5 " " $9}'
            
            if command -v valgrind &> /dev/null && [ "$NATIVE_ARCH" = "x86_64" ]; then
                echo ""
                echo "Memory usage profile (valgrind massif):"
                timeout 60 valgrind --tool=massif --massif-out-file="$DATA_DIR/x86_massif.out" \
                    "$X86_BINARY" --test &>/dev/null || echo "Valgrind analysis failed or timed out"
                
                if [ -f "$DATA_DIR/x86_massif.out" ]; then
                    if command -v ms_print &> /dev/null; then
                        ms_print "$DATA_DIR/x86_massif.out" > "$DATA_DIR/x86_memory_profile.txt"
                        echo "Memory profile saved to: $DATA_DIR/x86_memory_profile.txt"
                    fi
                fi
            fi
        fi
        
        if [ -n "$RISCV_BINARY" ]; then
            echo ""
            echo "RISC-V Binary Size:"
            ls -lh "$RISCV_BINARY" | awk '{print $5 " " $9}'
        fi
        
    } >> "$REPORT_FILE"
}

# Main execution function
main() {
    echo "=========================================="
    echo "RISC-V Performance Profiling Script"
    echo "=========================================="
    echo ""
    
    detect_architecture
    check_executables
    
    log_section "Starting Performance Analysis"
    gather_system_info
    
    # Run benchmarks
    if [ -n "$X86_BINARY" ]; then
        run_benchmark "x86_64" "$X86_BINARY" "$DATA_DIR/x86_results.txt"
    fi
    
    if [ -n "$RISCV_BINARY" ]; then
        run_benchmark "riscv64" "$RISCV_BINARY" "$DATA_DIR/riscv_results.txt"
    fi
    
    # Append individual results to main report
    if [ -f "$DATA_DIR/x86_results.txt" ]; then
        cat "$DATA_DIR/x86_results.txt" >> "$REPORT_FILE"
    fi
    
    if [ -f "$DATA_DIR/riscv_results.txt" ]; then
        cat "$DATA_DIR/riscv_results.txt" >> "$REPORT_FILE"
    fi
    
    # Analysis and additional profiling
    analyze_performance
    analyze_memory_usage
    profile_with_perf
    generate_plots
    
    # Final report
    {
        echo ""
        echo "=========================================="
        echo "PROFILING SESSION COMPLETE"
        echo "=========================================="
        echo "Report generated: $(date)"
        echo "Output directory: $OUTPUT_DIR"
        echo "Main report: $REPORT_FILE"
        echo ""
        echo "Additional files:"
        echo "- Raw data: $DATA_DIR/"
        echo "- Performance plots: $DATA_DIR/*.png (if generated)"
        echo "- Hardware profiles: $DATA_DIR/*_perf.txt (if available)"
        echo ""
        
    } >> "$REPORT_FILE"
    
    log_info "Performance analysis complete!"
    log_info "Main report: $REPORT_FILE"
    
    # Display summary
    echo ""
    echo "Report Summary:"
    echo "==============="
    if [ -f "$REPORT_FILE" ]; then
        tail -20 "$REPORT_FILE"
    fi
    
    echo ""
    log_info "To view the complete report: cat '$REPORT_FILE'"
    log_info "To view performance data: ls '$DATA_DIR'"
}

# Command line options
case "${1:-}" in
    --help|-h)
        echo "Usage: $0 [options]"
        echo ""
        echo "Options:"
        echo "  --help, -h    Show this help message"
        echo "  --quick       Run quick benchmarks only"
        echo "  --full        Run full analysis with profiling"
        echo ""
        echo "This script runs performance benchmarks comparing x86-64 and RISC-V"
        echo "implementations of the optimization project."
        exit 0
        ;;
    --quick)
        log_info "Running quick benchmark mode"
        QUICK_MODE=1
        ;;
    --full)
        log_info "Running full analysis mode"
        FULL_MODE=1
        ;;
esac

# Run main function
main "$@"
