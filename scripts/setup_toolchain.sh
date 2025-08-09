#!/bin/bash

# RISC-V Development Environment Setup Script
# This script sets up the complete RISC-V development toolchain

set -e

echo "=========================================="
echo "RISC-V Development Environment Setup"
echo "=========================================="

# Configuration
RISCV_PREFIX="/opt/riscv"
TOOLCHAIN_VERSION="2023.04.29"
BUILD_DIR="/tmp/riscv-build"
PARALLEL_JOBS=$(nproc)

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running as root for system installation
check_permissions() {
    if [[ $EUID -ne 0 ]]; then
        log_warn "Not running as root. Some installations may require sudo."
        SUDO="sudo"
    else
        SUDO=""
    fi
}

# Install system dependencies
install_dependencies() {
    log_info "Installing system dependencies..."
    
    if command -v apt-get &> /dev/null; then
        # Ubuntu/Debian
        $SUDO apt-get update
        $SUDO apt-get install -y \
            autoconf automake autotools-dev curl python3 libmpc-dev \
            libmpfr-dev libgmp-dev gawk build-essential bison flex \
            texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev \
            git cmake ninja-build
            
    elif command -v yum &> /dev/null; then
        # CentOS/RHEL/Fedora
        $SUDO yum groupinstall -y "Development Tools"
        $SUDO yum install -y \
            autoconf automake curl python3 libmpc-devel mpfr-devel \
            gmp-devel gawk bison flex texinfo patchutils zlib-devel \
            expat-devel git cmake ninja-build
            
    elif command -v pacman &> /dev/null; then
        # Arch Linux
        $SUDO pacman -S --needed --noconfirm \
            autoconf automake curl python libmpc mpfr gmp gawk \
            base-devel bison flex texinfo bc zlib expat git cmake ninja
            
    else
        log_error "Unsupported package manager. Please install dependencies manually."
        exit 1
    fi
    
    log_info "System dependencies installed successfully."
}

# Download RISC-V GNU toolchain
download_toolchain() {
    log_info "Downloading RISC-V GNU toolchain..."
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    if [ ! -d "riscv-gnu-toolchain" ]; then
        git clone --recursive https://github.com/riscv/riscv-gnu-toolchain
    fi
    
    cd riscv-gnu-toolchain
    git checkout "$TOOLCHAIN_VERSION" 2>/dev/null || log_warn "Could not checkout specific version, using latest"
    git submodule update --init --recursive
    
    log_info "Toolchain source downloaded."
}

# Build and install toolchain
build_toolchain() {
    log_info "Building RISC-V GNU toolchain (this may take 30-60 minutes)..."
    
    cd "$BUILD_DIR/riscv-gnu-toolchain"
    
    # Create installation directory
    $SUDO mkdir -p "$RISCV_PREFIX"
    $SUDO chown $(whoami):$(whoami) "$RISCV_PREFIX" 2>/dev/null || true
    
    # Configure for 64-bit Linux targets
    ./configure \
        --prefix="$RISCV_PREFIX" \
        --enable-multilib \
        --with-cmodel=medany
    
    # Build toolchain
    make -j"$PARALLEL_JOBS" linux
    
    log_info "RISC-V GNU toolchain built and installed to $RISCV_PREFIX"
}

# Install pre-built toolchain (faster alternative)
install_prebuilt_toolchain() {
    log_info "Installing pre-built RISC-V toolchain..."
    
    if command -v apt-get &> /dev/null; then
        $SUDO apt-get install -y gcc-riscv64-linux-gnu
        log_info "Pre-built toolchain installed via package manager."
        return 0
    fi
    
    # For other systems, download pre-built binaries
    ARCH=$(uname -m)
    if [ "$ARCH" = "x86_64" ]; then
        TOOLCHAIN_URL="https://github.com/riscv-collab/riscv-gnu-toolchain/releases/download/2023.04.29/riscv64-linux-ubuntu-20.04-gcc-nightly-2023.04.29-nightly.tar.gz"
    else
        log_error "Pre-built toolchain not available for architecture: $ARCH"
        return 1
    fi
    
    cd /tmp
    wget -O riscv-toolchain.tar.gz "$TOOLCHAIN_URL"
    $SUDO tar -xzf riscv-toolchain.tar.gz -C /opt/
    
    log_info "Pre-built toolchain installed."
}

# Setup QEMU for RISC-V emulation
setup_qemu() {
    log_info "Setting up QEMU for RISC-V emulation..."
    
    if command -v apt-get &> /dev/null; then
        $SUDO apt-get install -y qemu-user-static qemu-system-riscv64
    elif command -v yum &> /dev/null; then
        $SUDO yum install -y qemu-user-static qemu-system-riscv64
    elif command -v pacman &> /dev/null; then
        $SUDO pacman -S --needed --noconfirm qemu-user-static qemu-arch-extra
    fi
    
    # Test QEMU installation
    if qemu-riscv64-static --version &> /dev/null; then
        log_info "QEMU RISC-V emulation ready."
    else
        log_error "QEMU installation failed."
        return 1
    fi
}

# Setup environment variables
setup_environment() {
    log_info "Setting up environment variables..."
    
    ENV_FILE="$HOME/.riscv_env"
    
    cat > "$ENV_FILE" << EOF
# RISC-V Development Environment
export RISCV="$RISCV_PREFIX"
export PATH="\$RISCV/bin:\$PATH"

# Convenient aliases
alias riscv-gcc='riscv64-unknown-linux-gnu-gcc'
alias riscv-g++='riscv64-unknown-linux-gnu-g++'
alias riscv-gdb='riscv64-unknown-linux-gnu-gdb'
alias riscv-objdump='riscv64-unknown-linux-gnu-objdump'
alias riscv-readelf='riscv64-unknown-linux-gnu-readelf'

# QEMU aliases
alias qemu-riscv='qemu-riscv64-static'
alias qemu-riscv-system='qemu-system-riscv64'
EOF
    
    # Add to shell profile
    if [ -f "$HOME/.bashrc" ]; then
        echo "source $ENV_FILE" >> "$HOME/.bashrc"
    fi
    
    if [ -f "$HOME/.zshrc" ]; then
        echo "source $ENV_FILE" >> "$HOME/.zshrc"
    fi
    
    log_info "Environment setup complete. Run 'source ~/.bashrc' or start a new terminal."
}

# Install development tools
install_dev_tools() {
    log_info "Installing additional development tools..."
    
    # Spike RISC-V ISA Simulator
    cd "$BUILD_DIR"
    if [ ! -d "riscv-isa-sim" ]; then
        git clone https://github.com/riscv/riscv-isa-sim.git
    fi
    
    cd riscv-isa-sim
    mkdir -p build
    cd build
    ../configure --prefix="$RISCV_PREFIX"
    make -j"$PARALLEL_JOBS"
    make install
    
    # RISC-V Proxy Kernel
    cd "$BUILD_DIR"
    if [ ! -d "riscv-pk" ]; then
        git clone https://github.com/riscv/riscv-pk.git
    fi
    
    cd riscv-pk
    mkdir -p build
    cd build
    ../configure --prefix="$RISCV_PREFIX" --host=riscv64-unknown-elf
    make -j"$PARALLEL_JOBS"
    make install
    
    log_info "Development tools installed."
}

# Verify installation
verify_installation() {
    log_info "Verifying installation..."
    
    # Source environment
    source "$HOME/.riscv_env" 2>/dev/null || true
    
    # Test toolchain
    if command -v riscv64-unknown-linux-gnu-gcc &> /dev/null; then
        VERSION=$(riscv64-unknown-linux-gnu-gcc --version | head -1)
        log_info "Toolchain installed: $VERSION"
    elif command -v riscv64-linux-gnu-gcc &> /dev/null; then
        VERSION=$(riscv64-linux-gnu-gcc --version | head -1)
        log_info "Toolchain installed: $VERSION"
    else
        log_error "RISC-V GCC not found in PATH"
        return 1
    fi
    
    # Test QEMU
    if command -v qemu-riscv64-static &> /dev/null; then
        VERSION=$(qemu-riscv64-static --version | head -1)
        log_info "QEMU installed: $VERSION"
    else
        log_error "QEMU RISC-V not found"
        return 1
    fi
    
    # Create test program
    TEST_DIR="/tmp/riscv-test"
    mkdir -p "$TEST_DIR"
    
    cat > "$TEST_DIR/hello.c" << 'EOF'
#include <stdio.h>

int main() {
    printf("Hello from RISC-V!\n");
    printf("Architecture: %s\n", 
#ifdef __riscv
           "RISC-V"
#else
           "Unknown"
#endif
    );
    return 0;
}
EOF
    
    cd "$TEST_DIR"
    
    # Try compilation
    if command -v riscv64-unknown-linux-gnu-gcc &> /dev/null; then
        RISCV_GCC="riscv64-unknown-linux-gnu-gcc"
    else
        RISCV_GCC="riscv64-linux-gnu-gcc"
    fi
    
    if $RISCV_GCC -o hello hello.c; then
        log_info "Test compilation successful."
        
        # Try execution
        if qemu-riscv64-static ./hello; then
            log_info "Test execution successful!"
        else
            log_warn "Compilation works but execution failed."
        fi
    else
        log_error "Test compilation failed."
        return 1
    fi
    
    rm -rf "$TEST_DIR"
    
    log_info "Installation verification complete!"
}

# Main installation function
main() {
    echo "This script will install the RISC-V development environment."
    echo "Installation prefix: $RISCV_PREFIX"
    echo "Build directory: $BUILD_DIR"
    echo ""
    
    read -p "Choose installation method: (1) Build from source, (2) Pre-built packages [1/2]: " choice
    
    check_permissions
    install_dependencies
    setup_qemu
    
    case $choice in
        1)
            download_toolchain
            build_toolchain
            install_dev_tools
            ;;
        2)
            if ! install_prebuilt_toolchain; then
                log_warn "Pre-built installation failed, falling back to source build..."
                download_toolchain
                build_toolchain
            fi
            ;;
        *)
            log_error "Invalid choice"
            exit 1
            ;;
    esac
    
    setup_environment
    verify_installation
    
    log_info "RISC-V development environment setup complete!"
    log_info "To use the tools, run: source ~/.bashrc"
    
    # Cleanup
    read -p "Remove build directory $BUILD_DIR? [y/N]: " cleanup
    if [[ $cleanup =~ ^[Yy]$ ]]; then
        rm -rf "$BUILD_DIR"
        log_info "Build directory cleaned up."
    fi
}

# Run main function
main "$@"
