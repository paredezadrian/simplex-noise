# Build System Guide

This guide covers the build system configuration and compilation options for the Simplex Noise Library.

## Overview

The library uses CMake as its primary build system, providing cross-platform support and easy integration with other projects.

## Prerequisites

### Required Tools

- **CMake** 3.10 or later
- **C99 compatible compiler** (GCC, Clang, MSVC)
- **Make** or **Ninja** (for compilation)

### Optional Dependencies

- **libpng** (for PNG image generation)
- **pkg-config** (for library discovery)

## Basic Build

### Quick Start

```bash
# Clone the repository
git clone https://github.com/paredezadrian/simplex-noise.git
cd simplex-noise

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the library
make

# Install (optional)
sudo make install
```

### One-liner Build

```bash
mkdir build && cd build && cmake .. && make
```

## CMake Configuration

### Basic Configuration

```bash
# Debug build (default)
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build
cmake -DCMAKE_BUILD_TYPE=Release ..

# RelWithDebInfo build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
```

### Advanced Configuration

```bash
# Custom install prefix
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..

# Custom compiler
cmake -DCMAKE_C_COMPILER=gcc ..

# Enable/disable features
cmake -DENABLE_PNG=ON ..
cmake -DENABLE_TESTS=ON ..
cmake -DENABLE_EXAMPLES=ON ..
```

### Available CMake Options

| Option                 | Default    | Description                                             |
| ---------------------- | ---------- | ------------------------------------------------------- |
| `CMAKE_BUILD_TYPE`     | Debug      | Build type (Debug, Release, RelWithDebInfo, MinSizeRel) |
| `CMAKE_INSTALL_PREFIX` | /usr/local | Installation directory                                  |
| `ENABLE_PNG`           | ON         | Enable PNG image generation                             |
| `ENABLE_TESTS`         | ON         | Build test programs                                     |
| `ENABLE_EXAMPLES`      | ON         | Build example programs                                  |
| `ENABLE_DOCS`          | OFF        | Build documentation                                     |

## Build Types

### Debug Build

```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

**Characteristics:**

- Debug symbols included
- No optimization
- Assertions enabled
- Larger binary size
- Slower execution

### Release Build

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

**Characteristics:**

- Full optimization
- No debug symbols
- Assertions disabled
- Smaller binary size
- Faster execution

### RelWithDebInfo Build

```bash
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
make
```

**Characteristics:**

- Full optimization
- Debug symbols included
- Assertions disabled
- Medium binary size
- Fast execution with debugging

## Compiler-Specific Options

### GCC

```bash
# Use GCC with specific version
cmake -DCMAKE_C_COMPILER=gcc-9 ..

# Custom GCC flags
cmake -DCMAKE_C_FLAGS="-Wall -Wextra -Wpedantic" ..
```

### Clang

```bash
# Use Clang
cmake -DCMAKE_C_COMPILER=clang ..

# Custom Clang flags
cmake -DCMAKE_C_FLAGS="-Wall -Wextra -Wpedantic" ..
```

### MSVC (Windows)

```bash
# Use MSVC
cmake -DCMAKE_C_COMPILER=cl ..

# Custom MSVC flags
cmake -DCMAKE_C_FLAGS="/W4" ..
```

## Cross-Platform Building

### Linux

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake libpng-dev

# CentOS/RHEL
sudo yum install gcc cmake libpng-devel

# Build
mkdir build && cd build
cmake ..
make
```

### macOS

```bash
# Install dependencies with Homebrew
brew install cmake libpng

# Build
mkdir build && cd build
cmake ..
make
```

### Windows

```bash
# Using Visual Studio
mkdir build && cd build
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config Release

# Using MinGW
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
make
```

## Integration with Other Projects

### Using as Submodule

```bash
# Add as git submodule
git submodule add https://github.com/paredezadrian/simplex-noise.git external/simplex-noise

# In your CMakeLists.txt
add_subdirectory(external/simplex-noise)
target_link_libraries(your_target simplex_noise)
```

### Using with pkg-config

```bash
# Install library
sudo make install

# Use pkg-config
pkg-config --cflags --libs simplex_noise
```

### Manual Integration

```bash
# Copy headers and library
cp include/simplex_noise.h /usr/local/include/
cp include/simplex_image.h /usr/local/include/
cp build/libsimplex_noise.a /usr/local/lib/

# Link with your project
gcc -o my_program my_program.c -lsimplex_noise -lm
```

## Custom Build Scripts

### Build Script for Development

```bash
#!/bin/bash
# build-dev.sh

set -e

# Create build directory
mkdir -p build
cd build

# Configure for development
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DENABLE_TESTS=ON \
      -DENABLE_EXAMPLES=ON \
      -DENABLE_DOCS=ON \
      ..

# Build
make -j$(nproc)

# Run tests
make test

echo "Build completed successfully!"
```

### Build Script for Release

```bash
#!/bin/bash
# build-release.sh

set -e

# Create build directory
mkdir -p build
cd build

# Configure for release
cmake -DCMAKE_BUILD_TYPE=Release \
      -DENABLE_PNG=ON \
      -DENABLE_TESTS=OFF \
      -DENABLE_EXAMPLES=OFF \
      ..

# Build
make -j$(nproc)

# Install
sudo make install

echo "Release build completed successfully!"
```

## Testing

### Running Tests

```bash
# Build with tests enabled
cmake -DENABLE_TESTS=ON ..
make

# Run tests
make test

# Run specific test
./tests/test_simplex_noise
```

### Test Coverage

```bash
# Build with coverage
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DENABLE_COVERAGE=ON \
      ..
make

# Generate coverage report
make coverage
```

## Documentation

### Building Documentation

```bash
# Build with documentation
cmake -DENABLE_DOCS=ON ..
make docs

# View documentation
open docs/html/index.html
```

### Documentation Requirements

- **Doxygen** (for API documentation)
- **Sphinx** (for user guides)
- **Graphviz** (for diagrams)

## Packaging

### Creating Packages

```bash
# Create source package
make package_source

# Create binary package
make package

# Create RPM package
cpack -G RPM

# Create DEB package
cpack -G DEB
```

### Installation

```bash
# Install from package
sudo dpkg -i simplex-noise-1.0.0.deb

# Install from source
sudo make install

# Uninstall
sudo make uninstall
```

## Troubleshooting

### Common Issues

1. **CMake not found**: Install CMake 3.10 or later
2. **Compiler not found**: Install build-essential or development tools
3. **Library not found**: Install required dependencies (libpng, etc.)
4. **Permission denied**: Use sudo for installation

### Debug Build Issues

```bash
# Enable verbose output
make VERBOSE=1

# Check CMake configuration
cmake -LAH ..

# Check compiler flags
make -n
```

### Performance Issues

```bash
# Use parallel compilation
make -j$(nproc)

# Use Ninja for faster builds
cmake -GNinja ..
ninja
```

## Continuous Integration

### GitHub Actions

```yaml
name: Build and Test

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y build-essential cmake libpng-dev
      - name: Build
        run: |
          mkdir build && cd build
          cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=ON ..
          make -j$(nproc)
      - name: Test
        run: |
          cd build
          make test
```

### Travis CI

```yaml
language: c

compiler:
  - gcc
  - clang

before_install:
  - sudo apt-get update
  - sudo apt-get install -y libpng-dev

script:
  - mkdir build && cd build
  - cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_TESTS=ON ..
  - make -j$(nproc)
  - make test
```

## Best Practices

1. **Use out-of-source builds** - Always build in a separate directory
2. **Enable tests** - Always run tests before releasing
3. **Use appropriate build type** - Debug for development, Release for production
4. **Parallel compilation** - Use `-j$(nproc)` for faster builds
5. **Clean builds** - Remove build directory for clean builds
6. **Version control** - Don't commit build artifacts

---

_For more information about using the library, see [Getting Started](getting-started.md)_
