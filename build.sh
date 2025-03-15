#!/bin/bash

# Script to build the cart project using CMake

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure CMake
echo "Configuring CMake build..."
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build the project
echo "Building project..."
cmake --build . -- -j$(nproc)

# Set executable permission so it can be run
chmod +x main/cart

echo "Build complete. You can run the executable with: ./build/main/cart"
echo "To run all tests: cd build && make run_tests"