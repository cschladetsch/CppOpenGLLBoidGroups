#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build

# Navigate to build directory
cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build with 16 cores
make -j16

# Check if build succeeded
if [ $? -eq 0 ]; then
    echo "Build successful!"
else
    echo "Build failed!"
    exit 1
fi