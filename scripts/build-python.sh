#!/bin/bash
# Build script for Python bindings

set -e

echo "=== Building Python Bindings for Simplex Noise ==="
echo ""

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: Please run this script from the project root directory"
    exit 1
fi

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DSIMPLEX_BUILD_PYTHON=ON

# Build the C library
echo "Building C library..."
make -j$(nproc)

# Build Python bindings
echo "Building Python bindings..."
make python_install

echo ""
echo "=== Python Bindings Built Successfully ==="
echo ""
echo "To test the Python bindings:"
echo "  cd build/python"
echo "  python examples/basic_usage.py"
echo ""
echo "To run tests:"
echo "  cd build/python"
echo "  python -m pytest tests/ -v"
echo ""
echo "To install system-wide:"
echo "  cd build/python"
echo "  pip install -e ."
