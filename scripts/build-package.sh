#!/bin/bash
# Build Python package for PyPI distribution

set -e

echo "=== Building Python Package for PyPI ==="
echo ""

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ]; then
    echo "Error: Please run this script from the project root directory"
    exit 1
fi

# Build C library first
echo "Building C library..."
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cd ..

# Install build tools
echo "Installing build tools..."
pip install build twine

# Build Python package
echo "Building Python package..."
cd python
python -m build

echo ""
echo "=== Package Built Successfully ==="
echo "Package files created in python/dist/"
echo ""
echo "To test the package locally:"
echo "  pip install python/dist/simplex_noise-*.whl"
echo ""
echo "To upload to PyPI:"
echo "  python -m twine upload python/dist/*"
