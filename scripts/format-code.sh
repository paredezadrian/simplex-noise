#!/bin/bash
# Format code using clang-format and clang-tidy

set -e

echo "🔧 Running clang-format and clang-tidy on simplex noise library..."

# Check if tools are available
if ! command -v clang-format &> /dev/null; then
    echo "❌ clang-format not found. Please install it first."
    exit 1
fi

if ! command -v clang-tidy &> /dev/null; then
    echo "❌ clang-tidy not found. Please install it first."
    exit 1
fi

# Create build directory for clang-tidy
mkdir -p build

echo "📝 Running clang-format..."
clang-format -i src/*.c include/*.h examples/*.c tests/*.c

echo "🔍 Running clang-tidy..."
clang-tidy src/*.c include/*.h examples/*.c tests/*.c -- -Iinclude -std=c99

echo "✅ Code formatting and analysis complete!"
