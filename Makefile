# Compiler Configuration
CC_X86 = gcc
CC_RISCV = riscv64-linux-gnu-gcc

# Compiler Flags
CFLAGS_COMMON = -Wall -Wextra -std=c99
CFLAGS_DEBUG = -g -O0 -DDEBUG
CFLAGS_RELEASE = -O3 -DNDEBUG -march=native
CFLAGS_RISCV = -O3 -DNDEBUG -march=rv64gc

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BUILD_X86_DIR = $(BUILD_DIR)/x86
BUILD_RISCV_DIR = $(BUILD_DIR)/riscv
BENCHMARK_DIR = benchmarks

# Source Files
MATRIX_SOURCES = $(SRC_DIR)/matrix/matrix_ops.c $(SRC_DIR)/matrix/matrix_multiply.c
STRING_SOURCES = $(SRC_DIR)/string/string_ops.c $(SRC_DIR)/string/string_search.c
MATH_SOURCES = $(SRC_DIR)/math/math_ops.c $(SRC_DIR)/math/complex_math.c
MAIN_SOURCE = $(SRC_DIR)/main.c

ALL_SOURCES = $(MATRIX_SOURCES) $(STRING_SOURCES) $(MATH_SOURCES) $(MAIN_SOURCE)

# Target Executables
TARGET_X86 = $(BUILD_X86_DIR)/riscv_optimizer
TARGET_RISCV = $(BUILD_RISCV_DIR)/riscv_optimizer

# Include Paths
INCLUDES = -I$(INCLUDE_DIR)

# Default target
.PHONY: all clean test benchmark profile help

all: x86 riscv

# Create build directories
$(BUILD_X86_DIR):
	mkdir -p $(BUILD_X86_DIR)

$(BUILD_RISCV_DIR):
	mkdir -p $(BUILD_RISCV_DIR)

# x86-64 build
x86: $(BUILD_X86_DIR) $(TARGET_X86)

$(TARGET_X86): $(ALL_SOURCES)
	$(CC_X86) $(CFLAGS_COMMON) $(CFLAGS_RELEASE) $(INCLUDES) -o $@ $^

# RISC-V build
riscv: $(BUILD_RISCV_DIR) $(TARGET_RISCV)

$(TARGET_RISCV): $(ALL_SOURCES)
	$(CC_RISCV) $(CFLAGS_COMMON) $(CFLAGS_RISCV) $(INCLUDES) -o $@ $^

# Debug builds
debug-x86: $(BUILD_X86_DIR)
	$(CC_X86) $(CFLAGS_COMMON) $(CFLAGS_DEBUG) $(INCLUDES) -o $(BUILD_X86_DIR)/riscv_optimizer_debug $(ALL_SOURCES)

debug-riscv: $(BUILD_RISCV_DIR)
	$(CC_RISCV) $(CFLAGS_COMMON) $(CFLAGS_DEBUG) $(INCLUDES) -o $(BUILD_RISCV_DIR)/riscv_optimizer_debug $(ALL_SOURCES)

# Performance profiling build (x86 only)
profile: $(BUILD_X86_DIR)
	$(CC_X86) $(CFLAGS_COMMON) $(CFLAGS_RELEASE) -pg $(INCLUDES) -o $(BUILD_X86_DIR)/riscv_optimizer_profile $(ALL_SOURCES)

# Run benchmarks
benchmark: x86
	@echo "Running performance benchmarks..."
	./$(TARGET_X86) --benchmark

# Run with profiling
run-profile: profile
	./$(BUILD_X86_DIR)/riscv_optimizer_profile --benchmark
	gprof $(BUILD_X86_DIR)/riscv_optimizer_profile gmon.out > profile_report.txt
	@echo "Profile report generated: profile_report.txt"

# Test targets
test: x86
	@echo "Running basic functionality tests..."
	./$(TARGET_X86) --test

# Cross-compilation verification
verify-riscv: riscv
	@echo "Verifying RISC-V binary..."
	file $(TARGET_RISCV)
	readelf -h $(TARGET_RISCV) | grep Machine

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	rm -f gmon.out profile_report.txt *.gcda *.gcno

# Install RISC-V toolchain (Ubuntu/Debian)
install-toolchain:
	@echo "Installing RISC-V toolchain..."
	sudo apt-get update
	sudo apt-get install gcc-riscv64-linux-gnu qemu-user-static

# Performance comparison
compare: x86 riscv
	@echo "Performance Comparison Report" > performance_comparison.txt
	@echo "=============================" >> performance_comparison.txt
	@echo "" >> performance_comparison.txt
	@echo "x86-64 Performance:" >> performance_comparison.txt
	./$(TARGET_X86) --benchmark >> performance_comparison.txt 2>&1
	@echo "" >> performance_comparison.txt
	@echo "RISC-V Performance (emulated):" >> performance_comparison.txt
	qemu-riscv64-static $(TARGET_RISCV) --benchmark >> performance_comparison.txt 2>&1
	@echo "Comparison report generated: performance_comparison.txt"

# Help target
help:
	@echo "Available targets:"
	@echo "  all              - Build both x86 and RISC-V versions"
	@echo "  x86              - Build x86-64 version"
	@echo "  riscv            - Build RISC-V version"
	@echo "  debug-x86        - Build x86 debug version"
	@echo "  debug-riscv      - Build RISC-V debug version"
	@echo "  profile          - Build with profiling support"
	@echo "  test             - Run functionality tests"
	@echo "  benchmark        - Run performance benchmarks"
	@echo "  run-profile      - Run with gprof profiling"
	@echo "  verify-riscv     - Verify RISC-V binary properties"
	@echo "  compare          - Compare x86 vs RISC-V performance"
	@echo "  install-toolchain- Install RISC-V development tools"
	@echo "  clean            - Remove build artifacts"
	@echo "  help             - Show this help message"
