# Comprehensive Matrix Operations Benchmark
# This benchmark tests various matrix algorithms and optimizations

import numpy as np
import time
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
from typing import List, Tuple, Dict
import subprocess
import json
import os

class MatrixBenchmark:
    """Comprehensive matrix operations benchmark suite"""
    
    def __init__(self, sizes: List[int] = None):
        self.sizes = sizes or [64, 128, 256, 512, 1024]
        self.results = {}
        
    def benchmark_numpy_operations(self) -> Dict:
        """Benchmark NumPy matrix operations for comparison"""
        results = {}
        
        for size in self.sizes:
            print(f"Benchmarking NumPy operations for {size}x{size} matrices...")
            
            # Create random matrices
            A = np.random.random((size, size)).astype(np.float64)
            B = np.random.random((size, size)).astype(np.float64)
            
            # Matrix multiplication
            start_time = time.perf_counter()
            C = np.dot(A, B)
            numpy_time = time.perf_counter() - start_time
            
            # Matrix operations
            start_time = time.perf_counter()
            trace = np.trace(A)
            det = np.linalg.det(A)
            eigenvals = np.linalg.eigvals(A)
            ops_time = time.perf_counter() - start_time
            
            results[size] = {
                'multiply_time': numpy_time * 1000,  # Convert to ms
                'operations_time': ops_time * 1000,
                'gflops': (2 * size**3) / (numpy_time * 1e9)  # GFLOPS for matrix multiply
            }
            
        return results
    
    def benchmark_c_implementation(self, binary_path: str) -> Dict:
        """Benchmark C implementation via subprocess"""
        results = {}
        
        if not os.path.exists(binary_path):
            print(f"Binary not found: {binary_path}")
            return results
            
        for size in self.sizes:
            print(f"Benchmarking C implementation for {size}x{size} matrices...")
            
            try:
                # Run C benchmark with specific size
                cmd = [binary_path, '--matrix', '--size', str(size)]
                result = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
                
                if result.returncode == 0:
                    # Parse output for timing information
                    output_lines = result.stdout.split('\n')
                    timing_data = self._parse_c_output(output_lines)
                    results[size] = timing_data
                else:
                    print(f"C benchmark failed for size {size}: {result.stderr}")
                    
            except subprocess.TimeoutExpired:
                print(f"C benchmark timed out for size {size}")
            except Exception as e:
                print(f"Error running C benchmark: {e}")
                
        return results
    
    def _parse_c_output(self, lines: List[str]) -> Dict:
        """Parse C benchmark output to extract timing data"""
        data = {}
        
        for line in lines:
            if 'Naive Algorithm:' in line:
                try:
                    time_ms = float(line.split(':')[1].strip().split()[0])
                    data['naive_time'] = time_ms
                except:
                    pass
                    
            elif 'Optimized Algorithm:' in line:
                try:
                    parts = line.split(':')[1].strip().split()
                    time_ms = float(parts[0])
                    speedup = float(parts[1].replace('(', '').replace('x', ''))
                    data['optimized_time'] = time_ms
                    data['speedup'] = speedup
                except:
                    pass
                    
            elif 'RISC-V Optimized:' in line:
                try:
                    parts = line.split(':')[1].strip().split()
                    time_ms = float(parts[0])
                    speedup = float(parts[1].replace('(', '').replace('x', ''))
                    data['riscv_time'] = time_ms
                    data['riscv_speedup'] = speedup
                except:
                    pass
                    
        return data
    
    def generate_performance_plots(self, output_dir: str = './benchmark_plots'):
        """Generate comprehensive performance visualization"""
        os.makedirs(output_dir, exist_ok=True)
        
        # Set style
        plt.style.use('seaborn-v0_8')
        sns.set_palette("husl")
        
        # 1. Performance vs Matrix Size
        self._plot_performance_scaling(output_dir)
        
        # 2. Algorithm Comparison
        self._plot_algorithm_comparison(output_dir)
        
        # 3. GFLOPS Comparison
        self._plot_gflops_comparison(output_dir)
        
        # 4. Speedup Analysis
        self._plot_speedup_analysis(output_dir)
        
        print(f"Performance plots saved to: {output_dir}")
    
    def _plot_performance_scaling(self, output_dir: str):
        """Plot performance scaling with matrix size"""
        if 'numpy' not in self.results:
            return
            
        fig, ax = plt.subplots(figsize=(12, 8))
        
        sizes = list(self.results['numpy'].keys())
        numpy_times = [self.results['numpy'][s]['multiply_time'] for s in sizes]
        
        ax.loglog(sizes, numpy_times, 'o-', label='NumPy (BLAS)', linewidth=2, markersize=8)
        
        # Add C implementation if available
        if 'c_x86' in self.results:
            c_times = [self.results['c_x86'][s].get('optimized_time', 0) for s in sizes]
            if any(c_times):
                ax.loglog(sizes, c_times, 's-', label='C Optimized (x86)', linewidth=2, markersize=8)
        
        if 'c_riscv' in self.results:
            riscv_times = [self.results['c_riscv'][s].get('riscv_time', 0) for s in sizes]
            if any(riscv_times):
                ax.loglog(sizes, riscv_times, '^-', label='C Optimized (RISC-V)', linewidth=2, markersize=8)
        
        # Theoretical O(n³) scaling
        theoretical = [(s/sizes[0])**3 * numpy_times[0] for s in sizes]
        ax.loglog(sizes, theoretical, '--', alpha=0.7, label='O(n³) scaling')
        
        ax.set_xlabel('Matrix Size (N×N)', fontsize=12)
        ax.set_ylabel('Execution Time (ms)', fontsize=12)
        ax.set_title('Matrix Multiplication Performance Scaling', fontsize=14, fontweight='bold')
        ax.legend(fontsize=11)
        ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(f'{output_dir}/performance_scaling.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    def _plot_algorithm_comparison(self, output_dir: str):
        """Compare different algorithm implementations"""
        if 'c_x86' not in self.results:
            return
            
        fig, ax = plt.subplots(figsize=(12, 8))
        
        sizes = list(self.results['c_x86'].keys())
        algorithms = ['naive_time', 'optimized_time', 'riscv_time']
        algorithm_labels = ['Naive', 'Cache Optimized', 'RISC-V Optimized']
        
        x = np.arange(len(sizes))
        width = 0.25
        
        for i, (alg, label) in enumerate(zip(algorithms, algorithm_labels)):
            times = [self.results['c_x86'][s].get(alg, 0) for s in sizes]
            if any(times):
                ax.bar(x + i*width, times, width, label=label, alpha=0.8)
        
        ax.set_xlabel('Matrix Size (N×N)', fontsize=12)
        ax.set_ylabel('Execution Time (ms)', fontsize=12)
        ax.set_title('Algorithm Performance Comparison', fontsize=14, fontweight='bold')
        ax.set_xticks(x + width)
        ax.set_xticklabels([f'{s}×{s}' for s in sizes])
        ax.legend(fontsize=11)
        ax.set_yscale('log')
        ax.grid(True, alpha=0.3, axis='y')
        
        plt.tight_layout()
        plt.savefig(f'{output_dir}/algorithm_comparison.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    def _plot_gflops_comparison(self, output_dir: str):
        """Plot GFLOPS performance comparison"""
        if 'numpy' not in self.results:
            return
            
        fig, ax = plt.subplots(figsize=(12, 8))
        
        sizes = list(self.results['numpy'].keys())
        numpy_gflops = [self.results['numpy'][s]['gflops'] for s in sizes]
        
        ax.plot(sizes, numpy_gflops, 'o-', label='NumPy (BLAS)', linewidth=2, markersize=8)
        
        # Calculate GFLOPS for C implementations
        if 'c_x86' in self.results:
            c_gflops = []
            for s in sizes:
                time_ms = self.results['c_x86'][s].get('optimized_time', 0)
                if time_ms > 0:
                    gflops = (2 * s**3) / (time_ms * 1e6)  # Convert ms to seconds
                    c_gflops.append(gflops)
                else:
                    c_gflops.append(0)
            
            if any(c_gflops):
                ax.plot(sizes, c_gflops, 's-', label='C Optimized (x86)', linewidth=2, markersize=8)
        
        ax.set_xlabel('Matrix Size (N×N)', fontsize=12)
        ax.set_ylabel('Performance (GFLOPS)', fontsize=12)
        ax.set_title('Matrix Multiplication Performance (GFLOPS)', fontsize=14, fontweight='bold')
        ax.legend(fontsize=11)
        ax.grid(True, alpha=0.3)
        ax.set_xscale('log')
        
        plt.tight_layout()
        plt.savefig(f'{output_dir}/gflops_comparison.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    def _plot_speedup_analysis(self, output_dir: str):
        """Plot speedup analysis for different optimizations"""
        if 'c_x86' not in self.results:
            return
            
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))
        
        sizes = list(self.results['c_x86'].keys())
        
        # Speedup from cache optimization
        cache_speedups = [self.results['c_x86'][s].get('speedup', 1) for s in sizes]
        ax1.bar(range(len(sizes)), cache_speedups, alpha=0.7, color='skyblue')
        ax1.set_xlabel('Matrix Size', fontsize=12)
        ax1.set_ylabel('Speedup Factor', fontsize=12)
        ax1.set_title('Cache Optimization Speedup', fontsize=14, fontweight='bold')
        ax1.set_xticks(range(len(sizes)))
        ax1.set_xticklabels([f'{s}×{s}' for s in sizes])
        ax1.grid(True, alpha=0.3, axis='y')
        
        # RISC-V specific optimization speedup
        riscv_speedups = [self.results['c_x86'][s].get('riscv_speedup', 1) for s in sizes]
        ax2.bar(range(len(sizes)), riscv_speedups, alpha=0.7, color='lightcoral')
        ax2.set_xlabel('Matrix Size', fontsize=12)
        ax2.set_ylabel('Speedup Factor', fontsize=12)
        ax2.set_title('RISC-V Optimization Speedup', fontsize=14, fontweight='bold')
        ax2.set_xticks(range(len(sizes)))
        ax2.set_xticklabels([f'{s}×{s}' for s in sizes])
        ax2.grid(True, alpha=0.3, axis='y')
        
        plt.tight_layout()
        plt.savefig(f'{output_dir}/speedup_analysis.png', dpi=300, bbox_inches='tight')
        plt.close()
    
    def generate_report(self, output_file: str = './matrix_benchmark_report.json'):
        """Generate detailed benchmark report"""
        report = {
            'timestamp': time.strftime('%Y-%m-%d %H:%M:%S'),
            'matrix_sizes': self.sizes,
            'results': self.results,
            'summary': self._generate_summary()
        }
        
        with open(output_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Detailed report saved to: {output_file}")
        
    def _generate_summary(self) -> Dict:
        """Generate performance summary statistics"""
        summary = {}
        
        if 'numpy' in self.results:
            numpy_times = [self.results['numpy'][s]['multiply_time'] for s in self.sizes]
            summary['numpy'] = {
                'min_time_ms': min(numpy_times),
                'max_time_ms': max(numpy_times),
                'avg_gflops': np.mean([self.results['numpy'][s]['gflops'] for s in self.sizes])
            }
        
        if 'c_x86' in self.results:
            opt_times = [self.results['c_x86'][s].get('optimized_time', 0) for s in self.sizes]
            opt_times = [t for t in opt_times if t > 0]
            if opt_times:
                summary['c_optimized'] = {
                    'min_time_ms': min(opt_times),
                    'max_time_ms': max(opt_times),
                    'avg_speedup': np.mean([self.results['c_x86'][s].get('speedup', 1) for s in self.sizes])
                }
        
        return summary
    
    def run_comprehensive_benchmark(self, c_binary_x86: str = None, c_binary_riscv: str = None):
        """Run complete benchmark suite"""
        print("Starting comprehensive matrix benchmark...")
        
        # Benchmark NumPy
        print("\n1. Benchmarking NumPy operations...")
        self.results['numpy'] = self.benchmark_numpy_operations()
        
        # Benchmark C implementations
        if c_binary_x86 and os.path.exists(c_binary_x86):
            print("\n2. Benchmarking C implementation (x86)...")
            self.results['c_x86'] = self.benchmark_c_implementation(c_binary_x86)
        
        if c_binary_riscv and os.path.exists(c_binary_riscv):
            print("\n3. Benchmarking C implementation (RISC-V)...")
            self.results['c_riscv'] = self.benchmark_c_implementation(c_binary_riscv)
        
        # Generate visualizations and reports
        print("\n4. Generating performance plots...")
        self.generate_performance_plots()
        
        print("\n5. Generating detailed report...")
        self.generate_report()
        
        print("\nBenchmark complete!")
        self._print_summary()
    
    def _print_summary(self):
        """Print benchmark summary to console"""
        print("\n" + "="*60)
        print("MATRIX BENCHMARK SUMMARY")
        print("="*60)
        
        for impl, data in self.results.items():
            print(f"\n{impl.upper()} Results:")
            print("-" * 30)
            
            if impl == 'numpy':
                for size in self.sizes:
                    if size in data:
                        time_ms = data[size]['multiply_time']
                        gflops = data[size]['gflops']
                        print(f"  {size}×{size}: {time_ms:.2f} ms ({gflops:.2f} GFLOPS)")
            else:
                for size in self.sizes:
                    if size in data and data[size]:
                        opt_time = data[size].get('optimized_time', 0)
                        speedup = data[size].get('speedup', 1)
                        print(f"  {size}×{size}: {opt_time:.2f} ms ({speedup:.2f}x speedup)")

def main():
    """Main benchmark execution"""
    # Configuration
    matrix_sizes = [64, 128, 256, 512]  # Adjust based on available memory/time
    c_binary_x86 = "./build/x86/riscv_optimizer"
    c_binary_riscv = "./build/riscv/riscv_optimizer"
    
    # Create benchmark instance
    benchmark = MatrixBenchmark(sizes=matrix_sizes)
    
    # Run comprehensive benchmark
    benchmark.run_comprehensive_benchmark(c_binary_x86, c_binary_riscv)

if __name__ == "__main__":
    main()
