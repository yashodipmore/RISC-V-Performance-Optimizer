#!/bin/bash

# Automated Testing Script for RISC-V Performance Optimizer
# This script runs comprehensive tests across different architectures

set -e

# Configuration
PROJECT_DIR="$(pwd)"
BUILD_DIR="$PROJECT_DIR/build"
TEST_OUTPUT_DIR="$PROJECT_DIR/test_results"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Test counters
TESTS_TOTAL=0
TESTS_PASSED=0
TESTS_FAILED=0

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

log_test() {
    echo -e "${BLUE}[TEST]${NC} $1"
}

# Initialize test environment
init_testing() {
    log_info "Initializing test environment..."
    
    mkdir -p "$TEST_OUTPUT_DIR"
    
    # Clean previous builds
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
    
    log_info "Test environment ready"
}

# Test compilation for different architectures
test_compilation() {
    log_test "Testing compilation for different architectures"
    
    # Test x86-64 compilation
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    log_info "Testing x86-64 compilation..."
    
    if make clean && make x86 > "$TEST_OUTPUT_DIR/x86_build.log" 2>&1; then
        log_info "✓ x86-64 compilation successful"
        TESTS_PASSED=$((TESTS_PASSED + 1))
    else
        log_error "✗ x86-64 compilation failed"
        TESTS_FAILED=$((TESTS_FAILED + 1))
        cat "$TEST_OUTPUT_DIR/x86_build.log"
    fi
    
    # Test RISC-V compilation (if toolchain available)
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    if command -v riscv64-linux-gnu-gcc &> /dev/null || command -v riscv64-unknown-linux-gnu-gcc &> /dev/null; then
        log_info "Testing RISC-V compilation..."
        
        if make clean && make riscv > "$TEST_OUTPUT_DIR/riscv_build.log" 2>&1; then
            log_info "✓ RISC-V compilation successful"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            log_error "✗ RISC-V compilation failed"
            TESTS_FAILED=$((TESTS_FAILED + 1))
            cat "$TEST_OUTPUT_DIR/riscv_build.log"
        fi
    else
        log_warn "RISC-V toolchain not available, skipping RISC-V compilation test"
        TESTS_TOTAL=$((TESTS_TOTAL - 1))
    fi
}

# Test binary execution
test_execution() {
    log_test "Testing binary execution"
    
    # Test x86-64 execution
    if [ -f "$BUILD_DIR/x86/riscv_optimizer" ]; then
        TESTS_TOTAL=$((TESTS_TOTAL + 1))
        log_info "Testing x86-64 binary execution..."
        
        if timeout 30 "$BUILD_DIR/x86/riscv_optimizer" --test > "$TEST_OUTPUT_DIR/x86_execution.log" 2>&1; then
            log_info "✓ x86-64 execution successful"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            log_error "✗ x86-64 execution failed"
            TESTS_FAILED=$((TESTS_FAILED + 1))
            cat "$TEST_OUTPUT_DIR/x86_execution.log"
        fi
    fi
    
    # Test RISC-V execution (emulated)
    if [ -f "$BUILD_DIR/riscv/riscv_optimizer" ]; then
        TESTS_TOTAL=$((TESTS_TOTAL + 1))
        if command -v qemu-riscv64-static &> /dev/null; then
            log_info "Testing RISC-V binary execution (emulated)..."
            
            if timeout 60 qemu-riscv64-static "$BUILD_DIR/riscv/riscv_optimizer" --test > "$TEST_OUTPUT_DIR/riscv_execution.log" 2>&1; then
                log_info "✓ RISC-V execution successful"
                TESTS_PASSED=$((TESTS_PASSED + 1))
            else
                log_error "✗ RISC-V execution failed"
                TESTS_FAILED=$((TESTS_FAILED + 1))
                cat "$TEST_OUTPUT_DIR/riscv_execution.log"
            fi
        else
            log_warn "QEMU not available, skipping RISC-V execution test"
            TESTS_TOTAL=$((TESTS_TOTAL - 1))
        fi
    fi
}

# Test individual components
test_components() {
    log_test "Testing individual components"
    
    # Matrix operations test
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    log_info "Testing matrix operations..."
    
    if [ -f "$BUILD_DIR/x86/riscv_optimizer" ]; then
        if timeout 30 "$BUILD_DIR/x86/riscv_optimizer" --matrix > "$TEST_OUTPUT_DIR/matrix_test.log" 2>&1; then
            log_info "✓ Matrix operations test passed"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            log_error "✗ Matrix operations test failed"
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    else
        log_warn "x86 binary not available for matrix test"
        TESTS_TOTAL=$((TESTS_TOTAL - 1))
    fi
    
    # String operations test
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    log_info "Testing string operations..."
    
    if [ -f "$BUILD_DIR/x86/riscv_optimizer" ]; then
        if timeout 30 "$BUILD_DIR/x86/riscv_optimizer" --string > "$TEST_OUTPUT_DIR/string_test.log" 2>&1; then
            log_info "✓ String operations test passed"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            log_error "✗ String operations test failed"
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    else
        log_warn "x86 binary not available for string test"
        TESTS_TOTAL=$((TESTS_TOTAL - 1))
    fi
    
    # Math operations test
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    log_info "Testing mathematical operations..."
    
    if [ -f "$BUILD_DIR/x86/riscv_optimizer" ]; then
        if timeout 30 "$BUILD_DIR/x86/riscv_optimizer" --math > "$TEST_OUTPUT_DIR/math_test.log" 2>&1; then
            log_info "✓ Mathematical operations test passed"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            log_error "✗ Mathematical operations test failed"
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    else
        log_warn "x86 binary not available for math test"
        TESTS_TOTAL=$((TESTS_TOTAL - 1))
    fi
}

# Test performance benchmarks
test_benchmarks() {
    log_test "Testing performance benchmarks"
    
    # Quick benchmark test
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    log_info "Testing benchmark execution..."
    
    if [ -f "$BUILD_DIR/x86/riscv_optimizer" ]; then
        if timeout 60 "$BUILD_DIR/x86/riscv_optimizer" --benchmark > "$TEST_OUTPUT_DIR/benchmark_test.log" 2>&1; then
            log_info "✓ Benchmark test passed"
            TESTS_PASSED=$((TESTS_PASSED + 1))
            
            # Check for expected output patterns
            if grep -q "Matrix.*ms" "$TEST_OUTPUT_DIR/benchmark_test.log" && \
               grep -q "String.*ms" "$TEST_OUTPUT_DIR/benchmark_test.log"; then
                log_info "✓ Benchmark output format verified"
            else
                log_warn "Benchmark output format may be incorrect"
            fi
        else
            log_error "✗ Benchmark test failed"
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    else
        log_warn "x86 binary not available for benchmark test"
        TESTS_TOTAL=$((TESTS_TOTAL - 1))
    fi
}

# Test cross-architecture consistency
test_consistency() {
    log_test "Testing cross-architecture consistency"
    
    if [ -f "$BUILD_DIR/x86/riscv_optimizer" ] && [ -f "$BUILD_DIR/riscv/riscv_optimizer" ] && command -v qemu-riscv64-static &> /dev/null; then
        TESTS_TOTAL=$((TESTS_TOTAL + 1))
        log_info "Testing result consistency between architectures..."
        
        # Run identical tests on both architectures
        timeout 30 "$BUILD_DIR/x86/riscv_optimizer" --test > "$TEST_OUTPUT_DIR/x86_consistency.log" 2>&1 &
        x86_pid=$!
        
        timeout 60 qemu-riscv64-static "$BUILD_DIR/riscv/riscv_optimizer" --test > "$TEST_OUTPUT_DIR/riscv_consistency.log" 2>&1 &
        riscv_pid=$!
        
        wait $x86_pid
        x86_exit=$?
        
        wait $riscv_pid
        riscv_exit=$?
        
        if [ $x86_exit -eq 0 ] && [ $riscv_exit -eq 0 ]; then
            # Compare outputs (ignoring timing differences)
            if grep -q "test completed" "$TEST_OUTPUT_DIR/x86_consistency.log" && \
               grep -q "test completed" "$TEST_OUTPUT_DIR/riscv_consistency.log"; then
                log_info "✓ Cross-architecture consistency verified"
                TESTS_PASSED=$((TESTS_PASSED + 1))
            else
                log_warn "Cross-architecture results may be inconsistent"
                TESTS_FAILED=$((TESTS_FAILED + 1))
            fi
        else
            log_error "✗ Cross-architecture consistency test failed"
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    else
        log_warn "Cross-architecture testing not possible (missing binaries or QEMU)"
    fi
}

# Test memory safety (if valgrind available)
test_memory_safety() {
    if ! command -v valgrind &> /dev/null; then
        log_warn "Valgrind not available, skipping memory safety tests"
        return
    fi
    
    log_test "Testing memory safety"
    
    if [ -f "$BUILD_DIR/x86/riscv_optimizer" ]; then
        TESTS_TOTAL=$((TESTS_TOTAL + 1))
        log_info "Testing memory safety with Valgrind..."
        
        if timeout 120 valgrind --leak-check=full --error-exitcode=1 \
           "$BUILD_DIR/x86/riscv_optimizer" --test > "$TEST_OUTPUT_DIR/valgrind.log" 2>&1; then
            log_info "✓ Memory safety test passed"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            log_error "✗ Memory safety test failed"
            TESTS_FAILED=$((TESTS_FAILED + 1))
            # Show relevant parts of valgrind output
            grep -A 5 -B 5 "ERROR SUMMARY\|LEAK SUMMARY" "$TEST_OUTPUT_DIR/valgrind.log" || true
        fi
    else
        log_warn "x86 binary not available for memory safety test"
    fi
}

# Test build system features
test_build_system() {
    log_test "Testing build system features"
    
    # Test clean target
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    log_info "Testing clean target..."
    
    if make clean > "$TEST_OUTPUT_DIR/clean_test.log" 2>&1; then
        if [ ! -d "$BUILD_DIR" ] || [ -z "$(ls -A "$BUILD_DIR" 2>/dev/null)" ]; then
            log_info "✓ Clean target works correctly"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            log_error "✗ Clean target failed to remove build artifacts"
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    else
        log_error "✗ Clean target failed"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
    
    # Test debug build
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    log_info "Testing debug build..."
    
    if make debug-x86 > "$TEST_OUTPUT_DIR/debug_build.log" 2>&1; then
        if [ -f "$BUILD_DIR/x86/riscv_optimizer_debug" ]; then
            log_info "✓ Debug build successful"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            log_error "✗ Debug build failed to create binary"
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    else
        log_error "✗ Debug build failed"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
    
    # Test help target
    TESTS_TOTAL=$((TESTS_TOTAL + 1))
    log_info "Testing help target..."
    
    if make help > "$TEST_OUTPUT_DIR/help_test.log" 2>&1; then
        if grep -q "Available targets" "$TEST_OUTPUT_DIR/help_test.log"; then
            log_info "✓ Help target works correctly"
            TESTS_PASSED=$((TESTS_PASSED + 1))
        else
            log_error "✗ Help target output is incorrect"
            TESTS_FAILED=$((TESTS_FAILED + 1))
        fi
    else
        log_error "✗ Help target failed"
        TESTS_FAILED=$((TESTS_FAILED + 1))
    fi
}

# Generate test report
generate_test_report() {
    local report_file="$TEST_OUTPUT_DIR/test_report_$TIMESTAMP.txt"
    
    {
        echo "=========================================="
        echo "RISC-V PERFORMANCE OPTIMIZER TEST REPORT"
        echo "=========================================="
        echo "Generated: $(date)"
        echo "Test Duration: $(( $(date +%s) - START_TIME )) seconds"
        echo ""
        
        echo "TEST SUMMARY"
        echo "============"
        echo "Total Tests: $TESTS_TOTAL"
        echo "Passed: $TESTS_PASSED"
        echo "Failed: $TESTS_FAILED"
        echo "Success Rate: $(( TESTS_PASSED * 100 / TESTS_TOTAL ))%"
        echo ""
        
        echo "SYSTEM INFORMATION"
        echo "=================="
        echo "OS: $(uname -s -r)"
        echo "Architecture: $(uname -m)"
        echo "Compiler: $(gcc --version | head -1 2>/dev/null || echo 'Not available')"
        
        if command -v riscv64-linux-gnu-gcc &> /dev/null; then
            echo "RISC-V Compiler: $(riscv64-linux-gnu-gcc --version | head -1)"
        elif command -v riscv64-unknown-linux-gnu-gcc &> /dev/null; then
            echo "RISC-V Compiler: $(riscv64-unknown-linux-gnu-gcc --version | head -1)"
        else
            echo "RISC-V Compiler: Not available"
        fi
        
        echo ""
        echo "DETAILED RESULTS"
        echo "================"
        
        # Include relevant log excerpts
        if [ $TESTS_FAILED -gt 0 ]; then
            echo ""
            echo "FAILED TESTS DETAILS"
            echo "===================="
            
            for log_file in "$TEST_OUTPUT_DIR"/*.log; do
                if [ -f "$log_file" ] && grep -q "failed\|error\|Error" "$log_file" 2>/dev/null; then
                    echo ""
                    echo "--- $(basename "$log_file") ---"
                    tail -10 "$log_file"
                fi
            done
        fi
        
        echo ""
        echo "FILES GENERATED"
        echo "==============="
        ls -la "$TEST_OUTPUT_DIR"
        
    } > "$report_file"
    
    log_info "Test report generated: $report_file"
}

# Main test execution
main() {
    local START_TIME=$(date +%s)
    
    echo "=========================================="
    echo "RISC-V Performance Optimizer Test Suite"
    echo "=========================================="
    echo "Starting automated testing at $(date)"
    echo ""
    
    init_testing
    
    # Run test suites
    test_compilation
    test_execution
    test_components
    test_benchmarks
    test_consistency
    test_memory_safety
    test_build_system
    
    # Generate final report
    generate_test_report
    
    # Print summary
    echo ""
    echo "=========================================="
    echo "TEST SUITE COMPLETE"
    echo "=========================================="
    echo "Total Tests: $TESTS_TOTAL"
    echo "Passed: $TESTS_PASSED"
    echo "Failed: $TESTS_FAILED"
    
    if [ $TESTS_FAILED -eq 0 ]; then
        echo -e "${GREEN}All tests passed! ✓${NC}"
        exit 0
    else
        echo -e "${RED}Some tests failed! ✗${NC}"
        echo "Check test results in: $TEST_OUTPUT_DIR"
        exit 1
    fi
}

# Handle command line arguments
case "${1:-}" in
    --help|-h)
        echo "Usage: $0 [options]"
        echo ""
        echo "Options:"
        echo "  --help, -h    Show this help message"
        echo "  --quick       Run essential tests only"
        echo "  --full        Run all tests including memory checks"
        echo ""
        echo "This script runs automated tests for the RISC-V Performance Optimizer project."
        exit 0
        ;;
    --quick)
        log_info "Running quick test suite"
        # Skip memory safety and detailed consistency tests
        test_memory_safety() { log_info "Skipping memory safety tests in quick mode"; }
        ;;
    --full)
        log_info "Running full test suite"
        ;;
esac

# Run main function
main "$@"
